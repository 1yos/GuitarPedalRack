#include "Reverb.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

ReverbEffect::ReverbEffect()
    : AudioModule("Reverb")
{
    // Initialize pitch buffer for shimmer mode
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < PITCH_BUFFER_SIZE; ++i)
            pitchBuffer[ch][i] = 0.0f;
        pitchWritePos[ch] = 0;
    }
    
    updateParameters();
}

void ReverbEffect::prepare(double sampleRate, int maxBlockSize)
{
    AudioModule::prepare(sampleRate, maxBlockSize);
    currentSampleRate = sampleRate;
    
    reverbProcessor.setSampleRate(sampleRate);
    updateParameters();
    reset();
}

void ReverbEffect::reset()
{
    AudioModule::reset();
    reverbProcessor.reset();
    
    // Clear pitch buffers
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < PITCH_BUFFER_SIZE; ++i)
            pitchBuffer[ch][i] = 0.0f;
        pitchWritePos[ch] = 0;
    }
}

void ReverbEffect::processInternal(AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    if (numChannels == 0 || numSamples == 0)
        return;
    
    // Read parameters from APVTS (if connected)
    if (hasParameter("size"))
        sizeParam = getParameterValue("size");
    if (hasParameter("decay"))
        dampingParam = 1.0f - getParameterValue("decay"); // Invert: decay → damping
    if (hasParameter("mix"))
        mixParam = getParameterValue("mix");
    if (hasParameter("bypass"))
    {
        bool shouldBypass = getParameterValue("bypass") > 0.5f;
        setBypass(shouldBypass);
    }
    
    updateParameters();
    
    if (currentMode == Shimmer)
    {
        // Shimmer mode: add octave-up feedback
        AudioBuffer<float> shimmerBuffer(numChannels, numSamples);
        shimmerBuffer.clear();
        
        // Process shimmer effect
        for (int channel = 0; channel < numChannels && channel < 2; ++channel)
        {
            const float* input = buffer.getReadPointer(channel);
            float* shimmer = shimmerBuffer.getWritePointer(channel);
            
            for (int i = 0; i < numSamples; ++i)
            {
                shimmer[i] = processShimmer(input[i], channel);
            }
        }
        
        // Mix shimmer with original
        for (int channel = 0; channel < numChannels; ++channel)
        {
            buffer.addFrom(channel, 0, shimmerBuffer, channel, 0, numSamples, 0.3f);
        }
    }
    
    // Process with JUCE reverb
    if (numChannels == 1)
    {
        // Mono to stereo for reverb
        AudioBuffer<float> stereoBuffer(2, numSamples);
        stereoBuffer.copyFrom(0, 0, buffer, 0, 0, numSamples);
        stereoBuffer.copyFrom(1, 0, buffer, 0, 0, numSamples);
        
        reverbProcessor.processStereo(stereoBuffer.getWritePointer(0),
                                     stereoBuffer.getWritePointer(1),
                                     numSamples);
        
        // Mix back to mono
        buffer.copyFrom(0, 0, stereoBuffer, 0, 0, numSamples);
        buffer.addFrom(0, 0, stereoBuffer, 1, 0, numSamples, 0.5f);
    }
    else if (numChannels >= 2)
    {
        reverbProcessor.processStereo(buffer.getWritePointer(0),
                                     buffer.getWritePointer(1),
                                     numSamples);
    }
}

void ReverbEffect::setSize(float size)
{
    sizeParam = jlimit(0.0f, 1.0f, size);
    updateParameters();
}

void ReverbEffect::setDamping(float damping)
{
    dampingParam = jlimit(0.0f, 1.0f, damping);
    updateParameters();
}

void ReverbEffect::setMix(float mix)
{
    mixParam = jlimit(0.0f, 1.0f, mix);
    updateParameters();
}

void ReverbEffect::setWidth(float width)
{
    widthParam = jlimit(0.0f, 1.0f, width);
    updateParameters();
}

void ReverbEffect::setMode(Mode mode)
{
    currentMode = mode;
    updateParameters();
}

void ReverbEffect::updateParameters()
{
    reverbParams.roomSize = sizeParam;
    reverbParams.damping = dampingParam;
    reverbParams.wetLevel = mixParam;
    reverbParams.dryLevel = 1.0f - mixParam;
    reverbParams.width = widthParam;
    reverbParams.freezeMode = 0.0f;
    
    // Adjust parameters based on mode
    switch (currentMode)
    {
        case Spring:
            // Spring: shorter decay, brighter, more reflections
            reverbParams.roomSize *= 0.6f;
            reverbParams.damping *= 0.5f;  // Brighter
            break;
            
        case Plate:
            // Plate: dense, bright, medium decay
            reverbParams.roomSize *= 0.8f;
            reverbParams.damping *= 0.6f;  // Bright
            break;
            
        case Hall:
            // Hall: spacious, natural, long decay
            reverbParams.roomSize = 0.5f + sizeParam * 0.5f;  // Larger space
            reverbParams.damping = 0.3f + dampingParam * 0.7f;  // Natural
            break;
            
        case Shimmer:
            // Shimmer: large space, long decay, less damping
            reverbParams.roomSize = 0.7f + sizeParam * 0.3f;  // Very large
            reverbParams.damping *= 0.4f;  // Bright for shimmer
            reverbParams.wetLevel *= 1.2f;  // More wet
            break;
    }
    
    reverbProcessor.setParameters(reverbParams);
}

float ReverbEffect::processShimmer(float sample, int channel)
{
    // Simple pitch shifter: octave up using buffer read at half speed
    pitchBuffer[channel][pitchWritePos[channel]] = sample;
    
    // Read at half the buffer size for octave up
    int readPos = (pitchWritePos[channel] + PITCH_BUFFER_SIZE / 2) % PITCH_BUFFER_SIZE;
    float pitchShifted = pitchBuffer[channel][readPos];
    
    pitchWritePos[channel] = (pitchWritePos[channel] + 1) % PITCH_BUFFER_SIZE;
    
    return pitchShifted * 0.5f; // Attenuate for mixing
}

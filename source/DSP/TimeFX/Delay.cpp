#include "Delay.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Delay::Delay()
    : AudioModule("Delay")
{
    // Initialize delay buffer
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < MAX_DELAY_SAMPLES; ++i)
            delayBuffer[ch][i] = 0.0f;
        delayWritePos[ch] = 0;
    }
    
    updateParameters();
}

void Delay::prepare(double sampleRate, int maxBlockSize)
{
    AudioModule::prepare(sampleRate, maxBlockSize);
    currentSampleRate = sampleRate;
    updateParameters();
    reset();
}

void Delay::reset()
{
    AudioModule::reset();
    
    // Clear delay buffers
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < MAX_DELAY_SAMPLES; ++i)
            delayBuffer[ch][i] = 0.0f;
        delayWritePos[ch] = 0;
        dampingZ1[ch] = 0.0f;
    }
    
    modulationPhase = 0.0f;
    wowPhase = 0.0f;
    flutterPhase = 0.0f;
}

void Delay::processInternal(AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    if (numChannels == 0 || numSamples == 0)
        return;
    
    updateParameters();
    
    const float delaySamples = (delayTimeMs / 1000.0f) * (float)currentSampleRate;
    
    for (int i = 0; i < numSamples; ++i)
    {
        for (int channel = 0; channel < numChannels && channel < 2; ++channel)
        {
            float* channelData = buffer.getWritePointer(channel);
            float inputSample = channelData[i];
            
            // Determine read channel for ping-pong
            int readChannel = channel;
            if (pingPongEnabled && numChannels == 2)
                readChannel = 1 - channel; // Read from opposite channel
            
            // Read delayed sample
            float delayedSample = readDelay(readChannel, delaySamples);
            
            // Apply mode-specific character
            delayedSample = applyModeCharacter(delayedSample, channel);
            
            // Damping filter (low-pass)
            float filtered = dampingCoeff * delayedSample + (1.0f - dampingCoeff) * dampingZ1[channel];
            dampingZ1[channel] = filtered;
            
            // Feedback
            float feedbackSample = inputSample + filtered * feedbackAmount;
            
            // Write to delay buffer
            delayBuffer[channel][delayWritePos[channel]] = feedbackSample;
            
            // Mix dry and wet
            channelData[i] = dryLevel * inputSample + wetLevel * filtered;
        }
        
        // Advance write position
        for (int ch = 0; ch < numChannels && ch < 2; ++ch)
            delayWritePos[ch] = (delayWritePos[ch] + 1) % MAX_DELAY_SAMPLES;
        
        // Advance modulation phases
        modulationPhase += modulationRate / (float)currentSampleRate;
        if (modulationPhase >= 1.0f) modulationPhase -= 1.0f;
        
        wowPhase += 0.2f / (float)currentSampleRate; // 0.2 Hz wow
        if (wowPhase >= 1.0f) wowPhase -= 1.0f;
        
        flutterPhase += 6.0f / (float)currentSampleRate; // 6 Hz flutter
        if (flutterPhase >= 1.0f) flutterPhase -= 1.0f;
    }
}

void Delay::setTime(float time)
{
    timeParam = jlimit(0.0f, 1.0f, time);
    updateParameters();
}

void Delay::setFeedback(float feedback)
{
    feedbackParam = jlimit(0.0f, 1.0f, feedback);
    updateParameters();
}

void Delay::setMix(float mix)
{
    mixParam = jlimit(0.0f, 1.0f, mix);
    updateParameters();
}

void Delay::setDamping(float damping)
{
    dampingParam = jlimit(0.0f, 1.0f, damping);
    updateParameters();
}

void Delay::setMode(Mode mode)
{
    currentMode = mode;
}

void Delay::setPingPong(bool enabled)
{
    pingPongEnabled = enabled;
}

void Delay::updateParameters()
{
    // Delay time: 10ms to 2000ms
    delayTimeMs = 10.0f + timeParam * 1990.0f;
    
    // Feedback: 0% to 95% (prevent runaway)
    feedbackAmount = feedbackParam * 0.95f;
    
    // Dry/wet mix
    wetLevel = mixParam;
    dryLevel = 1.0f - mixParam * 0.5f; // Keep some dry signal
    
    // Damping coefficient (higher = darker/warmer repeats)
    dampingCoeff = 0.3f + dampingParam * 0.65f;
}

float Delay::readDelay(int channel, float delaySamples)
{
    // Calculate read position
    float readPos = (float)delayWritePos[channel] - delaySamples;
    
    while (readPos < 0.0f)
        readPos += (float)MAX_DELAY_SAMPLES;
    while (readPos >= (float)MAX_DELAY_SAMPLES)
        readPos -= (float)MAX_DELAY_SAMPLES;
    
    // Linear interpolation
    int pos1 = (int)readPos;
    int pos2 = (pos1 + 1) % MAX_DELAY_SAMPLES;
    float frac = readPos - (float)pos1;
    
    float sample1 = delayBuffer[channel][pos1];
    float sample2 = delayBuffer[channel][pos2];
    
    return sample1 + frac * (sample2 - sample1);
}

float Delay::applyModeCharacter(float sample, int channel)
{
    switch (currentMode)
    {
        case Digital:
            // Clean, no additional processing
            return sample;
            
        case Analog:
        {
            // Add subtle modulation (BBD character)
            float lfo = std::sin(modulationPhase * 2.0f * float(M_PI));
            float modulated = sample * (1.0f + lfo * 0.002f); // Very subtle pitch modulation
            return modulated * 0.95f; // Slight level reduction
        }
            
        case Tape:
        {
            // Add wow and flutter
            float wow = std::sin(wowPhase * 2.0f * float(M_PI)) * 0.005f;
            float flutter = std::sin(flutterPhase * 2.0f * float(M_PI)) * 0.002f;
            float pitchMod = 1.0f + wow + flutter;
            
            // Soft saturation (tape character)
            float saturated = std::tanh(sample * 1.2f) * 0.9f;
            
            return saturated * pitchMod;
        }
    }
    
    return sample;
}

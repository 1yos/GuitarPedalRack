#include "Chorus.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Chorus::Chorus()
    : AudioModule("Chorus")
{
    // Initialize delay buffer
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < MAX_DELAY_SAMPLES; ++i)
            delayBuffer[ch][i] = 0.0f;
        delayWritePos[ch] = 0;
    }
    
    // Initialize LFO phases (spread evenly)
    for (int v = 0; v < MAX_VOICES; ++v)
    {
        lfoPhase[v] = (float)v / (float)MAX_VOICES; // 0, 0.25, 0.5, 0.75 for 4 voices
        lfoPhaseInc[v] = 0.0f;
    }
    
    updateParameters();
}

void Chorus::prepare(double sampleRate, int maxBlockSize)
{
    AudioModule::prepare(sampleRate, maxBlockSize);
    currentSampleRate = sampleRate;
    updateParameters();
    reset();
}

void Chorus::reset()
{
    AudioModule::reset();
    
    // Clear delay buffers
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < MAX_DELAY_SAMPLES; ++i)
            delayBuffer[ch][i] = 0.0f;
        delayWritePos[ch] = 0;
    }
}

void Chorus::processInternal(AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    if (numChannels == 0 || numSamples == 0)
        return;
    
    // Read parameters from APVTS (if connected)
    if (hasParameter("rate"))
        rateParam = getParameterValue("rate") / 10.0f; // Scale from 0.1-10 to 0-1
    if (hasParameter("depth"))
        depthParam = getParameterValue("depth");
    if (hasParameter("mix"))
        mixParam = getParameterValue("mix");
    if (hasParameter("bypass"))
    {
        bool shouldBypass = getParameterValue("bypass") > 0.5f;
        setBypass(shouldBypass);
    }
    
    updateParameters();
    
    for (int channel = 0; channel < numChannels && channel < 2; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        for (int i = 0; i < numSamples; ++i)
        {
            float inputSample = channelData[i];
            
            // Write to delay buffer
            delayBuffer[channel][delayWritePos[channel]] = inputSample;
            
            // Mix voices
            float chorusSum = 0.0f;
            
            for (int v = 0; v < voices; ++v)
            {
                // Generate LFO (sine wave)
                float lfo = std::sin(lfoPhase[v] * 2.0f * float(M_PI));
                
                // Calculate delay time (base delay + LFO modulation)
                float baseDelay = 10.0f; // 10 ms base delay
                float delayMs = baseDelay + lfo * modDepth;
                float delaySamples = (delayMs / 1000.0f) * (float)currentSampleRate;
                
                // Read from delay buffer with linear interpolation
                float delayedSample = readDelayLinear(channel, delaySamples);
                chorusSum += delayedSample;
                
                // Advance LFO phase
                lfoPhase[v] += lfoPhaseInc[v];
                if (lfoPhase[v] >= 1.0f)
                    lfoPhase[v] -= 1.0f;
            }
            
            // Average the voices
            float chorusOutput = chorusSum / (float)voices;
            
            // Mix dry and wet
            channelData[i] = dryLevel * inputSample + wetLevel * chorusOutput;
            
            // Advance write position
            delayWritePos[channel] = (delayWritePos[channel] + 1) % MAX_DELAY_SAMPLES;
        }
    }
}

void Chorus::setRate(float rate)
{
    rateParam = jlimit(0.0f, 1.0f, rate);
    updateParameters();
}

void Chorus::setDepth(float depth)
{
    depthParam = jlimit(0.0f, 1.0f, depth);
    updateParameters();
}

void Chorus::setMix(float mix)
{
    mixParam = jlimit(0.0f, 1.0f, mix);
    updateParameters();
}

void Chorus::setVoices(int numVoices)
{
    voices = jlimit(1, MAX_VOICES, numVoices);
}

void Chorus::updateParameters()
{
    // LFO rate: 0.1 Hz to 5 Hz
    lfoRate = 0.1f + rateParam * 4.9f;
    
    // LFO phase increment per sample
    for (int v = 0; v < MAX_VOICES; ++v)
    {
        lfoPhaseInc[v] = lfoRate / (float)currentSampleRate;
    }
    
    // Modulation depth: 0 to 10 ms
    modDepth = depthParam * 10.0f;
    
    // Dry/wet mix
    wetLevel = mixParam;
    dryLevel = 1.0f - mixParam * 0.5f; // Keep some dry signal even at full wet
}

float Chorus::readDelayLinear(int channel, float delaySamples)
{
    // Safety: validate channel
    if (channel < 0 || channel >= 2)
        return 0.0f;
    
    // Safety: clamp delay samples to valid range
    delaySamples = jlimit(0.0f, (float)(MAX_DELAY_SAMPLES - 2), delaySamples);
    
    // Calculate read position
    float readPos = (float)delayWritePos[channel] - delaySamples;
    
    while (readPos < 0.0f)
        readPos += (float)MAX_DELAY_SAMPLES;
    while (readPos >= (float)MAX_DELAY_SAMPLES)
        readPos -= (float)MAX_DELAY_SAMPLES;
    
    // Linear interpolation with bounds checking
    int pos1 = (int)readPos;
    int pos2 = (pos1 + 1) % MAX_DELAY_SAMPLES;
    
    // Safety: validate indices
    if (pos1 < 0 || pos1 >= MAX_DELAY_SAMPLES || pos2 < 0 || pos2 >= MAX_DELAY_SAMPLES)
        return 0.0f;
    
    float frac = readPos - (float)pos1;
    frac = jlimit(0.0f, 1.0f, frac);
    
    float sample1 = delayBuffer[channel][pos1];
    float sample2 = delayBuffer[channel][pos2];
    
    // Safety: check for invalid values
    if (std::isnan(sample1) || std::isinf(sample1))
        sample1 = 0.0f;
    if (std::isnan(sample2) || std::isinf(sample2))
        sample2 = 0.0f;
    
    return sample1 + frac * (sample2 - sample1);
}

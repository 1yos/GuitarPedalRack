#include "Harmonizer.h"
#include <cmath>

Harmonizer::Harmonizer() : AudioModule("Harmonizer")
{
}

Harmonizer::~Harmonizer()
{
}

void Harmonizer::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    
    for (auto& state : channelState)
    {
        state.delayBuffer.setSize(1, delayBufferSize);
        state.delayBuffer.clear();
    }
    
    reset();
}

void Harmonizer::reset()
{
    for (auto& state : channelState)
    {
        state.delayBuffer.clear();
        state.writePos = 0;
        state.readPos = 0.0f;
    }
}

void Harmonizer::setInterval(float interval)
{
    intervalParam.store(jlimit(0.0f, 1.0f, interval), std::memory_order_relaxed);
}

void Harmonizer::setKey(float key)
{
    keyParam.store(jlimit(0.0f, 1.0f, key), std::memory_order_relaxed);
}

void Harmonizer::setMix(float mix)
{
    mixParam.store(jlimit(0.0f, 1.0f, mix), std::memory_order_relaxed);
}

void Harmonizer::processInternal(AudioBuffer<float>& buffer)
{
    if (!bypassed)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        
        // Map interval parameter to common intervals
        const float intervalNorm = intervalParam.load(std::memory_order_relaxed);
        float semitones;
        
        if (intervalNorm < 0.2f)
            semitones = 3.0f;  // Minor 3rd
        else if (intervalNorm < 0.4f)
            semitones = 4.0f;  // Major 3rd
        else if (intervalNorm < 0.6f)
            semitones = 5.0f;  // Perfect 4th
        else if (intervalNorm < 0.8f)
            semitones = 7.0f;  // Perfect 5th
        else
            semitones = 12.0f; // Octave
        
        const float pitchRatio = std::pow(2.0f, semitones / 12.0f);
        const float mix = mixParam.load(std::memory_order_relaxed);
        
        for (int ch = 0; ch < numChannels && ch < (int)channelState.size(); ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            auto& state = channelState[ch];
            float* delayData = state.delayBuffer.getWritePointer(0);
            
            for (int i = 0; i < numSamples; ++i)
            {
                const float dry = channelData[i];
                
                // Write to delay buffer
                delayData[state.writePos] = dry;
                
                // Read harmonized signal
                const int readPos1 = static_cast<int>(state.readPos) % delayBufferSize;
                const int readPos2 = (readPos1 + 1) % delayBufferSize;
                const float frac = state.readPos - static_cast<int>(state.readPos);
                
                const float sample1 = delayData[readPos1];
                const float sample2 = delayData[readPos2];
                const float harmonized = sample1 + frac * (sample2 - sample1);
                
                // Mix dry and harmonized
                channelData[i] = dry * (1.0f - mix) + harmonized * mix;
                
                // Update positions
                state.readPos += pitchRatio;
                if (state.readPos >= delayBufferSize)
                    state.readPos -= delayBufferSize;
                
                state.writePos = (state.writePos + 1) % delayBufferSize;
            }
        }
    }
}

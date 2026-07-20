#include "PitchCorrection.h"
#include <cmath>

PitchCorrection::PitchCorrection() : AudioModule("PitchCorrection")
{
}

PitchCorrection::~PitchCorrection()
{
}

void PitchCorrection::prepare(double sampleRate, int samplesPerBlock)
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

void PitchCorrection::reset()
{
    for (auto& state : channelState)
    {
        state.delayBuffer.clear();
        state.writePos = 0;
        state.readPos = 0.0f;
    }
}

void PitchCorrection::setKey(float key)
{
    keyParam.store(jlimit(0.0f, 1.0f, key), std::memory_order_relaxed);
}

void PitchCorrection::setRetune(float retune)
{
    retuneParam.store(jlimit(0.0f, 1.0f, retune), std::memory_order_relaxed);
}

void PitchCorrection::setAmount(float amount)
{
    amountParam.store(jlimit(0.0f, 1.0f, amount), std::memory_order_relaxed);
}

void PitchCorrection::processInternal(AudioBuffer<float>& buffer)
{
    if (!bypassed)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        
        const float amount = amountParam.load(std::memory_order_relaxed);
        const float retune = 0.01f + retuneParam.load(std::memory_order_relaxed) * 0.99f;
        
        // Simple pitch correction: snap to nearest semitone
        for (int ch = 0; ch < numChannels && ch < (int)channelState.size(); ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            auto& state = channelState[ch];
            float* delayData = state.delayBuffer.getWritePointer(0);
            
            for (int i = 0; i < numSamples; ++i)
            {
                const float dry = channelData[i];
                
                delayData[state.writePos] = dry;
                
                // Simplified pitch correction
                const int readPos1 = static_cast<int>(state.readPos) % delayBufferSize;
                const int readPos2 = (readPos1 + 1) % delayBufferSize;
                const float frac = state.readPos - static_cast<int>(state.readPos);
                
                const float sample1 = delayData[readPos1];
                const float sample2 = delayData[readPos2];
                const float corrected = sample1 + frac * (sample2 - sample1);
                
                channelData[i] = dry * (1.0f - amount) + corrected * amount;
                
                state.readPos += 1.0f + (retune - 0.5f) * 0.02f;
                if (state.readPos >= delayBufferSize)
                    state.readPos -= delayBufferSize;
                
                state.writePos = (state.writePos + 1) % delayBufferSize;
            }
        }
    }
}

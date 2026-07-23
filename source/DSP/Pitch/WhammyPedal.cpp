#include "WhammyPedal.h"
#include <cmath>

WhammyPedal::WhammyPedal() : AudioModule("WhammyPedal")
{
}

WhammyPedal::~WhammyPedal()
{
}

void WhammyPedal::prepare(double sampleRate, int samplesPerBlock)
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

void WhammyPedal::reset()
{
    for (auto& state : channelState)
    {
        state.delayBuffer.clear();
        state.writePos = 0;
        state.readPos = 0.0f;
    }
}

void WhammyPedal::setRange(float range)
{
    rangeParam.store(jlimit(0.0f, 1.0f, range), std::memory_order_relaxed);
}

void WhammyPedal::setExpression(float expr)
{
    expressionParam.store(jlimit(0.0f, 1.0f, expr), std::memory_order_relaxed);
}

void WhammyPedal::setMode(float mode)
{
    modeParam.store(jlimit(0.0f, 1.0f, mode), std::memory_order_relaxed);
}

void WhammyPedal::processInternal(AudioBuffer<float>& buffer)
{
    if (!bypassed)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        
        // Calculate pitch bend amount
        const float rangeNorm = rangeParam.load(std::memory_order_relaxed);
        const float expression = expressionParam.load(std::memory_order_relaxed);
        const float mode = modeParam.load(std::memory_order_relaxed);
        
        // Range: 1 to 24 semitones
        const float rangeSemitones = 1.0f + rangeNorm * 23.0f;
        
        // Calculate target pitch based on mode
        float targetSemitones;
        if (mode < 0.33f)
            targetSemitones = -rangeSemitones * expression; // Down
        else if (mode < 0.67f)
            targetSemitones = rangeSemitones * expression;  // Up
        else
            targetSemitones = (expression - 0.5f) * 2.0f * rangeSemitones; // Both
        
        const float pitchRatio = std::pow(2.0f, targetSemitones / 12.0f);
        
        for (int ch = 0; ch < numChannels && ch < (int)channelState.size(); ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            auto& state = channelState[ch];
            float* delayData = state.delayBuffer.getWritePointer(0);
            
            for (int i = 0; i < numSamples; ++i)
            {
                // Write to delay buffer
                delayData[state.writePos] = channelData[i];
                
                // Read with whammy pitch shift
                const int readPos1 = static_cast<int>(state.readPos) % delayBufferSize;
                const int readPos2 = (readPos1 + 1) % delayBufferSize;
                const float frac = state.readPos - static_cast<int>(state.readPos);
                
                const float sample1 = delayData[readPos1];
                const float sample2 = delayData[readPos2];
                channelData[i] = sample1 + frac * (sample2 - sample1);
                
                // Update positions
                state.readPos += pitchRatio;
                if (state.readPos >= delayBufferSize)
                    state.readPos -= delayBufferSize;
                
                state.writePos = (state.writePos + 1) % delayBufferSize;
            }
        }
    }
}

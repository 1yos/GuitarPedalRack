#include "FormantShifter.h"
#include <cmath>

FormantShifter::FormantShifter() : AudioModule("FormantShifter")
{
}

FormantShifter::~FormantShifter()
{
}

void FormantShifter::prepare(double sampleRate, int samplesPerBlock)
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

void FormantShifter::reset()
{
    for (auto& state : channelState)
    {
        state.delayBuffer.clear();
        state.writePos = 0;
        state.readPos = 0.0f;
    }
}

void FormantShifter::setFormantShift(float shift)
{
    formantShiftParam.store(jlimit(0.0f, 1.0f, shift), std::memory_order_relaxed);
}

void FormantShifter::setPitchShift(float pitch)
{
    pitchShiftParam.store(jlimit(0.0f, 1.0f, pitch), std::memory_order_relaxed);
}

void FormantShifter::processInternal(AudioBuffer<float>& buffer)
{
    if (!bypassed)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        
        const float formantSemitones = -12.0f + formantShiftParam.load(std::memory_order_relaxed) * 24.0f;
        const float pitchSemitones = -12.0f + pitchShiftParam.load(std::memory_order_relaxed) * 24.0f;
        const float pitchRatio = std::pow(2.0f, pitchSemitones / 12.0f);
        
        for (int ch = 0; ch < numChannels && ch < (int)channelState.size(); ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            auto& state = channelState[ch];
            float* delayData = state.delayBuffer.getWritePointer(0);
            
            for (int i = 0; i < numSamples; ++i)
            {
                delayData[state.writePos] = channelData[i];
                
                const int readPos1 = static_cast<int>(state.readPos) % delayBufferSize;
                const int readPos2 = (readPos1 + 1) % delayBufferSize;
                const float frac = state.readPos - static_cast<int>(state.readPos);
                
                const float sample1 = delayData[readPos1];
                const float sample2 = delayData[readPos2];
                channelData[i] = sample1 + frac * (sample2 - sample1);
                
                state.readPos += pitchRatio;
                if (state.readPos >= delayBufferSize)
                    state.readPos -= delayBufferSize;
                
                state.writePos = (state.writePos + 1) % delayBufferSize;
            }
        }
    }
}

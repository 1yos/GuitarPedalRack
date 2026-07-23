#include "PitchVibrato.h"
#include <cmath>

PitchVibrato::PitchVibrato() : AudioModule("PitchVibrato")
{
}

PitchVibrato::~PitchVibrato()
{
}

void PitchVibrato::prepare(double sampleRate, int samplesPerBlock)
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

void PitchVibrato::reset()
{
    for (auto& state : channelState)
    {
        state.delayBuffer.clear();
        state.writePos = 0;
        state.lfoPhase = 0.0f;
    }
}

void PitchVibrato::setRate(float rate)
{
    rateParam.store(jlimit(0.0f, 1.0f, rate), std::memory_order_relaxed);
}

void PitchVibrato::setDepth(float depth)
{
    depthParam.store(jlimit(0.0f, 1.0f, depth), std::memory_order_relaxed);
}

void PitchVibrato::setWaveform(float waveform)
{
    waveformParam.store(jlimit(0.0f, 1.0f, waveform), std::memory_order_relaxed);
}

void PitchVibrato::processInternal(AudioBuffer<float>& buffer)
{
    if (!bypassed)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        
        const float rate = 0.1f + rateParam.load(std::memory_order_relaxed) * 9.9f;
        const float depth = depthParam.load(std::memory_order_relaxed) * 50.0f;
        const float lfoInc = rate / static_cast<float>(currentSampleRate);
        
        for (int ch = 0; ch < numChannels && ch < (int)channelState.size(); ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            auto& state = channelState[ch];
            float* delayData = state.delayBuffer.getWritePointer(0);
            
            for (int i = 0; i < numSamples; ++i)
            {
                delayData[state.writePos] = channelData[i];
                
                const float lfo = std::sin(juce::MathConstants<float>::twoPi * state.lfoPhase);
                const float delaySamples = depth * lfo;
                float readPos = state.writePos - delaySamples;
                
                while (readPos < 0.0f)
                    readPos += delayBufferSize;
                while (readPos >= delayBufferSize)
                    readPos -= delayBufferSize;
                
                const int readPos1 = static_cast<int>(readPos);
                const int readPos2 = (readPos1 + 1) % delayBufferSize;
                const float frac = readPos - readPos1;
                
                const float sample1 = delayData[readPos1];
                const float sample2 = delayData[readPos2];
                channelData[i] = sample1 + frac * (sample2 - sample1);
                
                state.lfoPhase += lfoInc;
                if (state.lfoPhase >= 1.0f)
                    state.lfoPhase -= 1.0f;
                
                state.writePos = (state.writePos + 1) % delayBufferSize;
            }
        }
    }
}

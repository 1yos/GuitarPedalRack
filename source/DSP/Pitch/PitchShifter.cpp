#include "PitchShifter.h"
#include <cmath>

PitchShifter::PitchShifter() : AudioModule("PitchShifter")
{
}

PitchShifter::~PitchShifter()
{
}

void PitchShifter::prepare(double sampleRate, int samplesPerBlock)
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

void PitchShifter::reset()
{
    for (auto& state : channelState)
    {
        state.delayBuffer.clear();
        state.writePos = 0;
        state.readPos = 0.0f;
        state.phase = 0.0f;
    }
}

void PitchShifter::setPitch(float pitch)
{
    pitchParam.store(jlimit(0.0f, 1.0f, pitch), std::memory_order_relaxed);
}

void PitchShifter::setFormant(float formant)
{
    formantParam.store(jlimit(0.0f, 1.0f, formant), std::memory_order_relaxed);
}

void PitchShifter::processInternal(AudioBuffer<float>& buffer)
{
    if (!bypassed)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        
        // Convert pitch parameter to semitones (-12 to +12)
        const float semitones = -12.0f + pitchParam.load(std::memory_order_relaxed) * 24.0f;
        const float pitchRatio = std::pow(2.0f, semitones / 12.0f);
        
        for (int ch = 0; ch < numChannels && ch < (int)channelState.size(); ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            auto& state = channelState[ch];
            float* delayData = state.delayBuffer.getWritePointer(0);
            
            for (int i = 0; i < numSamples; ++i)
            {
                // Write to delay buffer
                delayData[state.writePos] = channelData[i];
                
                // Read with pitch shift using linear interpolation
                const int readPos1 = static_cast<int>(state.readPos) % delayBufferSize;
                const int readPos2 = (readPos1 + 1) % delayBufferSize;
                const float frac = state.readPos - static_cast<int>(state.readPos);
                
                const float sample1 = delayData[readPos1];
                const float sample2 = delayData[readPos2];
                const float interpolated = sample1 + frac * (sample2 - sample1);
                
                // Crossfade window for smooth pitch shifting
                const float window = 0.5f + 0.5f * std::cos(juce::MathConstants<float>::twoPi * state.phase);
                
                channelData[i] = interpolated * window;
                
                // Update positions
                state.readPos += pitchRatio;
                if (state.readPos >= delayBufferSize)
                    state.readPos -= delayBufferSize;
                
                state.writePos = (state.writePos + 1) % delayBufferSize;
                
                state.phase += 1.0f / 2048.0f;
                if (state.phase >= 1.0f)
                    state.phase -= 1.0f;
            }
        }
    }
}

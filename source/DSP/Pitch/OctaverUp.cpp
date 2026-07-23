#include "OctaverUp.h"
#include <cmath>

OctaverUp::OctaverUp() : AudioModule("OctaverUp")
{
}

OctaverUp::~OctaverUp()
{
}

void OctaverUp::prepare(double sampleRate, int samplesPerBlock)
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

void OctaverUp::reset()
{
    for (auto& state : channelState)
    {
        state.delayBuffer.clear();
        state.writePos = 0;
        state.readPos = 0.0f;
    }
}

void OctaverUp::setOctaveLevel(float level)
{
    octaveLevelParam.store(jlimit(0.0f, 1.0f, level), std::memory_order_relaxed);
}

void OctaverUp::setDirect(float direct)
{
    directParam.store(jlimit(0.0f, 1.0f, direct), std::memory_order_relaxed);
}

void OctaverUp::processInternal(AudioBuffer<float>& buffer)
{
    if (!bypassed)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        
        const float octaveLevel = octaveLevelParam.load(std::memory_order_relaxed);
        const float directLevel = directParam.load(std::memory_order_relaxed);
        const float pitchRatio = 2.0f; // One octave up
        
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
                
                // Read octave up signal
                const int readPos1 = static_cast<int>(state.readPos) % delayBufferSize;
                const int readPos2 = (readPos1 + 1) % delayBufferSize;
                const float frac = state.readPos - static_cast<int>(state.readPos);
                
                const float sample1 = delayData[readPos1];
                const float sample2 = delayData[readPos2];
                const float octaveUp = sample1 + frac * (sample2 - sample1);
                
                // Mix direct and octave
                channelData[i] = dry * directLevel + octaveUp * octaveLevel;
                
                // Update positions
                state.readPos += pitchRatio;
                if (state.readPos >= delayBufferSize)
                    state.readPos -= delayBufferSize;
                
                state.writePos = (state.writePos + 1) % delayBufferSize;
            }
        }
    }
}

#include "OctaverDown.h"
#include <cmath>

OctaverDown::OctaverDown() : AudioModule("OctaverDown")
{
}

OctaverDown::~OctaverDown()
{
}

void OctaverDown::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    reset();
}

void OctaverDown::reset()
{
    for (auto& state : channelState)
    {
        state.lastSample = 0.0f;
        state.flip = false;
    }
}

void OctaverDown::setOctaveLevel(float level)
{
    octaveLevelParam.store(jlimit(0.0f, 1.0f, level), std::memory_order_relaxed);
}

void OctaverDown::setDirect(float direct)
{
    directParam.store(jlimit(0.0f, 1.0f, direct), std::memory_order_relaxed);
}

void OctaverDown::processInternal(AudioBuffer<float>& buffer)
{
    if (!bypassed)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        
        const float octaveLevel = octaveLevelParam.load(std::memory_order_relaxed);
        const float directLevel = directParam.load(std::memory_order_relaxed);
        
        for (int ch = 0; ch < numChannels && ch < (int)channelState.size(); ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            auto& state = channelState[ch];
            
            for (int i = 0; i < numSamples; ++i)
            {
                const float input = channelData[i];
                
                // Simple octave down: flip phase on zero crossings
                float octaveDown = input;
                
                if ((state.lastSample < 0.0f && input >= 0.0f) || 
                    (state.lastSample >= 0.0f && input < 0.0f))
                {
                    state.flip = !state.flip;
                }
                
                if (state.flip)
                    octaveDown = -input;
                
                state.lastSample = input;
                
                // Mix direct and octave down
                channelData[i] = input * directLevel + octaveDown * octaveLevel;
            }
        }
    }
}

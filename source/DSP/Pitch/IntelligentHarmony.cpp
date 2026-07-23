#include "IntelligentHarmony.h"
#include <cmath>

IntelligentHarmony::IntelligentHarmony() : AudioModule("IntelligentHarmony")
{
}

IntelligentHarmony::~IntelligentHarmony()
{
}

void IntelligentHarmony::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    
    for (auto& state : channelState)
    {
        state.delayBuffer.setSize(1, delayBufferSize);
        state.delayBuffer.clear();
        
        // Initialize harmony voices (3rd, 5th, octave)
        state.voices[0].interval = 4;  // Major 3rd
        state.voices[1].interval = 7;  // Perfect 5th
        state.voices[2].interval = 12; // Octave
    }
    
    reset();
}

void IntelligentHarmony::reset()
{
    for (auto& state : channelState)
    {
        state.delayBuffer.clear();
        state.writePos = 0;
        
        for (int v = 0; v < maxVoices; ++v)
            state.voices[v].readPos = 0.0f;
    }
}

void IntelligentHarmony::setKey(float key)
{
    keyParam.store(jlimit(0.0f, 1.0f, key), std::memory_order_relaxed);
}

void IntelligentHarmony::setScale(float scale)
{
    scaleParam.store(jlimit(0.0f, 1.0f, scale), std::memory_order_relaxed);
}

void IntelligentHarmony::setVoices(float voices)
{
    voicesParam.store(jlimit(0.0f, 1.0f, voices), std::memory_order_relaxed);
}

void IntelligentHarmony::processInternal(AudioBuffer<float>& buffer)
{
    if (!bypassed)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        
        const float voicesLevel = voicesParam.load(std::memory_order_relaxed);
        const int activeVoices = 1 + static_cast<int>(voicesLevel * (maxVoices - 1));
        
        for (int ch = 0; ch < numChannels && ch < (int)channelState.size(); ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            auto& state = channelState[ch];
            float* delayData = state.delayBuffer.getWritePointer(0);
            
            for (int i = 0; i < numSamples; ++i)
            {
                const float dry = channelData[i];
                
                delayData[state.writePos] = dry;
                
                float harmony = dry;
                for (int v = 0; v < activeVoices; ++v)
                {
                    auto& voice = state.voices[v];
                    
                    const float pitchRatio = std::pow(2.0f, voice.interval / 12.0f);
                    
                    const int readPos1 = static_cast<int>(voice.readPos) % delayBufferSize;
                    const int readPos2 = (readPos1 + 1) % delayBufferSize;
                    const float frac = voice.readPos - static_cast<int>(voice.readPos);
                    
                    const float sample1 = delayData[readPos1];
                    const float sample2 = delayData[readPos2];
                    harmony += (sample1 + frac * (sample2 - sample1)) / (activeVoices + 1);
                    
                    voice.readPos += pitchRatio;
                    if (voice.readPos >= delayBufferSize)
                        voice.readPos -= delayBufferSize;
                }
                
                channelData[i] = harmony;
                
                state.writePos = (state.writePos + 1) % delayBufferSize;
            }
        }
    }
}

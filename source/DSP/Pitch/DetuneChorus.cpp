#include "DetuneChorus.h"
#include <cmath>

DetuneChorus::DetuneChorus() : AudioModule("DetuneChorus")
{
}

DetuneChorus::~DetuneChorus()
{
}

void DetuneChorus::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    
    for (auto& state : channelState)
    {
        state.delayBuffer.setSize(1, delayBufferSize);
        state.delayBuffer.clear();
        
        // Initialize voice detunes
        for (int v = 0; v < numVoices; ++v)
        {
            state.voices[v].readPos = 0.0f;
            state.voices[v].detuneCents = (v - numVoices / 2.0f) * 10.0f;
        }
    }
    
    reset();
}

void DetuneChorus::reset()
{
    for (auto& state : channelState)
    {
        state.delayBuffer.clear();
        state.writePos = 0;
    }
}

void DetuneChorus::setDetune(float detune)
{
    detuneParam.store(jlimit(0.0f, 1.0f, detune), std::memory_order_relaxed);
}

void DetuneChorus::setVoices(float voices)
{
    voicesParam.store(jlimit(0.0f, 1.0f, voices), std::memory_order_relaxed);
}

void DetuneChorus::setSpread(float spread)
{
    spreadParam.store(jlimit(0.0f, 1.0f, spread), std::memory_order_relaxed);
}

void DetuneChorus::processInternal(AudioBuffer<float>& buffer)
{
    if (!bypassed)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        
        const float detune = 1.0f + detuneParam.load(std::memory_order_relaxed) * 49.0f; // 1-50 cents
        const float voicesLevel = voicesParam.load(std::memory_order_relaxed);
        
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
                
                // Sum detuned voices
                float wet = 0.0f;
                for (int v = 0; v < numVoices; ++v)
                {
                    auto& voice = state.voices[v];
                    
                    // Calculate pitch ratio for this voice
                    const float cents = voice.detuneCents * detune;
                    const float pitchRatio = std::pow(2.0f, cents / 1200.0f);
                    
                    // Read sample
                    const int readPos1 = static_cast<int>(voice.readPos) % delayBufferSize;
                    const int readPos2 = (readPos1 + 1) % delayBufferSize;
                    const float frac = voice.readPos - static_cast<int>(voice.readPos);
                    
                    const float sample1 = delayData[readPos1];
                    const float sample2 = delayData[readPos2];
                    wet += (sample1 + frac * (sample2 - sample1)) / numVoices;
                    
                    // Update voice position
                    voice.readPos += pitchRatio;
                    if (voice.readPos >= delayBufferSize)
                        voice.readPos -= delayBufferSize;
                }
                
                // Mix
                channelData[i] = dry * (1.0f - voicesLevel) + wet * voicesLevel;
                
                state.writePos = (state.writePos + 1) % delayBufferSize;
            }
        }
    }
}

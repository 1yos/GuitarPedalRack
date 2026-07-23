#include "StateVariableFilter.h"

StateVariableFilter::StateVariableFilter() : AudioModule("StateVariableFilter") {}
StateVariableFilter::~StateVariableFilter() {}

void StateVariableFilter::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
}

void StateVariableFilter::reset()
{
    for (auto& ch : channelState)
    {
        ch.low = 0.0f;
        ch.band = 0.0f;
        ch.high = 0.0f;
    }
}

void StateVariableFilter::setFrequency(float frequency) { frequencyParam.store(frequency); }
void StateVariableFilter::setResonance(float resonance) { resonanceParam.store(resonance); }
void StateVariableFilter::setMode(float mode) { modeParam.store(mode); }

void StateVariableFilter::processInternal(AudioBuffer<float>& buffer)
{
    const float freq = frequencyParam.load() * 18000.0f + 20.0f;
    const float Q = resonanceParam.load() * 10.0f + 0.5f;
    const float mode = modeParam.load();
    const float f = 2.0f * std::sin(juce::MathConstants<float>::pi * freq / (float)currentSampleRate);
    const float qInv = 1.0f / Q;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float in = data[i];
            state.low += f * state.band;
            state.high = in - state.low - qInv * state.band;
            state.band += f * state.high;
            
            if (mode < 0.33f)
                data[i] = state.low;
            else if (mode < 0.66f)
                data[i] = state.band;
            else
                data[i] = state.high;
        }
    }
}

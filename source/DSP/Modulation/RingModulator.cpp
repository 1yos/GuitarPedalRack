#include "RingModulator.h"

RingModulator::RingModulator() : AudioModule("RingModulator") {}
RingModulator::~RingModulator() {}

void RingModulator::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) ch.phase = 0.0f;
}

void RingModulator::reset()
{
    for (auto& ch : channelState) ch.phase = 0.0f;
}

void RingModulator::setFrequency(float freq) { freqParam.store(freq); }
void RingModulator::setMix(float mix) { mixParam.store(mix); }

void RingModulator::processInternal(AudioBuffer<float>& buffer)
{
    const float freq = freqParam.load() * 150.0f + 30.0f;
    const float mix = mixParam.load();
    const float phaseInc = freq / (float)currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float carrier = std::sin(state.phase * juce::MathConstants<float>::twoPi);
            float modulated = data[i] * carrier;
            data[i] = data[i] * (1.0f - mix) + modulated * mix;
            state.phase += phaseInc;
            if (state.phase >= 1.0f) state.phase -= 1.0f;
        }
    }
}

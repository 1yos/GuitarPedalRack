#include "RingModAnalog.h"

RingModAnalog::RingModAnalog() : AudioModule("RingModAnalog") {}
RingModAnalog::~RingModAnalog() {}

void RingModAnalog::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) ch.phase = 0.0f;
}

void RingModAnalog::reset()
{
    for (auto& ch : channelState) ch.phase = 0.0f;
}

void RingModAnalog::setFrequency(float freq) { freqParam.store(freq); }
void RingModAnalog::setMix(float mix) { mixParam.store(mix); }
void RingModAnalog::setWave(float wave) { waveParam.store(wave); }

void RingModAnalog::processInternal(AudioBuffer<float>& buffer)
{
    const float freq = freqParam.load() * 200.0f + 20.0f;
    const float mix = mixParam.load();
    const float wave = waveParam.load();
    const float phaseInc = freq / (float)currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float sine = std::sin(state.phase * juce::MathConstants<float>::twoPi);
            float square = state.phase < 0.5f ? 1.0f : -1.0f;
            float carrier = sine * (1.0f - wave) + square * wave;
            float modulated = data[i] * carrier;
            data[i] = data[i] * (1.0f - mix) + modulated * mix;
            state.phase += phaseInc;
            if (state.phase >= 1.0f) state.phase -= 1.0f;
        }
    }
}

#include "Phaser90.h"

Phaser90::Phaser90() : AudioModule("Phaser90") {}
Phaser90::~Phaser90() {}

void Phaser90::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.lfoPhase = 0.0f; for (auto& apf : ch.apf) apf.z1 = 0.0f; }
}

void Phaser90::reset()
{
    for (auto& ch : channelState) { for (auto& apf : ch.apf) apf.z1 = 0.0f; }
}

void Phaser90::setRate(float rate) { rateParam.store(rate); }

void Phaser90::processInternal(AudioBuffer<float>& buffer)
{
    const float rate = rateParam.load() * 1.0f;
    const float phaseInc = rate / (float)currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float lfo = std::sin(state.lfoPhase * juce::MathConstants<float>::twoPi);
            float freq = 500.0f + 1000.0f * (lfo + 1.0f) * 0.5f;
            float a = (float)std::tan(juce::MathConstants<float>::pi * freq / currentSampleRate);
            float coeff = (a - 1.0f) / (a + 1.0f);
            float in = data[i];
            float filtered = in;
            for (int s = 0; s < 4; ++s)
            {
                float y = coeff * filtered + state.apf[s].z1;
                state.apf[s].z1 = filtered - coeff * y;
                filtered = y;
            }
            data[i] = (in + filtered) * 0.5f;
            state.lfoPhase += phaseInc;
            if (state.lfoPhase >= 1.0f) state.lfoPhase -= 1.0f;
        }
    }
}

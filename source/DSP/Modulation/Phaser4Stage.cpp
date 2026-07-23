#include "Phaser4Stage.h"

Phaser4Stage::Phaser4Stage() : AudioModule("Phaser4Stage") {}
Phaser4Stage::~Phaser4Stage() {}

void Phaser4Stage::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.lfoPhase = 0.0f; for (auto& apf : ch.apf) apf.z1 = 0.0f; }
}

void Phaser4Stage::reset()
{
    for (auto& ch : channelState) { for (auto& apf : ch.apf) apf.z1 = 0.0f; }
}

void Phaser4Stage::setRate(float rate) { rateParam.store(rate); }
void Phaser4Stage::setDepth(float depth) { depthParam.store(depth); }
void Phaser4Stage::setFeedback(float feedback) { feedbackParam.store(feedback); }

void Phaser4Stage::processInternal(AudioBuffer<float>& buffer)
{
    const float rate = rateParam.load() * 0.8f;
    const float depth = depthParam.load();
    const float feedback = feedbackParam.load() * 0.7f;
    const float phaseInc = rate / (float)currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float lfo = std::sin(state.lfoPhase * juce::MathConstants<float>::twoPi);
            float freq = 200.0f + depth * 1500.0f * (lfo + 1.0f) * 0.5f;
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
            data[i] = in + filtered * feedback;
            state.lfoPhase += phaseInc;
            if (state.lfoPhase >= 1.0f) state.lfoPhase -= 1.0f;
        }
    }
}

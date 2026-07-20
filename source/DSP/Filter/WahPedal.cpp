#include "WahPedal.h"

WahPedal::WahPedal() : AudioModule("WahPedal") {}
WahPedal::~WahPedal() {}

void WahPedal::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
}

void WahPedal::reset()
{
    for (auto& ch : channelState) { ch.z1 = 0.0f; ch.z2 = 0.0f; }
}

void WahPedal::setPosition(float position) { positionParam.store(position); }
void WahPedal::setResonance(float resonance) { resonanceParam.store(resonance); }

void WahPedal::processInternal(AudioBuffer<float>& buffer)
{
    const float position = positionParam.load();
    const float freq = 400.0f + position * 2000.0f;
    const float Q = resonanceParam.load() * 10.0f + 2.0f;
    const float w0 = juce::MathConstants<float>::twoPi * freq / (float)currentSampleRate;
    const float alpha = std::sin(w0) / (2.0f * Q);
    const float b0 = alpha;
    const float b1 = 0.0f;
    const float b2 = -alpha;
    const float a0 = 1.0f + alpha;
    const float a1 = -2.0f * std::cos(w0);
    const float a2 = 1.0f - alpha;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float in = data[i];
            float out = (b0/a0) * in + (b1/a0) * state.z1 + (b2/a0) * state.z2
                      - (a1/a0) * state.z1 - (a2/a0) * state.z2;
            state.z2 = state.z1;
            state.z1 = in;
            data[i] = out;
        }
    }
}

#include "HighPassFilter.h"

HighPassFilter::HighPassFilter() : AudioModule("HighPassFilter") {}
HighPassFilter::~HighPassFilter() {}

void HighPassFilter::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
}

void HighPassFilter::reset()
{
    for (auto& ch : channelState) { ch.z1 = 0.0f; ch.z2 = 0.0f; }
}

void HighPassFilter::setFrequency(float frequency) { frequencyParam.store(frequency); }
void HighPassFilter::setResonance(float resonance) { resonanceParam.store(resonance); }
void HighPassFilter::setSlope(float slope) { slopeParam.store(slope); }

void HighPassFilter::processInternal(AudioBuffer<float>& buffer)
{
    const float freq = frequencyParam.load() * 8000.0f + 20.0f;
    const float Q = resonanceParam.load() * 8.0f + 0.707f;
    const float w0 = juce::MathConstants<float>::twoPi * freq / (float)currentSampleRate;
    const float alpha = std::sin(w0) / (2.0f * Q);
    const float cosW0 = std::cos(w0);
    const float b0 = (1.0f + cosW0) / 2.0f;
    const float b1 = -(1.0f + cosW0);
    const float b2 = (1.0f + cosW0) / 2.0f;
    const float a0 = 1.0f + alpha;
    const float a1 = -2.0f * cosW0;
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

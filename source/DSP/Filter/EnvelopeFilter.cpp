#include "EnvelopeFilter.h"

EnvelopeFilter::EnvelopeFilter() : AudioModule("EnvelopeFilter") {}
EnvelopeFilter::~EnvelopeFilter() {}

void EnvelopeFilter::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
}

void EnvelopeFilter::reset()
{
    for (auto& ch : channelState) { ch.envelope = 0.0f; ch.z1 = 0.0f; ch.z2 = 0.0f; }
}

void EnvelopeFilter::setSensitivity(float sensitivity) { sensitivityParam.store(sensitivity); }
void EnvelopeFilter::setAttack(float attack) { attackParam.store(attack); }
void EnvelopeFilter::setDecay(float decay) { decayParam.store(decay); }
void EnvelopeFilter::setResonance(float resonance) { resonanceParam.store(resonance); }

void EnvelopeFilter::processInternal(AudioBuffer<float>& buffer)
{
    const float sensitivity = sensitivityParam.load();
    const float attackCoeff = 1.0f - std::exp(-1.0f / (attackParam.load() * 0.1f * currentSampleRate + 1.0f));
    const float decayCoeff = 1.0f - std::exp(-1.0f / (decayParam.load() * 0.5f * currentSampleRate + 1.0f));
    const float Q = resonanceParam.load() * 12.0f + 1.0f;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float inputLevel = std::abs(data[i]);
            if (inputLevel > state.envelope)
                state.envelope += (inputLevel - state.envelope) * attackCoeff;
            else
                state.envelope += (inputLevel - state.envelope) * decayCoeff;
            
            float modFreq = 300.0f + state.envelope * sensitivity * 3000.0f;
            modFreq = juce::jlimit(200.0f, 5000.0f, modFreq);
            float w0 = juce::MathConstants<float>::twoPi * modFreq / (float)currentSampleRate;
            float alpha = std::sin(w0) / (2.0f * Q);
            float b0 = (1.0f - std::cos(w0)) / 2.0f;
            float b1 = 1.0f - std::cos(w0);
            float b2 = (1.0f - std::cos(w0)) / 2.0f;
            float a0 = 1.0f + alpha;
            float a1 = -2.0f * std::cos(w0);
            float a2 = 1.0f - alpha;
            float in = data[i];
            float out = (b0/a0) * in + (b1/a0) * state.z1 + (b2/a0) * state.z2
                      - (a1/a0) * state.z1 - (a2/a0) * state.z2;
            state.z2 = state.z1;
            state.z1 = in;
            data[i] = out;
        }
    }
}

#include "TremoloHarmonic.h"

TremoloHarmonic::TremoloHarmonic() : AudioModule("TremoloHarmonic") {}
TremoloHarmonic::~TremoloHarmonic() {}

void TremoloHarmonic::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) ch.lfoPhase = 0.0f;
}

void TremoloHarmonic::reset()
{
    for (auto& ch : channelState) ch.lfoPhase = 0.0f;
}

void TremoloHarmonic::setRate(float rate) { rateParam.store(rate); }
void TremoloHarmonic::setDepth(float depth) { depthParam.store(depth); }
void TremoloHarmonic::setHarmonic(float harmonic) { harmonicParam.store(harmonic); }

void TremoloHarmonic::processInternal(AudioBuffer<float>& buffer)
{
    const float rate = rateParam.load() * 10.0f + 0.5f;
    const float depth = depthParam.load();
    const float harmonic = harmonicParam.load();
    const float phaseInc = rate / (float)currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float lfo1 = std::sin(state.lfoPhase * juce::MathConstants<float>::twoPi);
            float lfo2 = std::sin(state.lfoPhase * 2.0f * juce::MathConstants<float>::twoPi);
            float mod = lfo1 * (1.0f - harmonic) + lfo2 * harmonic;
            float amp = 1.0f - depth * (1.0f - (mod + 1.0f) * 0.5f);
            data[i] *= amp;
            state.lfoPhase += phaseInc;
            if (state.lfoPhase >= 1.0f) state.lfoPhase -= 1.0f;
        }
    }
}

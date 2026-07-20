#include "TremoloOptical.h"

TremoloOptical::TremoloOptical() : AudioModule("TremoloOptical") {}
TremoloOptical::~TremoloOptical() {}

void TremoloOptical::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) ch.lfoPhase = 0.0f;
}

void TremoloOptical::reset()
{
    for (auto& ch : channelState) ch.lfoPhase = 0.0f;
}

void TremoloOptical::setRate(float rate) { rateParam.store(rate); }
void TremoloOptical::setDepth(float depth) { depthParam.store(depth); }

void TremoloOptical::processInternal(AudioBuffer<float>& buffer)
{
    const float rate = rateParam.load() * 9.0f + 0.5f;
    const float depth = depthParam.load();
    const float phaseInc = rate / (float)currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float lfo = std::sin(state.lfoPhase * juce::MathConstants<float>::twoPi);
            float smoothLfo = lfo * lfo * lfo;
            float amp = 1.0f - depth * (1.0f - (smoothLfo + 1.0f) * 0.5f);
            data[i] *= amp;
            state.lfoPhase += phaseInc;
            if (state.lfoPhase >= 1.0f) state.lfoPhase -= 1.0f;
        }
    }
}

#include "FlangerElectric.h"

FlangerElectric::FlangerElectric() : AudioModule("FlangerElectric") {}
FlangerElectric::~FlangerElectric() {}

void FlangerElectric::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState)
    {
        ch.delayBuf.resize(maxDelay, 0.0f);
        ch.writePos = 0;
        ch.lfoPhase = 0.0f;
    }
}

void FlangerElectric::reset()
{
    for (auto& ch : channelState)
    {
        std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f);
        ch.writePos = 0;
    }
}

void FlangerElectric::setRate(float rate) { rateParam.store(rate); }
void FlangerElectric::setDepth(float depth) { depthParam.store(depth); }
void FlangerElectric::setFeedback(float feedback) { feedbackParam.store(feedback); }
void FlangerElectric::setMix(float mix) { mixParam.store(mix); }

void FlangerElectric::processInternal(AudioBuffer<float>& buffer)
{
    const float rate = rateParam.load() * 2.0f;
    const float depth = depthParam.load() * 3.0f;
    const float feedback = feedbackParam.load() * 0.85f;
    const float mix = mixParam.load();
    const float phaseInc = rate / (float)currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float lfo = std::sin(state.lfoPhase * juce::MathConstants<float>::twoPi);
            float delayTime = 1.0f + depth * (lfo + 1.0f) * 0.5f;
            float readPos = state.writePos - delayTime;
            if (readPos < 0) readPos += maxDelay;
            int idx = (int)readPos;
            float frac = readPos - idx;
            float delayed = state.delayBuf[idx] * (1.0f - frac) + state.delayBuf[(idx + 1) % maxDelay] * frac;
            state.delayBuf[state.writePos] = data[i] + delayed * feedback;
            data[i] = data[i] * (1.0f - mix) + delayed * mix;
            state.writePos = (state.writePos + 1) % maxDelay;
            state.lfoPhase += phaseInc;
            if (state.lfoPhase >= 1.0f) state.lfoPhase -= 1.0f;
        }
    }
}

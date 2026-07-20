#include "AnalogDelay.h"

AnalogDelay::AnalogDelay() : AudioModule("AnalogDelay") {}
AnalogDelay::~AnalogDelay() {}

void AnalogDelay::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.delayBuf.resize(maxDelay, 0.0f); ch.writePos = 0; }
}

void AnalogDelay::reset()
{
    for (auto& ch : channelState) { std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f); ch.writePos = 0; }
}

void AnalogDelay::setTime(float time) { timeParam.store(time); }
void AnalogDelay::setFeedback(float feedback) { feedbackParam.store(feedback); }
void AnalogDelay::setModulation(float modulation) { modulationParam.store(modulation); }
void AnalogDelay::setMix(float mix) { mixParam.store(mix); }

void AnalogDelay::processInternal(AudioBuffer<float>& buffer)
{
    const float time = timeParam.load() * 1400.0f + 100.0f;
    const float feedback = feedbackParam.load() * 0.88f;
    const float modulation = modulationParam.load();
    const float mix = mixParam.load();
    const float delaySamples = (time / 1000.0f) * currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float lfo = std::sin(state.modPhase * juce::MathConstants<float>::twoPi);
            float modDelay = delaySamples * (1.0f + modulation * 0.008f * lfo);
            float readPos = state.writePos - modDelay;
            if (readPos < 0) readPos += maxDelay;
            int idx = (int)readPos;
            float frac = readPos - idx;
            float delayed = state.delayBuf[idx] * (1.0f - frac) + state.delayBuf[(idx + 1) % maxDelay] * frac;
            state.hpfZ1 = state.hpfZ1 + 0.002f * (delayed - state.hpfZ1);
            delayed = delayed - state.hpfZ1;
            state.lpfZ1 = state.lpfZ1 + 0.4f * (delayed - state.lpfZ1);
            state.delayBuf[state.writePos] = data[i] + state.lpfZ1 * feedback;
            data[i] = data[i] * (1.0f - mix) + state.lpfZ1 * mix;
            state.writePos = (state.writePos + 1) % maxDelay;
            state.modPhase += 0.8f / currentSampleRate;
            if (state.modPhase >= 1.0f) state.modPhase -= 1.0f;
        }
    }
}

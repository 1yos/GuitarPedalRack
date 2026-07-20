#include "SpaceEcho.h"

SpaceEcho::SpaceEcho() : AudioModule("SpaceEcho") {}
SpaceEcho::~SpaceEcho() {}

void SpaceEcho::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.delayBuf.resize(maxDelay, 0.0f); ch.writePos = 0; }
}

void SpaceEcho::reset()
{
    for (auto& ch : channelState) { std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f); ch.writePos = 0; }
}

void SpaceEcho::setTime(float time) { timeParam.store(time); }
void SpaceEcho::setFeedback(float feedback) { feedbackParam.store(feedback); }
void SpaceEcho::setWow(float wow) { wowParam.store(wow); }
void SpaceEcho::setMix(float mix) { mixParam.store(mix); }

void SpaceEcho::processInternal(AudioBuffer<float>& buffer)
{
    const float time = timeParam.load() * 1800.0f + 100.0f;
    const float feedback = feedbackParam.load() * 0.92f;
    const float wow = wowParam.load();
    const float mix = mixParam.load();
    const float delaySamples = (time / 1000.0f) * currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float wowLfo = std::sin(state.wowPhase * juce::MathConstants<float>::twoPi);
            float modDelay = delaySamples * (1.0f + wow * 0.012f * wowLfo);
            float readPos = state.writePos - modDelay;
            if (readPos < 0) readPos += maxDelay;
            int idx = (int)readPos;
            float frac = readPos - idx;
            float delayed = state.delayBuf[idx] * (1.0f - frac) + state.delayBuf[(idx + 1) % maxDelay] * frac;
            state.hpfZ1 = state.hpfZ1 + 0.004f * (delayed - state.hpfZ1);
            delayed = delayed - state.hpfZ1;
            state.lpfZ1 = state.lpfZ1 + 0.22f * (delayed - state.lpfZ1);
            state.delayBuf[state.writePos] = data[i] + state.lpfZ1 * feedback;
            data[i] = data[i] * (1.0f - mix) + state.lpfZ1 * mix;
            state.writePos = (state.writePos + 1) % maxDelay;
            state.wowPhase += 0.6f / currentSampleRate;
            if (state.wowPhase >= 1.0f) state.wowPhase -= 1.0f;
        }
    }
}

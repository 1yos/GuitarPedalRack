#include "DualDelay.h"

DualDelay::DualDelay() : AudioModule("DualDelay") {}
DualDelay::~DualDelay() {}

void DualDelay::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.delayBuf.resize(maxDelay, 0.0f); ch.writePos = 0; }
}

void DualDelay::reset()
{
    for (auto& ch : channelState) { std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f); ch.writePos = 0; }
}

void DualDelay::setTimeL(float timeL) { timeLParam.store(timeL); }
void DualDelay::setTimeR(float timeR) { timeRParam.store(timeR); }
void DualDelay::setFeedback(float feedback) { feedbackParam.store(feedback); }
void DualDelay::setMix(float mix) { mixParam.store(mix); }

void DualDelay::processInternal(AudioBuffer<float>& buffer)
{
    const float timeL = timeLParam.load() * 1400.0f + 100.0f;
    const float timeR = timeRParam.load() * 1400.0f + 100.0f;
    const float feedback = feedbackParam.load() * 0.85f;
    const float mix = mixParam.load();
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        const float time = (ch == 0) ? timeL : timeR;
        const float delaySamples = (time / 1000.0f) * currentSampleRate;
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float readPos = state.writePos - delaySamples;
            if (readPos < 0) readPos += maxDelay;
            int idx = (int)readPos;
            float frac = readPos - idx;
            float delayed = state.delayBuf[idx] * (1.0f - frac) + state.delayBuf[(idx + 1) % maxDelay] * frac;
            state.lpfZ1 = state.lpfZ1 + 0.5f * (delayed - state.lpfZ1);
            state.delayBuf[state.writePos] = data[i] + state.lpfZ1 * feedback;
            data[i] = data[i] * (1.0f - mix) + state.lpfZ1 * mix;
            state.writePos = (state.writePos + 1) % maxDelay;
        }
    }
}

#include "ReverseDelay.h"

ReverseDelay::ReverseDelay() : AudioModule("ReverseDelay") {}
ReverseDelay::~ReverseDelay() {}

void ReverseDelay::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.delayBuf.resize(maxDelay, 0.0f); ch.writePos = 0; }
}

void ReverseDelay::reset()
{
    for (auto& ch : channelState) { std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f); ch.writePos = 0; }
}

void ReverseDelay::setTime(float time) { timeParam.store(time); }
void ReverseDelay::setFeedback(float feedback) { feedbackParam.store(feedback); }
void ReverseDelay::setMix(float mix) { mixParam.store(mix); }

void ReverseDelay::processInternal(AudioBuffer<float>& buffer)
{
    const float time = timeParam.load() * 1400.0f + 100.0f;
    const float feedback = feedbackParam.load() * 0.75f;
    const float mix = mixParam.load();
    const float delaySamples = (time / 1000.0f) * currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float readPos = state.writePos + delaySamples;
            while (readPos >= maxDelay) readPos -= maxDelay;
            int idx = (int)readPos;
            float delayed = state.delayBuf[idx];
            state.delayBuf[state.writePos] = data[i] + delayed * feedback;
            data[i] = data[i] * (1.0f - mix) + delayed * mix;
            state.writePos = (state.writePos + 1) % maxDelay;
        }
    }
}

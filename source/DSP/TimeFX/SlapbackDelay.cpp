#include "SlapbackDelay.h"

SlapbackDelay::SlapbackDelay() : AudioModule("SlapbackDelay") {}
SlapbackDelay::~SlapbackDelay() {}

void SlapbackDelay::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.delayBuf.resize(maxDelay, 0.0f); ch.writePos = 0; }
}

void SlapbackDelay::reset()
{
    for (auto& ch : channelState) { std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f); ch.writePos = 0; }
}

void SlapbackDelay::setTime(float time) { timeParam.store(time); }
void SlapbackDelay::setMix(float mix) { mixParam.store(mix); }

void SlapbackDelay::processInternal(AudioBuffer<float>& buffer)
{
    const float time = timeParam.load() * 130.0f + 30.0f;
    const float mix = mixParam.load();
    const float delaySamples = (time / 1000.0f) * currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float readPos = state.writePos - delaySamples;
            if (readPos < 0) readPos += maxDelay;
            int idx = (int)readPos;
            float delayed = state.delayBuf[idx];
            state.delayBuf[state.writePos] = data[i];
            data[i] = data[i] * (1.0f - mix) + delayed * mix;
            state.writePos = (state.writePos + 1) % maxDelay;
        }
    }
}

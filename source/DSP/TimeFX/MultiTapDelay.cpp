#include "MultiTapDelay.h"

MultiTapDelay::MultiTapDelay() : AudioModule("MultiTapDelay") {}
MultiTapDelay::~MultiTapDelay() {}

void MultiTapDelay::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.delayBuf.resize(maxDelay, 0.0f); ch.writePos = 0; }
}

void MultiTapDelay::reset()
{
    for (auto& ch : channelState) { std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f); ch.writePos = 0; }
}

void MultiTapDelay::setTime(float time) { timeParam.store(time); }
void MultiTapDelay::setFeedback(float feedback) { feedbackParam.store(feedback); }
void MultiTapDelay::setTaps(float taps) { tapsParam.store(taps); }
void MultiTapDelay::setMix(float mix) { mixParam.store(mix); }

void MultiTapDelay::processInternal(AudioBuffer<float>& buffer)
{
    const float time = timeParam.load() * 1400.0f + 100.0f;
    const float feedback = feedbackParam.load() * 0.7f;
    const float tapsVal = tapsParam.load();
    const float mix = mixParam.load();
    const float baseDelay = (time / 1000.0f) * currentSampleRate;
    const int numTaps = 3 + (int)(tapsVal * 5.0f);
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float wet = 0.0f;
            for (int tap = 0; tap < numTaps; ++tap)
            {
                float tapDelay = baseDelay * (tap + 1) / numTaps;
                float readPos = state.writePos - tapDelay;
                if (readPos < 0) readPos += maxDelay;
                int idx = (int)readPos;
                float tapGain = 1.0f / (tap + 1);
                wet += state.delayBuf[idx] * tapGain;
            }
            wet /= numTaps;
            state.delayBuf[state.writePos] = data[i] + wet * feedback;
            data[i] = data[i] * (1.0f - mix) + wet * mix;
            state.writePos = (state.writePos + 1) % maxDelay;
        }
    }
}

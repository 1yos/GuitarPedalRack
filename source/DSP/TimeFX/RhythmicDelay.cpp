#include "RhythmicDelay.h"

RhythmicDelay::RhythmicDelay() : AudioModule("RhythmicDelay") {}
RhythmicDelay::~RhythmicDelay() {}

void RhythmicDelay::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.delayBuf.resize(maxDelay, 0.0f); ch.writePos = 0; }
}

void RhythmicDelay::reset()
{
    for (auto& ch : channelState) { std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f); ch.writePos = 0; }
}

void RhythmicDelay::setTime(float time) { timeParam.store(time); }
void RhythmicDelay::setPattern(float pattern) { patternParam.store(pattern); }
void RhythmicDelay::setFeedback(float feedback) { feedbackParam.store(feedback); }
void RhythmicDelay::setMix(float mix) { mixParam.store(mix); }

void RhythmicDelay::processInternal(AudioBuffer<float>& buffer)
{
    const float time = timeParam.load() * 900.0f + 100.0f;
    const float pattern = patternParam.load();
    const float feedback = feedbackParam.load() * 0.7f;
    const float mix = mixParam.load();
    const float delaySamples = (time / 1000.0f) * currentSampleRate;
    const int stepSize = (int)(delaySamples / 8);
    const int patternIdx = (int)(pattern * 7.99f);
    const int patterns[8] = {0xAA, 0xCC, 0xF0, 0xDB, 0x96, 0xE4, 0xBA, 0xFF};
    const int currentPattern = patterns[patternIdx];
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            int step = (state.stepCounter / stepSize) % 8;
            bool tapActive = (currentPattern & (1 << step)) != 0;
            float readPos = state.writePos - delaySamples;
            if (readPos < 0) readPos += maxDelay;
            int idx = (int)readPos;
            float delayed = state.delayBuf[idx] * (tapActive ? 1.0f : 0.3f);
            state.delayBuf[state.writePos] = data[i] + delayed * feedback;
            data[i] = data[i] * (1.0f - mix) + delayed * mix;
            state.writePos = (state.writePos + 1) % maxDelay;
            state.stepCounter++;
        }
    }
}

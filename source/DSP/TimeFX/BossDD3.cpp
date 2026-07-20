#include "BossDD3.h"

BossDD3::BossDD3() : AudioModule("BossDD3") {}
BossDD3::~BossDD3() {}

void BossDD3::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.delayBuf.resize(maxDelay, 0.0f); ch.writePos = 0; }
}

void BossDD3::reset()
{
    for (auto& ch : channelState) { std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f); ch.writePos = 0; }
}

void BossDD3::setTime(float time) { timeParam.store(time); }
void BossDD3::setFeedback(float feedback) { feedbackParam.store(feedback); }
void BossDD3::setLevel(float level) { levelParam.store(level); }
void BossDD3::setMode(float mode) { modeParam.store(mode); }

void BossDD3::processInternal(AudioBuffer<float>& buffer)
{
    const float time = timeParam.load() * 1900.0f + 50.0f;
    const float feedback = feedbackParam.load() * 0.85f;
    const float level = levelParam.load();
    const float mode = modeParam.load();
    const float delaySamples = (time / 1000.0f) * currentSampleRate;
    const bool holdMode = mode > 0.66f;
    const bool analogMode = mode > 0.33f && mode <= 0.66f;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float modDelay = delaySamples;
            if (analogMode)
            {
                float lfo = std::sin(state.modPhase * juce::MathConstants<float>::twoPi);
                modDelay = delaySamples * (1.0f + 0.005f * lfo);
                state.modPhase += 0.5f / currentSampleRate;
                if (state.modPhase >= 1.0f) state.modPhase -= 1.0f;
            }
            float readPos = state.writePos - modDelay;
            if (readPos < 0) readPos += maxDelay;
            int idx = (int)readPos;
            float delayed = state.delayBuf[idx];
            if (!holdMode)
            {
                state.delayBuf[state.writePos] = data[i] + delayed * feedback;
            }
            data[i] = data[i] + delayed * level;
            state.writePos = (state.writePos + 1) % maxDelay;
        }
    }
}

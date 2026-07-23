#include "MemoryMan.h"

MemoryMan::MemoryMan() : AudioModule("MemoryMan") {}
MemoryMan::~MemoryMan() {}

void MemoryMan::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.delayBuf.resize(maxDelay, 0.0f); ch.writePos = 0; }
}

void MemoryMan::reset()
{
    for (auto& ch : channelState) { std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f); ch.writePos = 0; }
}

void MemoryMan::setTime(float time) { timeParam.store(time); }
void MemoryMan::setFeedback(float feedback) { feedbackParam.store(feedback); }
void MemoryMan::setModulation(float modulation) { modulationParam.store(modulation); }
void MemoryMan::setBlend(float blend) { blendParam.store(blend); }

void MemoryMan::processInternal(AudioBuffer<float>& buffer)
{
    const float time = timeParam.load() * 1400.0f + 100.0f;
    const float feedback = feedbackParam.load() * 0.90f;
    const float modulation = modulationParam.load();
    const float blend = blendParam.load();
    const float delaySamples = (time / 1000.0f) * currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float lfo = std::sin(state.modPhase * juce::MathConstants<float>::twoPi);
            float modDelay = delaySamples * (1.0f + modulation * 0.015f * lfo);
            float readPos = state.writePos - modDelay;
            if (readPos < 0) readPos += maxDelay;
            int idx = (int)readPos;
            float frac = readPos - idx;
            float delayed = state.delayBuf[idx] * (1.0f - frac) + state.delayBuf[(idx + 1) % maxDelay] * frac;
            state.lpfZ1 = state.lpfZ1 + 0.35f * (delayed - state.lpfZ1);
            state.delayBuf[state.writePos] = data[i] + state.lpfZ1 * feedback;
            data[i] = data[i] * (1.0f - blend) + state.lpfZ1 * blend;
            state.writePos = (state.writePos + 1) % maxDelay;
            state.modPhase += 1.2f / currentSampleRate;
            if (state.modPhase >= 1.0f) state.modPhase -= 1.0f;
        }
    }
}

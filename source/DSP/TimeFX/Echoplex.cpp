#include "Echoplex.h"

Echoplex::Echoplex() : AudioModule("Echoplex") {}
Echoplex::~Echoplex() {}

void Echoplex::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.delayBuf.resize(maxDelay, 0.0f); ch.writePos = 0; }
}

void Echoplex::reset()
{
    for (auto& ch : channelState) { std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f); ch.writePos = 0; }
}

void Echoplex::setTime(float time) { timeParam.store(time); }
void Echoplex::setFeedback(float feedback) { feedbackParam.store(feedback); }
void Echoplex::setVolume(float volume) { volumeParam.store(volume); }

void Echoplex::processInternal(AudioBuffer<float>& buffer)
{
    const float time = timeParam.load() * 1800.0f + 150.0f;
    const float feedback = feedbackParam.load() * 0.95f;
    const float volume = volumeParam.load();
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
            state.hpfZ1 = state.hpfZ1 + 0.003f * (delayed - state.hpfZ1);
            delayed = delayed - state.hpfZ1;
            state.lpfZ1 = state.lpfZ1 + 0.25f * (delayed - state.lpfZ1);
            state.delayBuf[state.writePos] = data[i] + state.lpfZ1 * feedback;
            data[i] = data[i] + state.lpfZ1 * volume;
            state.writePos = (state.writePos + 1) % maxDelay;
        }
    }
}

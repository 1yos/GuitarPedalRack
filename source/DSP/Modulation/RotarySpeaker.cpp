#include "RotarySpeaker.h"

RotarySpeaker::RotarySpeaker() : AudioModule("RotarySpeaker") {}
RotarySpeaker::~RotarySpeaker() {}

void RotarySpeaker::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState)
    {
        ch.delayBuf.resize(maxDelay, 0.0f);
        ch.writePos = 0;
        ch.hornPhase = 0.0f;
        ch.bassPhase = 0.0f;
    }
}

void RotarySpeaker::reset()
{
    for (auto& ch : channelState)
    {
        std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f);
        ch.writePos = 0;
    }
}

void RotarySpeaker::setSpeed(float speed) { speedParam.store(speed); }
void RotarySpeaker::setSeparation(float separation) { separationParam.store(separation); }

void RotarySpeaker::processInternal(AudioBuffer<float>& buffer)
{
    const float speed = speedParam.load();
    const float separation = separationParam.load();
    const float hornRate = 6.0f * speed + 0.5f;
    const float bassRate = 0.8f * speed + 0.1f;
    const float hornInc = hornRate / (float)currentSampleRate;
    const float bassInc = bassRate / (float)currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            state.delayBuf[state.writePos] = data[i];
            float hornLfo = std::sin(state.hornPhase * juce::MathConstants<float>::twoPi);
            float bassLfo = std::sin(state.bassPhase * juce::MathConstants<float>::twoPi);
            float hornDelay = 3.0f + separation * 4.0f * (hornLfo + 1.0f) * 0.5f;
            float bassDelay = 5.0f + separation * 8.0f * (bassLfo + 1.0f) * 0.5f;
            float avgDelay = (hornDelay + bassDelay) * 0.5f;
            float readPos = state.writePos - avgDelay;
            if (readPos < 0) readPos += maxDelay;
            int idx = (int)readPos;
            float frac = readPos - idx;
            data[i] = state.delayBuf[idx] * (1.0f - frac) + state.delayBuf[(idx + 1) % maxDelay] * frac;
            data[i] *= 0.5f + 0.5f * (hornLfo + bassLfo) * 0.25f;
            state.writePos = (state.writePos + 1) % maxDelay;
            state.hornPhase += hornInc;
            state.bassPhase += bassInc;
            if (state.hornPhase >= 1.0f) state.hornPhase -= 1.0f;
            if (state.bassPhase >= 1.0f) state.bassPhase -= 1.0f;
        }
    }
}

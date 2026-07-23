#include "VibratoClassic.h"

VibratoClassic::VibratoClassic() : AudioModule("VibratoClassic") {}
VibratoClassic::~VibratoClassic() {}

void VibratoClassic::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState)
    {
        ch.delayBuf.resize(maxDelay, 0.0f);
        ch.writePos = 0;
        ch.lfoPhase = 0.0f;
    }
}

void VibratoClassic::reset()
{
    for (auto& ch : channelState)
    {
        std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f);
        ch.writePos = 0;
    }
}

void VibratoClassic::setRate(float rate) { rateParam.store(rate); }
void VibratoClassic::setDepth(float depth) { depthParam.store(depth); }

void VibratoClassic::processInternal(AudioBuffer<float>& buffer)
{
    const float rate = rateParam.load() * 8.0f + 1.0f;
    const float depth = depthParam.load() * 8.0f;
    const float phaseInc = rate / (float)currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            state.delayBuf[state.writePos] = data[i];
            float lfo = std::sin(state.lfoPhase * juce::MathConstants<float>::twoPi);
            float delayTime = 5.0f + depth * (lfo + 1.0f) * 0.5f;
            float readPos = state.writePos - delayTime;
            if (readPos < 0) readPos += maxDelay;
            int idx = (int)readPos;
            float frac = readPos - idx;
            data[i] = state.delayBuf[idx] * (1.0f - frac) + state.delayBuf[(idx + 1) % maxDelay] * frac;
            state.writePos = (state.writePos + 1) % maxDelay;
            state.lfoPhase += phaseInc;
            if (state.lfoPhase >= 1.0f) state.lfoPhase -= 1.0f;
        }
    }
}

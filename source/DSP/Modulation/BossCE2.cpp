#include "BossCE2.h"

BossCE2::BossCE2() : AudioModule("Boss CE-2") {}
BossCE2::~BossCE2() {}

void BossCE2::prepare(double r, int b)
{
    AudioModule::prepare(r, b);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.delayBuf.resize(maxDelay, 0.0f); ch.writePos = 0; }
}

void BossCE2::processInternal(AudioBuffer<float>& buffer)
{
    float rate = rateParam.load() * 5.0f + 0.3f;
    float depth = depthParam.load() * 4.0f + 1.0f;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* d = buffer.getWritePointer(ch);
        auto& s = channelState[ch < channelState.size() ? ch : 0];
        float lfoInc = rate / static_cast<float>(currentSampleRate);
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            s.delayBuf[s.writePos] = d[i];
            float lfo = std::sin(s.lfoPhase * MathConstants<float>::twoPi);
            float delayTime = 300.0f + lfo * depth * static_cast<float>(currentSampleRate) / 1000.0f;
            float readPos = s.writePos - delayTime;
            while (readPos < 0) readPos += maxDelay;
            int p1 = static_cast<int>(readPos) % maxDelay;
            int p2 = (p1 + 1) % maxDelay;
            float frac = readPos - std::floor(readPos);
            float delayed = s.delayBuf[p1] * (1.0f - frac) + s.delayBuf[p2] * frac;
            d[i] = (d[i] + delayed) * 0.5f;  // 50/50 mix (CE-2 style)
            s.lfoPhase += lfoInc;
            if (s.lfoPhase >= 1.0f) s.lfoPhase -= 1.0f;
            s.writePos = (s.writePos + 1) % maxDelay;
        }
    }
}

void BossCE2::reset() { for (auto& ch : channelState) std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f); }
void BossCE2::setRate(float r) { rateParam.store(jlimit(0.0f, 1.0f, r)); }
void BossCE2::setDepth(float d) { depthParam.store(jlimit(0.0f, 1.0f, d)); }

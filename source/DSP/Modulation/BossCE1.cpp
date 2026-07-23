#include "BossCE1.h"

BossCE1::BossCE1() : AudioModule("Boss CE-1") {}
BossCE1::~BossCE1() {}

void BossCE1::prepare(double r, int b)
{
    AudioModule::prepare(r, b);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.delayBuf.resize(maxDelay, 0.0f); }
}

void BossCE1::processInternal(AudioBuffer<float>& buffer)
{
    float rate = rateParam.load() * 4.0f + 0.2f;
    float depth = depthParam.load() * 6.0f + 1.5f;
    float mode = modeParam.load();  // 0=chorus, 1=vibrato
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* d = buffer.getWritePointer(ch);
        auto& s = channelState[ch < channelState.size() ? ch : 0];
        float lfoInc = rate / static_cast<float>(currentSampleRate);
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float input = d[i];
            s.delayBuf[s.writePos] = input;
            float lfo = std::sin(s.lfoPhase * MathConstants<float>::twoPi);
            float delayTime = 400.0f + lfo * depth * static_cast<float>(currentSampleRate) / 1000.0f;
            float rp = s.writePos - delayTime;
            while (rp < 0) rp += maxDelay;
            int p1 = static_cast<int>(rp) % maxDelay, p2 = (p1 + 1) % maxDelay;
            float frac = rp - std::floor(rp);
            float delayed = s.delayBuf[p1] * (1.0f - frac) + s.delayBuf[p2] * frac;
            
            // Mode blending: chorus (50/50 mix) to vibrato (100% modulated)
            d[i] = input * (1.0f - mode * 0.5f) + delayed * (0.5f + mode * 0.5f);
            
            s.lfoPhase += lfoInc;
            if (s.lfoPhase >= 1.0f) s.lfoPhase -= 1.0f;
            s.writePos = (s.writePos + 1) % maxDelay;
        }
    }
}

void BossCE1::reset() { for (auto& ch : channelState) std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f); }
void BossCE1::setRate(float r) { rateParam.store(jlimit(0.0f, 1.0f, r)); }
void BossCE1::setDepth(float d) { depthParam.store(jlimit(0.0f, 1.0f, d)); }
void BossCE1::setMode(float m) { modeParam.store(jlimit(0.0f, 1.0f, m)); }

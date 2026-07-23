#include "ChorusDimension.h"

ChorusDimension::ChorusDimension() : AudioModule("Chorus Dimension") {}
ChorusDimension::~ChorusDimension() {}

void ChorusDimension::prepare(double r, int b)
{
    AudioModule::prepare(r, b);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.delayBuf.resize(maxDelay, 0.0f); }
}

void ChorusDimension::processInternal(AudioBuffer<float>& buffer)
{
    float intensity = intensityParam.load();
    float depth = intensity * 3.0f + 1.0f;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* d = buffer.getWritePointer(ch);
        auto& s = channelState[ch < channelState.size() ? ch : 0];
        float rate1 = 0.7f / static_cast<float>(currentSampleRate);
        float rate2 = 1.1f / static_cast<float>(currentSampleRate);
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            s.delayBuf[s.writePos] = d[i];
            float lfo1Val = std::sin(s.lfo1 * MathConstants<float>::twoPi);
            float lfo2Val = std::sin(s.lfo2 * MathConstants<float>::twoPi);
            float delay1 = 250.0f + lfo1Val * depth * static_cast<float>(currentSampleRate) / 1000.0f;
            float delay2 = 280.0f + lfo2Val * depth * static_cast<float>(currentSampleRate) / 1000.0f;
            
            auto read = [&](float dly) -> float {
                float rp = s.writePos - dly;
                while (rp < 0) rp += maxDelay;
                int p1 = static_cast<int>(rp) % maxDelay, p2 = (p1 + 1) % maxDelay;
                float frac = rp - std::floor(rp);
                return s.delayBuf[p1] * (1.0f - frac) + s.delayBuf[p2] * frac;
            };
            
            float chorus = (read(delay1) + read(delay2)) * 0.5f;
            d[i] = (d[i] * 0.6f + chorus * 0.4f) * (1.0f + intensity * 0.3f);  // Dimension boost
            s.lfo1 += rate1; s.lfo2 += rate2;
            if (s.lfo1 >= 1.0f) s.lfo1 -= 1.0f;
            if (s.lfo2 >= 1.0f) s.lfo2 -= 1.0f;
            s.writePos = (s.writePos + 1) % maxDelay;
        }
    }
}

void ChorusDimension::reset() { for (auto& ch : channelState) std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f); }
void ChorusDimension::setIntensity(float i) { intensityParam.store(jlimit(0.0f, 1.0f, i)); }

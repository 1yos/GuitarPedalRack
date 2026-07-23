#include "BigMuffRussian.h"

BigMuffRussian::BigMuffRussian() : AudioModule("Big Muff Russian") {}
BigMuffRussian::~BigMuffRussian() {}
void BigMuffRussian::prepare(double r, int b) { AudioModule::prepare(r, b); channelState.resize(2); reset(); }

void BigMuffRussian::processInternal(AudioBuffer<float>& buffer)
{
    float sus = sustainParam.load();
    float tone = toneParam.load();
    float vol = volumeParam.load();
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* d = buffer.getWritePointer(ch);
        auto& s = channelState[ch < channelState.size() ? ch : 0];
        float g1 = 1.0f + sus * 29.0f;  // Russian: warmer, less gain per stage
        float g2 = 3.0f + sus * 22.0f;
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float x = d[i];
            float hp = x - s.hp_z1 + 0.995f * s.hp_z1; s.hp_z1 = hp; x = hp * g1;
            x = SIMDProcessor::fastTanh(x * 1.3f);  // Warmer clipping
            float o1 = 2.0f * MathConstants<float>::pi * 450.0f / static_cast<float>(currentSampleRate);
            x = s.s1_z1 + (1.0f - std::exp(-o1)) * (x - s.s1_z1); s.s1_z1 = x; x *= g2;
            x = SIMDProcessor::fastTanh(x * 1.3f);
            float o2 = 2.0f * MathConstants<float>::pi * 750.0f / static_cast<float>(currentSampleRate);
            x = s.s2_z1 + (1.0f - std::exp(-o2)) * (x - s.s2_z1); s.s2_z1 = x;
            float ob = 2.0f * MathConstants<float>::pi * 150.0f / static_cast<float>(currentSampleRate);  // Lower bass
            float bass = s.bass_z1 + (1.0f - std::exp(-ob)) * (x - s.bass_z1); s.bass_z1 = bass;
            float treb = x - s.treb_z1 + 0.99f * s.treb_z1; s.treb_z1 = treb;
            x = bass * (1.0f - tone) + treb * tone + x * 0.15f;  // Less scoop
            x *= vol * 3.2f;
            hp = x - s.hpOut_z1 + 0.995f * s.hpOut_z1; s.hpOut_z1 = hp;
            d[i] = SIMDProcessor::fastTanh(hp * 0.65f);
        }
    }
}

void BigMuffRussian::reset() { for (auto& s : channelState) { s.hp_z1 = 0; s.s1_z1 = 0; s.s2_z1 = 0; s.bass_z1 = 0; s.treb_z1 = 0; s.hpOut_z1 = 0; } }
void BigMuffRussian::setSustain(float s) { sustainParam.store(jlimit(0.0f, 1.0f, s)); }
void BigMuffRussian::setTone(float t) { toneParam.store(jlimit(0.0f, 1.0f, t)); }
void BigMuffRussian::setVolume(float v) { volumeParam.store(jlimit(0.0f, 1.0f, v)); }

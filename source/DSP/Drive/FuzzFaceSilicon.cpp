#include "FuzzFaceSilicon.h"

FuzzFaceSilicon::FuzzFaceSilicon() : AudioModule("Fuzz Face Silicon") {}
FuzzFaceSilicon::~FuzzFaceSilicon() {}
void FuzzFaceSilicon::prepare(double r, int b) { AudioModule::prepare(r, b); channelState.resize(2); reset(); }

void FuzzFaceSilicon::processInternal(AudioBuffer<float>& buffer)
{
    float fuzz = fuzzParam.load();
    float vol = volumeParam.load();
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* d = buffer.getWritePointer(ch);
        auto& s = channelState[ch < channelState.size() ? ch : 0];
        float g1 = 6.0f + fuzz * 29.0f;  // Silicon: more gain, harsher
        float g2 = 3.5f + fuzz * 21.0f;
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float x = d[i];
            float hp = x - s.hp_z1 + 0.995f * s.hp_z1; s.hp_z1 = hp; x = hp * g1;
            // Silicon: harder threshold, more aggressive
            float t1 = 0.35f; if (x > t1) x = t1 + (x - t1) / (1.0f + (x - t1) * 1.5f); else if (x < -t1 * 0.85f) x = -(t1 * 0.85f + (-x - t1 * 0.85f) / (1.0f + (-x - t1 * 0.85f) * 1.5f));
            float o1 = 2.0f * MathConstants<float>::pi * 2500.0f / static_cast<float>(currentSampleRate);  // Brighter
            x = s.q1_z1 + (1.0f - std::exp(-o1)) * (x - s.q1_z1); s.q1_z1 = x; x *= g2;
            float t2 = 0.5f; if (x > t2) x = t2 + (x - t2) / (1.0f + (x - t2) * 2.0f); else if (x < -t2 * 0.8f) x = -(t2 * 0.8f + (-x - t2 * 0.8f) / (1.0f + (-x - t2 * 0.8f) * 2.0f));
            float o2 = 2.0f * MathConstants<float>::pi * 3500.0f / static_cast<float>(currentSampleRate);
            x = s.q2_z1 + (1.0f - std::exp(-o2)) * (x - s.q2_z1); s.q2_z1 = x; x *= vol * 3.8f;
            hp = x - s.hpOut_z1 + 0.995f * s.hpOut_z1; s.hpOut_z1 = hp;
            d[i] = SIMDProcessor::fastTanh(hp * 0.75f);
        }
    }
}

void FuzzFaceSilicon::reset() { for (auto& s : channelState) { s.hp_z1 = 0; s.q1_z1 = 0; s.q2_z1 = 0; s.hpOut_z1 = 0; } }
void FuzzFaceSilicon::setFuzz(float f) { fuzzParam.store(jlimit(0.0f, 1.0f, f)); }
void FuzzFaceSilicon::setVolume(float v) { volumeParam.store(jlimit(0.0f, 1.0f, v)); }

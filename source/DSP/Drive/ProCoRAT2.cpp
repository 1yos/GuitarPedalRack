#include "ProCoRAT2.h"

ProCoRAT2::ProCoRAT2() : AudioModule("ProCo RAT 2") {}
ProCoRAT2::~ProCoRAT2() {}
void ProCoRAT2::prepare(double r, int b) { AudioModule::prepare(r, b); channelState.resize(2); reset(); }

void ProCoRAT2::processInternal(AudioBuffer<float>& buffer)
{
    float dist = distortionParam.load();
    float filt = filterParam.load();
    float vol = volumeParam.load();
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* d = buffer.getWritePointer(ch);
        auto& s = channelState[ch < channelState.size() ? ch : 0];
        float g = 15.0f + dist * 145.0f;  // RAT2: slightly more gain than RAT1
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float x = d[i];
            float hp = x - s.hpIn_z1 + hpCoeff * s.hpIn_z1; s.hpIn_z1 = hp; x = hp;
            float preOmega = 2.0f * MathConstants<float>::pi * (1000.0f + filt * 2000.0f) / static_cast<float>(currentSampleRate);
            x = s.preLp_z1 + (1.0f - std::exp(-preOmega)) * (x - s.preLp_z1); s.preLp_z1 = x;
            x *= g;
            float t = 0.55f; if (x > t) x = t; else if (x < -t) x = -t;  // Hard clip
            float tOmega = 2.0f * MathConstants<float>::pi * (300.0f + filt * 4700.0f) / static_cast<float>(currentSampleRate);
            x = s.tone_z1 + (1.0f - std::exp(-tOmega)) * (x - s.tone_z1); s.tone_z1 = x;
            x *= vol * 2.2f;
            hp = x - s.hpOut_z1 + hpCoeff * s.hpOut_z1; s.hpOut_z1 = hp;
            d[i] = SIMDProcessor::fastTanh(hp * 0.8f);
        }
    }
}

void ProCoRAT2::reset() { for (auto& s : channelState) { s.hpIn_z1 = 0; s.preLp_z1 = 0; s.tone_z1 = 0; s.hpOut_z1 = 0; } }
void ProCoRAT2::setDistortion(float d) { distortionParam.store(jlimit(0.0f, 1.0f, d)); }
void ProCoRAT2::setFilter(float f) { filterParam.store(jlimit(0.0f, 1.0f, f)); }
void ProCoRAT2::setVolume(float v) { volumeParam.store(jlimit(0.0f, 1.0f, v)); }

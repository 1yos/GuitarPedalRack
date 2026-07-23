#include "TurboRAT.h"

TurboRAT::TurboRAT() : AudioModule("Turbo RAT") {}
TurboRAT::~TurboRAT() {}
void TurboRAT::prepare(double r, int b) { AudioModule::prepare(r, b); channelState.resize(2); reset(); }

void TurboRAT::processInternal(AudioBuffer<float>& buffer)
{
    float dist = distortionParam.load();
    float tone = toneParam.load();
    float vol = volumeParam.load();
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* d = buffer.getWritePointer(ch);
        auto& s = channelState[ch < channelState.size() ? ch : 0];
        float g = 20.0f + dist * 200.0f;  // Turbo: even more gain
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float x = d[i];
            float hp = x - s.hp_z1 + 0.995f * s.hp_z1; s.hp_z1 = hp; x = hp * g;
            // LED clipping (Turbo RAT uses LEDs instead of diodes - different sound)
            float t = 0.7f; if (x > t) x = t + (x - t) * 0.15f; else if (x < -t) x = -t + (x + t) * 0.15f;
            float omega = 2.0f * MathConstants<float>::pi * (400.0f + tone * 4600.0f) / static_cast<float>(currentSampleRate);
            x = s.tone_z1 + (1.0f - std::exp(-omega)) * (x - s.tone_z1); s.tone_z1 = x; x *= vol * 2.5f;
            hp = x - s.hpOut_z1 + 0.995f * s.hpOut_z1; s.hpOut_z1 = hp;
            d[i] = SIMDProcessor::fastTanh(hp * 0.75f);
        }
    }
}

void TurboRAT::reset() { for (auto& s : channelState) { s.hp_z1 = 0; s.tone_z1 = 0; s.hpOut_z1 = 0; } }
void TurboRAT::setDistortion(float d) { distortionParam.store(jlimit(0.0f, 1.0f, d)); }
void TurboRAT::setTone(float t) { toneParam.store(jlimit(0.0f, 1.0f, t)); }
void TurboRAT::setVolume(float v) { volumeParam.store(jlimit(0.0f, 1.0f, v)); }

#include "TimmyOverdrive.h"

TimmyOverdrive::TimmyOverdrive() : AudioModule("Timmy Overdrive") {}
TimmyOverdrive::~TimmyOverdrive() {}
void TimmyOverdrive::prepare(double r, int b) { AudioModule::prepare(r, b); channelState.resize(2); reset(); }

void TimmyOverdrive::processInternal(AudioBuffer<float>& buffer)
{
    float gain = gainParam.load();
    float bass = bassParam.load();
    float treble = trebleParam.load();
    float vol = volumeParam.load();
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* d = buffer.getWritePointer(ch);
        auto& s = channelState[ch < channelState.size() ? ch : 0];
        float g = 1.0f + gain * 35.0f;  // Timmy: transparent, moderate gain
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float x = d[i];
            float clean = x;  // Store clean
            float hp = x - s.hp_z1 + 0.995f * s.hp_z1; s.hp_z1 = hp; x = hp;
            // Bass cut/boost before clipping
            float bassOmega = 2.0f * MathConstants<float>::pi * 100.0f / static_cast<float>(currentSampleRate);
            float bassLP = s.bass_z1 + (1.0f - std::exp(-bassOmega)) * (x - s.bass_z1); s.bass_z1 = bassLP;
            x = x + (bassLP - x) * (bass - 0.5f) * 2.0f;
            x *= g;
            // Soft asymmetric clipping (transparent)
            float t = 0.65f; if (x > t) x = t + (x - t) / (1.0f + (x - t)); else if (x < -t * 0.9f) x = -(t * 0.9f + (-x - t * 0.9f) / (1.0f + (-x - t * 0.9f)));
            // Treble cut/boost after clipping
            float trebHP = x - s.treb_z1 + 0.97f * s.treb_z1; s.treb_z1 = trebHP;
            x = x + trebHP * (treble - 0.5f) * 2.5f;
            // Blend with clean (Timmy transparency)
            x = clean * (1.0f - gain * 0.5f) + x * (0.5f + gain * 0.5f);
            x *= vol * 1.8f;
            hp = x - s.hpOut_z1 + 0.995f * s.hpOut_z1; s.hpOut_z1 = hp;
            d[i] = SIMDProcessor::fastTanh(hp * 0.95f);
        }
    }
}

void TimmyOverdrive::reset() { for (auto& s : channelState) { s.hp_z1 = 0; s.bass_z1 = 0; s.treb_z1 = 0; s.hpOut_z1 = 0; } }
void TimmyOverdrive::setGain(float g) { gainParam.store(jlimit(0.0f, 1.0f, g)); }
void TimmyOverdrive::setBass(float b) { bassParam.store(jlimit(0.0f, 1.0f, b)); }
void TimmyOverdrive::setTreble(float t) { trebleParam.store(jlimit(0.0f, 1.0f, t)); }
void TimmyOverdrive::setVolume(float v) { volumeParam.store(jlimit(0.0f, 1.0f, v)); }

#include "TubeScreamerMini.h"

TubeScreamerMini::TubeScreamerMini() : AudioModule("Tube Screamer Mini") {}
TubeScreamerMini::~TubeScreamerMini() {}

void TubeScreamerMini::prepare(double newSampleRate, int samplesPerBlock)
{
    AudioModule::prepare(newSampleRate, samplesPerBlock);
    channelState.resize(2);
    reset();
}

void TubeScreamerMini::processInternal(AudioBuffer<float>& buffer)
{
    float od = overdriveParam.load();
    float tone = toneParam.load();
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch < channelState.size() ? ch : 0];
        float gain = 1.0f + od * 79.0f;  // Compact version: 1x to 80x
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float s = data[i];
            float hp = s - state.hpIn_z1 + hpCoeff * state.hpIn_z1; state.hpIn_z1 = hp; s = hp;
            float omega = 2.0f * MathConstants<float>::pi * 720.0f / static_cast<float>(currentSampleRate);
            float mid = state.mid_z1 + (1.0f - std::exp(-omega)) * (s - state.mid_z1); state.mid_z1 = mid;
            s = s * 0.6f + mid * 0.4f * od;
            s *= gain;
            float thresh = 0.65f;
            if (s > thresh) s = thresh + (s - thresh) / (1.0f + (s - thresh) * (s - thresh));
            else if (s < -thresh * 0.7f) s = -(thresh * 0.7f + (-s - thresh * 0.7f) / (1.0f + (-s - thresh * 0.7f) * (-s - thresh * 0.7f)));
            float toneOmega = 2.0f * MathConstants<float>::pi * (500.0f + tone * 4500.0f) / static_cast<float>(currentSampleRate);
            s = state.tone_z1 + (1.0f - std::exp(-toneOmega)) * (s - state.tone_z1); state.tone_z1 = s;
            hp = s - state.hpOut_z1 + hpCoeff * state.hpOut_z1; state.hpOut_z1 = hp;
            data[i] = SIMDProcessor::fastTanh(hp * 0.9f);
        }
    }
}

void TubeScreamerMini::reset() { for (auto& s : channelState) { s.hpIn_z1 = 0; s.mid_z1 = 0; s.tone_z1 = 0; s.hpOut_z1 = 0; } }
void TubeScreamerMini::setOverdrive(float od) { overdriveParam.store(jlimit(0.0f, 1.0f, od)); }
void TubeScreamerMini::setTone(float tone) { toneParam.store(jlimit(0.0f, 1.0f, tone)); }

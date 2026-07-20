#include "TubeScreamerTS9.h"

TubeScreamerTS9::TubeScreamerTS9() : AudioModule("Tube Screamer TS9") {}
TubeScreamerTS9::~TubeScreamerTS9() {}

void TubeScreamerTS9::prepare(double newSampleRate, int samplesPerBlock)
{
    AudioModule::prepare(newSampleRate, samplesPerBlock);
    channelState.resize(2);
    reset();
}

void TubeScreamerTS9::processInternal(AudioBuffer<float>& buffer)
{
    float drive = driveParam.load();
    float tone = toneParam.load();
    float level = levelParam.load();
    
    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();
    
    if (static_cast<size_t>(numChannels) > channelState.size())
        channelState.resize(numChannels);
    
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        float driveGain = 1.0f + drive * 109.0f;  // Slightly more gain than 808
        
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = data[i];
            
            // Input HP
            float hp = sample - state.hpIn_z1 + hpCoeff * state.hpIn_z1;
            state.hpIn_z1 = hp;
            sample = hp;
            
            // Mid boost (less than 808)
            float midFreq = 750.0f;
            float omega = 2.0f * MathConstants<float>::pi * midFreq / static_cast<float>(currentSampleRate);
            float coeff = 1.0f - std::exp(-omega);
            float mid = state.mid_z1 + coeff * (sample - state.mid_z1);
            state.mid_z1 = mid;
            sample = sample * 0.7f + mid * 0.3f * drive;  // Less mid boost than 808
            
            sample *= driveGain;
            
            // Harder clipping than 808
            float threshold = 0.55f;  // Lower threshold = harder clip
            if (sample > threshold)
                sample = threshold + (sample - threshold) / (1.0f + ((sample - threshold) / (1.0f - threshold)) * 2.5f);
            else if (sample < -threshold * 0.75f)
                sample = -(threshold * 0.75f + (-sample - threshold * 0.75f) / (1.0f + ((-sample - threshold * 0.75f) / (1.0f - threshold * 0.75f)) * 2.5f));
            
            // Tone
            float cutoff = 500.0f + tone * 4500.0f;
            float toneOmega = 2.0f * MathConstants<float>::pi * cutoff / static_cast<float>(currentSampleRate);
            float toneCoeff = 1.0f - std::exp(-toneOmega);
            float toneOut = state.tone_z1 + toneCoeff * (sample - state.tone_z1);
            state.tone_z1 = toneOut;
            
            sample = toneOut * level;
            
            // Output HP
            hp = sample - state.hpOut_z1 + hpCoeff * state.hpOut_z1;
            state.hpOut_z1 = hp;
            
            data[i] = SIMDProcessor::fastTanh(hp * 0.9f);
        }
    }
}

void TubeScreamerTS9::reset()
{
    for (auto& state : channelState)
    {
        state.hpIn_z1 = 0.0f;
        state.mid_z1 = 0.0f;
        state.tone_z1 = 0.0f;
        state.hpOut_z1 = 0.0f;
    }
}

void TubeScreamerTS9::setDrive(float drive) { driveParam.store(jlimit(0.0f, 1.0f, drive)); }
void TubeScreamerTS9::setTone(float tone) { toneParam.store(jlimit(0.0f, 1.0f, tone)); }
void TubeScreamerTS9::setLevel(float level) { levelParam.store(jlimit(0.0f, 1.0f, level)); }

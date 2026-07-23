#include "MetalZone.h"

MetalZone::MetalZone() : AudioModule("Metal Zone") {}
MetalZone::~MetalZone() {}

void MetalZone::prepare(double newSampleRate, int samplesPerBlock)
{
    AudioModule::prepare(newSampleRate, samplesPerBlock);
    channelState.resize(2);
    reset();
}

void MetalZone::processInternal(AudioBuffer<float>& buffer)
{
    float dist = distParam.load();
    float treble = trebleParam.load();
    float bass = bassParam.load();
    float level = levelParam.load();
    
    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();
    
    if (static_cast<size_t>(numChannels) > channelState.size())
        channelState.resize(numChannels);
    
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        float gain = 50.0f + dist * 250.0f;  // Extreme gain (50x to 300x)
        
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = data[i];
            
            // Input HP
            float hp = sample - state.hpIn_z1 + hpCoeff * state.hpIn_z1;
            state.hpIn_z1 = hp;
            sample = hp;
            
            // Pre-EQ: Bass control
            float bassFreq = 120.0f;
            float bassOmega = 2.0f * MathConstants<float>::pi * bassFreq / static_cast<float>(currentSampleRate);
            float bassCoeff = 1.0f - std::exp(-bassOmega);
            float bassOut = state.bass_z1 + bassCoeff * (sample - state.bass_z1);
            state.bass_z1 = bassOut;
            float bassGain = 0.5f + bass * 2.0f;  // 0.5x to 2.5x
            sample = sample + (bassOut - sample) * (bass - 0.5f) * 2.0f;
            
            sample *= gain;
            
            // Asymmetric hard clipping (metal character)
            float posThresh = 0.4f;
            float negThresh = 0.35f;
            if (sample > posThresh) sample = posThresh + (sample - posThresh) * 0.1f;
            else if (sample < -negThresh) sample = -negThresh + (sample + negThresh) * 0.1f;
            
            // Post-EQ: Treble control
            float trebleFreq = 2000.0f;
            float trebleOmega = 2.0f * MathConstants<float>::pi * trebleFreq / static_cast<float>(currentSampleRate);
            float trebleHP = sample - state.treble_z1 + 0.95f * state.treble_z1;
            state.treble_z1 = trebleHP;
            sample = sample + trebleHP * (treble - 0.5f) * 2.0f;
            
            sample *= level * 3.0f;
            
            // Output HP
            hp = sample - state.hpOut_z1 + hpCoeff * state.hpOut_z1;
            state.hpOut_z1 = hp;
            
            data[i] = SIMDProcessor::fastTanh(hp * 0.7f);
        }
    }
}

void MetalZone::reset()
{
    for (auto& state : channelState)
    {
        state.hpIn_z1 = 0.0f;
        state.bass_z1 = 0.0f;
        state.treble_z1 = 0.0f;
        state.hpOut_z1 = 0.0f;
    }
}

void MetalZone::setDistortion(float dist) { distParam.store(jlimit(0.0f, 1.0f, dist)); }
void MetalZone::setTreble(float treble) { trebleParam.store(jlimit(0.0f, 1.0f, treble)); }
void MetalZone::setBass(float bass) { bassParam.store(jlimit(0.0f, 1.0f, bass)); }
void MetalZone::setLevel(float level) { levelParam.store(jlimit(0.0f, 1.0f, level)); }

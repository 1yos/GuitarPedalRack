#include "BigMuffPi.h"

//==============================================================================
BigMuffPi::BigMuffPi()
    : AudioModule("Big Muff Pi")
{
}

BigMuffPi::~BigMuffPi()
{
}

//==============================================================================
void BigMuffPi::prepare(double newSampleRate, int samplesPerBlock)
{
    AudioModule::prepare(newSampleRate, samplesPerBlock);
    
    // Initialize channel states
    int numChannels = 2;  // Stereo
    channelState.resize(numChannels);
    
    reset();
}

void BigMuffPi::processInternal(AudioBuffer<float>& buffer)
{
    // Get current parameter values
    float sustain = sustainParam.load();
    float tone = toneParam.load();
    float volume = volumeParam.load();
    
    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();
    
    // Ensure we have enough channel states
    if (static_cast<size_t>(numChannels) > channelState.size())
        channelState.resize(numChannels);
    
    // Process each channel
    for (int ch = 0; ch < numChannels; ++ch)
    {
        float* channelData = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        // Calculate gain for each stage (transistor stages)
        float stageGain = 1.0f + sustain * 24.0f;  // Per-stage gain
        
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = channelData[i];
            
            // 1. Input high-pass filter (remove DC)
            float hpOut = sample - state.hpIn_z1 + hpCoeff * state.hpIn_z1;
            state.hpIn_z1 = hpOut;
            sample = hpOut;
            
            // 2. Stage 1: Gain + soft clipping + filtering
            sample *= stageGain;
            
            // Soft clipping (transistor saturation)
            sample = SIMDProcessor::fastTanh(sample * 1.5f);
            
            // Stage 1 low-pass filter (around 500Hz)
            float omega1 = 2.0f * MathConstants<float>::pi * 500.0f / static_cast<float>(currentSampleRate);
            float coeff1 = 1.0f - std::exp(-omega1);
            float stage1Out = state.stage1_z1 + coeff1 * (sample - state.stage1_z1);
            state.stage1_z1 = stage1Out;
            sample = stage1Out;
            
            // 3. Stage 2: More gain + clipping + filtering
            sample *= stageGain;
            
            // Soft clipping
            sample = SIMDProcessor::fastTanh(sample * 1.5f);
            
            // Stage 2 low-pass filter (around 800Hz)
            float omega2 = 2.0f * MathConstants<float>::pi * 800.0f / static_cast<float>(currentSampleRate);
            float coeff2 = 1.0f - std::exp(-omega2);
            float stage2Out = state.stage2_z1 + coeff2 * (sample - state.stage2_z1);
            state.stage2_z1 = stage2Out;
            sample = stage2Out;
            
            // 4. Big Muff tone stack (famous scooped-mid circuit)
            // Parallel bass and treble paths with variable mixing
            
            // Bass path (low-pass at 200Hz)
            float bassOmega = 2.0f * MathConstants<float>::pi * 200.0f / static_cast<float>(currentSampleRate);
            float bassCoeff = 1.0f - std::exp(-bassOmega);
            float bassOut = state.bassPath_z1 + bassCoeff * (sample - state.bassPath_z1);
            state.bassPath_z1 = bassOut;
            
            // Treble path (high-pass at 1.5kHz)
            float trebleOmega = 2.0f * MathConstants<float>::pi * 1500.0f / static_cast<float>(currentSampleRate);
            float trebleCoeff = 1.0f - std::exp(-trebleOmega);
            float trebleHp = sample - state.treblePath_z1 + 0.99f * state.treblePath_z1;
            state.treblePath_z1 = trebleHp;
            float trebleOut = trebleHp;
            
            // Mix bass and treble based on tone control
            // Tone = 0.0 -> all bass
            // Tone = 0.5 -> scooped (both)
            // Tone = 1.0 -> all treble
            float toneBlend = tone;
            sample = bassOut * (1.0f - toneBlend) + trebleOut * toneBlend;
            
            // Add a bit of mids back to avoid extreme scoop
            sample = sample * 0.8f + stage2Out * 0.2f;
            
            // 5. Output volume
            sample *= volume * 3.0f;  // Big Muff has lots of output
            
            // 6. Output high-pass filter (remove DC)
            hpOut = sample - state.hpOut_z1 + hpCoeff * state.hpOut_z1;
            state.hpOut_z1 = hpOut;
            sample = hpOut;
            
            // 7. Final soft limiting
            sample = SIMDProcessor::fastTanh(sample * 0.6f);
            
            channelData[i] = sample;
        }
    }
}

void BigMuffPi::reset()
{
    for (auto& state : channelState)
    {
        state.hpIn_z1 = 0.0f;
        state.stage1_z1 = 0.0f;
        state.stage2_z1 = 0.0f;
        state.bassPath_z1 = 0.0f;
        state.treblePath_z1 = 0.0f;
        state.hpOut_z1 = 0.0f;
    }
}

//==============================================================================
void BigMuffPi::setSustain(float sustain)
{
    sustainParam.store(jlimit(0.0f, 1.0f, sustain));
}

void BigMuffPi::setTone(float tone)
{
    toneParam.store(jlimit(0.0f, 1.0f, tone));
}

void BigMuffPi::setVolume(float volume)
{
    volumeParam.store(jlimit(0.0f, 1.0f, volume));
}

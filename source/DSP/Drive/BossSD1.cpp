#include "BossSD1.h"

//==============================================================================
BossSD1::BossSD1()
    : AudioModule("Boss SD-1")
{
}

BossSD1::~BossSD1()
{
}

//==============================================================================
void BossSD1::prepare(double newSampleRate, int samplesPerBlock)
{
    AudioModule::prepare(newSampleRate, samplesPerBlock);
    
    // Initialize channel states
    int numChannels = 2;  // Stereo
    channelState.resize(numChannels);
    
    reset();
}

void BossSD1::processInternal(AudioBuffer<float>& buffer)
{
    // Get current parameter values
    float drive = driveParam.load();
    float tone = toneParam.load();
    float level = levelParam.load();
    
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
        
        // Calculate gain (10x to 120x - more than TS808)
        float driveGain = 10.0f + drive * 110.0f;
        
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = channelData[i];
            
            // 1. Input high-pass filter (remove DC)
            float hpOut = sample - state.hpIn_z1 + hpCoeff * state.hpIn_z1;
            state.hpIn_z1 = hpOut;
            sample = hpOut;
            
            // 2. Mid boost (similar to TS but less pronounced)
            float midFreq = 800.0f;
            float omega = 2.0f * MathConstants<float>::pi * midFreq / static_cast<float>(currentSampleRate);
            float coeff = 1.0f - std::exp(-omega);
            float midBoost = sample + (sample - state.mid_z1) * drive * 0.3f;
            state.mid_z1 = sample;
            sample = midBoost;
            
            // 3. Apply drive gain
            sample *= driveGain;
            
            // 4. Asymmetric clipping (SD-1 style - harder than TS808)
            float posThreshold = 0.5f;  // Harder clipping than TS
            float negThreshold = 0.4f;  // Asymmetric
            
            if (sample > posThreshold)
            {
                // Harder knee than TS808
                float over = (sample - posThreshold) / posThreshold;
                sample = posThreshold + posThreshold * over / (1.0f + over * over * 2.0f);
            }
            else if (sample < -negThreshold)
            {
                float over = (-sample - negThreshold) / negThreshold;
                sample = -(negThreshold + negThreshold * over / (1.0f + over * over * 2.0f));
            }
            
            // 5. Tone control (variable low-pass)
            float toneCutoff = 600.0f + tone * 4400.0f;  // 600Hz to 5kHz
            float toneOmega = 2.0f * MathConstants<float>::pi * toneCutoff / static_cast<float>(currentSampleRate);
            float toneCoeff = 1.0f - std::exp(-toneOmega);
            float toneOut = state.tone_z1 + toneCoeff * (sample - state.tone_z1);
            state.tone_z1 = toneOut;
            sample = toneOut;
            
            // 6. Output level
            sample *= level * 2.2f;
            
            // 7. Output high-pass filter (remove DC)
            hpOut = sample - state.hpOut_z1 + hpCoeff * state.hpOut_z1;
            state.hpOut_z1 = hpOut;
            sample = hpOut;
            
            // 8. Final soft limiting
            sample = SIMDProcessor::fastTanh(sample * 0.85f);
            
            channelData[i] = sample;
        }
    }
}

void BossSD1::reset()
{
    for (auto& state : channelState)
    {
        state.hpIn_z1 = 0.0f;
        state.mid_z1 = 0.0f;
        state.tone_z1 = 0.0f;
        state.hpOut_z1 = 0.0f;
    }
}

//==============================================================================
void BossSD1::setDrive(float drive)
{
    driveParam.store(jlimit(0.0f, 1.0f, drive));
}

void BossSD1::setTone(float tone)
{
    toneParam.store(jlimit(0.0f, 1.0f, tone));
}

void BossSD1::setLevel(float level)
{
    levelParam.store(jlimit(0.0f, 1.0f, level));
}

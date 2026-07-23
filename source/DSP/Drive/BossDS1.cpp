#include "BossDS1.h"

//==============================================================================
BossDS1::BossDS1()
    : AudioModule("Boss DS-1")
{
}

BossDS1::~BossDS1()
{
}

//==============================================================================
void BossDS1::prepare(double newSampleRate, int samplesPerBlock)
{
    AudioModule::prepare(newSampleRate, samplesPerBlock);
    
    // Initialize channel states
    int numChannels = 2;  // Stereo
    channelState.resize(numChannels);
    
    reset();
}

void BossDS1::processInternal(AudioBuffer<float>& buffer)
{
    // Get current parameter values
    float dist = distParam.load();
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
        
        // Calculate gain (5x to 200x for high-gain distortion)
        float gain = 5.0f + dist * 195.0f;
        
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = channelData[i];
            
            // 1. Input high-pass filter (remove DC)
            float hpOut = sample - state.hpIn_z1 + hpCoeff * state.hpIn_z1;
            state.hpIn_z1 = hpOut;
            sample = hpOut;
            
            // 2. Apply gain
            sample *= gain;
            
            // 3. Asymmetric hard clipping (DS-1 characteristic)
            // Different thresholds for positive and negative
            float posThreshold = 0.7f;
            float negThreshold = 0.5f;
            
            if (sample > posThreshold)
                sample = posThreshold;
            else if (sample < -negThreshold)
                sample = -negThreshold;
            
            // 4. Active tone control (shelving EQ)
            // Simulates the DS-1's active EQ circuit
            // Tone control boosts/cuts highs
            float shelfFreq = 800.0f;  // Shelving frequency
            float omega = 2.0f * MathConstants<float>::pi * shelfFreq / static_cast<float>(currentSampleRate);
            
            // High-shelf gain based on tone knob
            float shelfGain = (tone - 0.5f) * 24.0f;  // ±12dB
            float A = std::pow(10.0f, shelfGain / 40.0f);
            
            float cos_w = std::cos(omega);
            float sin_w = std::sin(omega);
            float alpha = sin_w / 2.0f * std::sqrt((A + 1.0f / A) * (1.0f / 0.7f - 1.0f) + 2.0f);
            
            // High-shelf filter coefficients
            float b0 = A * ((A + 1.0f) + (A - 1.0f) * cos_w + 2.0f * std::sqrt(A) * alpha);
            float b1 = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cos_w);
            float b2 = A * ((A + 1.0f) + (A - 1.0f) * cos_w - 2.0f * std::sqrt(A) * alpha);
            float a0 = (A + 1.0f) - (A - 1.0f) * cos_w + 2.0f * std::sqrt(A) * alpha;
            float a1 = 2.0f * ((A - 1.0f) - (A + 1.0f) * cos_w);
            float a2 = (A + 1.0f) - (A - 1.0f) * cos_w - 2.0f * std::sqrt(A) * alpha;
            
            // Normalize
            b0 /= a0;
            b1 /= a0;
            b2 /= a0;
            a1 /= a0;
            a2 /= a0;
            
            // Apply filter
            float toneOut = b0 * sample + b1 * state.tone_z1 + b2 * state.tone_z2
                          - a1 * state.tone_z1 - a2 * state.tone_z2;
            state.tone_z2 = state.tone_z1;
            state.tone_z1 = toneOut;
            sample = toneOut;
            
            // 5. Output level
            sample *= level * 2.5f;  // Compensate for clipping loss
            
            // 6. Output high-pass filter (remove DC from clipping)
            hpOut = sample - state.hpOut_z1 + hpCoeff * state.hpOut_z1;
            state.hpOut_z1 = hpOut;
            sample = hpOut;
            
            // 7. Final soft limiting
            sample = SIMDProcessor::fastTanh(sample * 0.7f);
            
            channelData[i] = sample;
        }
    }
}

void BossDS1::reset()
{
    for (auto& state : channelState)
    {
        state.hpIn_z1 = 0.0f;
        state.tone_z1 = 0.0f;
        state.tone_z2 = 0.0f;
        state.hpOut_z1 = 0.0f;
    }
}

//==============================================================================
void BossDS1::setDist(float dist)
{
    distParam.store(jlimit(0.0f, 1.0f, dist));
}

void BossDS1::setTone(float tone)
{
    toneParam.store(jlimit(0.0f, 1.0f, tone));
}

void BossDS1::setLevel(float level)
{
    levelParam.store(jlimit(0.0f, 1.0f, level));
}

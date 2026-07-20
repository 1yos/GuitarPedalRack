#include "BluesDriver.h"

//==============================================================================
BluesDriver::BluesDriver()
    : AudioModule("Blues Driver")
{
}

BluesDriver::~BluesDriver()
{
}

//==============================================================================
void BluesDriver::prepare(double newSampleRate, int samplesPerBlock)
{
    AudioModule::prepare(newSampleRate, samplesPerBlock);
    
    // Initialize channel states
    int numChannels = 2;  // Stereo
    channelState.resize(numChannels);
    
    reset();
}

void BluesDriver::processInternal(AudioBuffer<float>& buffer)
{
    // Get current parameter values
    float gain = gainParam.load();
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
        
        // Calculate gain (2x to 80x - moderate range for tube-like response)
        float driveGain = 2.0f + gain * 78.0f;
        
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = channelData[i];
            
            // 1. Input high-pass filter (remove DC)
            float hpOut = sample - state.hpIn_z1 + hpCoeff * state.hpIn_z1;
            state.hpIn_z1 = hpOut;
            sample = hpOut;
            
            // 2. Pre-emphasis (slight high-frequency boost before clipping)
            float emphFreq = 2000.0f;
            float emphOmega = 2.0f * MathConstants<float>::pi * emphFreq / static_cast<float>(currentSampleRate);
            float hp = sample - state.preEmph_z1 + 0.97f * state.preEmph_z1;
            state.preEmph_z1 = hp;
            sample = sample + hp * 0.2f;  // Subtle boost
            
            // 3. Apply drive gain
            sample *= driveGain;
            
            // 4. Asymmetric soft clipping (BD-2 characteristic - warm, tube-like)
            // Smoother than TS, more harmonics than clean boost
            float posThreshold = 0.6f;
            float negThreshold = 0.5f;  // Asymmetric for even harmonics
            
            // Soft knee clipping with smooth curves
            if (sample > posThreshold)
            {
                float over = (sample - posThreshold) / posThreshold;
                sample = posThreshold + posThreshold * over / (1.0f + over * over);
            }
            else if (sample < -negThreshold)
            {
                float over = (-sample - negThreshold) / negThreshold;
                sample = -(negThreshold + negThreshold * over / (1.0f + over * over));
            }
            else
            {
                // Subtle compression in the middle region (tube-like)
                sample *= (1.0f + gain * 0.1f);
            }
            
            // 5. Tone control (variable low-pass with resonance)
            float toneCutoff = 400.0f + tone * 4600.0f;  // 400Hz to 5kHz
            float toneOmega = 2.0f * MathConstants<float>::pi * toneCutoff / static_cast<float>(currentSampleRate);
            float toneCoeff = 1.0f - std::exp(-toneOmega);
            float toneOut = state.tone_z1 + toneCoeff * (sample - state.tone_z1);
            state.tone_z1 = toneOut;
            
            // Add slight resonance at tone frequency (blues character)
            float resonance = (sample - toneOut) * tone * 0.15f;
            sample = toneOut + resonance;
            
            // 6. Output level
            sample *= level * 2.3f;
            
            // 7. Output high-pass filter (remove DC)
            hpOut = sample - state.hpOut_z1 + hpCoeff * state.hpOut_z1;
            state.hpOut_z1 = hpOut;
            sample = hpOut;
            
            // 8. Final soft limiting (gentle, preserves dynamics)
            sample = SIMDProcessor::fastTanh(sample * 0.85f);
            
            channelData[i] = sample;
        }
    }
}

void BluesDriver::reset()
{
    for (auto& state : channelState)
    {
        state.hpIn_z1 = 0.0f;
        state.preEmph_z1 = 0.0f;
        state.tone_z1 = 0.0f;
        state.hpOut_z1 = 0.0f;
    }
}

//==============================================================================
void BluesDriver::setGain(float gain)
{
    gainParam.store(jlimit(0.0f, 1.0f, gain));
}

void BluesDriver::setTone(float tone)
{
    toneParam.store(jlimit(0.0f, 1.0f, tone));
}

void BluesDriver::setLevel(float level)
{
    levelParam.store(jlimit(0.0f, 1.0f, level));
}

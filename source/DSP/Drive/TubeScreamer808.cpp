#include "TubeScreamer808.h"

//==============================================================================
TubeScreamer808::TubeScreamer808()
    : AudioModule("Tube Screamer 808")
{
}

TubeScreamer808::~TubeScreamer808()
{
}

//==============================================================================
void TubeScreamer808::prepare(double newSampleRate, int samplesPerBlock)
{
    AudioModule::prepare(newSampleRate, samplesPerBlock);
    
    // Initialize channel states
    int numChannels = 2;  // Stereo
    channelState.resize(numChannels);
    
    reset();
}

void TubeScreamer808::processInternal(AudioBuffer<float>& buffer)
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
        
        // Calculate drive gain (1x to 100x)
        float driveGain = 1.0f + drive * 99.0f;
        
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = channelData[i];
            
            // 1. Input high-pass filter (remove DC)
            float hpOut = sample - state.hpIn_z1 + hpCoeff * state.hpIn_z1;
            state.hpIn_z1 = hpOut;
            sample = hpOut;
            
            // 2. Apply drive gain
            sample *= driveGain;
            
            // 3. Mid-range boost (720Hz characteristic hump)
            float omega = 2.0f * MathConstants<float>::pi * midFreq / static_cast<float>(currentSampleRate);
            float q = 2.0f;
            float alpha = std::sin(omega) / (2.0f * q);
            float cos_w = std::cos(omega);
            
            float b0 = alpha;
            float b2 = -alpha;
            float a0 = 1.0f + alpha;
            float a1 = -2.0f * cos_w;
            float a2 = 1.0f - alpha;
            
            b0 /= a0;
            b2 /= a0;
            a1 /= a0;
            a2 /= a0;
            
            float boosted = b0 * sample + b2 * state.midBoost_z2 - a1 * state.midBoost_z1 - a2 * state.midBoost_z2;
            state.midBoost_z2 = state.midBoost_z1;
            state.midBoost_z1 = boosted;
            boosted *= (1.0f + midGain / 20.0f);
            
            // Blend based on drive amount
            sample = sample * (1.0f - drive * 0.5f) + boosted * (drive * 0.5f);
            
            // 4. Soft asymmetric clipping (TS808 diode characteristic)
            float threshold = 0.7f / (1.0f + drive * 9.0f);
            if (sample > threshold)
                sample = threshold + (sample - threshold) / (1.0f + ((sample - threshold) / (1.0f - threshold)) * ((sample - threshold) / (1.0f - threshold)));
            else if (sample < -threshold * 0.7f)
                sample = -threshold * 0.7f + (sample + threshold * 0.7f) / (1.0f + ((sample + threshold * 0.7f) / (1.0f - threshold * 0.7f)) * ((sample + threshold * 0.7f) / (1.0f - threshold * 0.7f)));
            
            // 5. Tone control (variable low-pass)
            float cutoffHz = 500.0f + tone * 4500.0f;
            float toneOmega = 2.0f * MathConstants<float>::pi * cutoffHz / static_cast<float>(currentSampleRate);
            float coeff = 1.0f - std::exp(-toneOmega);
            float toneOut = state.tone_z1 + coeff * (sample - state.tone_z1);
            state.tone_z1 = toneOut;
            sample = toneOut;
            
            // 6. Output level
            sample *= level;
            
            // 7. Output high-pass filter (remove DC from clipping)
            hpOut = sample - state.hpOut_z1 + hpCoeff * state.hpOut_z1;
            state.hpOut_z1 = hpOut;
            sample = hpOut;
            
            // 8. Final soft limiting
            sample = SIMDProcessor::fastTanh(sample * 0.9f);
            
            channelData[i] = sample;
        }
    }
}

void TubeScreamer808::reset()
{
    for (auto& state : channelState)
    {
        state.hpIn_z1 = 0.0f;
        state.midBoost_z1 = 0.0f;
        state.midBoost_z2 = 0.0f;
        state.tone_z1 = 0.0f;
        state.hpOut_z1 = 0.0f;
    }
}

//==============================================================================
void TubeScreamer808::setDrive(float drive)
{
    driveParam.store(jlimit(0.0f, 1.0f, drive));
}

void TubeScreamer808::setTone(float tone)
{
    toneParam.store(jlimit(0.0f, 1.0f, tone));
}

void TubeScreamer808::setLevel(float level)
{
    levelParam.store(jlimit(0.0f, 1.0f, level));
}

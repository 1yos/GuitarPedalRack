#include "KlonCentaur.h"

//==============================================================================
KlonCentaur::KlonCentaur()
    : AudioModule("Klon Centaur")
{
}

KlonCentaur::~KlonCentaur()
{
}

//==============================================================================
void KlonCentaur::prepare(double newSampleRate, int samplesPerBlock)
{
    AudioModule::prepare(newSampleRate, samplesPerBlock);
    
    // Initialize channel states
    int numChannels = 2;  // Stereo
    channelState.resize(numChannels);
    
    reset();
}

void KlonCentaur::processInternal(AudioBuffer<float>& buffer)
{
    // Get current parameter values
    float gain = gainParam.load();
    float treble = trebleParam.load();
    float output = outputParam.load();
    
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
        
        // Calculate gain and blend (Klon's key feature)
        float driveGain = 1.0f + gain * 49.0f;  // 1x to 50x (more transparent than TS)
        float cleanBlend = 1.0f - gain * 0.7f;  // Keep more clean signal
        
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = channelData[i];
            float cleanSample = sample;  // Store clean signal for blending
            
            // 1. Input high-pass filter (remove DC)
            float hpOut = sample - state.hpIn_z1 + hpCoeff * state.hpIn_z1;
            state.hpIn_z1 = hpOut;
            sample = hpOut;
            
            // 2. Treble boost before clipping (Klon characteristic)
            if (treble > 0.5f)
            {
                // High-shelf boost: run a 1-pole HP and add it back
                float boost = (treble - 0.5f) * 4.0f;  // Up to 2x boost
                // 1-pole HP: y = x - x[n-1] * coeff  (input stored, not output)
                float hp = sample - state.treble_z1;
                state.treble_z1 = sample;   // FIXED: store input, not hp output
                sample = sample + hp * boost * 0.3f;
            }
            
            // 3. Apply drive gain
            sample *= driveGain;
            
            // 4. Germanium diode soft clipping (Klon uses germanium diodes)
            // Softer knee than silicon, lower threshold
            float threshold = 0.3f;  // Lower than typical (germanium characteristic)
            
            // Soft knee clipping (smoother than hard clip)
            if (sample > threshold)
            {
                float over = sample - threshold;
                sample = threshold + over / (1.0f + over * over);
            }
            else if (sample < -threshold)
            {
                float over = -sample - threshold;
                sample = -(threshold + over / (1.0f + over * over));
            }
            
            // 5. Blend clean and dirty (Klon's transparency secret)
            sample = cleanSample * cleanBlend + sample * (1.0f - cleanBlend);
            
            // 6. Output level
            sample *= output * 2.0f;
            
            // 7. Output high-pass filter (remove DC)
            hpOut = sample - state.hpOut_z1 + hpCoeff * state.hpOut_z1;
            state.hpOut_z1 = hpOut;
            sample = hpOut;
            
            // 8. Final soft limiting (very gentle)
            sample = SIMDProcessor::fastTanh(sample * 0.9f);
            
            channelData[i] = sample;
        }
    }
}

void KlonCentaur::reset()
{
    for (auto& state : channelState)
    {
        state.hpIn_z1 = 0.0f;
        state.treble_z1 = 0.0f;
        state.hpOut_z1 = 0.0f;
    }
}

//==============================================================================
void KlonCentaur::setGain(float gain)
{
    gainParam.store(jlimit(0.0f, 1.0f, gain));
}

void KlonCentaur::setTreble(float treble)
{
    trebleParam.store(jlimit(0.0f, 1.0f, treble));
}

void KlonCentaur::setOutput(float output)
{
    outputParam.store(jlimit(0.0f, 1.0f, output));
}

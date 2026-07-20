#include "ProCoRAT.h"

//==============================================================================
ProCoRAT::ProCoRAT()
    : AudioModule("ProCo RAT")
{
}

ProCoRAT::~ProCoRAT()
{
}

//==============================================================================
void ProCoRAT::prepare(double newSampleRate, int samplesPerBlock)
{
    AudioModule::prepare(newSampleRate, samplesPerBlock);
    
    // Initialize channel states
    int numChannels = 2;  // Stereo
    channelState.resize(numChannels);
    
    reset();
}

void ProCoRAT::processInternal(AudioBuffer<float>& buffer)
{
    // Get current parameter values
    float distortion = distortionParam.load();
    float filter = filterParam.load();
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
        
        // Calculate gain (10x to 150x for aggressive distortion)
        float gain = 10.0f + distortion * 140.0f;
        
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = channelData[i];
            
            // 1. Input high-pass filter (remove DC)
            float hpOut = sample - state.hpIn_z1 + hpCoeff * state.hpIn_z1;
            state.hpIn_z1 = hpOut;
            sample = hpOut;
            
            // 2. Pre-distortion low-pass filter (RAT characteristic)
            // This shapes the frequency response before clipping
            float preCutoff = 1000.0f + filter * 2000.0f;  // 1kHz to 3kHz
            float preOmega = 2.0f * MathConstants<float>::pi * preCutoff / static_cast<float>(currentSampleRate);
            float preCoeff = 1.0f - std::exp(-preOmega);
            float preFiltered = state.preLp_z1 + preCoeff * (sample - state.preLp_z1);
            state.preLp_z1 = preFiltered;
            
            // 3. Apply gain
            sample = preFiltered * gain;
            
            // 4. Hard clipping (op-amp saturation)
            // RAT uses hard clipping at lower threshold than typical op-amp
            float clipThreshold = 0.6f;
            if (sample > clipThreshold)
                sample = clipThreshold;
            else if (sample < -clipThreshold)
                sample = -clipThreshold;
            
            // 5. Post-clipping tone control (RAT "Filter" knob)
            // Lower values = more filtering = darker sound
            float toneCutoff = 300.0f + filter * 4700.0f;  // 300Hz to 5kHz
            float toneOmega = 2.0f * MathConstants<float>::pi * toneCutoff / static_cast<float>(currentSampleRate);
            float toneCoeff = 1.0f - std::exp(-toneOmega);
            float toneFiltered = state.tone_z1 + toneCoeff * (sample - state.tone_z1);
            state.tone_z1 = toneFiltered;
            sample = toneFiltered;
            
            // 6. Output volume
            sample *= volume * 2.0f;  // Compensate for clipping loss
            
            // 7. Output high-pass filter (remove DC from clipping)
            hpOut = sample - state.hpOut_z1 + hpCoeff * state.hpOut_z1;
            state.hpOut_z1 = hpOut;
            sample = hpOut;
            
            // 8. Final soft limiting
            sample = SIMDProcessor::fastTanh(sample * 0.8f);
            
            channelData[i] = sample;
        }
    }
}

void ProCoRAT::reset()
{
    for (auto& state : channelState)
    {
        state.hpIn_z1 = 0.0f;
        state.preLp_z1 = 0.0f;
        state.tone_z1 = 0.0f;
        state.hpOut_z1 = 0.0f;
    }
}

//==============================================================================
void ProCoRAT::setDistortion(float distortion)
{
    distortionParam.store(jlimit(0.0f, 1.0f, distortion));
}

void ProCoRAT::setFilter(float filter)
{
    filterParam.store(jlimit(0.0f, 1.0f, filter));
}

void ProCoRAT::setVolume(float volume)
{
    volumeParam.store(jlimit(0.0f, 1.0f, volume));
}

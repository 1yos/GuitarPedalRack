#include "FuzzFace.h"

//==============================================================================
FuzzFace::FuzzFace()
    : AudioModule("Fuzz Face")
{
}

FuzzFace::~FuzzFace()
{
}

//==============================================================================
void FuzzFace::prepare(double newSampleRate, int samplesPerBlock)
{
    AudioModule::prepare(newSampleRate, samplesPerBlock);
    
    // Initialize channel states
    int numChannels = 2;  // Stereo
    channelState.resize(numChannels);
    
    reset();
}

void FuzzFace::processInternal(AudioBuffer<float>& buffer)
{
    // Get current parameter values
    float fuzz = fuzzParam.load();
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
        
        // Q1 and Q2 gain stages (Fuzz Face uses two transistors)
        float q1Gain = 5.0f + fuzz * 25.0f;   // Q1: 5x to 30x
        float q2Gain = 3.0f + fuzz * 17.0f;   // Q2: 3x to 20x
        
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = channelData[i];
            
            // 1. Input high-pass filter (AC coupling cap)
            float hpOut = sample - state.hpIn_z1 + hpCoeff * state.hpIn_z1;
            state.hpIn_z1 = hpOut;
            sample = hpOut;
            
            // 2. Q1 transistor stage (first gain stage)
            sample *= q1Gain;
            
            // Germanium soft clipping (very smooth, warm)
            // Lower threshold than silicon
            float q1Threshold = 0.25f;
            if (sample > q1Threshold)
            {
                float over = sample - q1Threshold;
                sample = q1Threshold + over / (1.0f + over);
            }
            else if (sample < -q1Threshold * 0.8f)  // Slightly asymmetric
            {
                float over = -sample - q1Threshold * 0.8f;
                sample = -(q1Threshold * 0.8f + over / (1.0f + over));
            }
            
            // Q1 low-pass filter (transistor bandwidth limitation)
            float q1Cutoff = 2000.0f;
            float q1Omega = 2.0f * MathConstants<float>::pi * q1Cutoff / static_cast<float>(currentSampleRate);
            float q1Coeff = 1.0f - std::exp(-q1Omega);
            float q1Out = state.q1Lp_z1 + q1Coeff * (sample - state.q1Lp_z1);
            state.q1Lp_z1 = q1Out;
            sample = q1Out;
            
            // 3. Q2 transistor stage (second gain stage)
            sample *= q2Gain;
            
            // More germanium soft clipping
            float q2Threshold = 0.4f;
            if (sample > q2Threshold)
            {
                float over = sample - q2Threshold;
                sample = q2Threshold + over / (1.0f + over * 1.5f);
            }
            else if (sample < -q2Threshold * 0.7f)
            {
                float over = -sample - q2Threshold * 0.7f;
                sample = -(q2Threshold * 0.7f + over / (1.0f + over * 1.5f));
            }
            
            // Q2 low-pass filter
            float q2Cutoff = 3000.0f;
            float q2Omega = 2.0f * MathConstants<float>::pi * q2Cutoff / static_cast<float>(currentSampleRate);
            float q2Coeff = 1.0f - std::exp(-q2Omega);
            float q2Out = state.q2Lp_z1 + q2Coeff * (sample - state.q2Lp_z1);
            state.q2Lp_z1 = q2Out;
            sample = q2Out;
            
            // 4. Output volume
            sample *= volume * 3.5f;  // Fuzz Face has lots of output
            
            // 5. Output high-pass filter (remove DC)
            hpOut = sample - state.hpOut_z1 + hpCoeff * state.hpOut_z1;
            state.hpOut_z1 = hpOut;
            sample = hpOut;
            
            // 6. Final soft limiting (very gentle for germanium character)
            sample = SIMDProcessor::fastTanh(sample * 0.7f);
            
            channelData[i] = sample;
        }
    }
}

void FuzzFace::reset()
{
    for (auto& state : channelState)
    {
        state.hpIn_z1 = 0.0f;
        state.q1Lp_z1 = 0.0f;
        state.q2Lp_z1 = 0.0f;
        state.hpOut_z1 = 0.0f;
    }
}

//==============================================================================
void FuzzFace::setFuzz(float fuzz)
{
    fuzzParam.store(jlimit(0.0f, 1.0f, fuzz));
}

void FuzzFace::setVolume(float volume)
{
    volumeParam.store(jlimit(0.0f, 1.0f, volume));
}

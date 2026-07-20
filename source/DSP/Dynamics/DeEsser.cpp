#include "DeEsser.h"
#include <cmath>

DeEsser::DeEsser() : AudioModule("DeEsser")
{
}

DeEsser::~DeEsser()
{
}

void DeEsser::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    reset();
}

void DeEsser::reset()
{
    for (auto& state : channelState)
    {
        state.bpZ1 = 0.0f;
        state.bpZ2 = 0.0f;
        state.envelope = 0.0f;
    }
}

void DeEsser::setThreshold(float threshold)
{
    thresholdParam.store(jlimit(0.0f, 1.0f, threshold), std::memory_order_relaxed);
}

void DeEsser::setFrequency(float frequency)
{
    frequencyParam.store(jlimit(0.0f, 1.0f, frequency), std::memory_order_relaxed);
}

void DeEsser::setRatio(float ratio)
{
    ratioParam.store(jlimit(0.0f, 1.0f, ratio), std::memory_order_relaxed);
}

void DeEsser::setAttack(float attack)
{
    attackParam.store(jlimit(0.0f, 1.0f, attack), std::memory_order_relaxed);
}

void DeEsser::setRelease(float release)
{
    releaseParam.store(jlimit(0.0f, 1.0f, release), std::memory_order_relaxed);
}

void DeEsser::processInternal(AudioBuffer<float>& buffer)
{
    if (!bypassed)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        
        // Get parameter values
        const float threshold = -60.0f + thresholdParam.load(std::memory_order_relaxed) * 60.0f;  // -60dB to 0dB
        const float frequency = 5000.0f + frequencyParam.load(std::memory_order_relaxed) * 10000.0f;  // 5kHz to 15kHz
        const float ratio = 1.0f + ratioParam.load(std::memory_order_relaxed) * 9.0f;  // 1:1 to 10:1
        const float attack = 0.001f + attackParam.load(std::memory_order_relaxed) * 0.009f;  // 1ms to 10ms
        const float release = 0.01f + releaseParam.load(std::memory_order_relaxed) * 0.49f;  // 10ms to 500ms
        
        // Calculate time constants
        const float attackCoeff = std::exp(-1.0f / (attack * static_cast<float>(currentSampleRate)));
        const float releaseCoeff = std::exp(-1.0f / (release * static_cast<float>(currentSampleRate)));
        
        // Calculate bandpass filter coefficients for sibilance detection
        const float omega = juce::MathConstants<float>::twoPi * frequency / static_cast<float>(currentSampleRate);
        const float Q = 2.0f;  // Narrow bandwidth for sibilance
        const float alpha = std::sin(omega) / (2.0f * Q);
        const float cosOmega = std::cos(omega);
        
        const float b0 = alpha;
        const float b1 = 0.0f;
        const float b2 = -alpha;
        const float a0 = 1.0f + alpha;
        const float a1 = -2.0f * cosOmega;
        const float a2 = 1.0f - alpha;
        
        for (int ch = 0; ch < numChannels && ch < (int)channelState.size(); ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            auto& state = channelState[ch];
            
            for (int i = 0; i < numSamples; ++i)
            {
                const float input = channelData[i];
                
                // Bandpass filter to isolate sibilance frequencies
                const float bpOut = (b0 * input + b1 * state.bpZ1 + b2 * state.bpZ2 - a1 * state.bpZ1 - a2 * state.bpZ2) / a0;
                state.bpZ2 = state.bpZ1;
                state.bpZ1 = bpOut;
                
                // Convert to dB for envelope detection
                const float sibilanceLevel = 20.0f * std::log10(std::abs(bpOut) + 1e-6f);
                
                // Envelope follower
                if (sibilanceLevel > state.envelope)
                    state.envelope += (sibilanceLevel - state.envelope) * (1.0f - attackCoeff);
                else
                    state.envelope += (sibilanceLevel - state.envelope) * (1.0f - releaseCoeff);
                
                // Calculate gain reduction
                float gainReduction = 0.0f;
                if (state.envelope > threshold)
                {
                    const float excess = state.envelope - threshold;
                    gainReduction = excess * (1.0f - 1.0f / ratio);
                }
                
                // Apply gain reduction (convert from dB)
                const float linearGain = std::pow(10.0f, -gainReduction / 20.0f);
                channelData[i] = input * linearGain;
            }
        }
    }
}

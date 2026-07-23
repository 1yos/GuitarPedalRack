#include "ToneStackFender.h"
#include <cmath>

ToneStackFender::ToneStackFender() : AudioModule("ToneStackFender")
{
}

ToneStackFender::~ToneStackFender()
{
}

void ToneStackFender::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    updateCoefficients();
    reset();
}

void ToneStackFender::reset()
{
    for (auto& ch : channelState)
    {
        ch.z1 = 0.0f;
        ch.z2 = 0.0f;
        ch.z3 = 0.0f;
    }
}

void ToneStackFender::setBass(float bass)
{
    bassParam.store(jlimit(0.0f, 1.0f, bass), std::memory_order_relaxed);
    updateCoefficients();
}

void ToneStackFender::setMid(float mid)
{
    midParam.store(jlimit(0.0f, 1.0f, mid), std::memory_order_relaxed);
    updateCoefficients();
}

void ToneStackFender::setTreble(float treble)
{
    trebleParam.store(jlimit(0.0f, 1.0f, treble), std::memory_order_relaxed);
    updateCoefficients();
}

void ToneStackFender::updateCoefficients()
{
    // Fender Bassman/Twin tone stack (simplified)
    const float bass = bassParam.load(std::memory_order_relaxed);
    const float mid = midParam.load(std::memory_order_relaxed);
    const float treble = trebleParam.load(std::memory_order_relaxed);
    
    // Simplified tone stack model
    const float bassFreq = 100.0f;
    const float midFreq = 500.0f;
    const float trebleFreq = 4000.0f;
    
    // Calculate bass shelf
    const float bassGain = -12.0f + bass * 24.0f;
    const float bassA = std::pow(10.0f, bassGain / 40.0f);
    
    const float omega1 = juce::MathConstants<float>::twoPi * bassFreq / static_cast<float>(currentSampleRate);
    const float sn1 = std::sin(omega1);
    const float cs1 = std::cos(omega1);
    const float beta1 = std::sqrt(bassA);
    
    b0 = bassA * ((bassA + 1.0f) - (bassA - 1.0f) * cs1 + beta1 * sn1);
    b1 = 2.0f * bassA * ((bassA - 1.0f) - (bassA + 1.0f) * cs1);
    b2 = bassA * ((bassA + 1.0f) - (bassA - 1.0f) * cs1 - beta1 * sn1);
    a0 = (bassA + 1.0f) + (bassA - 1.0f) * cs1 + beta1 * sn1;
    a1 = -2.0f * ((bassA - 1.0f) + (bassA + 1.0f) * cs1);
    a2 = (bassA + 1.0f) + (bassA - 1.0f) * cs1 - beta1 * sn1;
    
    // Normalize
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;
}

void ToneStackFender::processInternal(AudioBuffer<float>& buffer)
{
    if (!bypassed)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        
        for (int ch = 0; ch < numChannels && ch < (int)channelState.size(); ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            auto& state = channelState[ch];
            
            for (int i = 0; i < numSamples; ++i)
            {
                const float input = channelData[i];
                
                const float output = b0 * input + b1 * state.z1 + b2 * state.z2
                                   - a1 * state.z1 - a2 * state.z2;
                
                state.z2 = state.z1;
                state.z1 = input;
                
                channelData[i] = output;
            }
        }
    }
}

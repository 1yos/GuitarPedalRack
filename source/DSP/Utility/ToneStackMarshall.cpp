#include "ToneStackMarshall.h"
#include <cmath>

ToneStackMarshall::ToneStackMarshall() : AudioModule("ToneStackMarshall")
{
}

ToneStackMarshall::~ToneStackMarshall()
{
}

void ToneStackMarshall::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    updateCoefficients();
    reset();
}

void ToneStackMarshall::reset()
{
    for (auto& ch : channelState)
    {
        ch.z1 = 0.0f;
        ch.z2 = 0.0f;
        ch.z3 = 0.0f;
        ch.z4 = 0.0f;
    }
}

void ToneStackMarshall::setBass(float bass)
{
    bassParam.store(jlimit(0.0f, 1.0f, bass), std::memory_order_relaxed);
    updateCoefficients();
}

void ToneStackMarshall::setMid(float mid)
{
    midParam.store(jlimit(0.0f, 1.0f, mid), std::memory_order_relaxed);
    updateCoefficients();
}

void ToneStackMarshall::setTreble(float treble)
{
    trebleParam.store(jlimit(0.0f, 1.0f, treble), std::memory_order_relaxed);
    updateCoefficients();
}

void ToneStackMarshall::setPresence(float presence)
{
    presenceParam.store(jlimit(0.0f, 1.0f, presence), std::memory_order_relaxed);
    updateCoefficients();
}

void ToneStackMarshall::updateCoefficients()
{
    // Marshall JCM800 tone stack (simplified)
    const float bass = bassParam.load(std::memory_order_relaxed);
    const float mid = midParam.load(std::memory_order_relaxed);
    const float treble = trebleParam.load(std::memory_order_relaxed);
    const float presence = presenceParam.load(std::memory_order_relaxed);
    
    // Simplified Marshall tone stack model
    const float bassFreq = 80.0f;
    const float midFreq = 650.0f;
    const float trebleFreq = 3000.0f;
    const float presenceFreq = 5000.0f;
    
    // Calculate bass boost
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

void ToneStackMarshall::processInternal(AudioBuffer<float>& buffer)
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

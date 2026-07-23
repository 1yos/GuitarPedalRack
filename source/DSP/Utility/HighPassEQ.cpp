#include "HighPassEQ.h"
#include <cmath>

HighPassEQ::HighPassEQ() : AudioModule("HighPassEQ")
{
}

HighPassEQ::~HighPassEQ()
{
}

void HighPassEQ::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    updateCoefficients();
    reset();
}

void HighPassEQ::reset()
{
    for (auto& ch : channelState)
    {
        ch.filter1.z1 = 0.0f;
        ch.filter1.z2 = 0.0f;
        ch.filter2.z1 = 0.0f;
        ch.filter2.z2 = 0.0f;
    }
}

void HighPassEQ::setFrequency(float freq)
{
    frequencyParam.store(jlimit(0.0f, 1.0f, freq), std::memory_order_relaxed);
    updateCoefficients();
}

void HighPassEQ::setResonance(float res)
{
    resonanceParam.store(jlimit(0.0f, 1.0f, res), std::memory_order_relaxed);
    updateCoefficients();
}

void HighPassEQ::setSlope(float slope)
{
    slopeParam.store(jlimit(0.0f, 1.0f, slope), std::memory_order_relaxed);
}

void HighPassEQ::updateCoefficients()
{
    const float fc = 20.0f + frequencyParam.load(std::memory_order_relaxed) * 1980.0f; // 20-2000Hz
    const float Q = 0.5f + resonanceParam.load(std::memory_order_relaxed) * 9.5f; // 0.5-10
    
    const float omega = juce::MathConstants<float>::twoPi * fc / static_cast<float>(currentSampleRate);
    const float sn = std::sin(omega);
    const float cs = std::cos(omega);
    const float alpha = sn / (2.0f * Q);
    
    // High-pass filter coefficients
    const float b0 = (1.0f + cs) / 2.0f;
    const float b1 = -(1.0f + cs);
    const float b2 = (1.0f + cs) / 2.0f;
    const float a0 = 1.0f + alpha;
    const float a1 = -2.0f * cs;
    const float a2 = 1.0f - alpha;
    
    for (auto& ch : channelState)
    {
        ch.filter1.b0 = b0 / a0;
        ch.filter1.b1 = b1 / a0;
        ch.filter1.b2 = b2 / a0;
        ch.filter1.a1 = a1 / a0;
        ch.filter1.a2 = a2 / a0;
        
        // Same coefficients for second stage (24dB mode)
        ch.filter2.b0 = b0 / a0;
        ch.filter2.b1 = b1 / a0;
        ch.filter2.b2 = b2 / a0;
        ch.filter2.a1 = a1 / a0;
        ch.filter2.a2 = a2 / a0;
    }
}

void HighPassEQ::processInternal(AudioBuffer<float>& buffer)
{
    if (!bypassed)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        const bool use24dB = slopeParam.load(std::memory_order_relaxed) > 0.5f;
        
        for (int ch = 0; ch < numChannels && ch < (int)channelState.size(); ++ch)
        {
            float* channelData = buffer.getWritePointer(ch);
            auto& state = channelState[ch];
            
            for (int i = 0; i < numSamples; ++i)
            {
                float sample = channelData[i];
                
                // First stage (12dB)
                {
                    auto& filter = state.filter1;
                    const float output = filter.b0 * sample + filter.b1 * filter.z1 + filter.b2 * filter.z2
                                       - filter.a1 * filter.z1 - filter.a2 * filter.z2;
                    filter.z2 = filter.z1;
                    filter.z1 = sample;
                    sample = output;
                }
                
                // Second stage (24dB mode)
                if (use24dB)
                {
                    auto& filter = state.filter2;
                    const float output = filter.b0 * sample + filter.b1 * filter.z1 + filter.b2 * filter.z2
                                       - filter.a1 * filter.z1 - filter.a2 * filter.z2;
                    filter.z2 = filter.z1;
                    filter.z1 = sample;
                    sample = output;
                }
                
                channelData[i] = sample;
            }
        }
    }
}

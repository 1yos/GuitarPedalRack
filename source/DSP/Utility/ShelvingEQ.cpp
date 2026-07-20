#include "ShelvingEQ.h"
#include <cmath>

ShelvingEQ::ShelvingEQ() : AudioModule("ShelvingEQ")
{
}

ShelvingEQ::~ShelvingEQ()
{
}

void ShelvingEQ::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    updateCoefficients();
    reset();
}

void ShelvingEQ::reset()
{
    for (auto& ch : channelState)
    {
        ch.lowShelf.z1 = 0.0f;
        ch.lowShelf.z2 = 0.0f;
        ch.highShelf.z1 = 0.0f;
        ch.highShelf.z2 = 0.0f;
    }
}

void ShelvingEQ::setLowShelfFreq(float freq)
{
    lowShelfFreqParam.store(jlimit(0.0f, 1.0f, freq), std::memory_order_relaxed);
    updateCoefficients();
}

void ShelvingEQ::setLowShelfGain(float gain)
{
    lowShelfGainParam.store(jlimit(0.0f, 1.0f, gain), std::memory_order_relaxed);
    updateCoefficients();
}

void ShelvingEQ::setHighShelfFreq(float freq)
{
    highShelfFreqParam.store(jlimit(0.0f, 1.0f, freq), std::memory_order_relaxed);
    updateCoefficients();
}

void ShelvingEQ::setHighShelfGain(float gain)
{
    highShelfGainParam.store(jlimit(0.0f, 1.0f, gain), std::memory_order_relaxed);
    updateCoefficients();
}

void ShelvingEQ::updateCoefficients()
{
    // Low shelf parameters
    const float lowFreq = 20.0f + lowShelfFreqParam.load(std::memory_order_relaxed) * 480.0f; // 20-500Hz
    const float lowGainDb = -12.0f + lowShelfGainParam.load(std::memory_order_relaxed) * 24.0f;
    const float lowA = std::pow(10.0f, lowGainDb / 40.0f);
    
    // High shelf parameters
    const float highFreq = 2000.0f + highShelfFreqParam.load(std::memory_order_relaxed) * 14000.0f; // 2k-16kHz
    const float highGainDb = -12.0f + highShelfGainParam.load(std::memory_order_relaxed) * 24.0f;
    const float highA = std::pow(10.0f, highGainDb / 40.0f);
    
    const float S = 1.0f; // Shelf slope
    
    // Low shelf coefficients
    {
        const float omega = juce::MathConstants<float>::twoPi * lowFreq / static_cast<float>(currentSampleRate);
        const float sn = std::sin(omega);
        const float cs = std::cos(omega);
        const float beta = std::sqrt(lowA) / S;
        
        const float b0 = lowA * ((lowA + 1.0f) - (lowA - 1.0f) * cs + beta * sn);
        const float b1 = 2.0f * lowA * ((lowA - 1.0f) - (lowA + 1.0f) * cs);
        const float b2 = lowA * ((lowA + 1.0f) - (lowA - 1.0f) * cs - beta * sn);
        const float a0 = (lowA + 1.0f) + (lowA - 1.0f) * cs + beta * sn;
        const float a1 = -2.0f * ((lowA - 1.0f) + (lowA + 1.0f) * cs);
        const float a2 = (lowA + 1.0f) + (lowA - 1.0f) * cs - beta * sn;
        
        for (auto& ch : channelState)
        {
            ch.lowShelf.b0 = b0 / a0;
            ch.lowShelf.b1 = b1 / a0;
            ch.lowShelf.b2 = b2 / a0;
            ch.lowShelf.a1 = a1 / a0;
            ch.lowShelf.a2 = a2 / a0;
        }
    }
    
    // High shelf coefficients
    {
        const float omega = juce::MathConstants<float>::twoPi * highFreq / static_cast<float>(currentSampleRate);
        const float sn = std::sin(omega);
        const float cs = std::cos(omega);
        const float beta = std::sqrt(highA) / S;
        
        const float b0 = highA * ((highA + 1.0f) + (highA - 1.0f) * cs + beta * sn);
        const float b1 = -2.0f * highA * ((highA - 1.0f) + (highA + 1.0f) * cs);
        const float b2 = highA * ((highA + 1.0f) + (highA - 1.0f) * cs - beta * sn);
        const float a0 = (highA + 1.0f) - (highA - 1.0f) * cs + beta * sn;
        const float a1 = 2.0f * ((highA - 1.0f) - (highA + 1.0f) * cs);
        const float a2 = (highA + 1.0f) - (highA - 1.0f) * cs - beta * sn;
        
        for (auto& ch : channelState)
        {
            ch.highShelf.b0 = b0 / a0;
            ch.highShelf.b1 = b1 / a0;
            ch.highShelf.b2 = b2 / a0;
            ch.highShelf.a1 = a1 / a0;
            ch.highShelf.a2 = a2 / a0;
        }
    }
}

void ShelvingEQ::processInternal(AudioBuffer<float>& buffer)
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
                float sample = channelData[i];
                
                // Apply low shelf
                {
                    auto& filter = state.lowShelf;
                    const float output = filter.b0 * sample + filter.b1 * filter.z1 + filter.b2 * filter.z2
                                       - filter.a1 * filter.z1 - filter.a2 * filter.z2;
                    filter.z2 = filter.z1;
                    filter.z1 = sample;
                    sample = output;
                }
                
                // Apply high shelf
                {
                    auto& filter = state.highShelf;
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

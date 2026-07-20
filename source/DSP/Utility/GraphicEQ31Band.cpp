#include "GraphicEQ31Band.h"
#include <cmath>

GraphicEQ31Band::GraphicEQ31Band() : AudioModule("GraphicEQ31Band")
{
    // Initialize all bands to 0dB (0.5 normalized)
    for (int i = 0; i < numBands; ++i)
        bandGains[i].store(0.5f, std::memory_order_relaxed);
}

GraphicEQ31Band::~GraphicEQ31Band()
{
}

void GraphicEQ31Band::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    updateFilterCoefficients();
    reset();
}

void GraphicEQ31Band::reset()
{
    for (auto& ch : channelState)
    {
        for (int i = 0; i < numBands; ++i)
        {
            ch.filters[i].z1 = 0.0f;
            ch.filters[i].z2 = 0.0f;
        }
    }
}

void GraphicEQ31Band::setBandGain(int bandIndex, float gain)
{
    if (bandIndex >= 0 && bandIndex < numBands)
    {
        bandGains[bandIndex].store(jlimit(0.0f, 1.0f, gain), std::memory_order_relaxed);
        updateFilterCoefficients();
    }
}

void GraphicEQ31Band::updateFilterCoefficients()
{
    const float Q = 2.14f; // 1/3 octave bandwidth
    
    for (int i = 0; i < numBands; ++i)
    {
        const float fc = frequencies[i];
        if (fc >= currentSampleRate * 0.48f) continue; // Skip frequencies too close to Nyquist
        
        const float gainDb = -12.0f + bandGains[i].load(std::memory_order_relaxed) * 24.0f;
        const float A = std::pow(10.0f, gainDb / 40.0f);
        
        const float omega = juce::MathConstants<float>::twoPi * fc / static_cast<float>(currentSampleRate);
        const float sn = std::sin(omega);
        const float cs = std::cos(omega);
        const float alpha = sn / (2.0f * Q);
        
        const float b0 = 1.0f + alpha * A;
        const float b1 = -2.0f * cs;
        const float b2 = 1.0f - alpha * A;
        const float a0 = 1.0f + alpha / A;
        const float a1 = -2.0f * cs;
        const float a2 = 1.0f - alpha / A;
        
        for (auto& ch : channelState)
        {
            ch.filters[i].b0 = b0 / a0;
            ch.filters[i].b1 = b1 / a0;
            ch.filters[i].b2 = b2 / a0;
            ch.filters[i].a1 = a1 / a0;
            ch.filters[i].a2 = a2 / a0;
        }
    }
}

void GraphicEQ31Band::processInternal(AudioBuffer<float>& buffer)
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
                
                // Apply all 31 band filters
                for (int band = 0; band < numBands; ++band)
                {
                    auto& filter = state.filters[band];
                    
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

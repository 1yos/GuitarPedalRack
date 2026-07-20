#include "GraphicEQ10Band.h"
#include <cmath>

GraphicEQ10Band::GraphicEQ10Band() : AudioModule("GraphicEQ10Band")
{
}

GraphicEQ10Band::~GraphicEQ10Band()
{
}

void GraphicEQ10Band::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    updateFilterCoefficients();
    reset();
}

void GraphicEQ10Band::reset()
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

void GraphicEQ10Band::setBand31Hz(float gain) { bandGains[0].store(jlimit(0.0f, 1.0f, gain), std::memory_order_relaxed); updateFilterCoefficients(); }
void GraphicEQ10Band::setBand62Hz(float gain) { bandGains[1].store(jlimit(0.0f, 1.0f, gain), std::memory_order_relaxed); updateFilterCoefficients(); }
void GraphicEQ10Band::setBand125Hz(float gain) { bandGains[2].store(jlimit(0.0f, 1.0f, gain), std::memory_order_relaxed); updateFilterCoefficients(); }
void GraphicEQ10Band::setBand250Hz(float gain) { bandGains[3].store(jlimit(0.0f, 1.0f, gain), std::memory_order_relaxed); updateFilterCoefficients(); }
void GraphicEQ10Band::setBand500Hz(float gain) { bandGains[4].store(jlimit(0.0f, 1.0f, gain), std::memory_order_relaxed); updateFilterCoefficients(); }
void GraphicEQ10Band::setBand1kHz(float gain) { bandGains[5].store(jlimit(0.0f, 1.0f, gain), std::memory_order_relaxed); updateFilterCoefficients(); }
void GraphicEQ10Band::setBand2kHz(float gain) { bandGains[6].store(jlimit(0.0f, 1.0f, gain), std::memory_order_relaxed); updateFilterCoefficients(); }
void GraphicEQ10Band::setBand4kHz(float gain) { bandGains[7].store(jlimit(0.0f, 1.0f, gain), std::memory_order_relaxed); updateFilterCoefficients(); }
void GraphicEQ10Band::setBand8kHz(float gain) { bandGains[8].store(jlimit(0.0f, 1.0f, gain), std::memory_order_relaxed); updateFilterCoefficients(); }
void GraphicEQ10Band::setBand16kHz(float gain) { bandGains[9].store(jlimit(0.0f, 1.0f, gain), std::memory_order_relaxed); updateFilterCoefficients(); }

void GraphicEQ10Band::updateFilterCoefficients()
{
    const float Q = 1.4f; // Filter bandwidth
    
    for (int i = 0; i < numBands; ++i)
    {
        const float fc = frequencies[i];
        const float gainDb = -12.0f + bandGains[i].load(std::memory_order_relaxed) * 24.0f; // -12dB to +12dB
        const float A = std::pow(10.0f, gainDb / 40.0f);
        
        const float omega = juce::MathConstants<float>::twoPi * fc / static_cast<float>(currentSampleRate);
        const float sn = std::sin(omega);
        const float cs = std::cos(omega);
        const float alpha = sn / (2.0f * Q);
        
        // Peak filter coefficients
        const float b0 = 1.0f + alpha * A;
        const float b1 = -2.0f * cs;
        const float b2 = 1.0f - alpha * A;
        const float a0 = 1.0f + alpha / A;
        const float a1 = -2.0f * cs;
        const float a2 = 1.0f - alpha / A;
        
        // Store normalized coefficients
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

void GraphicEQ10Band::processInternal(AudioBuffer<float>& buffer)
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
                
                // Apply all band filters in series
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

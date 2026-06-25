#include "EQ.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

EQ::EQ()
    : AudioModule("EQ")
{
    // Initialize bands with different default frequencies
    bands[0].frequency = 100.0f;   // Low
    bands[1].frequency = 500.0f;   // Low-mid
    bands[2].frequency = 2000.0f;  // High-mid
    bands[3].frequency = 8000.0f;  // High
    
    for (int i = 0; i < NUM_BANDS; ++i)
    {
        bands[i].q = 1.0f;
        bands[i].gainDb = 0.0f;
        bands[i].enabled = true;
    }
}

void EQ::prepare(double sampleRate, int maxBlockSize)
{
    AudioModule::prepare(sampleRate, maxBlockSize);
    currentSampleRate = sampleRate;
    
    for (int i = 0; i < NUM_BANDS; ++i)
        updateBandCoefficients(i);
    
    updateFilterCoefficients();
    reset();
}

void EQ::reset()
{
    AudioModule::reset();
    
    // Clear filter states
    for (int i = 0; i < NUM_BANDS; ++i)
    {
        for (int ch = 0; ch < 2; ++ch)
        {
            bands[i].z1[ch] = 0.0f;
            bands[i].z2[ch] = 0.0f;
        }
    }
    
    for (int ch = 0; ch < 2; ++ch)
    {
        hpZ1[ch] = hpZ2[ch] = 0.0f;
        lpZ1[ch] = lpZ2[ch] = 0.0f;
    }
}

void EQ::processInternal(AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    if (numChannels == 0 || numSamples == 0)
        return;
    
    for (int channel = 0; channel < numChannels && channel < 2; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = channelData[i];
            
            // High-pass filter
            if (highPassEnabled)
            {
                float filtered = hpA0 * sample + hpA1 * hpZ1[channel] + hpA2 * hpZ2[channel];
                filtered -= hpB1 * hpZ1[channel] + hpB2 * hpZ2[channel];
                hpZ2[channel] = hpZ1[channel];
                hpZ1[channel] = filtered;
                sample = filtered;
            }
            
            // Process each parametric band
            for (int band = 0; band < NUM_BANDS; ++band)
            {
                if (bands[band].enabled && std::abs(bands[band].gainDb) > 0.1f)
                {
                    sample = processBiquad(sample, bands[band], channel);
                }
            }
            
            // Low-pass filter
            if (lowPassEnabled)
            {
                float filtered = lpA0 * sample + lpA1 * lpZ1[channel] + lpA2 * lpZ2[channel];
                filtered -= lpB1 * lpZ1[channel] + lpB2 * lpZ2[channel];
                lpZ2[channel] = lpZ1[channel];
                lpZ1[channel] = filtered;
                sample = filtered;
            }
            
            channelData[i] = sample;
        }
    }
}

void EQ::setBandFrequency(int band, float frequency)
{
    if (band >= 0 && band < NUM_BANDS)
    {
        bands[band].frequency = jlimit(20.0f, 20000.0f, frequency);
        updateBandCoefficients(band);
    }
}

void EQ::setBandQ(int band, float q)
{
    if (band >= 0 && band < NUM_BANDS)
    {
        bands[band].q = jlimit(0.1f, 10.0f, q);
        updateBandCoefficients(band);
    }
}

void EQ::setBandGain(int band, float gainDb)
{
    if (band >= 0 && band < NUM_BANDS)
    {
        bands[band].gainDb = jlimit(-15.0f, 15.0f, gainDb);
        updateBandCoefficients(band);
    }
}

void EQ::setBandEnabled(int band, bool enabled)
{
    if (band >= 0 && band < NUM_BANDS)
        bands[band].enabled = enabled;
}

void EQ::setHighPassFreq(float frequency)
{
    highPassFreq = jlimit(20.0f, 1000.0f, frequency);
    updateFilterCoefficients();
}

void EQ::setLowPassFreq(float frequency)
{
    lowPassFreq = jlimit(1000.0f, 20000.0f, frequency);
    updateFilterCoefficients();
}

void EQ::setHighPassEnabled(bool enabled)
{
    highPassEnabled = enabled;
}

void EQ::setLowPassEnabled(bool enabled)
{
    lowPassEnabled = enabled;
}

float EQ::getBandFrequency(int band) const
{
    return (band >= 0 && band < NUM_BANDS) ? bands[band].frequency : 0.0f;
}

float EQ::getBandQ(int band) const
{
    return (band >= 0 && band < NUM_BANDS) ? bands[band].q : 0.0f;
}

float EQ::getBandGain(int band) const
{
    return (band >= 0 && band < NUM_BANDS) ? bands[band].gainDb : 0.0f;
}

bool EQ::getBandEnabled(int band) const
{
    return (band >= 0 && band < NUM_BANDS) ? bands[band].enabled : false;
}

void EQ::updateBandCoefficients(int band)
{
    if (band < 0 || band >= NUM_BANDS)
        return;
    
    Band& b = bands[band];
    
    // Peaking EQ filter (biquad)
    double omega = 2.0 * M_PI * b.frequency / currentSampleRate;
    double sinOmega = std::sin(omega);
    double cosOmega = std::cos(omega);
    double alpha = sinOmega / (2.0 * b.q);
    double A = std::pow(10.0, b.gainDb / 40.0); // Gain as amplitude
    
    double a0 = 1.0 + alpha / A;
    double a1 = -2.0 * cosOmega;
    double a2 = 1.0 - alpha / A;
    double b0 = 1.0 + alpha * A;
    double b1 = -2.0 * cosOmega;
    double b2 = 1.0 - alpha * A;
    
    // Normalize
    b.a0 = (float)(b0 / a0);
    b.a1 = (float)(b1 / a0);
    b.a2 = (float)(b2 / a0);
    b.b1 = (float)(a1 / a0);
    b.b2 = (float)(a2 / a0);
}

void EQ::updateFilterCoefficients()
{
    // High-pass filter (2nd order Butterworth)
    {
        double omega = 2.0 * M_PI * highPassFreq / currentSampleRate;
        double sinOmega = std::sin(omega);
        double cosOmega = std::cos(omega);
        double alpha = sinOmega / (2.0 * 0.707); // Q = 0.707 for Butterworth
        
        double a0 = 1.0 + alpha;
        double a1 = -2.0 * cosOmega;
        double a2 = 1.0 - alpha;
        double b0 = (1.0 + cosOmega) / 2.0;
        double b1 = -(1.0 + cosOmega);
        double b2 = (1.0 + cosOmega) / 2.0;
        
        hpA0 = (float)(b0 / a0);
        hpA1 = (float)(b1 / a0);
        hpA2 = (float)(b2 / a0);
        hpB1 = (float)(a1 / a0);
        hpB2 = (float)(a2 / a0);
    }
    
    // Low-pass filter (2nd order Butterworth)
    {
        double omega = 2.0 * M_PI * lowPassFreq / currentSampleRate;
        double sinOmega = std::sin(omega);
        double cosOmega = std::cos(omega);
        double alpha = sinOmega / (2.0 * 0.707);
        
        double a0 = 1.0 + alpha;
        double a1 = -2.0 * cosOmega;
        double a2 = 1.0 - alpha;
        double b0 = (1.0 - cosOmega) / 2.0;
        double b1 = 1.0 - cosOmega;
        double b2 = (1.0 - cosOmega) / 2.0;
        
        lpA0 = (float)(b0 / a0);
        lpA1 = (float)(b1 / a0);
        lpA2 = (float)(b2 / a0);
        lpB1 = (float)(a1 / a0);
        lpB2 = (float)(a2 / a0);
    }
}

float EQ::processBiquad(float sample, Band& band, int channel)
{
    float output = band.a0 * sample + band.a1 * band.z1[channel] + band.a2 * band.z2[channel];
    output -= band.b1 * band.z1[channel] + band.b2 * band.z2[channel];
    
    band.z2[channel] = band.z1[channel];
    band.z1[channel] = output;
    
    return output;
}

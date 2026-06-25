#include "Distortion.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Distortion::Distortion()
    : AudioModule("Distortion"),
      oversampling(2, 2, dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, false)
{
    updateParameters();
}

void Distortion::prepare(double sampleRate, int maxBlockSize)
{
    AudioModule::prepare(sampleRate, maxBlockSize);
    currentSampleRate = sampleRate;
    
    // Prepare oversampling
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (uint32)maxBlockSize;
    spec.numChannels = 2;
    oversampling.initProcessing(maxBlockSize);
    
    updateFilterCoefficients();
    reset();
}

void Distortion::reset()
{
    AudioModule::reset();
    
    // Clear filter states
    for (int i = 0; i < 2; ++i)
    {
        preEmphasisZ1[i] = 0.0f;
        toneZ1[i] = 0.0f;
        filterZ1[i] = 0.0f;
        filterZ2[i] = 0.0f;
    }
    
    oversampling.reset();
}

void Distortion::processInternal(AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    if (numChannels == 0 || numSamples == 0)
        return;
    
    updateParameters();
    
    // Upsample
    dsp::AudioBlock<float> block(buffer);
    auto oversampledBlock = oversampling.processSamplesUp(block);
    
    // Process each channel
    for (int channel = 0; channel < numChannels && channel < 2; ++channel)
    {
        float* channelData = oversampledBlock.getChannelPointer(channel);
        const int oversampledSamples = (int)oversampledBlock.getNumSamples();
        
        for (int i = 0; i < oversampledSamples; ++i)
        {
            float sample = channelData[i];
            
            // Pre-emphasis filter (high-pass to boost highs before clipping)
            float preEmph = sample - preEmphasisCoeff * preEmphasisZ1[channel];
            preEmphasisZ1[channel] = sample;
            
            // Apply drive/gain
            float driven = preEmph * drive;
            
            // Hard clipping
            float clipped = hardClip(driven);
            
            // Tone filter (low-pass to control brightness)
            float toneFiltered = toneCoeff * clipped + (1.0f - toneCoeff) * toneZ1[channel];
            toneZ1[channel] = toneFiltered;
            
            // Post filter (resonant low-pass for character)
            float filtered = filterA0 * toneFiltered + filterA1 * filterZ1[channel] + filterA2 * filterZ2[channel];
            filtered -= filterB1 * filterZ1[channel] + filterB2 * filterZ2[channel];
            
            filterZ2[channel] = filterZ1[channel];
            filterZ1[channel] = filtered;
            
            // Output level
            channelData[i] = filtered * outputLevel;
        }
    }
    
    // Downsample
    oversampling.processSamplesDown(block);
}

void Distortion::setDrive(float drive)
{
    driveParam = jlimit(0.0f, 1.0f, drive);
}

void Distortion::setTone(float tone)
{
    toneParam = jlimit(0.0f, 1.0f, tone);
    updateFilterCoefficients();
}

void Distortion::setLevel(float level)
{
    levelParam = jlimit(0.0f, 1.0f, level);
}

void Distortion::setFilter(float filter)
{
    filterParam = jlimit(0.0f, 1.0f, filter);
    updateFilterCoefficients();
}

void Distortion::updateParameters()
{
    // Drive: 1.0 to 100.0 (exponential scaling)
    drive = std::pow(10.0f, driveParam * 2.0f); // 10^0 = 1, 10^2 = 100
    
    // Output level compensation (inverse of drive)
    outputLevel = levelParam / std::sqrt(drive);
    
    // Tone coefficient (0.0 = bright, 1.0 = dark)
    toneCoeff = 0.3f + toneParam * 0.65f;
}

void Distortion::updateFilterCoefficients()
{
    // Resonant low-pass filter (biquad)
    // Frequency: 500 Hz to 5000 Hz
    filterFreq = 500.0f + filterParam * 4500.0f;
    
    // Q (resonance): 0.5 to 3.0
    filterQ = 0.5f + filterParam * 2.5f;
    
    // Calculate biquad coefficients
    double omega = 2.0 * M_PI * filterFreq / (currentSampleRate * 4.0); // Account for oversampling
    double sinOmega = std::sin(omega);
    double cosOmega = std::cos(omega);
    double alpha = sinOmega / (2.0 * filterQ);
    
    double a0 = 1.0 + alpha;
    double a1 = -2.0 * cosOmega;
    double a2 = 1.0 - alpha;
    double b0 = (1.0 - cosOmega) / 2.0;
    double b1 = 1.0 - cosOmega;
    double b2 = (1.0 - cosOmega) / 2.0;
    
    // Normalize
    filterA0 = (float)(b0 / a0);
    filterA1 = (float)(b1 / a0);
    filterA2 = (float)(b2 / a0);
    filterB1 = (float)(a1 / a0);
    filterB2 = (float)(a2 / a0);
}

float Distortion::hardClip(float sample)
{
    // Hard clipping with slight softening at edges
    if (sample > 1.0f)
        return 1.0f;
    else if (sample < -1.0f)
        return -1.0f;
    else
        return sample;
}

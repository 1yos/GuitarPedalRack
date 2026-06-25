#include "TubeOverdrive.h"
#include <cmath>

TubeOverdrive::TubeOverdrive() 
    : AudioModule("TubeOverdrive"),
      oversampling(2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, false)
{
}

void TubeOverdrive::prepare(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    AudioModule::prepare(sampleRate, maximumExpectedSamplesPerBlock);
    
    // Prepare oversampling (4x oversampling)
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<uint32_t>(maximumExpectedSamplesPerBlock);
    spec.numChannels = 2;
    
    oversampling.initProcessing(static_cast<size_t>(maximumExpectedSamplesPerBlock));
    
    // Prepare filters at oversampled rate
    double oversampledRate = sampleRate * oversampling.getOversamplingFactor();
    
    // Pre-emphasis filter (720Hz resonant peak for mid-boost)
    auto preEmphasisCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        oversampledRate, 720.0, 4.0, 1.5);
    preEmphasisFilter.coefficients = preEmphasisCoeffs;
    
    updateFilters();
    reset();
}

void TubeOverdrive::reset()
{
    oversampling.reset();
    preEmphasisFilter.reset();
    toneFilterLow.reset();
    toneFilterHigh.reset();
}

void TubeOverdrive::setDrive(float newDrive)
{
    drive = jlimit(0.0f, 1.0f, newDrive);
}

void TubeOverdrive::setTone(float newTone)
{
    tone = jlimit(0.0f, 1.0f, newTone);
    updateFilters();
}

void TubeOverdrive::setLevel(float newLevel)
{
    level = jlimit(0.0f, 1.0f, newLevel);
}

void TubeOverdrive::processInternal(AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Read parameters from APVTS (if connected)
    if (hasParameter("drive"))
        drive = getParameterValue("drive");
    if (hasParameter("tone"))
    {
        float newTone = getParameterValue("tone");
        if (std::abs(newTone - tone) > 0.01f)
        {
            tone = newTone;
            updateFilters();
        }
    }
    if (hasParameter("level"))
        level = getParameterValue("level");
    if (hasParameter("bypass"))
    {
        bool shouldBypass = getParameterValue("bypass") > 0.5f;
        setBypass(shouldBypass);
    }
    
    // Upsample
    juce::dsp::AudioBlock<float> block(buffer);
    auto oversampledBlock = oversampling.processSamplesUp(block);
    
    const int oversampledSamples = static_cast<int>(oversampledBlock.getNumSamples());
    
    // Calculate gain from drive control (1x to 100x)
    float driveGain = 1.0f + drive * 99.0f;
    
    for (size_t channel = 0; channel < oversampledBlock.getNumChannels(); ++channel)
    {
        auto* channelData = oversampledBlock.getChannelPointer(channel);
        
        for (int sample = 0; sample < oversampledSamples; ++sample)
        {
            float inputSample = channelData[sample];
            
            // Apply input gain
            float gained = inputSample * driveGain;
            
            // Pre-emphasis (mid boost before clipping)
            gained = preEmphasisFilter.processSample(gained);
            
            // Asymmetric soft clipping (tube-like)
            float clipped = asymmetricClip(gained, drive);
            
            // Output level
            channelData[sample] = clipped * level;
        }
    }
    
    // Downsample
    oversampling.processSamplesDown(block);
    
    // Apply tone control (post-clipping)
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float sampleValue = channelData[sample];
            
            // Blend between low-passed and high-passed versions
            float lowPassed = toneFilterLow.processSample(sampleValue);
            float highPassed = toneFilterHigh.processSample(sampleValue);
            
            channelData[sample] = lowPassed * (1.0f - tone) + highPassed * tone;
        }
    }
}

float TubeOverdrive::asymmetricClip(float sample, float driveAmount)
{
    // Asymmetric soft clipping (simulates diode clipping)
    // Positive side clips harder than negative (asymmetric)
    
    if (sample > 0.0f)
    {
        // Positive side - harder clipping
        float threshold = 0.7f - driveAmount * 0.5f;
        if (sample > threshold)
        {
            float excess = sample - threshold;
            sample = threshold + std::tanh(excess * 2.0f) * 0.3f;
        }
    }
    else
    {
        // Negative side - softer clipping
        float threshold = -0.9f + driveAmount * 0.3f;
        if (sample < threshold)
        {
            float excess = sample - threshold;
            sample = threshold + std::tanh(excess) * 0.3f;
        }
    }
    
    // Final soft clipping to prevent any clipping
    return std::tanh(sample * 0.8f);
}

void TubeOverdrive::updateFilters()
{
    if (currentSampleRate <= 0.0)
        return;
    
    // Tone control: blend between low-pass and high-pass
    // Simulates the classic Tube Screamer tone circuit
    
    double cutoffFreq = 500.0 + tone * 3000.0; // 500Hz to 3.5kHz
    
    auto lowPassCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(
        currentSampleRate, cutoffFreq);
    toneFilterLow.coefficients = lowPassCoeffs;
    
    auto highPassCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        currentSampleRate, cutoffFreq * 0.5);
    toneFilterHigh.coefficients = highPassCoeffs;
}

void TubeOverdrive::saveState(MemoryBlock& destData)
{
    AudioModule::saveState(destData);
    
    MemoryOutputStream stream(destData, true);
    stream.writeFloat(drive);
    stream.writeFloat(tone);
    stream.writeFloat(level);
}

void TubeOverdrive::loadState(const void* data, int sizeInBytes)
{
    AudioModule::loadState(data, sizeInBytes);
    
    MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    stream.readFloat(); // wetDryMix
    stream.readBool();  // bypassed
    
    setDrive(stream.readFloat());
    setTone(stream.readFloat());
    setLevel(stream.readFloat());
}

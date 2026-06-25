#include "AmpSimulator.h"
#include <cmath>

AmpSimulator::AmpSimulator()
    : AudioModule("AmpSimulator"),
      oversampling(2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, false)
{
}

void AmpSimulator::prepare(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    AudioModule::prepare(sampleRate, maximumExpectedSamplesPerBlock);
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<uint32_t>(maximumExpectedSamplesPerBlock);
    spec.numChannels = 2;
    
    oversampling.initProcessing(static_cast<size_t>(maximumExpectedSamplesPerBlock));
    
    // Input high-pass to remove DC and sub-bass
    auto hpCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 80.0);
    inputHighPass.coefficients = hpCoeffs;
    
    updateFilters();
    reset();
}

void AmpSimulator::reset()
{
    oversampling.reset();
    bassFilter.reset();
    midFilter.reset();
    trebleFilter.reset();
    presenceFilter.reset();
    inputHighPass.reset();
}

void AmpSimulator::setAmpModel(AmpModel model)
{
    ampModel = model;
    updateFilters();
}

void AmpSimulator::setGain(float newGain)
{
    gain = jlimit(0.0f, 1.0f, newGain);
}

void AmpSimulator::setBass(float newBass)
{
    bass = jlimit(0.0f, 1.0f, newBass);
    updateFilters();
}

void AmpSimulator::setMid(float newMid)
{
    mid = jlimit(0.0f, 1.0f, newMid);
    updateFilters();
}

void AmpSimulator::setTreble(float newTreble)
{
    treble = jlimit(0.0f, 1.0f, newTreble);
    updateFilters();
}

void AmpSimulator::setPresence(float newPresence)
{
    presence = jlimit(0.0f, 1.0f, newPresence);
    updateFilters();
}

void AmpSimulator::processInternal(AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Read parameters from APVTS (if connected)
    if (hasParameter("gain"))
        gain = getParameterValue("gain") / 10.0f; // Scale from 0-10 to 0-1
    if (hasParameter("bass"))
    {
        float newBass = getParameterValue("bass") / 10.0f;
        if (std::abs(newBass - bass) > 0.01f)
        {
            bass = newBass;
            updateFilters();
        }
    }
    if (hasParameter("mid"))
    {
        float newMid = getParameterValue("mid") / 10.0f;
        if (std::abs(newMid - mid) > 0.01f)
        {
            mid = newMid;
            updateFilters();
        }
    }
    if (hasParameter("treble"))
    {
        float newTreble = getParameterValue("treble") / 10.0f;
        if (std::abs(newTreble - treble) > 0.01f)
        {
            treble = newTreble;
            updateFilters();
        }
    }
    if (hasParameter("presence"))
    {
        float newPresence = getParameterValue("presence") / 10.0f;
        if (std::abs(newPresence - presence) > 0.01f)
        {
            presence = newPresence;
            updateFilters();
        }
    }
    if (hasParameter("master"))
        master = getParameterValue("master") / 10.0f;
    if (hasParameter("channel"))
    {
        int channelValue = static_cast<int>(getParameterValue("channel"));
        ampModel = static_cast<AmpModel>(channelValue);
    }
    if (hasParameter("bypass"))
    {
        bool shouldBypass = getParameterValue("bypass") > 0.5f;
        setBypass(shouldBypass);
    }
    
    // Input filtering (remove DC)
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numSamples; ++sample)
            channelData[sample] = inputHighPass.processSample(channelData[sample]);
    }
    
    // Upsample for nonlinear processing
    juce::dsp::AudioBlock<float> block(buffer);
    auto oversampledBlock = oversampling.processSamplesUp(block);
    
    const int oversampledSamples = static_cast<int>(oversampledBlock.getNumSamples());
    
    // Calculate gain stages based on amp model
    float preampGain = 1.0f + gain * (ampModel == AmpModel::HighGain ? 100.0f : 
                                     ampModel == AmpModel::Crunch ? 30.0f : 5.0f);
    
    for (size_t channel = 0; channel < oversampledBlock.getNumChannels(); ++channel)
    {
        auto* channelData = oversampledBlock.getChannelPointer(channel);
        
        for (int sample = 0; sample < oversampledSamples; ++sample)
        {
            float sampleValue = channelData[sample];
            
            // Preamp stage (gain + clipping)
            sampleValue = preampStage(sampleValue, preampGain);
            
            // Power amp stage (compression + saturation)
            sampleValue = powerAmpStage(sampleValue);
            
            channelData[sample] = sampleValue;
        }
    }
    
    // Downsample
    oversampling.processSamplesDown(block);
    
    // Apply tone stack (post-preamp)
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float sampleValue = channelData[sample];
            
            // Tone stack filtering
            sampleValue = bassFilter.processSample(sampleValue);
            sampleValue = midFilter.processSample(sampleValue);
            sampleValue = trebleFilter.processSample(sampleValue);
            sampleValue = presenceFilter.processSample(sampleValue);
            
            channelData[sample] = sampleValue * 0.5f;  // Output level adjustment
        }
    }
}

float AmpSimulator::preampStage(float sample, float gainAmount)
{
    // Apply gain
    sample *= gainAmount;
    
    // Tube-style soft clipping (progressive saturation)
    if (std::abs(sample) > 0.1f)
        sample = tubeClip(sample);
    
    return sample;
}

float AmpSimulator::powerAmpStage(float sample)
{
    // Power amp compression (soft limiting)
    float compressed = std::tanh(sample * 0.8f);
    
    // Add subtle asymmetric distortion (tube characteristic)
    if (compressed > 0.0f)
        compressed *= 0.95f;  // Compress positive peaks slightly more
    
    return compressed;
}

float AmpSimulator::tubeClip(float sample)
{
    // Smooth tube-like saturation curve
    // Uses tanh for soft clipping with pre-emphasis
    
    float sign = (sample >= 0.0f) ? 1.0f : -1.0f;
    float abs = std::abs(sample);
    
    // Progressive clipping (harder as level increases)
    if (abs < 0.5f)
        return sample;
    else if (abs < 1.0f)
        return sign * (0.5f + (abs - 0.5f) * 0.8f);
    else
        return sign * (0.9f + std::tanh((abs - 1.0f) * 2.0f) * 0.1f);
}

void AmpSimulator::updateFilters()
{
    if (currentSampleRate <= 0.0)
        return;
    
    // Tone stack modeling (simplified reactive tone stack)
    
    // Bass: Low shelf filter (80Hz center)
    float bassGain = bass * 18.0f - 9.0f;  // -9dB to +9dB
    auto bassCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        currentSampleRate, 120.0, 0.707, std::pow(10.0f, bassGain / 20.0f));
    bassFilter.coefficients = bassCoeffs;
    
    // Mid: Peak filter (800Hz center)
    float midGain = mid * 18.0f - 9.0f;
    auto midCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        currentSampleRate, 800.0, 1.0, std::pow(10.0f, midGain / 20.0f));
    midFilter.coefficients = midCoeffs;
    
    // Treble: High shelf filter (3kHz center)
    float trebleGain = treble * 18.0f - 9.0f;
    auto trebleCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        currentSampleRate, 3000.0, 0.707, std::pow(10.0f, trebleGain / 20.0f));
    trebleFilter.coefficients = trebleCoeffs;
    
    // Presence: High shelf filter (5kHz center, post power-amp)
    float presenceGain = presence * 12.0f - 6.0f;
    auto presenceCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        currentSampleRate, 5000.0, 0.707, std::pow(10.0f, presenceGain / 20.0f));
    presenceFilter.coefficients = presenceCoeffs;
}

void AmpSimulator::saveState(MemoryBlock& destData)
{
    AudioModule::saveState(destData);
    
    MemoryOutputStream stream(destData, true);
    stream.writeInt(static_cast<int>(ampModel));
    stream.writeFloat(gain);
    stream.writeFloat(bass);
    stream.writeFloat(mid);
    stream.writeFloat(treble);
    stream.writeFloat(presence);
}

void AmpSimulator::loadState(const void* data, int sizeInBytes)
{
    AudioModule::loadState(data, sizeInBytes);
    
    MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    stream.readFloat(); // wetDryMix
    stream.readBool();  // bypassed
    
    setAmpModel(static_cast<AmpModel>(stream.readInt()));
    setGain(stream.readFloat());
    setBass(stream.readFloat());
    setMid(stream.readFloat());
    setTreble(stream.readFloat());
    setPresence(stream.readFloat());
}

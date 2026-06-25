#include "NoiseGate.h"
#include <cmath>

NoiseGate::NoiseGate() : AudioModule("NoiseGate")
{
    updateCoefficients();
}

void NoiseGate::prepare(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    AudioModule::prepare(sampleRate, maximumExpectedSamplesPerBlock);
    
    // Allocate per-channel state
    envelopeState.resize(2, 0.0f);  // Stereo
    gateOpen.resize(2, false);
    
    updateCoefficients();
    reset();
}

void NoiseGate::reset()
{
    std::fill(envelopeState.begin(), envelopeState.end(), 0.0f);
    std::fill(gateOpen.begin(), gateOpen.end(), false);
}

void NoiseGate::setThreshold(float thresholdDb_)
{
    thresholdDb = jlimit(-80.0f, 0.0f, thresholdDb_);
    threshold = dbToLinear(thresholdDb);
    closeThreshold = dbToLinear(thresholdDb - hysteresisDb);
}

void NoiseGate::setRatio(float newRatio)
{
    ratio = jlimit(1.0f, 100.0f, newRatio);
}

void NoiseGate::setAttack(float attackMs_)
{
    attackMs = jlimit(0.1f, 50.0f, attackMs_);
    updateCoefficients();
}

void NoiseGate::setRelease(float releaseMs_)
{
    releaseMs = jlimit(10.0f, 1000.0f, releaseMs_);
    updateCoefficients();
}

void NoiseGate::setHysteresis(float hysteresisDb_)
{
    hysteresisDb = jlimit(0.0f, 10.0f, hysteresisDb_);
    closeThreshold = dbToLinear(thresholdDb - hysteresisDb);
}

void NoiseGate::processInternal(AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    // Ensure we have enough state for all channels
    if ((int)envelopeState.size() < numChannels)
    {
        envelopeState.resize(numChannels, 0.0f);
        gateOpen.resize(numChannels, false);
    }
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        float& envelope = envelopeState[channel];
        bool isOpen = gateOpen[channel];
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Get input level (absolute value)
            const float inputLevel = std::abs(channelData[sample]);
            
            // Envelope follower
            if (inputLevel > envelope)
                envelope += attackCoeff * (inputLevel - envelope);
            else
                envelope += releaseCoeff * (inputLevel - envelope);
            
            // Gate state with hysteresis
            if (isOpen)
            {
                // Gate is open - check if we should close (using lower threshold)
                if (envelope < closeThreshold)
                    isOpen = false;
            }
            else
            {
                // Gate is closed - check if we should open (using higher threshold)
                if (envelope > threshold)
                    isOpen = true;
            }
            
            // Apply gain reduction
            if (isOpen)
            {
                // Gate open - pass signal through
                // No change to signal
            }
            else
            {
                // Gate closed - apply attenuation based on ratio
                float attenuation = 1.0f / ratio;
                channelData[sample] *= attenuation;
            }
        }
        
        // Update the gate state back to the vector
        gateOpen[channel] = isOpen;
    }
}

void NoiseGate::updateCoefficients()
{
    if (currentSampleRate <= 0.0)
        return;
    
    // Calculate time constants for envelope follower
    // Formula: coeff = 1 - exp(-1 / (time_ms * sampleRate / 1000))
    
    const float attackTimeSeconds = attackMs * 0.001f;
    const float releaseTimeSeconds = releaseMs * 0.001f;
    
    attackCoeff = 1.0f - std::exp(-1.0f / (attackTimeSeconds * static_cast<float>(currentSampleRate)));
    releaseCoeff = 1.0f - std::exp(-1.0f / (releaseTimeSeconds * static_cast<float>(currentSampleRate)));
}

float NoiseGate::dbToLinear(float db)
{
    return std::pow(10.0f, db / 20.0f);
}

float NoiseGate::linearToDb(float linear)
{
    return 20.0f * std::log10(linear + 1e-10f);
}

void NoiseGate::saveState(MemoryBlock& destData)
{
    AudioModule::saveState(destData);
    
    MemoryOutputStream stream(destData, true);
    stream.writeFloat(thresholdDb);
    stream.writeFloat(ratio);
    stream.writeFloat(attackMs);
    stream.writeFloat(releaseMs);
    stream.writeFloat(hysteresisDb);
}

void NoiseGate::loadState(const void* data, int sizeInBytes)
{
    AudioModule::loadState(data, sizeInBytes);
    
    MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    stream.readFloat(); // wetDryMix (already loaded in base class)
    stream.readBool();  // bypassed (already loaded in base class)
    
    setThreshold(stream.readFloat());
    setRatio(stream.readFloat());
    setAttack(stream.readFloat());
    setRelease(stream.readFloat());
    setHysteresis(stream.readFloat());
}

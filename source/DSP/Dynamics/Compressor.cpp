#include "Compressor.h"
#include <cmath>

Compressor::Compressor() : AudioModule("Compressor")
{
    updateCoefficients();
}

void Compressor::prepare(double sampleRate, int maximumExpectedSamplesPerBlock)
{
    AudioModule::prepare(sampleRate, maximumExpectedSamplesPerBlock);
    
    envelopeState.resize(2, 0.0f);
    rmsBuffer.resize(rmsBufferSize, 0.0f);
    rmsBufferIndex = 0;
    
    updateCoefficients();
    reset();
}

void Compressor::reset()
{
    std::fill(envelopeState.begin(), envelopeState.end(), 0.0f);
    std::fill(rmsBuffer.begin(), rmsBuffer.end(), 0.0f);
    rmsBufferIndex = 0;
}

void Compressor::setThreshold(float thresholdDb_)
{
    thresholdDb = jlimit(-60.0f, 0.0f, thresholdDb_);
    threshold = dbToLinear(thresholdDb);
}

void Compressor::setRatio(float newRatio)
{
    ratio = jlimit(1.0f, 20.0f, newRatio);
}

void Compressor::setAttack(float attackMs_)
{
    attackMs = jlimit(0.1f, 100.0f, attackMs_);
    updateCoefficients();
}

void Compressor::setRelease(float releaseMs_)
{
    releaseMs = jlimit(10.0f, 1000.0f, releaseMs_);
    updateCoefficients();
}

void Compressor::setMakeupGain(float gainDb)
{
    makeupGainDb = jlimit(0.0f, 30.0f, gainDb);
    makeupGain = dbToLinear(makeupGainDb);
}

void Compressor::setKnee(float kneeDb_)
{
    kneeDb = jlimit(0.0f, 12.0f, kneeDb_);
}

void Compressor::setMode(Mode newMode)
{
    mode = newMode;
    updateCoefficients();
}

void Compressor::setDetectionMode(DetectionMode newMode)
{
    detectionMode = newMode;
}

void Compressor::processInternal(AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    
    if ((int)envelopeState.size() < numChannels)
        envelopeState.resize(numChannels, 0.0f);
    
    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        float& envelope = envelopeState[channel];
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float inputSample = channelData[sample];
            float inputLevel = std::abs(inputSample);
            
            // RMS detection (optional)
            if (detectionMode == DetectionMode::RMS)
            {
                rmsBuffer[rmsBufferIndex] = inputSample * inputSample;
                rmsBufferIndex = (rmsBufferIndex + 1) % rmsBufferSize;
                
                float rmsSum = 0.0f;
                for (int i = 0; i < rmsBufferSize; ++i)
                    rmsSum += rmsBuffer[i];
                
                inputLevel = std::sqrt(rmsSum / rmsBufferSize);
            }
            
            // Envelope follower
            if (inputLevel > envelope)
                envelope += attackCoeff * (inputLevel - envelope);
            else
                envelope += releaseCoeff * (inputLevel - envelope);
            
            // Convert to dB for gain calculation
            float envelopeDb = linearToDb(envelope);
            
            // Compute gain reduction
            float gainReductionDb = computeGain(envelopeDb);
            float gainReduction = dbToLinear(gainReductionDb);
            
            // Apply compression and makeup gain
            channelData[sample] = inputSample * gainReduction * makeupGain;
        }
    }
}

float Compressor::computeGain(float inputLevelDb)
{
    if (inputLevelDb < thresholdDb)
        return 0.0f;  // No gain reduction below threshold
    
    float overshootDb = inputLevelDb - thresholdDb;
    
    // Soft knee
    if (kneeDb > 0.01f && overshootDb < kneeDb)
    {
        // Soft knee curve
        float kneeBlend = overshootDb / kneeDb;
        overshootDb = kneeBlend * kneeBlend * overshootDb;
    }
    
    // Apply ratio
    float gainReductionDb = overshootDb * (1.0f - 1.0f / ratio);
    
    return -gainReductionDb;  // Negative because we're reducing gain
}

void Compressor::updateCoefficients()
{
    if (currentSampleRate <= 0.0)
        return;
    
    float attackTime = attackMs;
    float releaseTime = releaseMs;
    
    // Optical mode has slower, smoother response
    if (mode == Mode::Optical)
    {
        attackTime *= 2.0f;
        releaseTime *= 1.5f;
    }
    
    const float attackTimeSeconds = attackTime * 0.001f;
    const float releaseTimeSeconds = releaseTime * 0.001f;
    
    attackCoeff = 1.0f - std::exp(-1.0f / (attackTimeSeconds * static_cast<float>(currentSampleRate)));
    releaseCoeff = 1.0f - std::exp(-1.0f / (releaseTimeSeconds * static_cast<float>(currentSampleRate)));
}

float Compressor::dbToLinear(float db)
{
    return std::pow(10.0f, db / 20.0f);
}

float Compressor::linearToDb(float linear)
{
    return 20.0f * std::log10(linear + 1e-10f);
}

void Compressor::saveState(MemoryBlock& destData)
{
    AudioModule::saveState(destData);
    
    MemoryOutputStream stream(destData, true);
    stream.writeFloat(thresholdDb);
    stream.writeFloat(ratio);
    stream.writeFloat(attackMs);
    stream.writeFloat(releaseMs);
    stream.writeFloat(makeupGainDb);
    stream.writeFloat(kneeDb);
    stream.writeInt(static_cast<int>(mode));
    stream.writeInt(static_cast<int>(detectionMode));
}

void Compressor::loadState(const void* data, int sizeInBytes)
{
    AudioModule::loadState(data, sizeInBytes);
    
    MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    stream.readFloat(); // wetDryMix
    stream.readBool();  // bypassed
    
    setThreshold(stream.readFloat());
    setRatio(stream.readFloat());
    setAttack(stream.readFloat());
    setRelease(stream.readFloat());
    setMakeupGain(stream.readFloat());
    setKnee(stream.readFloat());
    setMode(static_cast<Mode>(stream.readInt()));
    setDetectionMode(static_cast<DetectionMode>(stream.readInt()));
}

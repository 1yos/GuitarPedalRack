#pragma once

#include "../AudioModule.h"

//==============================================================================
/**
 * Noise Gate - Reduces noise and hum when the signal falls below a threshold.
 * 
 * Features:
 * - Adjustable threshold
 * - Attack and release times
 * - Hysteresis to prevent chattering
 * - Smooth envelope following
 */
class NoiseGate : public AudioModule
{
public:
    NoiseGate();
    ~NoiseGate() override = default;

    //==============================================================================
    // Parameters
    
    /** Sets the threshold in dB (-80dB to 0dB) */
    void setThreshold(float thresholdDb);
    float getThreshold() const { return thresholdDb; }
    
    /** Sets the ratio (1:1 to inf:1) */
    void setRatio(float newRatio);
    float getRatio() const { return ratio; }
    
    /** Sets the attack time in milliseconds (0.1ms to 50ms) */
    void setAttack(float attackMs);
    float getAttack() const { return attackMs; }
    
    /** Sets the release time in milliseconds (10ms to 1000ms) */
    void setRelease(float releaseMs);
    float getRelease() const { return releaseMs; }
    
    /** Sets the hysteresis amount in dB (0dB to 10dB) */
    void setHysteresis(float hysteresisDb);
    float getHysteresis() const { return hysteresisDb; }

    //==============================================================================
    // AudioModule overrides
    
    void prepare(double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "NoiseGate"; }
    String getDisplayName() const override { return "Noise Gate"; }
    
    void saveState(MemoryBlock& destData) override;
    void loadState(const void* data, int sizeInBytes) override;

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    //==============================================================================
    // Parameters
    float thresholdDb = -40.0f;
    float ratio = 10.0f;
    float attackMs = 1.0f;
    float releaseMs = 50.0f;
    float hysteresisDb = 3.0f;
    
    // Internal state
    float threshold = 0.01f;        // Linear threshold
    float closeThreshold = 0.005f;  // With hysteresis
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    
    std::vector<float> envelopeState;  // Per-channel envelope
    std::vector<bool> gateOpen;         // Per-channel gate state
    
    //==============================================================================
    // Helper functions
    void updateCoefficients();
    float dbToLinear(float db);
    float linearToDb(float linear);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseGate)
};

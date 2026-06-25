#pragma once

#include "../AudioModule.h"

//==============================================================================
/**
 * Studio-quality compressor with VCA and Optical modes.
 * 
 * Features:
 * - Threshold, Ratio, Attack, Release, Makeup Gain
 * - Soft knee option
 * - RMS and Peak detection modes
 * - VCA (fast, transparent) and Optical (slow, musical) modes
 */
class Compressor : public AudioModule
{
public:
    enum class Mode
    {
        VCA,        // Fast, transparent compression
        Optical     // Slow, smooth, musical compression
    };
    
    enum class DetectionMode
    {
        Peak,       // Peak detection (fast response)
        RMS         // RMS detection (average level)
    };

    Compressor();
    ~Compressor() override = default;

    //==============================================================================
    // Parameters
    
    /** Sets the threshold in dB (-60dB to 0dB) */
    void setThreshold(float thresholdDb);
    float getThreshold() const { return thresholdDb; }
    
    /** Sets the compression ratio (1:1 to 20:1) */
    void setRatio(float newRatio);
    float getRatio() const { return ratio; }
    
    /** Sets the attack time in milliseconds (0.1ms to 100ms) */
    void setAttack(float attackMs);
    float getAttack() const { return attackMs; }
    
    /** Sets the release time in milliseconds (10ms to 1000ms) */
    void setRelease(float releaseMs);
    float getRelease() const { return releaseMs; }
    
    /** Sets the makeup gain in dB (0dB to 30dB) */
    void setMakeupGain(float gainDb);
    float getMakeupGain() const { return makeupGainDb; }
    
    /** Sets the knee width (0dB = hard knee, 12dB = soft knee) */
    void setKnee(float kneeDb);
    float getKnee() const { return kneeDb; }
    
    /** Sets the compressor mode (VCA or Optical) */
    void setMode(Mode newMode);
    Mode getMode() const { return mode; }
    
    /** Sets the detection mode (Peak or RMS) */
    void setDetectionMode(DetectionMode newMode);
    DetectionMode getDetectionMode() const { return detectionMode; }

    //==============================================================================
    // AudioModule overrides
    
    void prepare(double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "Compressor"; }
    String getDisplayName() const override { return "Compressor"; }
    
    void saveState(MemoryBlock& destData) override;
    void loadState(const void* data, int sizeInBytes) override;

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    //==============================================================================
    // Parameters
    float thresholdDb = -20.0f;
    float ratio = 4.0f;
    float attackMs = 5.0f;
    float releaseMs = 50.0f;
    float makeupGainDb = 0.0f;
    float kneeDb = 6.0f;
    Mode mode = Mode::VCA;
    DetectionMode detectionMode = DetectionMode::RMS;
    
    // Internal state
    float threshold = 0.1f;
    float makeupGain = 1.0f;
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    
    std::vector<float> envelopeState;  // Per-channel envelope
    std::vector<float> rmsBuffer;      // For RMS detection
    int rmsBufferIndex = 0;
    static constexpr int rmsBufferSize = 64;
    
    //==============================================================================
    // Helper functions
    void updateCoefficients();
    float computeGain(float inputLevelDb);
    float dbToLinear(float db);
    float linearToDb(float linear);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Compressor)
};

#pragma once

#include "../AudioModule.h"
#include "../../JuceHeader.h"

using namespace juce;

/**
 * 4-Band Parametric EQ
 * 
 * Features:
 * - 4 fully parametric bands
 * - Frequency, Q, and Gain control per band
 * - High-pass and low-pass filters
 * - Professional-grade audio quality
 */
class EQ : public AudioModule
{
public:
    EQ();
    
    String getModuleType() const override { return "EQ"; }
    String getDisplayName() const override { return "EQ"; }

    void prepare(double sampleRate, int maxBlockSize) override;
    void reset() override;

    // Band parameters (band 0-3)
    void setBandFrequency(int band, float frequency);  // 20 Hz to 20 kHz
    void setBandQ(int band, float q);                  // 0.1 to 10.0
    void setBandGain(int band, float gainDb);          // -15 dB to +15 dB
    void setBandEnabled(int band, bool enabled);
    
    // Filter parameters
    void setHighPassFreq(float frequency);             // 20 Hz to 1 kHz
    void setLowPassFreq(float frequency);              // 1 kHz to 20 kHz
    void setHighPassEnabled(bool enabled);
    void setLowPassEnabled(bool enabled);

    float getBandFrequency(int band) const;
    float getBandQ(int band) const;
    float getBandGain(int band) const;
    bool getBandEnabled(int band) const;

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    static constexpr int NUM_BANDS = 4;
    
    // Band parameters
    struct Band
    {
        float frequency = 1000.0f;
        float q = 1.0f;
        float gainDb = 0.0f;
        bool enabled = true;
        
        // Biquad coefficients
        float a0 = 1.0f, a1 = 0.0f, a2 = 0.0f;
        float b1 = 0.0f, b2 = 0.0f;
        
        // State variables (per channel)
        float z1[2] = {0.0f, 0.0f};
        float z2[2] = {0.0f, 0.0f};
    };
    
    Band bands[NUM_BANDS];
    
    // High-pass and low-pass filters
    float highPassFreq = 20.0f;
    float lowPassFreq = 20000.0f;
    bool highPassEnabled = false;
    bool lowPassEnabled = false;
    
    // Filter coefficients
    float hpA0 = 1.0f, hpA1 = 0.0f, hpA2 = 0.0f;
    float hpB1 = 0.0f, hpB2 = 0.0f;
    float hpZ1[2] = {0.0f, 0.0f};
    float hpZ2[2] = {0.0f, 0.0f};
    
    float lpA0 = 1.0f, lpA1 = 0.0f, lpA2 = 0.0f;
    float lpB1 = 0.0f, lpB2 = 0.0f;
    float lpZ1[2] = {0.0f, 0.0f};
    float lpZ2[2] = {0.0f, 0.0f};
    
    double currentSampleRate = 44100.0;

    void updateBandCoefficients(int band);
    void updateFilterCoefficients();
    float processBiquad(float sample, Band& band, int channel);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQ)
};

#pragma once

#include "../AudioModule.h"
#include "../../JuceHeader.h"

using namespace juce;

/**
 * Hard Clipping Distortion (DS-1 / RAT style)
 * 
 * Features:
 * - Hard clipping with pre-emphasis filter
 * - Tone control (filter sweep)
 * - 4x oversampling for anti-aliasing
 * - Adjustable filter frequency
 */
class Distortion : public AudioModule
{
public:
    Distortion();
    
    String getModuleType() const override { return "Distortion"; }
    String getDisplayName() const override { return "Distortion"; }

    void prepare(double sampleRate, int maxBlockSize) override;
    void reset() override;

    // Parameters
    void setDrive(float drive);        // 0.0 to 1.0 (maps to gain 1.0 to 100.0)
    void setTone(float tone);          // 0.0 to 1.0 (filter frequency)
    void setLevel(float level);        // 0.0 to 1.0 (output volume)
    void setFilter(float filter);      // 0.0 to 1.0 (filter resonance)

    float getDrive() const { return driveParam; }
    float getTone() const { return toneParam; }
    float getLevel() const { return levelParam; }
    float getFilter() const { return filterParam; }

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    // Parameters
    float driveParam = 0.7f;
    float toneParam = 0.5f;
    float levelParam = 0.8f;
    float filterParam = 0.5f;

    // DSP state
    float drive = 30.0f;
    float outputLevel = 0.8f;
    
    // Pre-emphasis filter (high-pass)
    float preEmphasisZ1[2] = {0.0f, 0.0f};
    float preEmphasisCoeff = 0.95f;
    
    // Tone filter (low-pass variable)
    float toneZ1[2] = {0.0f, 0.0f};
    float toneCutoff = 1000.0f;
    float toneCoeff = 0.5f;
    
    // Post filter (resonant low-pass)
    float filterZ1[2] = {0.0f, 0.0f};
    float filterZ2[2] = {0.0f, 0.0f};
    float filterFreq = 2000.0f;
    float filterQ = 0.7f;
    float filterA0 = 1.0f, filterA1 = 0.0f, filterA2 = 0.0f;
    float filterB1 = 0.0f, filterB2 = 0.0f;
    
    // Oversampling
    dsp::Oversampling<float> oversampling;
    
    double currentSampleRate = 44100.0;

    void updateParameters();
    void updateFilterCoefficients();
    float hardClip(float sample);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Distortion)
};

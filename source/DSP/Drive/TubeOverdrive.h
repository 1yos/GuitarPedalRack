#pragma once

#include "../AudioModule.h"
#include <juce_dsp/juce_dsp.h>

//==============================================================================
/**
 * Tube Overdrive - Classic TS808 Tube Screamer style overdrive.
 * 
 * Features:
 * - Asymmetric soft clipping (diode simulation)
 * - Mid-boost tone circuit
 * - 4x oversampling for anti-aliasing
 * - Drive, Tone, and Level controls
 */
class TubeOverdrive : public AudioModule
{
public:
    TubeOverdrive();
    ~TubeOverdrive() override = default;

    //==============================================================================
    // Parameters
    
    /** Sets the drive amount (0.0 to 1.0) */
    void setDrive(float newDrive);
    float getDrive() const { return drive; }
    
    /** Sets the tone control (0.0 = dark, 1.0 = bright) */
    void setTone(float newTone);
    float getTone() const { return tone; }
    
    /** Sets the output level (0.0 to 1.0) */
    void setLevel(float newLevel);
    float getLevel() const { return level; }

    //==============================================================================
    // AudioModule overrides
    
    void prepare(double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "TubeOverdrive"; }
    String getDisplayName() const override { return "Tube Overdrive"; }
    
    void saveState(MemoryBlock& destData) override;
    void loadState(const void* data, int sizeInBytes) override;

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    //==============================================================================
    // Parameters
    float drive = 0.5f;
    float tone = 0.5f;
    float level = 0.7f;
    
    // Oversampling for anti-aliasing
    juce::dsp::Oversampling<float> oversampling;
    
    // Tone control filters
    juce::dsp::IIR::Filter<float> toneFilterLow;
    juce::dsp::IIR::Filter<float> toneFilterHigh;
    
    // Pre-emphasis filter (boosts mids before clipping)
    juce::dsp::IIR::Filter<float> preEmphasisFilter;
    
    //==============================================================================
    // Helper functions
    float softClip(float sample);
    float asymmetricClip(float sample, float drive);
    void updateFilters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TubeOverdrive)
};

#pragma once

#include "../AudioModule.h"
#include <juce_dsp/juce_dsp.h>

//==============================================================================
/**
 * Amp Simulator - Models guitar amplifier preamp, tone stack, and power amp.
 * 
 * Features:
 * - Three amp models: Clean, Crunch, High Gain
 * - Bass, Mid, Treble, Presence controls
 * - Power amp saturation and sag
 * - Tube-like compression and harmonics
 */
class AmpSimulator : public AudioModule
{
public:
    enum class AmpModel
    {
        Clean,      // Fender-style clean amp
        Crunch,     // Marshall-style crunch
        HighGain    // Mesa/Peavey high gain
    };

    AmpSimulator();
    ~AmpSimulator() override = default;

    //==============================================================================
    // Parameters
    
    /** Sets the amp model */
    void setAmpModel(AmpModel model);
    AmpModel getAmpModel() const { return ampModel; }
    
    /** Sets the gain (0.0 to 1.0, maps to 1-10) */
    void setGain(float newGain);
    float getGain() const { return gain; }
    
    /** Sets bass control (0.0 to 1.0) */
    void setBass(float newBass);
    float getBass() const { return bass; }
    
    /** Sets mid control (0.0 to 1.0) */
    void setMid(float newMid);
    float getMid() const { return mid; }
    
    /** Sets treble control (0.0 to 1.0) */
    void setTreble(float newTreble);
    float getTreble() const { return treble; }
    
    /** Sets presence control (0.0 to 1.0) */
    void setPresence(float newPresence);
    float getPresence() const { return presence; }

    //==============================================================================
    // AudioModule overrides
    
    void prepare(double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "AmpSimulator"; }
    String getDisplayName() const override { return "Amp Simulator"; }
    
    void saveState(MemoryBlock& destData) override;
    void loadState(const void* data, int sizeInBytes) override;

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    //==============================================================================
    // Parameters
    AmpModel ampModel = AmpModel::Crunch;
    float gain = 0.5f;
    float bass = 0.5f;
    float mid = 0.5f;
    float treble = 0.5f;
    float presence = 0.5f;
    float master = 0.7f;
    
    // DSP components
    juce::dsp::Oversampling<float> oversampling;
    
    // Tone stack filters
    juce::dsp::IIR::Filter<float> bassFilter;
    juce::dsp::IIR::Filter<float> midFilter;
    juce::dsp::IIR::Filter<float> trebleFilter;
    juce::dsp::IIR::Filter<float> presenceFilter;
    
    // Preamp input filter
    juce::dsp::IIR::Filter<float> inputHighPass;
    
    //==============================================================================
    // Helper functions
    float preampStage(float sample, float gainAmount);
    float powerAmpStage(float sample);
    float tubeClip(float sample);
    void updateFilters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpSimulator)
};

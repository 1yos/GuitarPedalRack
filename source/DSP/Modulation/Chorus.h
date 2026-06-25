#pragma once

#include "../AudioModule.h"
#include "../../JuceHeader.h"

using namespace juce;

/**
 * Chorus Effect (BBD Analog-Style)
 * 
 * Features:
 * - Multiple chorus voices (1-4)
 * - LFO modulation for pitch variation
 * - BBD-style analog character
 * - Adjustable depth and rate
 */
class Chorus : public AudioModule
{
public:
    Chorus();
    
    String getModuleType() const override { return "Chorus"; }
    String getDisplayName() const override { return "Chorus"; }

    void prepare(double sampleRate, int maxBlockSize) override;
    void reset() override;

    // Parameters
    void setRate(float rate);          // 0.0 to 1.0 (0.1 Hz to 5 Hz)
    void setDepth(float depth);        // 0.0 to 1.0 (modulation amount)
    void setMix(float mix);            // 0.0 to 1.0 (dry/wet)
    void setVoices(int numVoices);     // 1 to 4 voices

    float getRate() const { return rateParam; }
    float getDepth() const { return depthParam; }
    float getMix() const { return mixParam; }
    int getVoices() const { return voices; }

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    // Parameters
    float rateParam = 0.3f;      // LFO rate
    float depthParam = 0.5f;     // Modulation depth
    float mixParam = 0.5f;       // Dry/wet mix
    int voices = 2;              // Number of chorus voices
    
    // DSP state
    static constexpr int MAX_DELAY_SAMPLES = 48000;  // 1 second at 48kHz
    static constexpr int MAX_VOICES = 4;
    
    float delayBuffer[2][MAX_DELAY_SAMPLES];
    int delayWritePos[2];
    
    // LFO state for each voice
    float lfoPhase[MAX_VOICES];
    float lfoPhaseInc[MAX_VOICES];
    
    // Parameters in processing units
    float lfoRate = 1.0f;        // Hz
    float modDepth = 5.0f;       // milliseconds
    float wetLevel = 0.5f;
    float dryLevel = 0.5f;
    
    double currentSampleRate = 44100.0;

    void updateParameters();
    float readDelayLinear(int channel, float delaySamples);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Chorus)
};

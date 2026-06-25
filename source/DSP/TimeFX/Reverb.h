#pragma once

#include "../AudioModule.h"
#include "../../JuceHeader.h"

using namespace juce;

/**
 * Multi-Mode Reverb Effect
 * 
 * Features:
 * - Four modes: Spring, Plate, Hall, Shimmer
 * - Room size control
 * - Damping (high-frequency absorption)
 * - Pre-delay
 * - Width (stereo spread)
 */
class ReverbEffect : public AudioModule
{
public:
    enum Mode
    {
        Spring,     // Spring reverb (bright, metallic)
        Plate,      // Plate reverb (dense, bright)
        Hall,       // Hall reverb (spacious, natural)
        Shimmer     // Shimmer reverb (octave-up feedback)
    };
    
    ReverbEffect();
    
    String getModuleType() const override { return "Reverb"; }
    String getDisplayName() const override { return "Reverb"; }

    void prepare(double sampleRate, int maxBlockSize) override;
    void reset() override;

    // Parameters
    void setSize(float size);           // 0.0 to 1.0 (room size)
    void setDamping(float damping);     // 0.0 to 1.0 (high-freq absorption)
    void setMix(float mix);             // 0.0 to 1.0 (dry/wet)
    void setWidth(float width);         // 0.0 to 1.0 (stereo width)
    void setMode(Mode mode);            // Spring, Plate, Hall, or Shimmer

    float getSize() const { return sizeParam; }
    float getDamping() const { return dampingParam; }
    float getMix() const { return mixParam; }
    float getWidth() const { return widthParam; }
    Mode getMode() const { return currentMode; }

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    // Parameters
    float sizeParam = 0.5f;
    float dampingParam = 0.5f;
    float mixParam = 0.3f;
    float widthParam = 1.0f;
    Mode currentMode = Hall;
    
    // JUCE's built-in reverb
    juce::Reverb reverbProcessor;
    juce::Reverb::Parameters reverbParams;
    
    // Shimmer mode: pitch shifter (simple octave-up)
    static constexpr int PITCH_BUFFER_SIZE = 4096;
    float pitchBuffer[2][PITCH_BUFFER_SIZE];
    int pitchWritePos[2];
    
    double currentSampleRate = 44100.0;

    void updateParameters();
    float processShimmer(float sample, int channel);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbEffect)
};

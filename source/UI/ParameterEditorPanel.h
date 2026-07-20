#pragma once

#include "../JuceHeader.h"
#include "../DSP/AudioModule.h"
#include "VintageKnob.h"

// Include effect headers for parameter access
#include "../DSP/Drive/TubeScreamer808.h"
#include "../DSP/Drive/TubeScreamerTS9.h"
#include "../DSP/Drive/TubeOverdrive.h"
#include "../DSP/Drive/Distortion.h"
#include "../DSP/Modulation/Chorus.h"
#include "../DSP/TimeFX/Delay.h"
#include "../DSP/TimeFX/Reverb.h"
#include "../DSP/Dynamics/Compressor.h"
#include "../DSP/Dynamics/NoiseGate.h"
#include "../DSP/Utility/EQ.h"
#include "../DSP/Amp/AmpSimulator.h"
#include "../DSP/Cabinet/CabinetIR.h"

class GuitarPedalRackProcessor;

/**
 * ParameterEditorPanel - Edit parameters for selected effect
 * 
 * Features:
 * - Vintage rotary knobs (realistic guitar pedal aesthetic)
 * - Dynamic parameter controls based on effect type
 * - Real-time parameter adjustment
 * - Visual feedback
 * - Bypass toggle
 * - Close button
 */
class ParameterEditorPanel : public juce::Component
{
public:
    ParameterEditorPanel();
    ~ParameterEditorPanel() override = default;
    
    void setProcessor(GuitarPedalRackProcessor* p) { processor = p; }
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Set the effect to edit
    void setEffect(AudioModule* effect, const juce::String& effectName, const juce::String& category);
    
    // Clear current effect
    void clearEffect();
    
    // Check if editing an effect
    bool isEditingEffect() const { return currentEffect != nullptr; }
    
    // Callbacks
    std::function<void()> onClose;
    std::function<void(bool)> onBypassToggled;
    
private:
    AudioModule* currentEffect = nullptr;
    juce::String effectName;
    juce::String effectCategory;
    
    juce::Label titleLabel;
    juce::Label categoryLabel;
    juce::TextButton closeButton;
    juce::TextButton bypassButton;
    
    // Vintage knob controls (replaces generic sliders)
    juce::OwnedArray<VintageKnob> paramKnobs;
    
    void createParameterControls();
    void updateBypassButton();
    void setEffectParameter(const juce::String& paramName, float value);
    
    juce::Colour getCategoryColor() const;
    
    GuitarPedalRackProcessor* processor = nullptr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterEditorPanel)
};

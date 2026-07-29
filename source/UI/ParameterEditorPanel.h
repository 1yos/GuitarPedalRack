#pragma once

#include "../JuceHeader.h"
#include "../DSP/AudioModule.h"
#include "VintageKnob.h"

// Original effect headers
#include "../DSP/Drive/TubeScreamer808.h"
#include "../DSP/Drive/TubeScreamerTS9.h"
#include "../DSP/Drive/TubeScreamerMini.h"
#include "../DSP/Drive/TubeOverdrive.h"
#include "../DSP/Drive/Distortion.h"
#include "../DSP/Drive/KlonCentaur.h"
#include "../DSP/Drive/ProCoRAT.h"
#include "../DSP/Drive/ProCoRAT2.h"
#include "../DSP/Drive/TurboRAT.h"
#include "../DSP/Drive/BossDS1.h"
#include "../DSP/Drive/BossSD1.h"
#include "../DSP/Drive/BigMuffPi.h"
#include "../DSP/Drive/BigMuffRussian.h"
#include "../DSP/Drive/FuzzFace.h"
#include "../DSP/Drive/FuzzFaceSilicon.h"
#include "../DSP/Drive/BluesDriver.h"
#include "../DSP/Drive/MetalZone.h"
#include "../DSP/Drive/TimmyOverdrive.h"
#include "../DSP/Modulation/Chorus.h"
#include "../DSP/Modulation/ChorusEnsemble.h"
#include "../DSP/Modulation/FlangerBasic.h"
#include "../DSP/Modulation/Phaser90.h"
#include "../DSP/Modulation/Phaser4Stage.h"
#include "../DSP/Modulation/TremoloOptical.h"
#include "../DSP/Modulation/VibratoClassic.h"
#include "../DSP/Modulation/RotarySpeaker.h"
#include "../DSP/TimeFX/Delay.h"
#include "../DSP/TimeFX/AnalogDelay.h"
#include "../DSP/TimeFX/TapeDelay.h"
#include "../DSP/TimeFX/PingPongDelay.h"
#include "../DSP/TimeFX/PlateReverb.h"
#include "../DSP/TimeFX/SpringReverb.h"
#include "../DSP/TimeFX/Reverb.h"
#include "../DSP/Dynamics/Compressor.h"
#include "../DSP/Dynamics/NoiseGate.h"
#include "../DSP/Dynamics/FETCompressor.h"
#include "../DSP/Dynamics/VCACompressor.h"
#include "../DSP/Dynamics/OpticalCompressor.h"
#include "../DSP/Dynamics/TubeCompressor.h"
#include "../DSP/Dynamics/Limiter.h"
#include "../DSP/Dynamics/Expander.h"
#include "../DSP/Dynamics/MultibandCompressor.h"
#include "../DSP/Dynamics/DeEsser.h"
#include "../DSP/Utility/EQ.h"
#include "../DSP/Utility/ToneStackFender.h"
#include "../DSP/Utility/ToneStackMarshall.h"
#include "../DSP/Filter/WahPedal.h"
#include "../DSP/Filter/AutoWah.h"
#include "../DSP/Filter/EnvelopeFilter.h"
#include "../DSP/Filter/LowPassFilter.h"
#include "../DSP/Filter/TalkBox.h"
#include "../DSP/Pitch/PitchShifter.h"
#include "../DSP/Pitch/Harmonizer.h"
#include "../DSP/Pitch/OctaverUp.h"
#include "../DSP/Pitch/OctaverDown.h"
#include "../DSP/Amp/AmpSimulator.h"
#include "../DSP/Cabinet/CabinetIR.h"

class GuitarPedalRackProcessor;

class ParameterEditorPanel : public juce::Component
{
public:
    ParameterEditorPanel();
    ~ParameterEditorPanel() override = default;
    
    void setProcessor(GuitarPedalRackProcessor* p) { processor = p; }
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void setEffect(AudioModule* effect, const juce::String& effectName, const juce::String& category);
    void clearEffect();
    bool isEditingEffect() const { return currentEffect != nullptr; }
    
    // Public wrappers used by ModernPluginEditor for on-pedal knob sync
    void setEffectParameterPublic(const juce::String& paramName, float value)
    {
        setEffectParameter(paramName, value);
    }
    // Point the setter at a specific effect without rebuilding the panel
    void setEffectForKnobSync(AudioModule* effect, const juce::String& name, const juce::String& category)
    {
        currentEffect   = effect;
        effectName      = name;
        effectCategory  = category;
    }
    // Update a specific knob visual without rebuilding all controls
    void refreshKnob(int knobIndex, float value)
    {
        if (knobIndex >= 0 && knobIndex < paramKnobs.size())
            paramKnobs[knobIndex]->setValue(value, juce::sendNotification);
    }
    
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
    
    juce::OwnedArray<VintageKnob> paramKnobs;
    
    void createParameterControls();
    void updateBypassButton();
    void setEffectParameter(const juce::String& paramName, float value);
    
    juce::Colour getCategoryColor() const;
    
    GuitarPedalRackProcessor* processor = nullptr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterEditorPanel)
};

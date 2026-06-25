#pragma once

#include "../JuceHeader.h"
#include "HardwareKnob.h"

using namespace juce;

//==============================================================================
/**
 * Functional Amplifier Head Component
 * 
 * Complete implementation of a guitar amp with:
 * - 6 functional knobs (Gain, Bass, Mid, Treble, Presence, Master)
 * - Channel selector (Clean/Crunch/Lead)
 * - Power LED indicator
 * - APVTS parameter attachments
 * - Hardware-realistic appearance
 */
class FunctionalAmpHead : public Component
{
public:
    FunctionalAmpHead(AudioProcessorValueTreeState& apvts);
    ~FunctionalAmpHead() override = default;
    
    //==============================================================================
    // Component overrides
    void paint(Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    // State
    String getCurrentChannel() const;
    bool isBypassed() const;
    
private:
    //==============================================================================
    // Rendering helpers
    void drawAmpChassis(Graphics& g, Rectangle<float> bounds);
    void drawPowerLED(Graphics& g, Rectangle<float> bounds);
    void drawSpeakerGrill(Graphics& g, Rectangle<float> bounds);
    
    //==============================================================================
    // Member variables
    AudioProcessorValueTreeState& apvtsRef;
    
    // UI Components - 6 Knobs
    HardwareKnob gainKnob, bassKnob, midKnob;
    HardwareKnob trebleKnob, presenceKnob, masterKnob;
    
    Label gainLabel, bassLabel, midLabel;
    Label trebleLabel, presenceLabel, masterLabel;
    Label ampNameLabel;
    
    ComboBox channelSelector;
    
    // APVTS Attachments
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> bassAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> midAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> trebleAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> presenceAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> masterAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> channelAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FunctionalAmpHead)
};

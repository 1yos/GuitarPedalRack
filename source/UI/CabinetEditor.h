#pragma once

#include "../JuceHeader.h"
#include "HardwareKnob.h"

using namespace juce;

//==============================================================================
/**
 * Dynamic 2D Speaker Cone Component
 * Handles dragging a microphone icon over a 2D speaker grill.
 */
class SpeakerConeComponent : public Component
{
public:
    SpeakerConeComponent(AudioProcessorValueTreeState& apvts);
    ~SpeakerConeComponent() override = default;
    
    void paint(Graphics& g) override;
    void resized() override;
    
    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    
    void updateMicPositionFromAPVTS();
    
private:
    AudioProcessorValueTreeState& apvtsRef;
    
    Point<float> micPosNorm; // Normalized coordinates (-1.0 to 1.0)
    bool isDragging = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpeakerConeComponent)
};

//==============================================================================
/**
 * Dedicated Cabinet Simulation Editor
 * Features: speaker cone diagram, draggable mic, distance/angle sliders, and cab type selectors.
 */
class CabinetEditor : public Component, private Timer
{
public:
    CabinetEditor(AudioProcessorValueTreeState& apvts);
    ~CabinetEditor() override = default;
    
    void paint(Graphics& g) override;
    void resized() override;
    
private:
    void timerCallback() override;
    
    AudioProcessorValueTreeState& apvtsRef;
    
    // Components
    std::unique_ptr<SpeakerConeComponent> speakerCone;
    
    ComboBox cabTypeCombo;
    ComboBox micTypeCombo;
    
    Slider distanceSlider;
    Slider axisSlider;
    Slider levelSlider;
    
    Label cabTypeLabel;
    Label micTypeLabel;
    Label distanceLabel;
    Label axisLabel;
    Label levelLabel;
    
    Label distanceValDisplay;
    Label axisValDisplay;
    
    // Attachments
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> cabTypeAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> distanceAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> axisAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CabinetEditor)
};

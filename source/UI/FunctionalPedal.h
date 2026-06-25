#pragma once

#include "../JuceHeader.h"
#include "HardwareKnob.h"

using namespace juce;

//==============================================================================
/**
 * Functional Hardware Pedal Component
 * 
 * Complete implementation of a guitar effects pedal with:
 * - Real APVTS parameter attachments
 * - Functional footswitch (bypass control)
 * - Animated LED indicator
 * - 3 hardware knobs
 * - Hardware-realistic appearance
 * - Drag-and-drop support for reordering
 */
class FunctionalPedal : public Component, private Timer
{
public:
    FunctionalPedal(const String& pedalName,
                   const String& effectType,
                   const Colour& categoryColor,
                   AudioProcessorValueTreeState& apvts,
                   const String& bypassParamID,
                   const String& param1ID,
                   const String& param2ID,
                   const String& param3ID);
    
    ~FunctionalPedal() override = default;
    
    //==============================================================================
    // Component overrides
    void paint(Graphics& g) override;
    void resized() override;
    void mouseDown(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseEnter(const MouseEvent& e) override;
    void mouseExit(const MouseEvent& e) override;
    
    //==============================================================================
    // Appearance
    void setKnobLabels(const String& label1, const String& label2, const String& label3);
    void setBypassButtonBounds(Rectangle<int> bounds) { footswitchBounds = bounds; }
    
    //==============================================================================
    // State
    bool isBypassed() const;
    String getPedalName() const { return name; }
    String getEffectType() const { return type; }
    
    // Drag-and-drop callbacks
    std::function<void(FunctionalPedal*, const MouseEvent&)> onDragCallback;
    std::function<void(FunctionalPedal*)> onDragStartCallback;
    std::function<void(FunctionalPedal*)> onDragEndCallback;
    
private:
    //==============================================================================
    // Timer callback for LED animation
    void timerCallback() override;
    
    //==============================================================================
    // Rendering helpers
    void drawPedalEnclosure(Graphics& g, Rectangle<float> bounds);
    void drawFootswitch(Graphics& g, Rectangle<float> bounds);
    void drawLED(Graphics& g, Rectangle<float> bounds);
    void drawLabels(Graphics& g);
    
    //==============================================================================
    // Member variables
    String name;
    String type;
    Colour accentColor;
    
    AudioProcessorValueTreeState& apvtsRef;
    
    // UI Components
    HardwareKnob knob1, knob2, knob3;
    String knobLabel1, knobLabel2, knobLabel3;
    
    // APVTS Attachments
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> knob1Attachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> knob2Attachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> knob3Attachment;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
    
    // Bypass button (invisible, triggers parameter change)
    TextButton bypassButton;
    String bypassParamID;
    
    // Visual state
    bool isHovered = false;
    bool footswitchPressed = false;
    float ledBrightness = 0.0f;
    float targetLedBrightness = 1.0f;
    Rectangle<int> footswitchBounds;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FunctionalPedal)
};

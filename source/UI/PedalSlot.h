#pragma once

#include "../JuceHeader.h"
#include "Materials.h"

/**
 * PedalSlot - Boutique guitar pedal representation
 * 
 * Professional quality features:
 * - Powder-coated metal chassis
 * - Machined aluminum knobs with rubber grips
 * - Glass LED indicator with volumetric glow
 * - Soft rubber footswitch
 * - Stainless steel screws
 * - Realistic input/output jacks
 * - Professional typography
 * - Studio lighting response
 * - Natural material aging
 * - Smooth animations
 */
class PedalSlot : public juce::Component,
                  public juce::Timer
{
public:
    PedalSlot(const juce::String& name, const juce::String& category);
    ~PedalSlot() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;
    void mouseDoubleClick(const juce::MouseEvent& e) override;
    
    void timerCallback() override;
    
    // State
    void setBypassed(bool shouldBypass);
    void setSelected(bool shouldSelect);
    void setCpuUsage(float usage);
    
    bool isBypassed() const { return bypassed; }
    bool isSelected() const { return selected; }
    float getCpuUsage() const { return cpuUsage; }
    juce::String getEffectName() const { return effectName; }
    juce::String getCategory() const { return category; }
    juce::Colour getCategoryColor() const;
    juce::Colour getEnclosureColor() const;
    
    // Knob labels (effect-specific)
    juce::String getFirstKnobLabel() const;
    juce::String getSecondKnobLabel() const;
    juce::String getThirdKnobLabel() const;
    
    // Jack positions for cable connections
    juce::Point<float> getInputJackPosition() const;
    juce::Point<float> getOutputJackPosition() const;
    
    // Drag support
    bool isDragging() const { return dragging; }
    void startDragging();
    void stopDragging();
    
    // Callbacks
    std::function<void(PedalSlot*)> onSelected;
    std::function<void(PedalSlot*)> onBypassToggled;
    std::function<void(PedalSlot*)> onRemoveClicked;
    std::function<void(PedalSlot*, juce::Point<int>)> onDragStarted;
    std::function<void(PedalSlot*, juce::Point<int>)> onDragMoved;
    std::function<void(PedalSlot*)> onDragEnded;
    
private:
    juce::String effectName;
    juce::String category;
    
    bool hovered = false;
    bool bypassed = false;
    bool selected = false;
    bool dragging = false;
    bool footswitchPressed = false;
    float cpuUsage = 0.0f;
    
    // Animation
    float ledBrightness = 1.0f;
    float ledPhase = 0.0f;
    float hoverLift = 0.0f; // Hover animation (0 to 1)
    
    // Knob values and interaction
    float knob1Value = 0.5f;
    float knob2Value = 0.5f;
    float knob3Value = 0.5f;
    int activeKnob = -1; // -1 = none, 0-2 = which knob
    float dragStartValue = 0.0f;
    juce::Point<int> dragStartPos;
    juce::Rectangle<float> knob1Bounds;
    juce::Rectangle<float> knob2Bounds;
    juce::Rectangle<float> knob3Bounds;
    
    // Hit areas
    juce::Rectangle<float> footswitchArea;
    juce::Rectangle<float> removeButtonArea;
    juce::Rectangle<float> ledArea;
    juce::Point<float> inputJackPos;
    juce::Point<float> outputJackPos;
    
    // Visual helpers
    void drawBoutiquePedal(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawBrandPlate(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawProfessionalKnobs(juce::Graphics& g, juce::Rectangle<float> area, bool isDark);
    void drawProfessionalFootswitch(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawJacks(juce::Graphics& g);
    void drawScrews(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawRubberFeet(juce::Graphics& g, juce::Rectangle<float> bounds);
    void drawWornEnclosure(juce::Graphics& g, juce::Rectangle<float> bounds); // Legacy function kept for reference
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PedalSlot)
};

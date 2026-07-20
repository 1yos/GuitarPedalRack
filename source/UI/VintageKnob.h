#pragma once

#include "../JuceHeader.h"

/**
 * VintageKnob - Realistic guitar pedal-style rotary knob
 * 
 * Features:
 * - Realistic 3D appearance with lighting and shadows
 * - Smooth rotation with pointer indicator
 * - Category-themed colors
 * - Value display on hover
 * - Metal texture with beveled edges
 * - Knurled edge detail
 * - Authentic vintage aesthetic
 */
class VintageKnob : public juce::Component
{
public:
    VintageKnob(const juce::String& labelText, const juce::Colour& accentColor);
    ~VintageKnob() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Mouse interaction
    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;
    
    // Value control
    void setValue(float newValue, bool sendNotification = true);
    float getValue() const { return value; }
    
    void setRange(float newMin, float newMax);
    float getMinimum() const { return minValue; }
    float getMaximum() const { return maxValue; }
    
    // Display
    void setLabel(const juce::String& text);
    juce::String getLabel() const { return label; }
    
    void setValueSuffix(const juce::String& suffix) { valueSuffix = suffix; }
    
    // Callbacks
    std::function<void(float)> onValueChange;
    
private:
    // Value state
    float value = 0.5f;
    float minValue = 0.0f;
    float maxValue = 1.0f;
    
    // Display
    juce::String label;
    juce::String valueSuffix;
    juce::Colour accentColor;
    
    // Interaction state
    bool hovering = false;
    bool dragging = false;
    juce::Point<float> dragStartPos;
    float dragStartValue = 0.0f;
    
    // Constants
    static constexpr float rotationRangeRadians = juce::MathConstants<float>::pi * 1.5f; // 270 degrees
    static constexpr float startAngle = juce::MathConstants<float>::pi * 0.75f;          // Start at 7:30
    
    // Helper methods
    float valueToAngle() const;
    void drawKnobBody(juce::Graphics& g, juce::Rectangle<float> knobArea);
    void drawKnobPointer(juce::Graphics& g, juce::Rectangle<float> knobArea);
    void drawKnobShadow(juce::Graphics& g, juce::Rectangle<float> knobArea);
    void drawValueDisplay(juce::Graphics& g);
    void drawLabel(juce::Graphics& g);
    void drawKnurledEdge(juce::Graphics& g, juce::Rectangle<float> knobArea);
    
    juce::String getValueString() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VintageKnob)
};

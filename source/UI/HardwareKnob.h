#pragma once

#include "../JuceHeader.h"

using namespace juce;

//==============================================================================
/**
 * Hardware-Realistic Knob Component
 * 
 * Professional quality rotary control with:
 * - Realistic shadows and reflections
 * - Specular highlights
 * - Metal/plastic materials
 * - Smooth rotation animation
 * - Value tooltip
 * - Mouse interactions (drag, wheel, shift fine, double-click reset)
 */
class HardwareKnob : public Slider
{
public:
    enum class Style
    {
        MetalRounded,      // Shiny metal cylinder (Marshall, Fender style)
        PlasticDome,       // Plastic dome (Boss pedal style)
        ChickenHead,       // Pointer knob (vintage gear)
        Skirted           // Davies 1900h style (Moog, synth style)
    };
    
    HardwareKnob(Style knobStyle = Style::MetalRounded);
    ~HardwareKnob() override = default;
    
    //==============================================================================
    // Appearance
    void setKnobStyle(Style newStyle) { style = newStyle; repaint(); }
    void setAccentColor(const Colour& color) { accentColor = color; repaint(); }
    void setShowValueTooltip(bool shouldShow) { showTooltip = shouldShow; }
    
    //==============================================================================
    // Component overrides
    void paint(Graphics& g) override;
    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    void mouseDoubleClick(const MouseEvent& e) override;
    void mouseWheelMove(const MouseEvent& e, const MouseWheelDetails& wheel) override;
    void mouseEnter(const MouseEvent& e) override;
    void mouseExit(const MouseEvent& e) override;
    
private:
    //==============================================================================
    // Rendering methods
    void paintMetalRounded(Graphics& g, Rectangle<float> bounds, float angle);
    void paintPlasticDome(Graphics& g, Rectangle<float> bounds, float angle);
    void paintChickenHead(Graphics& g, Rectangle<float> bounds, float angle);
    void paintSkirted(Graphics& g, Rectangle<float> bounds, float angle);
    
    void drawIndicatorLine(Graphics& g, Rectangle<float> bounds, float angle);
    void drawValueTooltip(Graphics& g, Rectangle<float> bounds);
    
    //==============================================================================
    // Member variables
    Style style;
    Colour accentColor;
    bool showTooltip = true;
    bool isHovered = false;
    bool isDragging = false;
    
    Point<int> lastMousePos;
    double dragSensitivity = 0.01;
    double fineDragSensitivity = 0.002;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HardwareKnob)
};

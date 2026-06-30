#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"

using namespace juce;

//==============================================================================
// Custom Rotary Knob with realistic appearance
class RealisticKnob : public Slider
{
public:
    RealisticKnob()
    {
        setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
        setRange(0.0, 1.0, 0.01);
    }
    
    void paint(Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2);
        auto centre = bounds.getCentre();
        auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + (getValue() - getMinimum()) / (getMaximum() - getMinimum()) * (rotaryEndAngle - rotaryStartAngle);
        
        // Outer ring shadow
        g.setColour(Colours::black.withAlpha(0.3f));
        g.fillEllipse(bounds.reduced(-2));
        
        // Knob body with metallic gradient
        ColourGradient gradient(knobColor.brighter(0.4f), centre.x, centre.y - radius,
                                knobColor.darker(0.6f), centre.x, centre.y + radius, false);
        g.setGradientFill(gradient);
        g.fillEllipse(bounds);
        
        // Top highlight
        g.setColour(Colours::white.withAlpha(0.4f));
        g.fillEllipse(bounds.reduced(radius * 0.3f, radius * 0.5f).translated(0, -radius * 0.2f));
        
        // Pointer line
        Path pointer;
        auto pointerLength = radius * 0.6f;
        auto pointerThickness = 3.0f;
        pointer.addRectangle(-pointerThickness * 0.5f, -pointerLength, pointerThickness, pointerLength);
        g.setColour(Colours::white);
        g.fillPath(pointer, AffineTransform::rotation(toAngle).translated(centre));
        
        // Center cap
        g.setColour(Colours::black);
        g.fillEllipse(bounds.reduced(radius * 0.7f));
    }
    
    void setKnobColor(Colour color) { knobColor = color; }
    
private:
    Colour knobColor = Colours::darkgrey;
    const float rotaryStartAngle = MathConstants<float>::pi * 1.2f;
    const float rotaryEndAngle = MathConstants<float>::pi * 2.8f;
};

//==============================================================================
// Realistic LED Indicator
class LED : public Component, private Timer
{
public:
    LED(std::atomic<float>* param) : bypassParam(param)
    {
        startTimerHz(30);
    }
    
    void paint(Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        bool isOn = bypassParam && (*bypassParam < 0.5f);
        
        // LED bezel (dark)
        g.setColour(Colour(0xff1a1a1a));
        g.fillEllipse(bounds);
        
        if (isOn)
        {
            // Glow effect
            g.setColour(ledColor.withAlpha(0.3f));
            g.fillEllipse(bounds.expanded(4));
            
            // LED bright center
            g.setGradientFill(ColourGradient(ledColor.brighter(0.8f), bounds.getCentreX(), bounds.getCentreY(),
                                              ledColor, bounds.getRight(), bounds.getBottom(), true));
            g.fillEllipse(bounds.reduced(1));
            
            // Hot spot
            g.setColour(Colours::white.withAlpha(0.8f));
            g.fillEllipse(bounds.reduced(bounds.getWidth() * 0.4f));
        }
        else
        {
            // Off state - dark
            g.setColour(Colour(0xff2a2a2a));
            g.fillEllipse(bounds.reduced(2));
        }
    }
    
    void timerCallback() override { repaint(); }
    void setLEDColor(Colour color) { ledColor = color; }
    
private:
    std::atomic<float>* bypassParam;
    Colour ledColor = Colours::red;
};

//==============================================================================
/**
 * PRODUCTION-READY REALISTIC PEDAL UI
 */
class GuitarPedalRackEditor : public AudioProcessorEditor, private Timer
{
public:
    GuitarPedalRackEditor(GuitarPedalRackProcessor&);
    ~GuitarPedalRackEditor() override;

    void paint(Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    void drawRealisticPedal(Graphics& g, Rectangle<int> bounds, const String& name, 
                            Colour pedalColor, bool isBypassed);
    void drawRealisticAmp(Graphics& g, Rectangle<int> bounds);
    
    GuitarPedalRackProcessor& audioProcessor;
    
    // PEDAL 1: DEEP HEAT (Overdrive)
    RealisticKnob deepHeatDrive, deepHeatTone, deepHeatLevel;
    Label driveLabel, toneLabel, levelLabel, deepHeatNameLabel;
    TextButton deepHeatBypass;
    LED deepHeatLED;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> driveAttach, toneAttach, levelAttach;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> deepHeatBypassAttach;
    
    // PEDAL 2: PULSE (Chorus)
    RealisticKnob pulseRate, pulseDepth, pulseMix;
    Label rateLabel, depthLabel, mixLabel, pulseNameLabel;
    TextButton pulseBypass;
    LED pulseLED;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> rateAttach, depthAttach, mixAttach;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> pulseBypassAttach;
    
    // PEDAL 3: VOID (Reverb)
    RealisticKnob voidSize, voidDecay, voidMix2;
    Label sizeLabel, decayLabel, mix2Label, voidNameLabel;
    TextButton voidBypass;
    LED voidLED;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sizeAttach, decayAttach, mix2Attach;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> voidBypassAttach;
    
    // AMP: ALPHA AMP
    RealisticKnob ampGain, ampBass, ampMid, ampTreble, ampPresence, ampMaster;
    Label ampGainLabel, ampBassLabel, ampMidLabel, ampTrebleLabel, ampPresenceLabel, ampMasterLabel;
    Label ampNameLabel;
    ComboBox ampChannel;
    Label channelLabel;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> ampGainAttach, ampBassAttach, ampMidAttach;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> ampTrebleAttach, ampPresenceAttach, ampMasterAttach;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> ampChannelAttach;
    
    // GLOBAL
    Slider inputGain, outputGain;
    Label inputLabel, outputLabel, titleLabel, cpuLabel;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> inputAttach, outputAttach;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuitarPedalRackEditor)
};

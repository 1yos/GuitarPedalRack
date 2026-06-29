#pragma once

#include "../JuceHeader.h"
#include "../DSP/AudioModule.h"

using namespace juce;

//==============================================================================
/**
 * EffectModule - Visual representation of a single effect in the rack
 * 
 * Features:
 * - Hardware-realistic design (pedal/amp style)
 * - Bypass LED indicator
 * - Drag handle for reordering
 * - Remove button (X)
 * - Parameter knobs
 * - Visual feedback for active/bypassed state
 */
class EffectModule : public Component
{
public:
    enum class EffectType
    {
        NoiseGate,
        TubeOverdrive,
        Distortion,
        Compressor,
        Chorus,
        Delay,
        Reverb,
        AmpSimulator,
        CabinetIR,
        EQ
    };
    
    struct KnobControl
    {
        String label;
        Slider slider;
        std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> attachment;
    };
    
    //==============================================================================
    EffectModule(EffectType type, const String& displayName, AudioProcessorValueTreeState& apvts);
    ~EffectModule() override = default;
    
    //==============================================================================
    void paint(Graphics& g) override;
    void resized() override;
    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    
    //==============================================================================
    // State
    EffectType getEffectType() const { return effectType; }
    String getDisplayName() const { return displayName; }
    bool isBypassed() const { return bypassed; }
    void setBypassed(bool shouldBeBypassed);
    
    //==============================================================================
    // Drag & Drop
    bool isDragging() const { return dragging; }
    void setDragPosition(int yPos) { dragY = yPos; repaint(); }
    
    //==============================================================================
    // Callbacks
    std::function<void(EffectModule*)> onRemove;
    std::function<void(EffectModule*, int deltaY)> onDrag;
    std::function<void()> onBypassToggle;
    
private:
    void setupKnobs();
    Colour getEffectColor() const;
    void drawPedalStyle(Graphics& g);
    void drawAmpStyle(Graphics& g);
    void drawLED(Graphics& g, Rectangle<float> bounds, bool isOn);
    
    //==============================================================================
    EffectType effectType;
    String displayName;
    AudioProcessorValueTreeState& apvts;
    
    bool bypassed = false;
    bool isHovered = false;
    bool dragging = false;
    int dragY = 0;
    Point<int> dragStartPos;
    
    //==============================================================================
    // UI Components
    OwnedArray<KnobControl> knobs;
    TextButton bypassButton;
    TextButton removeButton;
    Label nameLabel;
    
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectModule)
};

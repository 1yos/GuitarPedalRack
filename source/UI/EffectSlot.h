#pragma once

#include "../JuceHeader.h"
#include "../DSP/AudioModule.h"

/**
 * EffectSlot - Visual representation of a single effect in the chain
 * 
 * Displays:
 * - Effect name and icon
 * - Bypass LED indicator
 * - CPU usage meter
 * - Drag handle for reordering
 * - Click to edit parameters
 */
class EffectSlot : public juce::Component
{
public:
    EffectSlot(const juce::String& effectName, const juce::String& category);
    ~EffectSlot() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    
    // State management
    void setBypassed(bool shouldBypass);
    void setSelected(bool shouldSelect);
    void setCpuUsage(float usage);
    
    bool isBypassed() const { return bypassed; }
    bool isSelected() const { return selected; }
    juce::String getEffectName() const { return effectName; }
    juce::String getCategory() const { return category; }
    float getCpuUsage() const { return cpuUsage; }
    
    // Drag and drop
    void startDragging();
    void stopDragging();
    bool isDragging() const { return dragging; }
    
    // Callbacks
    std::function<void(EffectSlot*)> onSelected;
    std::function<void(EffectSlot*)> onBypassToggled;
    std::function<void(EffectSlot*, juce::Point<int>)> onDragStarted;
    std::function<void(EffectSlot*, juce::Point<int>)> onDragMoved;
    std::function<void(EffectSlot*)> onDragEnded;
    std::function<void(EffectSlot*)> onRemoveClicked;
    
private:
    juce::String effectName;
    juce::String category;
    
    bool bypassed = false;
    bool selected = false;
    bool hovered = false;
    bool dragging = false;
    
    float cpuUsage = 0.0f;
    
    juce::Rectangle<int> bypassButtonArea;
    juce::Rectangle<int> removeButtonArea;
    juce::Rectangle<int> dragHandleArea;
    
    juce::Colour getCategoryColor() const;
    juce::Path getCategoryIcon() const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectSlot)
};

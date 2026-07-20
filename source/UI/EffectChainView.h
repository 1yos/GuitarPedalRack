#pragma once

#include "../JuceHeader.h"
#include "EffectSlot.h"

/**
 * EffectChainView - Scrollable container for the effect chain
 * 
 * Features:
 * - Vertical scrolling for unlimited effects
 * - Drag-and-drop reordering
 * - Visual insertion indicator
 * - Smooth animations
 * - Auto-scroll during drag
 */
class EffectChainView : public juce::Component,
                        public juce::ScrollBar::Listener
{
public:
    EffectChainView();
    ~EffectChainView() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // Effect management
    void addEffect(const juce::String& effectName, const juce::String& category);
    void removeEffect(int index);
    void removeEffect(EffectSlot* slot);
    void moveEffect(int fromIndex, int toIndex);
    void clearAllEffects();
    
    int getEffectCount() const { return effects.size(); }
    EffectSlot* getEffectSlot(int index) const;
    
    void setSelectedEffect(int index);
    int getSelectedEffectIndex() const { return selectedEffectIndex; }
    
    // Callbacks
    std::function<void(int index)> onEffectSelected;
    std::function<void(int index)> onEffectBypassToggled;
    std::function<void(int index)> onEffectRemoved;
    std::function<void(int fromIndex, int toIndex)> onEffectMoved;
    
    // ScrollBar::Listener
    void scrollBarMoved(juce::ScrollBar* scrollBar, double newRangeStart) override;
    
private:
    juce::OwnedArray<EffectSlot> effects;
    juce::ScrollBar scrollBar;
    
    int selectedEffectIndex = -1;
    int draggedEffectIndex = -1;
    int insertionIndex = -1;
    
    float scrollOffset = 0.0f;
    float effectSpacing = 10.0f;
    float effectHeight = 80.0f;
    
    void updateScrollBar();
    void updateEffectPositions();
    
    void handleEffectSelected(EffectSlot* slot);
    void handleEffectBypassToggled(EffectSlot* slot);
    void handleEffectRemoved(EffectSlot* slot);
    void handleDragStarted(EffectSlot* slot, juce::Point<int> position);
    void handleDragMoved(EffectSlot* slot, juce::Point<int> position);
    void handleDragEnded(EffectSlot* slot);
    
    int getEffectIndexAt(juce::Point<int> position) const;
    int calculateInsertionIndex(juce::Point<int> position) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectChainView)
};

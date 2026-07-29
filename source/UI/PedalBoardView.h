#pragma once

#include "../JuceHeader.h"
#include "PedalSlot.h"
#include "PatchCable.h"

class GuitarPedalRackProcessor;

/**
 * PedalBoardView - Professional boutique pedalboard
 * 
 * Features:
 * - Horizontal left-to-right signal flow
 * - Matte black powder-coated aluminum board
 * - Animated patch cables with signal visualization
 * - Studio lighting environment
 * - Smooth animations (60fps)
 * - Intelligent drag-and-drop reordering
 * - Professional layout and spacing
 */
class PedalBoardView : public juce::Component,
                       public juce::ScrollBar::Listener,
                       public juce::Timer
{
public:
    PedalBoardView();
    ~PedalBoardView() override;
    
    void setProcessor(GuitarPedalRackProcessor* p) { processor = p; }
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    
    // Pedal management
    void addPedal(const juce::String& effectName, const juce::String& category);
    void removePedal(int index);
    void removePedal(PedalSlot* slot);
    void movePedal(int fromIndex, int toIndex);
    void clearAllPedals();
    
    PedalSlot* getPedalSlot(int index) const;
    int getPedalCount() const { return pedals.size(); }
    int getSelectedPedalIndex() const { return selectedPedalIndex; }
    
    void setSelectedPedal(int index);
    
    // Callbacks
    std::function<void(int index)> onPedalSelected;
    std::function<void(int index)> onPedalBypassToggled;
    std::function<void(int index)> onPedalRemoved;
    std::function<void(int fromIndex, int toIndex)> onPedalMoved;
    // Fired when an on-pedal knob is dragged: (pedalIndex, knobIndex 0-2, value 0-1)
    std::function<void(int pedalIndex, int knobIndex, float value)> onPedalKnobChanged;
    
    // ScrollBar::Listener
    void scrollBarMoved(juce::ScrollBar* bar, double newRangeStart) override;
    
private:
    juce::OwnedArray<PedalSlot> pedals;
    juce::ScrollBar scrollBar;
    CableManager cableManager;
    
    int selectedPedalIndex = -1;
    int draggedPedalIndex = -1;
    int insertionIndex = -1;
    
    float scrollOffset = 0.0f;
    float pedalWidth = 155.0f;  // Compact pedal width matching reference
    float pedalHeight = 210.0f; // Compact pedal height matching reference
    float pedalSpacing = 18.0f; // Tight authentic spacing
    
    // Animation
    float targetScrollOffset = 0.0f;
    bool animatingScroll = false;
    float animationTime = 0.0f;
    
    // Visual helpers
    void updateScrollBar();
    void updatePedalPositions(bool animate = false);
    void updateCables();
    void drawPedalBoard(juce::Graphics& g);
    void drawCables(juce::Graphics& g);
    void drawInsertionIndicator(juce::Graphics& g);
    void drawSignalFlowIndicators(juce::Graphics& g);
    
    // Event handlers
    void handlePedalSelected(PedalSlot* slot);
    void handleBypassToggled(PedalSlot* slot);
    void handlePedalRemoved(PedalSlot* slot);
    void handleDragStarted(PedalSlot* slot, juce::Point<int> position);
    void handleDragMoved(PedalSlot* slot, juce::Point<int> position);
    void handleDragEnded(PedalSlot* slot);
    
    int getPedalIndexAt(juce::Point<int> position) const;
    int calculateInsertionIndex(juce::Point<int> position) const;
    
    GuitarPedalRackProcessor* processor = nullptr;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PedalBoardView)
};

#pragma once

#include "../JuceHeader.h"
#include "EffectModule.h"

using namespace juce;

//==============================================================================
/**
 * ModularRackView - Container that displays all active effects in the signal chain
 * 
 * Features:
 * - Shows all effects currently in the chain
 * - Drag-and-drop reordering
 * - Visual signal flow (top to bottom)
 * - Scrollable if chain exceeds viewport
 * - "Add Effect" button at bottom
 */
class ModularRackView : public Component
{
public:
    ModularRackView(AudioProcessorValueTreeState& apvts);
    ~ModularRackView() override = default;
    
    //==============================================================================
    void paint(Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    // Effect Management
    void addEffect(EffectModule::EffectType type, const String& displayName);
    void removeEffect(EffectModule* module);
    void reorderEffect(int fromIndex, int toIndex);
    void clearAllEffects();
    
    //==============================================================================
    // Chain Building
    void buildDefaultChain();
    void rebuildFromProcessor();
    
    //==============================================================================
    // Getters
    int getNumEffects() const { return effectModules.size(); }
    EffectModule* getEffect(int index) { return effectModules[index]; }
    
    //==============================================================================
    // Callbacks
    std::function<void(EffectModule::EffectType)> onEffectAdded;
    std::function<void(int effectIndex)> onEffectRemoved;
    std::function<void(int fromIndex, int toIndex)> onEffectReordered;
    std::function<void()> onAddEffectClicked;
    
private:
    void layoutEffects();
    void handleEffectDrag(EffectModule* module, int deltaY);
    
    //==============================================================================
    AudioProcessorValueTreeState& apvts;
    
    Viewport viewport;
    Component contentComponent;
    OwnedArray<EffectModule> effectModules;
    
    TextButton addEffectButton;
    
    // Drag & drop state
    EffectModule* draggingModule = nullptr;
    int dragStartIndex = -1;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModularRackView)
};

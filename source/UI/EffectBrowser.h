#pragma once

#include "../JuceHeader.h"
#include "EffectModule.h"

using namespace juce;

//==============================================================================
/**
 * EffectBrowser - Sidebar component for browsing and adding effects
 * 
 * Features:
 * - Categorized effect list (Dynamics, Drive, Modulation, etc.)
 * - Search/filter
 * - Click to add effect to chain
 * - Visual effect icons/colors
 */
class EffectBrowser : public Component
{
public:
    EffectBrowser();
    ~EffectBrowser() override = default;
    
    //==============================================================================
    void paint(Graphics& g) override;
    void resized() override;
    
    //==============================================================================
    // Callbacks
    std::function<void(EffectModule::EffectType, const String& name)> onEffectSelected;
    
private:
    struct EffectItem : public Component
    {
        EffectItem(EffectModule::EffectType type, const String& name, const String& category);
        
        void paint(Graphics& g) override;
        void mouseEnter(const MouseEvent&) override;
        void mouseExit(const MouseEvent&) override;
        void mouseDown(const MouseEvent&) override;
        
        EffectModule::EffectType effectType;
        String effectName;
        String categoryName;
        Colour effectColor;
        bool isHovered = false;
        
        std::function<void(EffectModule::EffectType, const String&)> onClick;
    };
    
    void setupEffectList();
    Colour getColorForEffect(EffectModule::EffectType type);
    
    //==============================================================================
    Label titleLabel;
    TextEditor searchBar;
    Viewport viewport;
    Component contentComponent;
    OwnedArray<EffectItem> effectItems;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectBrowser)
};

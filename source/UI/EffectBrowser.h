#pragma once

#include "../JuceHeader.h"
#include "../DSP/EffectLibrary.h"

/**
 * EffectBrowser - Searchable browser for all available effects
 * 
 * Features:
 * - Category filtering (Drive, Modulation, Delay, etc.)
 * - Search by name
 * - Visual effect cards with icons
 * - Drag to add to chain or click to insert
 * - CPU usage preview
 * - Effect description tooltips
 */
class EffectBrowser : public juce::Component,
                      public juce::TextEditor::Listener
{
public:
    EffectBrowser();
    ~EffectBrowser() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // TextEditor::Listener
    void textEditorTextChanged(juce::TextEditor& editor) override;
    
    // Callbacks
    std::function<void(const juce::String& effectId, const juce::String& category)> onEffectSelected;
    
private:
    class EffectCard : public juce::Component
    {
    public:
        EffectCard(const EffectDescriptor& desc);
        void paint(juce::Graphics& g) override;
        void mouseEnter(const juce::MouseEvent& e) override;
        void mouseExit(const juce::MouseEvent& e) override;
        void mouseDown(const juce::MouseEvent& e) override;
        
        EffectDescriptor descriptor;
        bool hovered = false;
        
        std::function<void(const EffectDescriptor&)> onClick;
    };
    
    juce::TextEditor searchBox;
    juce::TextButton categoryButtons[11]; // 10 categories + All
    juce::OwnedArray<EffectCard> effectCards;
    juce::Viewport viewport;
    juce::Component effectContainer;
    
    juce::String currentCategory = "All";
    juce::String searchText;
    
    void loadEffects();
    void filterEffects();
    void updateLayout();
    void selectCategory(const juce::String& category);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectBrowser)
};

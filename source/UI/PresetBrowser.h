#pragma once

#include "../JuceHeader.h"
#include "../State/PresetManager.h"

/**
 * PresetBrowser - Browse, load, and save presets
 * 
 * Features:
 * - Visual preset cards
 * - Category filtering
 * - Search functionality
 * - Save current chain as preset
 * - Load preset
 * - Delete user presets
 * - Preview preset info
 */
class PresetBrowser : public juce::Component,
                      public juce::TextEditor::Listener
{
public:
    PresetBrowser();
    ~PresetBrowser() override = default;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
    // TextEditor::Listener
    void textEditorTextChanged(juce::TextEditor& editor) override;
    
    // Set preset manager
    void setPresetManager(PresetManager* manager);
    
    // Callbacks
    std::function<void(const juce::String& presetName)> onPresetSelected;
    std::function<void()> onSaveNewPreset;
    std::function<void()> onClose;
    
private:
    class PresetCard : public juce::Component
    {
    public:
        PresetCard(const ChainPreset& preset);
        void paint(juce::Graphics& g) override;
        void mouseEnter(const juce::MouseEvent& e) override;
        void mouseExit(const juce::MouseEvent& e) override;
        void mouseDown(const juce::MouseEvent& e) override;
        
        ChainPreset preset;
        bool hovered = false;
        bool isFactory = true;
        
        std::function<void(const ChainPreset&)> onClick;
        std::function<void(const ChainPreset&)> onDelete;
    };
    
    PresetManager* presetManager = nullptr;
    
    juce::TextEditor searchBox;
    juce::TextButton categoryButtons[8]; // All, Clean, Blues, Rock, Metal, Lead, Ambient, Bass
    juce::TextButton saveButton;
    juce::TextButton closeButton;
    juce::OwnedArray<PresetCard> presetCards;
    juce::Viewport viewport;
    juce::Component presetContainer;
    
    juce::String currentCategory = "All";
    juce::String searchText;
    
    void loadPresets();
    void filterPresets();
    void updateLayout();
    void selectCategory(const juce::String& category);
    void showSaveDialog();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetBrowser)
};

#pragma once

#include "../JuceHeader.h"
#include "../PluginProcessor.h"
#include "PedalBoardView.h"
#include "EffectBrowser.h"
#include "ParameterEditorPanel.h"
#include "PresetBrowser.h"

// ============ CUSTOM LOOK AND FEEL FOR BOTTOM BUTTONS ============
class BoutiqueButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    BoutiqueButtonLookAndFeel() = default;
    ~BoutiqueButtonLookAndFeel() override = default;

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        auto themeColor = backgroundColour;
        
        g.saveState();
        
        // 1. Soft glowing aura behind the button on hover
        if (shouldDrawButtonAsHighlighted && !shouldDrawButtonAsDown)
        {
            juce::Path glowPath;
            glowPath.addRoundedRectangle(bounds.expanded(4.0f), 8.0f);
            
            juce::ColourGradient glowGradient(
                themeColor.withAlpha(0.18f), bounds.getCentreX(), bounds.getCentreY(),
                themeColor.withAlpha(0.0f), bounds.getCentreX(), bounds.getBottom() + 4.0f,
                true // radial
            );
            g.setGradientFill(glowGradient);
            g.fillPath(glowPath);
        }
        
        // Position shift for clicked state
        auto buttonArea = bounds.reduced(1.0f);
        if (shouldDrawButtonAsDown)
            buttonArea = buttonArea.translated(0.0f, 1.0f);
            
        juce::Path buttonPath;
        buttonPath.addRoundedRectangle(buttonArea, 8.0f);
        
        // 2. Acrylic/Glass background gradient
        juce::ColourGradient bgGradient(
            juce::Colour(0xff222222), buttonArea.getX(), buttonArea.getY(),
            juce::Colour(0xff121212), buttonArea.getX(), buttonArea.getBottom(),
            false
        );
        g.setGradientFill(bgGradient);
        g.fillPath(buttonPath);
        
        // 3. Subtle top light-edge highlight (3D glass shimmer)
        g.setColour(juce::Colours::white.withAlpha(shouldDrawButtonAsHighlighted ? 0.12f : 0.06f));
        g.strokePath(buttonPath, juce::PathStrokeType(1.0f));
        
        // 4. Glow neon border outline
        float borderAlpha = shouldDrawButtonAsDown ? 0.45f : (shouldDrawButtonAsHighlighted ? 0.95f : 0.65f);
        float borderWidth = shouldDrawButtonAsHighlighted ? 1.4f : 1.0f;
        
        g.setColour(themeColor.withAlpha(borderAlpha));
        g.strokePath(buttonPath, juce::PathStrokeType(borderWidth));
        
        g.restoreState();
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button,
                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        if (shouldDrawButtonAsDown)
            bounds = bounds.translated(0.0f, 1.0f);
            
        g.setFont(juce::Font("sans-serif", 10.5f, juce::Font::bold).withHorizontalScale(1.05f));
        g.setColour(juce::Colours::white.withAlpha(shouldDrawButtonAsHighlighted ? 1.0f : 0.85f));
        g.drawText(button.getButtonText(), bounds, juce::Justification::centred);
    }
};

/**
 * ModernPluginEditor - Professional UI System
 * 
 * Features:
 * - Dual UI modes: Modern (vertical) and Skeuomorphic (horizontal pedalboard)
 * - Scrollable effect chain (unlimited capacity)
 * - Effect browser with 103+ effects
 * - Drag-and-drop reordering
 * - Effect parameter editor panel
 * - Preset browser
 * - Signal flow visualization
 * - High-DPI support
 */
class ModernPluginEditor : public juce::AudioProcessorEditor,
                           private juce::Timer
{
public:
    ModernPluginEditor(GuitarPedalRackProcessor& processor);
    ~ModernPluginEditor() override;
    
    void paint(juce::Graphics& g) override;
    void resized() override;
    
private:
    void timerCallback() override;
    
    // Look and feel for custom buttons
    BoutiqueButtonLookAndFeel boutiqueLookAndFeel;
    
    // UI Components
    PedalBoardView pedalBoardView;        // Professional pedal board (ONLY MODE)
    EffectBrowser effectBrowser;
    ParameterEditorPanel parameterEditor;
    PresetBrowser presetBrowser;
    
    juce::TextButton addEffectButton;
    juce::TextButton clearAllButton;
    juce::TextButton presetsButton;
    // MODE BUTTON REMOVED - Pedal mode only
    
    juce::Label titleLabel;
    juce::Label cpuLabel;
    juce::Label effectCountLabel;
    
    // Panels
    juce::Component browserPanel;
    juce::Component editorPanel;
    juce::Component presetPanel;
    bool browserVisible = false;
    bool editorVisible = false;
    bool presetVisible = false;
    // MODE SWITCHING REMOVED - pedal mode only
    
    // Callbacks
    void handleEffectSelected(int index);
    void handleEffectBypassToggled(int index);
    void handleEffectRemoved(int index);
    void handleEffectMoved(int fromIndex, int toIndex);
    void handleAddEffectClicked();
    void handleBrowserEffectSelected(const juce::String& effectId, const juce::String& category);
    void handlePresetSelected(const juce::String& presetName);
    void handleSaveNewPreset();
    
    // Utility
    void toggleBrowser();
    void toggleEditor();
    void togglePresetBrowser();
    void updateStatus();
    // MODE SWITCHING REMOVED - pedal mode only
    
    GuitarPedalRackProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModernPluginEditor)
};

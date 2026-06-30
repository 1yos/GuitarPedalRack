#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "UI/ModularRackView.h"
#include "UI/EffectBrowser.h"

using namespace juce;

//==============================================================================
/**
 * GuitarPedalRackEditor - Modular Rack Edition
 * 
 * Layout:
 * - Top toolbar (menu, preset, save)
 * - Left sidebar (effect browser)
 * - Center (modular rack view - shows all active effects)
 * - Right sidebar (global controls, macros)
 * - Bottom status bar (CPU, undo/redo, live mode)
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
    void showEffectBrowserOverlay();
    void hideEffectBrowserOverlay();
    void handleEffectSelected(EffectModule::EffectType type, const String& name);
    void updateCpuDisplay();
    
    //==============================================================================
    GuitarPedalRackProcessor& audioProcessor;
    
    //==============================================================================
    // Layout sections
    
    // Top Toolbar (height: 60px)
    Component topToolbar;
    Label brandLabel;
    TextButton fileButton, editButton, settingsButton;
    TextButton savePresetButton;
    Label presetNameLabel;
    
    // Left Sidebar (width: 280px) - Hidden by default, shows on "+ Add Effect"
    Component leftSidebar;
    std::unique_ptr<EffectBrowser> effectBrowser;
    bool sidebarVisible = false;
    
    // Center - Modular Rack View
    std::unique_ptr<ModularRackView> rackView;
    
    // Right Panel (width: 300px)
    Component rightPanel;
    Label globalControlsLabel;
    Slider inputGainSlider, outputGainSlider;
    Label inputGainLabel, outputGainLabel;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;
    
    // Bottom Status Bar (height: 50px)
    Component bottomStatusBar;
    TextButton undoButton, redoButton;
    Label statusLabel;
    Label cpuLabel;
    TextButton liveModeButton;
    
    //==============================================================================
    // Effect Browser Overlay (modal)
    Component browserOverlay;
    bool overlayVisible = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuitarPedalRackEditor)
};

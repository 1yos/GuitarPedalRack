#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "UI/FunctionalPedal.h"
#include "UI/FunctionalAmpHead.h"

class CabinetEditor;
class PresetBrowser;
class RigGeneratorPanel;
class LibraryBrowser;
class HistoryBrowser;

//==============================================================================
// Premium Hardware-Inspired Look and Feel
class PremiumLookAndFeel : public LookAndFeel_V4
{
public:
    PremiumLookAndFeel();
    
    void drawRotarySlider(Graphics&, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle,
                         float rotaryEndAngle, Slider&) override;
    
    void drawButtonBackground(Graphics&, Button&, const Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

//==============================================================================
// Premium Pedal Component (hardware-realistic stompbox)
class PremiumPedal : public Component
{
public:
    PremiumPedal(const String& name, const Colour& categoryColor);
    
    void paint(Graphics& g) override;
    void resized() override;
    void mouseDown(const MouseEvent& e) override;
    
    void setActive(bool shouldBeActive);
    bool isActive() const { return active; }
    
    void addKnob(const String& label, float value);
    
private:
    String pedalName;
    Colour accentColor;
    bool active = true;
    bool ledOn = true;
    
    struct Knob
    {
        String label;
        float value;
        Rectangle<int> bounds;
    };
    std::vector<Knob> knobs;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PremiumPedal)
};

//==============================================================================
// Amp Head Component (visually dominant)
class AmpHead : public Component
{
public:
    AmpHead();
    
    void paint(Graphics& g) override;
    void resized() override;
    
private:
    Slider gainSlider, bassSlider, midSlider, trebleSlider, presenceSlider;
    Label gainLabel, bassLabel, midLabel, trebleLabel, presenceLabel;
    Label ampModelLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AmpHead)
};

//==============================================================================
// Preset Card Component (visual thumbnail)
class PresetCard : public Component
{
public:
    PresetCard(const String& name, const String& category);
    
    void paint(Graphics& g) override;
    void mouseEnter(const MouseEvent&) override;
    void mouseExit(const MouseEvent&) override;
    void mouseDown(const MouseEvent&) override;
    
private:
    String presetName;
    String categoryName;
    bool isHovered = false;
    Colour categoryColor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetCard)
};

//==============================================================================
// Rig Generator Overlay
class RigGeneratorOverlay : public Component
{
public:
    RigGeneratorOverlay();
    
    void paint(Graphics& g) override;
    void resized() override;
    void setVisible(bool shouldBeVisible) override;
    void mouseDown(const MouseEvent& e) override;
    
private:
    Label titleLabel;
    Slider styleSlider, energySlider, intensitySlider;
    Label styleLabel, energyLabel, intensityLabel;
    TextButton generateButton, cancelButton, closeButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RigGeneratorOverlay)
};

//==============================================================================
// Macro Control Panel (right side)
class MacroControlPanel : public Component
{
public:
    MacroControlPanel();
    
    void paint(Graphics& g) override;
    void resized() override;
    
private:
    Slider morphKnob;
    Slider brightnessSlider, spaceSlider, tightnessSlider;
    Label morphLabel, brightnessLabel, spaceLabel, tightnessLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MacroControlPanel)
};

//==============================================================================
/**
 * Premium Guitar Rig Interface
 * Hardware-inspired, performance-first UI
 */
class GuitarPedalRackEditor : public AudioProcessorEditor, private Timer
{
public:
    friend class PresetBrowser;
    friend class RigGeneratorPanel;
    friend class LibraryBrowser;
    friend class HistoryBrowser;

    GuitarPedalRackEditor(GuitarPedalRackProcessor&);
    ~GuitarPedalRackEditor() override;

    void paint(Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    GuitarPedalRackProcessor& audioProcessor;
    
    // Premium look and feel
    static PremiumLookAndFeel premiumLookAndFeel;
    
    //==============================================================================
    // TOP TOOLBAR (72px)
    Label brandLabel;
    TextButton fileButton;
    TextButton editButton;
    TextButton settingsButton;
    TextButton saveButton;  // Bright cyan
    
    //==============================================================================
    // LEFT SIDEBAR - Browser (300px)
    Component soundBrowserPanel;
    Label browserTitleLabel;
    
    TextButton presetsButton;      // Yellow when active
    TextButton rigGenButton;
    TextButton libraryButton;
    TextButton historyButton;
    TextButton generateRigButton;  // Bottom yellow button
    
    //==============================================================================
    // BOTTOM STATUS BAR (56px)
    TextButton undoButton;
    TextButton redoButton;
    Label statusLabel;
    Label cpuLabel;
    TextButton liveModeButton;     // Green indicator
    
    //==============================================================================
    // MAIN CANVAS - Pedalboard Stage
    Component pedalboardStage;
    OwnedArray<FunctionalPedal> pedals;
    std::unique_ptr<FunctionalAmpHead> ampHead;
    
    enum class MainView { Pedalboard, Amp, Cabinet };
    MainView currentMainView = MainView::Pedalboard;
    
    TextButton pedalboardViewButton;
    TextButton ampViewButton;
    TextButton cabinetViewButton;
    
    // Dynamic sidebar panels and Cabinet editor
    std::unique_ptr<CabinetEditor> cabinetEditor;
    std::unique_ptr<PresetBrowser> presetBrowser;
    std::unique_ptr<RigGeneratorPanel> rigGenPanel;
    std::unique_ptr<LibraryBrowser> libraryBrowser;
    std::unique_ptr<HistoryBrowser> historyBrowser;
    
    // Drag-and-drop state
    FunctionalPedal* draggedPedal = nullptr;
    
    // Signal flow visualization
    Path signalPath;
    
    //==============================================================================
    // RIGHT PANEL - Macro Controls
    std::unique_ptr<MacroControlPanel> macroPanel;
    
    //==============================================================================
    // RIG GENERATOR OVERLAY (Keep for compatibility, though we use sidebar panel now)
    std::unique_ptr<RigGeneratorOverlay> rigGenerator;
    
    //==============================================================================
    // State
    String currentPresetName = "Classic Rock Rhythm";
    int currentMode = 1;  // 0=Clean, 1=Crunch, 2=Lead, 3=Ambient
    float cpuUsage = 0.0f;
    
    //==============================================================================
    // Helper methods
    void updateCpuDisplay();
    void loadPresetCards();
    void createDefaultPedalboard();
    void showRigGenerator();
    void hideRigGenerator();
    void updateSignalPath();
    Colour getCategoryColor(const String& category);
    
    void updatePresetSelection();
    void selectMainView(MainView view);
    void selectSidebarTab(int tabIndex);
    void layoutPedalsExcept(FunctionalPedal* exceptedPedal);
    void updateDspRouting();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuitarPedalRackEditor)
};

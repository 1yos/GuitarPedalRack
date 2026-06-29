#include "PluginEditor.h"

//==============================================================================
GuitarPedalRackEditor::GuitarPedalRackEditor(GuitarPedalRackProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(1280, 720);
    setResizable(false, false);
    
    //==============================================================================
    // TOP TOOLBAR
    //==============================================================================
    addAndMakeVisible(topToolbar);
    
    topToolbar.addAndMakeVisible(brandLabel);
    brandLabel.setText("GUITAR PEDAL RACK", dontSendNotification);
    brandLabel.setFont(Font(18.0f, Font::bold));
    brandLabel.setColour(Label::textColourId, Colours::white);
    
    topToolbar.addAndMakeVisible(fileButton);
    fileButton.setButtonText("FILE");
    fileButton.setColour(TextButton::buttonColourId, Colour(0xff2A2A2A));
    fileButton.onClick = [this]() {
        PopupMenu menu;
        menu.addItem(1, "New Session");
        menu.addItem(2, "Load Preset...");
        menu.addItem(3, "Save Preset As...");
        menu.addSeparator();
        menu.addItem(4, "Export Settings...");
        menu.addItem(5, "Import Settings...");
        menu.addSeparator();
        menu.addItem(6, "Preferences");
        
        menu.showMenuAsync(PopupMenu::Options().withTargetComponent(&fileButton));
    };
    
    topToolbar.addAndMakeVisible(editButton);
    editButton.setButtonText("EDIT");
    editButton.setColour(TextButton::buttonColourId, Colour(0xff2A2A2A));
    
    topToolbar.addAndMakeVisible(settingsButton);
    settingsButton.setButtonText("SETTINGS");
    settingsButton.setColour(TextButton::buttonColourId, Colour(0xff2A2A2A));
    
    topToolbar.addAndMakeVisible(presetNameLabel);
    presetNameLabel.setText(audioProcessor.getCurrentPresetName(), dontSendNotification);
    presetNameLabel.setFont(Font(14.0f));
    presetNameLabel.setColour(Label::textColourId, Colour(0xff00E5FF));
    presetNameLabel.setJustificationType(Justification::centred);
    
    topToolbar.addAndMakeVisible(savePresetButton);
    savePresetButton.setButtonText("SAVE");
    savePresetButton.setColour(TextButton::buttonColourId, Colour(0xff00E5FF));
    savePresetButton.setColour(TextButton::textColourOffId, Colour(0xff000000));
    savePresetButton.onClick = [this]() {
        AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon,
                                         "Save Preset",
                                         "Preset saved: " + audioProcessor.getCurrentPresetName(),
                                         "OK");
    };
    
    //==============================================================================
    // CENTER - MODULAR RACK VIEW
    //==============================================================================
    rackView = std::make_unique<ModularRackView>(audioProcessor.getAPVTS());
    addAndMakeVisible(rackView.get());
    
    // Handle add effect clicked
    rackView->onAddEffectClicked = [this]() {
        showEffectBrowserOverlay();
    };
    
    // Handle effect added to UI - need to add to DSP chain too
    rackView->onEffectAdded = [this](EffectModule::EffectType type) {
        // This will be called after effect is added to UI
        // TODO: Sync with processor's signal chain
        DBG("Effect added to UI: " + String((int)type));
    };
    
    // Handle effect removed from UI - need to remove from DSP chain too
    rackView->onEffectRemoved = [this](int effectIndex) {
        // This will be called after effect is removed from UI
        // TODO: Sync with processor's signal chain
        DBG("Effect removed from UI at index: " + String(effectIndex));
    };
    
    // Handle effect reordered in UI - need to reorder in DSP chain too
    rackView->onEffectReordered = [this](int fromIndex, int toIndex) {
        // This will be called after effect is reordered in UI
        // TODO: Sync with processor's signal chain
        DBG("Effect reordered from " + String(fromIndex) + " to " + String(toIndex));
    };
    
    //==============================================================================
    // RIGHT PANEL - GLOBAL CONTROLS
    //==============================================================================
    addAndMakeVisible(rightPanel);
    
    rightPanel.addAndMakeVisible(globalControlsLabel);
    globalControlsLabel.setText("GLOBAL CONTROLS", dontSendNotification);
    globalControlsLabel.setFont(Font(14.0f, Font::bold));
    globalControlsLabel.setColour(Label::textColourId, Colour(0xff8A8A8A));
    globalControlsLabel.setJustificationType(Justification::centred);
    
    rightPanel.addAndMakeVisible(inputGainLabel);
    inputGainLabel.setText("INPUT GAIN", dontSendNotification);
    inputGainLabel.setFont(Font(11.0f, Font::bold));
    inputGainLabel.setColour(Label::textColourId, Colour(0xff8A8A8A));
    inputGainLabel.setJustificationType(Justification::centred);
    
    rightPanel.addAndMakeVisible(inputGainSlider);
    inputGainSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    inputGainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    inputGainSlider.setRange(-20.0, 20.0, 0.1);
    inputGainSlider.setValue(0.0);
    inputGainSlider.setTextValueSuffix(" dB");
    inputGainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "globalInputGain", inputGainSlider);
    
    rightPanel.addAndMakeVisible(outputGainLabel);
    outputGainLabel.setText("OUTPUT GAIN", dontSendNotification);
    outputGainLabel.setFont(Font(11.0f, Font::bold));
    outputGainLabel.setColour(Label::textColourId, Colour(0xff8A8A8A));
    outputGainLabel.setJustificationType(Justification::centred);
    
    rightPanel.addAndMakeVisible(outputGainSlider);
    outputGainSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    outputGainSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    outputGainSlider.setRange(-20.0, 20.0, 0.1);
    outputGainSlider.setValue(0.0);
    outputGainSlider.setTextValueSuffix(" dB");
    outputGainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getAPVTS(), "globalOutputGain", outputGainSlider);
    
    //==============================================================================
    // BOTTOM STATUS BAR
    //==============================================================================
    addAndMakeVisible(bottomStatusBar);
    
    bottomStatusBar.addAndMakeVisible(undoButton);
    undoButton.setButtonText("↶");
    undoButton.setColour(TextButton::buttonColourId, Colour(0xff2A2A2A));
    
    bottomStatusBar.addAndMakeVisible(redoButton);
    redoButton.setButtonText("↷");
    redoButton.setColour(TextButton::buttonColourId, Colour(0xff2A2A2A));
    
    bottomStatusBar.addAndMakeVisible(statusLabel);
    statusLabel.setText("Ready", dontSendNotification);
    statusLabel.setFont(Font(12.0f));
    statusLabel.setColour(Label::textColourId, Colour(0xff8A8A8A));
    
    bottomStatusBar.addAndMakeVisible(cpuLabel);
    cpuLabel.setText("CPU: 0%", dontSendNotification);
    cpuLabel.setFont(Font(12.0f, Font::bold));
    cpuLabel.setColour(Label::textColourId, Colour(0xff00E5FF));
    
    bottomStatusBar.addAndMakeVisible(liveModeButton);
    liveModeButton.setButtonText("● LIVE");
    liveModeButton.setColour(TextButton::buttonColourId, Colour(0xff66BB6A));
    liveModeButton.setColour(TextButton::textColourOffId, Colour(0xff000000));
    
    //==============================================================================
    // EFFECT BROWSER OVERLAY (Initially hidden)
    //==============================================================================
    addChildComponent(browserOverlay);
    browserOverlay.setAlwaysOnTop(true);
    
    effectBrowser = std::make_unique<EffectBrowser>();
    browserOverlay.addAndMakeVisible(effectBrowser.get());
    
    effectBrowser->onEffectSelected = [this](EffectModule::EffectType type, const String& name) {
        handleEffectSelected(type, name);
    };
    
    //==============================================================================
    // Start timer for CPU monitoring
    startTimerHz(30);
}

GuitarPedalRackEditor::~GuitarPedalRackEditor()
{
    stopTimer();
}

//==============================================================================
void GuitarPedalRackEditor::paint(Graphics& g)
{
    // Main background
    g.fillAll(Colour(0xff0A0A0A));
    
    // Top toolbar background
    g.setColour(Colour(0xff1A1A1A));
    g.fillRect(topToolbar.getBounds());
    
    // Right panel background
    g.setColour(Colour(0xff121212));
    g.fillRect(rightPanel.getBounds());
    
    // Bottom status bar background
    g.setColour(Colour(0xff1A1A1A));
    g.fillRect(bottomStatusBar.getBounds());
    
    // Browser overlay background (semi-transparent)
    if (overlayVisible)
    {
        g.setColour(Colour(0xff000000).withAlpha(0.7f));
        g.fillRect(browserOverlay.getBounds());
    }
}

void GuitarPedalRackEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Top toolbar (60px)
    topToolbar.setBounds(bounds.removeFromTop(60));
    auto toolbarArea = topToolbar.getLocalBounds().reduced(10, 10);
    brandLabel.setBounds(toolbarArea.removeFromLeft(250));
    fileButton.setBounds(toolbarArea.removeFromLeft(70).reduced(2));
    editButton.setBounds(toolbarArea.removeFromLeft(70).reduced(2));
    settingsButton.setBounds(toolbarArea.removeFromLeft(90).reduced(2));
    savePresetButton.setBounds(toolbarArea.removeFromRight(80).reduced(2));
    toolbarArea.removeFromRight(10);
    presetNameLabel.setBounds(toolbarArea);
    
    // Bottom status bar (50px)
    bottomStatusBar.setBounds(bounds.removeFromBottom(50));
    auto statusArea = bottomStatusBar.getLocalBounds().reduced(10, 8);
    undoButton.setBounds(statusArea.removeFromLeft(40).reduced(2));
    redoButton.setBounds(statusArea.removeFromLeft(40).reduced(2));
    statusArea.removeFromLeft(10);
    statusLabel.setBounds(statusArea.removeFromLeft(200));
    liveModeButton.setBounds(statusArea.removeFromRight(90).reduced(2));
    statusArea.removeFromRight(10);
    cpuLabel.setBounds(statusArea.removeFromRight(80));
    
    // Right panel (300px)
    rightPanel.setBounds(bounds.removeFromRight(300));
    auto rightArea = rightPanel.getLocalBounds().reduced(15, 20);
    globalControlsLabel.setBounds(rightArea.removeFromTop(30));
    rightArea.removeFromTop(20);
    inputGainLabel.setBounds(rightArea.removeFromTop(20));
    inputGainSlider.setBounds(rightArea.removeFromTop(100).withSizeKeepingCentre(90, 90));
    rightArea.removeFromTop(30);
    outputGainLabel.setBounds(rightArea.removeFromTop(20));
    outputGainSlider.setBounds(rightArea.removeFromTop(100).withSizeKeepingCentre(90, 90));
    
    // Center - Modular Rack View
    rackView->setBounds(bounds);
    
    // Browser overlay (modal, centered)
    if (overlayVisible)
    {
        browserOverlay.setBounds(getLocalBounds());
        auto browserBounds = getLocalBounds().withSizeKeepingCentre(400, 500);
        effectBrowser->setBounds(browserBounds);
    }
}

//==============================================================================
void GuitarPedalRackEditor::timerCallback()
{
    try
    {
        updateCpuDisplay();
        presetNameLabel.setText(audioProcessor.getCurrentPresetName(), dontSendNotification);
    }
    catch (...)
    {
        DBG("Exception in timerCallback");
    }
}

void GuitarPedalRackEditor::updateCpuDisplay()
{
    float cpuUsage = audioProcessor.getDspCpuUsage();
    cpuLabel.setText("CPU: " + String(cpuUsage, 1) + "%", dontSendNotification);
    
    // Color code CPU usage
    if (cpuUsage < 50.0f)
        cpuLabel.setColour(Label::textColourId, Colour(0xff66BB6A)); // Green
    else if (cpuUsage < 75.0f)
        cpuLabel.setColour(Label::textColourId, Colour(0xffFFC107)); // Yellow
    else
        cpuLabel.setColour(Label::textColourId, Colour(0xffF44336)); // Red
}

//==============================================================================
void GuitarPedalRackEditor::showEffectBrowserOverlay()
{
    overlayVisible = true;
    browserOverlay.setVisible(true);
    browserOverlay.toFront(true);
    resized();
    repaint();
}

void GuitarPedalRackEditor::hideEffectBrowserOverlay()
{
    overlayVisible = false;
    browserOverlay.setVisible(false);
    repaint();
}

void GuitarPedalRackEditor::handleEffectSelected(EffectModule::EffectType type, const String& name)
{
    // Add effect to rack
    rackView->addEffect(type, name);
    
    // Hide browser
    hideEffectBrowserOverlay();
    
    // Show feedback
    statusLabel.setText("Added: " + name, dontSendNotification);
}

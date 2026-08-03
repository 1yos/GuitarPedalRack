#include "ModernPluginEditor.h"

ModernPluginEditor::ModernPluginEditor(GuitarPedalRackProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(1024, 600);
    setResizable(true, true);
    setResizeLimits(900, 520, 2000, 1400);
    
    // ============ TITLE BAR ============
    // Title is drawn manually in paint() — label is hidden but kept for compatibility
    titleLabel.setVisible(false);
    titleLabel.setText("Aether Guitar Pedal Rack", juce::dontSendNotification);
    
    cpuLabel.setVisible(false);
    effectCountLabel.setVisible(false);
    
    // ============ PEDALBOARD VIEW (PROFESSIONAL PEDAL MODE ONLY) ============
    pedalBoardView.setProcessor(&audioProcessor);
    addAndMakeVisible(pedalBoardView);
    pedalBoardView.onPedalSelected = [this](int index) { handleEffectSelected(index); };
    pedalBoardView.onPedalBypassToggled = [this](int index) { handleEffectBypassToggled(index); };
    pedalBoardView.onPedalRemoved = [this](int index) { handleEffectRemoved(index); };
    pedalBoardView.onPedalMoved = [this](int from, int to) { handleEffectMoved(from, to); };
    pedalBoardView.onPedalKnobChanged = [this](int pedalIndex, int knobIndex, float value)
    {
        auto* pedal  = pedalBoardView.getPedalSlot(pedalIndex);
        auto* effect = audioProcessor.getSignalChain().getEffect(pedalIndex);
        if (!pedal || !effect) return;
        
        juce::String paramName = pedal->getParamNameForKnob(knobIndex);
        
        // Cache + call DSP setter via the editor's routing
        parameterEditor.setEffectForKnobSync(effect, pedal->getEffectName(), pedal->getCategory());
        parameterEditor.setEffectParameterPublic(paramName, value);
        
        // If this pedal is open in the bottom panel, sync its knob visually
        if (pedalBoardView.getSelectedPedalIndex() == pedalIndex)
            parameterEditor.refreshKnob(knobIndex, value);
    };
    
    // ============ CONTROL BUTTONS ============
    addAndMakeVisible(addEffectButton);
    addEffectButton.setLookAndFeel(&boutiqueLookAndFeel);
    addEffectButton.setButtonText("ADD EFFECT");
    addEffectButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2a2a2a));
    addEffectButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white.withAlpha(0.85f));
    addEffectButton.onClick = [this]() { handleAddEffectClicked(); };
    
    addAndMakeVisible(clearAllButton);
    clearAllButton.setLookAndFeel(&boutiqueLookAndFeel);
    clearAllButton.setButtonText("DELETE");
    clearAllButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2a2a2a));
    clearAllButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white.withAlpha(0.85f));
    clearAllButton.onClick = [this]()
    {
        juce::AlertWindow::showAsync(
            juce::MessageBoxOptions()
                .withIconType(juce::MessageBoxIconType::WarningIcon)
                .withTitle("Clear All Effects")
                .withMessage("Remove all effects from the chain?")
                .withButton("OK")
                .withButton("Cancel"),
            [this](int result)
            {
                if (result == 1) // OK clicked
                {
                    // Clear from processor first
                    while (audioProcessor.getEffectChainSize() > 0)
                    {
                        audioProcessor.removeEffectFromChain(0);
                    }
                    
                    // Clear pedal board view
                    pedalBoardView.clearAllPedals();
                    
                    updateStatus();
                }
            }
        );
    };
    
    addAndMakeVisible(presetsButton);
    presetsButton.setLookAndFeel(&boutiqueLookAndFeel);
    presetsButton.setButtonText("PRESETS");
    presetsButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2a2a2a));
    presetsButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white.withAlpha(0.85f));
    presetsButton.onClick = [this]()
    {
        togglePresetBrowser();
    };
    
    // ============ EFFECT BROWSER ============
    browserPanel.addAndMakeVisible(effectBrowser);
    addChildComponent(browserPanel);
    
    effectBrowser.onEffectSelected = [this](const juce::String& id, const juce::String& cat)
    {
        handleBrowserEffectSelected(id, cat);
    };
    
    // ============ PARAMETER EDITOR ============
    parameterEditor.setProcessor(&audioProcessor);
    addAndMakeVisible(editorPanel);
    editorPanel.addAndMakeVisible(parameterEditor);
    editorPanel.setVisible(true); // Always visible at bottom
    
    parameterEditor.onClose = [this]()
    {
        // Close just clears the effect, panel stays visible
        parameterEditor.clearEffect();
    };
    
    parameterEditor.onBypassToggled = [this](bool bypassed)
    {
        int selectedIndex = pedalBoardView.getSelectedPedalIndex();
        if (selectedIndex >= 0)
        {
            audioProcessor.setEffectBypassed(selectedIndex, bypassed);
            auto* pedal = pedalBoardView.getPedalSlot(selectedIndex);
            if (pedal)
                pedal->setBypassed(bypassed);
        }
    };
    
    // ============ PRESET BROWSER ============
    presetPanel.addAndMakeVisible(presetBrowser);
    addChildComponent(presetPanel);
    
    presetBrowser.setPresetManager(&audioProcessor.getPresetManager());
    
    presetBrowser.onPresetSelected = [this](const juce::String& presetName)
    {
        handlePresetSelected(presetName);
    };
    
    presetBrowser.onSaveNewPreset = [this]()
    {
        handleSaveNewPreset();
    };
    
    presetBrowser.onClose = [this]()
    {
        togglePresetBrowser();
    };
    
    // Synchronize UI with processor's active chain on start
    syncUIWithProcessorChain();
    
    // Select first pedal by default on startup if exists
    if (pedalBoardView.getPedalCount() > 0)
    {
        pedalBoardView.setSelectedPedal(0);
        handleEffectSelected(0);
    }
    
    // Start timer for status and meter updates (30Hz for smooth metering)
    startTimerHz(30);
    
    // Initial status
    updateStatus();
}

ModernPluginEditor::~ModernPluginEditor()
{
    stopTimer();
    addEffectButton.setLookAndFeel(nullptr);
    clearAllButton.setLookAndFeel(nullptr);
    presetsButton.setLookAndFeel(nullptr);
}

void ModernPluginEditor::paint(juce::Graphics& g)
{
    // Full dark background
    g.setColour(juce::Colour(0xff0d0d0d));
    g.fillAll();
    
    // ─── TOP HEADER BAR ────────────────────────────────────────────────────
    auto topH = getHeight() > 600 ? 42 : 36;
    g.setColour(juce::Colour(0xff111111));
    g.fillRect(0, 0, getWidth(), topH);
    
    // Bottom border of header
    g.setColour(juce::Colours::black.withAlpha(0.6f));
    g.fillRect(0, topH - 1, getWidth(), 1);
    g.setColour(juce::Colours::white.withAlpha(0.07f));
    g.fillRect(0, topH, getWidth(), 1);
    
    // AETHER logo text (bold white)
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.setColour(juce::Colours::white);
    g.drawText("AETHER", 14, 0, 80, topH, juce::Justification::centredLeft);
    
    // Subtitle
    g.setFont(juce::Font(10.0f));
    g.setColour(juce::Colours::white.withAlpha(0.4f));
    g.drawText("GUITAR PEDAL RACK", 96, 0, 160, topH, juce::Justification::centredLeft);
    
    // ─── BOTTOM TOOLBAR ────────────────────────────────────────────────────
    int tbH = 36;
    g.setColour(juce::Colour(0xff111111));
    g.fillRect(0, getHeight() - tbH, getWidth(), tbH);
    g.setColour(juce::Colours::white.withAlpha(0.07f));
    g.fillRect(0, getHeight() - tbH, getWidth(), 1);
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.fillRect(0, getHeight() - tbH + 1, getWidth(), 1);
    
    // Dividers between bottom toolbar sections
    auto drawDivider = [&](int x)
    {
        g.setColour(juce::Colours::white.withAlpha(0.08f));
        g.fillRect(x, getHeight() - tbH + 6, 1, tbH - 12);
    };
    drawDivider(80);       // after MODE section
    drawDivider(getWidth() - 220); // before PRESETS section
    drawDivider(getWidth() - 130); // before BPM
}

void ModernPluginEditor::resized()
{
    auto bounds = getLocalBounds();
    int topH = getHeight() > 600 ? 42 : 36;
    int botH = 36;
    int paramH = 170;
    
    // ─── TOP HEADER BAR ────────────────────────────────────────────────────
    auto topBar = bounds.removeFromTop(topH);
    
    // AETHER + subtitle drawn in paint() — no label needed in header
    // Skip 270px for the logo area
    topBar.removeFromLeft(270);
    
    // CPU and count labels hidden — clean header
    
    // ─── BOTTOM TOOLBAR ────────────────────────────────────────────────────
    auto botBar = bounds.removeFromBottom(botH);
    
    // MODE: PLAY / EDIT  (left)
    int bx = 8, by = botBar.getY() + 6, bh = botH - 12, bw = 34;
    addEffectButton.setBounds(bx + 85,   by, 95, bh);   // ADD EFFECT
    clearAllButton.setBounds( bx + 190,  by, 60, bh);   // DELETE
    presetsButton.setBounds(  getWidth() - 200, by, 80, bh);  // PRESETS
    
    // ─── MAIN CONTENT AREA ─────────────────────────────────────────────────
    auto mainArea = bounds; // remainder after header + toolbar removed
    
    // Parameter editor panel at bottom
    auto paramArea = mainArea.removeFromBottom(paramH);
    editorPanel.setBounds(paramArea);
    editorPanel.setVisible(true);
    parameterEditor.setBounds(editorPanel.getLocalBounds());
    
    // Pedal board fills remaining space
    pedalBoardView.setBounds(mainArea);
    pedalBoardView.setVisible(true);
    
    // ─── OVERLAYS ──────────────────────────────────────────────────────────
    if (browserVisible)
    {
        int bwid = juce::jmin(900, getWidth() - 80);
        int bhgt = juce::jmin(700, getHeight() - 120);
        browserPanel.setBounds((getWidth() - bwid) / 2, (getHeight() - bhgt) / 2, bwid, bhgt);
        effectBrowser.setBounds(browserPanel.getLocalBounds());
    }
    
    if (presetVisible)
    {
        int pwid = juce::jmin(1100, getWidth() - 80);
        int phgt = juce::jmin(750, getHeight() - 80);
        presetPanel.setBounds((getWidth() - pwid) / 2, (getHeight() - phgt) / 2, pwid, phgt);
        presetBrowser.setBounds(presetPanel.getLocalBounds());
    }
    
    juce::ignoreUnused(bw);
}

void ModernPluginEditor::timerCallback()
{
    // Update CPU usage
    float cpuUsage = audioProcessor.getCPUUsage();
    cpuLabel.setText(juce::String::formatted("CPU: %.1f%%", cpuUsage), juce::dontSendNotification);
    
    // Repaint board and editor to animate level meters in real-time
    pedalBoardView.repaint();
    parameterEditor.repaint();
    
    // Update effect count periodically (90 steps at 30Hz = 3 seconds)
    static int counter = 0;
    if (++counter >= 90)
    {
        updateStatus();
        counter = 0;
    }
}

void ModernPluginEditor::handleEffectSelected(int index)
{
    // Show parameter editor for selected effect
    auto* selectedEffect = audioProcessor.getSignalChain().getEffect(index);
    if (selectedEffect)
    {
        juce::String effectName, category;
        
        auto* pedal = pedalBoardView.getPedalSlot(index);
        if (pedal)
        {
            effectName = pedal->getEffectName();
            category = pedal->getCategory();
        }
        
        if (effectName.isNotEmpty())
        {
            parameterEditor.setEffect(selectedEffect, effectName, category);
        }
    }
    
    DBG("Effect selected: " + juce::String(index));
}

void ModernPluginEditor::handleEffectBypassToggled(int index)
{
    auto* pedal = pedalBoardView.getPedalSlot(index);
    if (pedal)
    {
        audioProcessor.setEffectBypassed(index, pedal->isBypassed());
        DBG("Effect bypass toggled: " + juce::String(index));
    }
}

void ModernPluginEditor::handleEffectRemoved(int index)
{
    int sizeBefore = audioProcessor.getEffectChainSize();
    
    // First remove from processor's signal chain
    audioProcessor.removeEffectFromChain(index);
    
    int sizeAfter = audioProcessor.getEffectChainSize();
    juce::ignoreUnused(sizeBefore, sizeAfter);
    
    DBG("Effect removed: idx=" + juce::String(index) + 
        " chain: " + juce::String(sizeBefore) + "->" + juce::String(sizeAfter));
    
    updateStatus();
}

void ModernPluginEditor::handleEffectMoved(int fromIndex, int toIndex)
{
    audioProcessor.moveEffectInChain(fromIndex, toIndex);
    DBG("Effect moved from " + juce::String(fromIndex) + " to " + juce::String(toIndex));
}

void ModernPluginEditor::handleAddEffectClicked()
{
    toggleBrowser();
}

void ModernPluginEditor::handleBrowserEffectSelected(const juce::String& effectId, const juce::String& category)
{
    // Add effect to processor's signal chain
    audioProcessor.addEffectToChain(effectId);
    
    // Add to pedal board view
    pedalBoardView.addPedal(effectId, category);
    
    // Link the new PedalSlot to its AudioModule so knobs are wired
    int newIndex = pedalBoardView.getPedalCount() - 1;
    auto* pedal = pedalBoardView.getPedalSlot(newIndex);
    auto* effect = audioProcessor.getSignalChain().getEffect(newIndex);
    if (pedal && effect)
    {
        pedal->setAudioModule(effect);
        // Seed default UI values so on-pedal knobs show sensible positions from the start
        parameterEditor.seedDefaultValues(effect, effectId);
    }
    
    DBG("Added effect: " + effectId);
    
    toggleBrowser();
    updateStatus();
}

void ModernPluginEditor::toggleBrowser()
{
    browserVisible = !browserVisible;
    browserPanel.setVisible(browserVisible);
    
    if (browserVisible)
    {
        browserPanel.toFront(true);
    }
    
    resized();
}

void ModernPluginEditor::toggleEditor()
{
    editorVisible = !editorVisible;
    editorPanel.setVisible(editorVisible);
    
    if (!editorVisible)
    {
        parameterEditor.clearEffect();
    }
    
    if (editorVisible)
    {
        editorPanel.toFront(true);
    }
    
    resized();
}

void ModernPluginEditor::updateStatus()
{
    int effectCount = pedalBoardView.getPedalCount();
    effectCountLabel.setText(juce::String(effectCount) + " pedals", 
                            juce::dontSendNotification);
}

void ModernPluginEditor::togglePresetBrowser()
{
    presetVisible = !presetVisible;
    presetPanel.setVisible(presetVisible);
    
    if (presetVisible)
    {
        presetPanel.toFront(true);
    }
    
    resized();
}

void ModernPluginEditor::handlePresetSelected(const juce::String& presetName)
{
    // Load preset via AudioProcessor — this restores ALL parameter values correctly
    if (!audioProcessor.loadPreset(presetName))
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::MessageBoxIconType::WarningIcon,
            "Load Failed",
            "Failed to load preset: " + presetName);
        return;
    }
    
    // Synchronize UI with processor's active chain
    syncUIWithProcessorChain();
    
    updateStatus();
    
    // Update parameter editor with first selected effect
    if (pedalBoardView.getPedalCount() > 0)
    {
        pedalBoardView.setSelectedPedal(0);
        handleEffectSelected(0);
    }
    
    // Mark active preset in browser
    presetBrowser.setActivePreset(presetName);
    
    // Close preset browser
    togglePresetBrowser();
}

void ModernPluginEditor::handleSaveNewPreset()
{
    auto window = std::make_unique<juce::AlertWindow>("Save Preset", 
                                                      "Enter preset details:", 
                                                      juce::MessageBoxIconType::QuestionIcon);
    
    window->addTextEditor("name", "", "Preset Name:");
    window->addTextEditor("description", "", "Description (optional):");
    window->addTextEditor("tags", "", "Tags (comma-separated, e.g. rock,lead):");
    window->addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
    window->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
    
    window->enterModalState(true, juce::ModalCallbackFunction::create([this, windowPtr = window.get()](int result)
    {
        if (result == 1)
        {
            juce::String presetName = windowPtr->getTextEditorContents("name").trim();
            juce::String description = windowPtr->getTextEditorContents("description").trim();
            juce::String tags        = windowPtr->getTextEditorContents("tags").trim();
            
            if (presetName.isEmpty())
            {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::MessageBoxIconType::WarningIcon,
                    "Invalid Name", "Preset name cannot be empty!");
                return;
            }
            
            // saveCurrentAsPreset captures all APVTS parameter values correctly
            if (audioProcessor.saveCurrentAsPreset(presetName))
            {
                // Enrich the saved preset with description, tags and module list
                ChainPreset enriched;
                if (audioProcessor.getPresetManager().loadPreset(presetName, enriched))
                {
                    enriched.description = description.isEmpty() ? "User preset" : description;
                    enriched.category    = "User";
                    
                    // Parse tags
                    if (tags.isNotEmpty())
                    {
                        auto tagArray = juce::StringArray::fromTokens(tags, ",", "");
                        for (auto& t : tagArray)
                            enriched.tags.addIfNotAlreadyThere(t.trim().toLowerCase());
                    }
                    
                    // Capture module chain from the current board
                    enriched.modules.clear();
                    for (int i = 0; i < audioProcessor.getEffectChainSize(); ++i)
                    {
                        auto* pedal = pedalBoardView.getPedalSlot(i);
                        if (pedal)
                        {
                            ModulePreset mp;
                            mp.moduleType = pedal->getEffectName();
                            mp.bypassed   = pedal->isBypassed();
                            enriched.modules.add(mp);
                        }
                    }
                    
                    audioProcessor.getPresetManager().savePreset(enriched);
                }
                
                // Refresh preset browser list
                presetBrowser.setPresetManager(&audioProcessor.getPresetManager());
                
                juce::AlertWindow::showMessageBoxAsync(
                    juce::MessageBoxIconType::InfoIcon,
                    "Saved", "Preset saved: " + presetName);
            }
            else
            {
                juce::AlertWindow::showMessageBoxAsync(
                    juce::MessageBoxIconType::WarningIcon,
                    "Save Failed", "Failed to save preset: " + presetName);
            }
        }
    }), true);
    
    window.release();
}

void ModernPluginEditor::syncUIWithProcessorChain()
{
    pedalBoardView.clearAllPedals();
    
    auto& chain = audioProcessor.getSignalChain();
    for (int i = 0; i < audioProcessor.getEffectChainSize(); ++i)
    {
        auto* activeEffect = chain.getEffect(i);
        if (activeEffect != nullptr)
        {
            juce::String type = activeEffect->getName();
            juce::String moduleType = activeEffect->getModuleType();
            
            juce::String catName = "Utility";
            if (moduleType == "NoiseGate" || moduleType == "Compressor") catName = "Dynamics";
            else if (moduleType.containsIgnoreCase("Overdrive") || moduleType.containsIgnoreCase("Drive")) catName = "Drive";
            else if (moduleType.containsIgnoreCase("Distortion") || moduleType.containsIgnoreCase("Fuzz")) catName = "Drive";
            else if (moduleType == "Chorus" || moduleType.containsIgnoreCase("Flanger")) catName = "Modulation";
            else if (moduleType.containsIgnoreCase("Amp")) catName = "Amp";
            else if (moduleType == "Reverb" || moduleType == "ReverbEffect") catName = "Reverb";
            else if (moduleType.containsIgnoreCase("Cabinet") || moduleType.containsIgnoreCase("Cab")) catName = "Cabinet";
            else if (moduleType.containsIgnoreCase("Delay")) catName = "Delay";
            else if (moduleType.containsIgnoreCase("EQ")) catName = "EQ";
            
            pedalBoardView.addPedal(type, catName);
            
            auto* pedal = pedalBoardView.getPedalSlot(i);
            if (pedal)
            {
                pedal->setBypassed(activeEffect->isBypassed());
                pedal->setAudioModule(activeEffect); // wire knobs to live DSP
                parameterEditor.seedDefaultValues(activeEffect, type); // seed on-pedal knob positions
            }
        }
    }
    
    updateStatus();
}

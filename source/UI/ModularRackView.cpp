#include "ModularRackView.h"

//==============================================================================
ModularRackView::ModularRackView(AudioProcessorValueTreeState& state)
    : apvts(state)
{
    // Setup viewport
    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&contentComponent, false);
    viewport.setScrollBarsShown(true, false);
    viewport.setScrollBarThickness(8);
    
    // Add Effect button
    addAndMakeVisible(addEffectButton);
    addEffectButton.setButtonText("+ ADD EFFECT");
    addEffectButton.setColour(TextButton::buttonColourId, Colour(0xff00E5FF));
    addEffectButton.setColour(TextButton::textColourOffId, Colour(0xff000000));
    addEffectButton.onClick = [this]() {
        if (onAddEffectClicked) onAddEffectClicked();
    };
    
    // Build default chain
    buildDefaultChain();
}

//==============================================================================
void ModularRackView::paint(Graphics& g)
{
    // Background
    g.fillAll(Colour(0xff0A0A0A));
    
    // Title
    g.setColour(Colour(0xff8A8A8A));
    g.setFont(Font(12.0f, Font::bold));
    g.drawText("SIGNAL CHAIN", 10, 5, getWidth() - 20, 20, Justification::centred);
}

void ModularRackView::resized()
{
    auto bounds = getLocalBounds();
    
    // Title area
    bounds.removeFromTop(30);
    
    // Add Effect button at bottom
    auto buttonArea = bounds.removeFromBottom(40).reduced(10, 5);
    addEffectButton.setBounds(buttonArea);
    
    // Viewport takes remaining space
    viewport.setBounds(bounds);
    
    // Layout effects in viewport
    layoutEffects();
}

//==============================================================================
void ModularRackView::addEffect(EffectModule::EffectType type, const String& displayName)
{
    auto* module = new EffectModule(type, displayName, apvts);
    
    // Setup callbacks
    module->onRemove = [this](EffectModule* m) { removeEffect(m); };
    module->onDrag = [this](EffectModule* m, int deltaY) { handleEffectDrag(m, deltaY); };
    module->onBypassToggle = [this]() { repaint(); };
    
    effectModules.add(module);
    contentComponent.addAndMakeVisible(module);
    
    layoutEffects();
    
    if (onEffectAdded)
        onEffectAdded(type);
}

void ModularRackView::removeEffect(EffectModule* module)
{
    int index = effectModules.indexOf(module);
    if (index >= 0)
    {
        effectModules.remove(index);
        layoutEffects();
        
        if (onEffectRemoved)
            onEffectRemoved(index);
    }
}

void ModularRackView::reorderEffect(int fromIndex, int toIndex)
{
    if (fromIndex >= 0 && fromIndex < effectModules.size() &&
        toIndex >= 0 && toIndex < effectModules.size() &&
        fromIndex != toIndex)
    {
        auto* module = effectModules.removeAndReturn(fromIndex);
        effectModules.insert(toIndex, module);
        
        layoutEffects();
        
        if (onEffectReordered)
            onEffectReordered(fromIndex, toIndex);
    }
}

void ModularRackView::clearAllEffects()
{
    effectModules.clear();
    layoutEffects();
}

//==============================================================================
void ModularRackView::buildDefaultChain()
{
    clearAllEffects();
    
    // Default chain: Gate → Overdrive → Chorus → Amp → Reverb → Cabinet
    addEffect(EffectModule::EffectType::NoiseGate, "Noise Gate");
    addEffect(EffectModule::EffectType::TubeOverdrive, "Deep Heat");
    addEffect(EffectModule::EffectType::Chorus, "Pulse");
    addEffect(EffectModule::EffectType::AmpSimulator, "Alpha Amp");
    addEffect(EffectModule::EffectType::Reverb, "Void");
    addEffect(EffectModule::EffectType::CabinetIR, "Cabinet");
}

void ModularRackView::rebuildFromProcessor()
{
    // This will be used to sync with processor's signal chain
    // For now, just rebuild default
    buildDefaultChain();
}

//==============================================================================
void ModularRackView::layoutEffects()
{
    int yPos = 10;
    int moduleHeight = 150; // Default height
    int spacing = 15;
    int moduleWidth = viewport.getWidth() - 20;
    
    for (auto* module : effectModules)
    {
        // Amp gets more height
        if (module->getEffectType() == EffectModule::EffectType::AmpSimulator)
            moduleHeight = 200;
        else
            moduleHeight = 150;
        
        module->setBounds(10, yPos, moduleWidth, moduleHeight);
        yPos += moduleHeight + spacing;
        
        // Draw signal flow arrow below each module (except last)
        if (module != effectModules.getLast())
        {
            yPos += 10; // Space for arrow
        }
    }
    
    // Set content component size
    contentComponent.setSize(viewport.getWidth(), yPos + 10);
}

void ModularRackView::handleEffectDrag(EffectModule* module, int deltaY)
{
    // Simple drag reordering logic
    int currentIndex = effectModules.indexOf(module);
    if (currentIndex < 0) return;
    
    int currentY = module->getY();
    int newY = currentY + deltaY;
    
    // Check if we should swap with adjacent module
    if (deltaY > 0 && currentIndex < effectModules.size() - 1)
    {
        // Dragging down
        auto* nextModule = effectModules[currentIndex + 1];
        if (newY > nextModule->getY() + nextModule->getHeight() / 2)
        {
            reorderEffect(currentIndex, currentIndex + 1);
        }
    }
    else if (deltaY < 0 && currentIndex > 0)
    {
        // Dragging up
        auto* prevModule = effectModules[currentIndex - 1];
        if (newY < prevModule->getY() + prevModule->getHeight() / 2)
        {
            reorderEffect(currentIndex, currentIndex - 1);
        }
    }
}

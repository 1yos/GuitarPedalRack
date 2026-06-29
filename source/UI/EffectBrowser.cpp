#include "EffectBrowser.h"

//==============================================================================
EffectBrowser::EffectItem::EffectItem(EffectModule::EffectType type, const String& name, const String& category)
    : effectType(type), effectName(name), categoryName(category)
{
    // Set color based on type
    switch (type)
    {
        case EffectModule::EffectType::NoiseGate:       effectColor = Colour(0xff66BB6A); break;
        case EffectModule::EffectType::TubeOverdrive:   effectColor = Colour(0xffFF9800); break;
        case EffectModule::EffectType::Distortion:      effectColor = Colour(0xffF44336); break;
        case EffectModule::EffectType::Compressor:      effectColor = Colour(0xff2196F3); break;
        case EffectModule::EffectType::Chorus:          effectColor = Colour(0xff9C27B0); break;
        case EffectModule::EffectType::Delay:           effectColor = Colour(0xff00BCD4); break;
        case EffectModule::EffectType::Reverb:          effectColor = Colour(0xff3F51B5); break;
        case EffectModule::EffectType::AmpSimulator:    effectColor = Colour(0xffFFC107); break;
        case EffectModule::EffectType::CabinetIR:       effectColor = Colour(0xff795548); break;
        case EffectModule::EffectType::EQ:              effectColor = Colour(0xff607D8B); break;
        default:                                        effectColor = Colour(0xff9E9E9E); break;
    }
}

void EffectBrowser::EffectItem::paint(Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    
    // Background
    g.setColour(isHovered ? Colour(0xff2A2A2A) : Colour(0xff1A1A1A));
    g.fillRoundedRectangle(bounds, 6.0f);
    
    // Color indicator stripe (left)
    auto stripe = bounds.removeFromLeft(6.0f);
    g.setColour(effectColor);
    g.fillRoundedRectangle(stripe, 3.0f);
    
    // Effect name
    g.setColour(Colours::white);
    g.setFont(Font(13.0f, Font::bold));
    g.drawText(effectName, bounds.getX() + 12, bounds.getY(), bounds.getWidth() - 60, bounds.getHeight(), Justification::centredLeft);
    
    // Category tag
    g.setColour(effectColor.withAlpha(0.7f));
    g.setFont(Font(9.0f, Font::bold));
    g.drawText(categoryName.toUpperCase(), bounds.getRight() - 55, bounds.getY(), 50, bounds.getHeight(), Justification::centredRight);
    
    // Hover border
    if (isHovered)
    {
        g.setColour(effectColor.withAlpha(0.6f));
        g.drawRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), 6.0f, 2.0f);
    }
}

void EffectBrowser::EffectItem::mouseEnter(const MouseEvent&)
{
    isHovered = true;
    repaint();
}

void EffectBrowser::EffectItem::mouseExit(const MouseEvent&)
{
    isHovered = false;
    repaint();
}

void EffectBrowser::EffectItem::mouseDown(const MouseEvent&)
{
    if (onClick)
        onClick(effectType, effectName);
}

//==============================================================================
EffectBrowser::EffectBrowser()
{
    // Title
    addAndMakeVisible(titleLabel);
    titleLabel.setText("EFFECTS", dontSendNotification);
    titleLabel.setFont(Font(14.0f, Font::bold));
    titleLabel.setColour(Label::textColourId, Colours::white);
    titleLabel.setJustificationType(Justification::centred);
    
    // Search bar
    addAndMakeVisible(searchBar);
    searchBar.setTextToShowWhenEmpty("Search effects...", Colour(0xff8A8A8A));
    searchBar.setColour(TextEditor::backgroundColourId, Colour(0xff161616));
    searchBar.setColour(TextEditor::outlineColourId, Colour(0xff2A2A2A));
    searchBar.setColour(TextEditor::focusedOutlineColourId, Colour(0xff00E5FF));
    
    // Viewport
    addAndMakeVisible(viewport);
    viewport.setScrollBarThickness(6);
    viewport.setViewedComponent(&contentComponent, false);
    
    // Setup effect list
    setupEffectList();
}

//==============================================================================
void EffectBrowser::paint(Graphics& g)
{
    g.fillAll(Colour(0xff0A0A0A));
}

void EffectBrowser::resized()
{
    auto bounds = getLocalBounds();
    
    // Title
    titleLabel.setBounds(bounds.removeFromTop(30).reduced(10, 5));
    
    // Search bar
    searchBar.setBounds(bounds.removeFromTop(35).reduced(10, 5));
    
    // Viewport
    viewport.setBounds(bounds);
    
    // Layout items
    int yPos = 5;
    int itemHeight = 40;
    int spacing = 5;
    int itemWidth = viewport.getWidth() - 10;
    
    for (auto* item : effectItems)
    {
        item->setBounds(5, yPos, itemWidth, itemHeight);
        yPos += itemHeight + spacing;
    }
    
    contentComponent.setSize(viewport.getWidth(), yPos);
}

//==============================================================================
void EffectBrowser::setupEffectList()
{
    struct EffectInfo
    {
        EffectModule::EffectType type;
        String name;
        String category;
    };
    
    std::vector<EffectInfo> effects = {
        // Dynamics
        { EffectModule::EffectType::NoiseGate,    "Noise Gate",     "Dynamics" },
        { EffectModule::EffectType::Compressor,   "Compressor",     "Dynamics" },
        
        // Drive
        { EffectModule::EffectType::TubeOverdrive, "Tube Overdrive", "Drive" },
        { EffectModule::EffectType::Distortion,    "Distortion",     "Drive" },
        
        // Modulation
        { EffectModule::EffectType::Chorus,       "Chorus",         "Modulation" },
        
        // Time FX
        { EffectModule::EffectType::Delay,        "Delay",          "Time FX" },
        { EffectModule::EffectType::Reverb,       "Reverb",         "Time FX" },
        
        // Amp & Cab
        { EffectModule::EffectType::AmpSimulator, "Amp Simulator",  "Amp" },
        { EffectModule::EffectType::CabinetIR,    "Cabinet IR",     "Cab" },
        
        // Utility
        { EffectModule::EffectType::EQ,           "Parametric EQ",  "Utility" },
    };
    
    for (const auto& info : effects)
    {
        auto* item = new EffectItem(info.type, info.name, info.category);
        item->onClick = [this](EffectModule::EffectType type, const String& name) {
            if (onEffectSelected)
                onEffectSelected(type, name);
        };
        
        effectItems.add(item);
        contentComponent.addAndMakeVisible(item);
    }
}

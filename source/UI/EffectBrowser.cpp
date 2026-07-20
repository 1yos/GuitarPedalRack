#include "EffectBrowser.h"

// Helper function to convert EffectCategory enum to string
static juce::String getCategoryString(EffectCategory category)
{
    switch (category)
    {
        case EffectCategory::Drive: return "Drive";
        case EffectCategory::Modulation: return "Modulation";
        case EffectCategory::Delay: return "Delay";
        case EffectCategory::Reverb: return "Reverb";
        case EffectCategory::Filter: return "Filter";
        case EffectCategory::Dynamics: return "Dynamics";
        case EffectCategory::EQ: return "EQ";
        case EffectCategory::Pitch: return "Pitch";
        case EffectCategory::Amp: return "Amp";
        case EffectCategory::Cabinet: return "Cabinet";
        case EffectCategory::Special: return "Special";
        default: return "Unknown";
    }
}

// ====================================================================
// EffectCard Implementation
// ====================================================================

EffectBrowser::EffectCard::EffectCard(const EffectDescriptor& desc)
    : descriptor(desc)
{
    setSize(180, 120);
}

void EffectBrowser::EffectCard::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4);
    
    // Get category color
    juce::Colour categoryColor = descriptor.pedalColor;
    juce::String categoryStr = getCategoryString(descriptor.category);
    
    if (hovered)
        categoryColor = categoryColor.brighter(0.3f);
    
    // Shadow
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillRoundedRectangle(bounds.translated(2, 3), 8.0f);
    
    // Card background
    juce::ColourGradient gradient(
        categoryColor.darker(0.3f), bounds.getCentreX(), bounds.getY(),
        categoryColor.darker(0.7f), bounds.getCentreX(), bounds.getBottom(),
        false
    );
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, 8.0f);
    
    // Border
    g.setColour(hovered ? juce::Colours::white : categoryColor.brighter(0.3f));
    g.drawRoundedRectangle(bounds, 8.0f, hovered ? 2.5f : 1.5f);
    
    // Top highlight
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.fillRoundedRectangle(bounds.reduced(8, 8).withHeight(25), 5.0f);
    
    // Category badge
    auto badgeBounds = juce::Rectangle<float>(bounds.getRight() - 80, bounds.getY() + 8, 70, 18);
    g.setColour(categoryColor.darker(0.5f).withAlpha(0.8f));
    g.fillRoundedRectangle(badgeBounds, 9.0f);
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.setColour(juce::Colours::white.withAlpha(0.9f));
    g.drawText(categoryStr, badgeBounds, juce::Justification::centred);
    
    // Effect name
    auto nameBounds = bounds.reduced(10, 10).withHeight(60);
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.setColour(juce::Colours::white);
    g.drawFittedText(descriptor.displayName, nameBounds.toNearestInt(), 
                     juce::Justification::centredLeft, 3);
    
    // CPU indicator
    auto cpuBounds = bounds.reduced(10, 10);
    cpuBounds.removeFromTop(bounds.getHeight() - 35);
    cpuBounds.setHeight(20);
    
    g.setFont(juce::Font(10.0f));
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    
    juce::String cpuText = "CPU: ";
    if (descriptor.estimatedCPUCost < 0.3f) cpuText += "Low";
    else if (descriptor.estimatedCPUCost < 0.6f) cpuText += "Medium";
    else cpuText += "High";
    
    g.drawText(cpuText, cpuBounds, juce::Justification::centredLeft);
    
    // CPU bar
    auto cpuBarBounds = cpuBounds.removeFromBottom(8).reduced(0, 2);
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.fillRoundedRectangle(cpuBarBounds, 2.0f);
    
    auto cpuFill = cpuBarBounds.removeFromLeft(cpuBarBounds.getWidth() * descriptor.estimatedCPUCost);
    auto cpuColor = descriptor.estimatedCPUCost < 0.5f ? juce::Colours::green :
                   descriptor.estimatedCPUCost < 0.8f ? juce::Colours::orange :
                   juce::Colours::red;
    g.setColour(cpuColor.withAlpha(0.7f));
    g.fillRoundedRectangle(cpuFill, 2.0f);
    
    // Hover overlay
    if (hovered)
    {
        g.setColour(juce::Colours::white.withAlpha(0.1f));
        g.fillRoundedRectangle(bounds, 8.0f);
    }
}

void EffectBrowser::EffectCard::mouseEnter(const juce::MouseEvent&)
{
    hovered = true;
    repaint();
}

void EffectBrowser::EffectCard::mouseExit(const juce::MouseEvent&)
{
    hovered = false;
    repaint();
}

void EffectBrowser::EffectCard::mouseDown(const juce::MouseEvent&)
{
    if (onClick)
        onClick(descriptor);
}

// ====================================================================
// EffectBrowser Implementation
// ====================================================================

EffectBrowser::EffectBrowser()
{
    // Search box
    addAndMakeVisible(searchBox);
    searchBox.setTextToShowWhenEmpty("Search effects...", juce::Colours::grey);
    searchBox.setFont(juce::Font(14.0f));
    searchBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff2a2a2a));
    searchBox.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    searchBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0xff444444));
    searchBox.addListener(this);
    
    // Category buttons
    juce::StringArray categories = { "All", "Drive", "Modulation", "Delay", "Reverb", 
                                     "Filter", "Dynamics", "EQ", "Pitch", "Amp", "Cabinet" };
    
    for (int i = 0; i < 11; ++i)
    {
        addAndMakeVisible(categoryButtons[i]);
        categoryButtons[i].setButtonText(categories[i]);
        categoryButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2a2a2a));
        categoryButtons[i].setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff00E5FF));
        categoryButtons[i].setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        categoryButtons[i].setColour(juce::TextButton::textColourOnId, juce::Colours::black);
        categoryButtons[i].setClickingTogglesState(true);
        categoryButtons[i].setRadioGroupId(1001);
        
        categoryButtons[i].onClick = [this, i, categories]()
        {
            selectCategory(categories[i]);
        };
    }
    
    categoryButtons[0].setToggleState(true, juce::dontSendNotification);
    
    // Viewport for scrolling
    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&effectContainer, false);
    viewport.setScrollBarsShown(true, false);
    
    loadEffects();
}

void EffectBrowser::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));
    
    // Header background
    g.setColour(juce::Colour(0xff0a0a0a));
    g.fillRect(0, 0, getWidth(), 120);
    
    // Title
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.setColour(juce::Colours::white);
    g.drawText("Effect Browser", 20, 10, 300, 30, juce::Justification::centredLeft);
    
    // Effect count
    g.setFont(juce::Font(12.0f));
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawText(juce::String(effectCards.size()) + " effects", 
               20, 35, 300, 20, juce::Justification::centredLeft);
}

void EffectBrowser::resized()
{
    auto bounds = getLocalBounds();
    
    // Header area
    auto header = bounds.removeFromTop(120);
    header.removeFromTop(60); // Skip title area
    
    // Search box
    searchBox.setBounds(header.removeFromLeft(250).reduced(20, 10));
    
    // Category buttons (below search)
    header = getLocalBounds().removeFromTop(120);
    header.removeFromTop(60);
    header.removeFromLeft(270); // Skip search area
    
    int buttonWidth = 90;
    int buttonHeight = 28;
    int spacing = 5;
    
    for (int i = 0; i < 6; ++i)
    {
        categoryButtons[i].setBounds(header.getX() + (i % 6) * (buttonWidth + spacing),
                                     header.getY() + 10,
                                     buttonWidth, buttonHeight);
    }
    
    for (int i = 6; i < 11; ++i)
    {
        categoryButtons[i].setBounds(header.getX() + ((i - 6) % 6) * (buttonWidth + spacing),
                                     header.getY() + 10 + buttonHeight + spacing,
                                     buttonWidth, buttonHeight);
    }
    
    // Viewport for effect cards
    viewport.setBounds(bounds);
    
    updateLayout();
}

void EffectBrowser::textEditorTextChanged(juce::TextEditor& editor)
{
    searchText = editor.getText().toLowerCase();
    filterEffects();
}

void EffectBrowser::loadEffects()
{
    auto& library = EffectLibrary::getInstance();
    auto allEffects = library.getAllEffects();
    
    effectCards.clear();
    
    for (const auto& descriptor : allEffects)
    {
        auto* card = new EffectCard(descriptor);
        card->onClick = [this](const EffectDescriptor& desc)
        {
            if (onEffectSelected)
                onEffectSelected(desc.id, getCategoryString(desc.category));
        };
        
        effectContainer.addAndMakeVisible(card);
        effectCards.add(card);
    }
    
    filterEffects();
}

void EffectBrowser::filterEffects()
{
    int visibleCount = 0;
    
    for (auto* card : effectCards)
    {
        juce::String cardCategory = getCategoryString(card->descriptor.category);
        bool matchesCategory = (currentCategory == "All" || cardCategory == currentCategory);
        bool matchesSearch = (searchText.isEmpty() || 
                             card->descriptor.displayName.toLowerCase().contains(searchText) ||
                             cardCategory.toLowerCase().contains(searchText));
        
        bool shouldBeVisible = matchesCategory && matchesSearch;
        card->setVisible(shouldBeVisible);
        
        if (shouldBeVisible)
            visibleCount++;
    }
    
    updateLayout();
    repaint();
}

void EffectBrowser::updateLayout()
{
    int cardWidth = 180;
    int cardHeight = 120;
    int spacing = 10;
    int padding = 10;
    
    int columns = juce::jmax(1, (viewport.getWidth() - padding * 2) / (cardWidth + spacing));
    int x = padding;
    int y = padding;
    int col = 0;
    
    for (auto* card : effectCards)
    {
        if (card->isVisible())
        {
            card->setBounds(x, y, cardWidth, cardHeight);
            
            col++;
            x += cardWidth + spacing;
            
            if (col >= columns)
            {
                col = 0;
                x = padding;
                y += cardHeight + spacing;
            }
        }
    }
    
    // Update container size
    int totalHeight = y + (col > 0 ? cardHeight + padding : padding);
    effectContainer.setSize(viewport.getWidth(), totalHeight);
}

void EffectBrowser::selectCategory(const juce::String& category)
{
    currentCategory = category;
    filterEffects();
}

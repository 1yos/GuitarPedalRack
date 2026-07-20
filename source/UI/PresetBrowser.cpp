#include "PresetBrowser.h"

// ====================================================================
// PresetCard Implementation
// ====================================================================

PresetBrowser::PresetCard::PresetCard(const ChainPreset& p)
    : preset(p)
{
    setSize(200, 140);
    isFactory = (preset.author != "User");
}

void PresetBrowser::PresetCard::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4);
    
    // Determine card color
    juce::Colour cardColor = juce::Colour(0xff2a2a2a);
    if (isFactory)
        cardColor = juce::Colour(0xff1a4a6a); // Blue tint for factory presets
    
    if (hovered)
        cardColor = cardColor.brighter(0.3f);
    
    // Shadow
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillRoundedRectangle(bounds.translated(2, 3), 8.0f);
    
    // Card background
    juce::ColourGradient gradient(
        cardColor.brighter(0.2f), bounds.getCentreX(), bounds.getY(),
        cardColor.darker(0.3f), bounds.getCentreX(), bounds.getBottom(),
        false
    );
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, 8.0f);
    
    // Border
    g.setColour(hovered ? juce::Colours::white : cardColor.brighter(0.5f));
    g.drawRoundedRectangle(bounds, 8.0f, hovered ? 2.5f : 1.5f);
    
    // Top highlight
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.fillRoundedRectangle(bounds.reduced(8, 8).withHeight(25), 5.0f);
    
    // Preset name
    auto textBounds = bounds.reduced(12, 12);
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.setColour(juce::Colours::white);
    g.drawFittedText(preset.name, textBounds.removeFromTop(40).toNearestInt(), 
                     juce::Justification::centredLeft, 2);
    
    // Description
    g.setFont(juce::Font(12.0f));
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.drawFittedText(preset.description, textBounds.removeFromTop(50).toNearestInt(), 
                     juce::Justification::topLeft, 3);
    
    // Author/type badge
    auto badge = bounds.reduced(10, 10).removeFromBottom(20);
    g.setFont(juce::Font(10.0f));
    g.setColour(juce::Colours::white.withAlpha(0.5f));
    g.drawText(isFactory ? "Factory" : "User", badge, juce::Justification::centredLeft);
    
    // Delete button for user presets
    if (!isFactory && hovered)
    {
        auto deleteBtn = juce::Rectangle<float>(bounds.getRight() - 30, bounds.getY() + 10, 20, 20);
        g.setColour(juce::Colours::red.withAlpha(0.8f));
        g.fillRoundedRectangle(deleteBtn, 3.0f);
        g.setColour(juce::Colours::white);
        g.drawLine(deleteBtn.getX() + 5, deleteBtn.getY() + 5, 
                   deleteBtn.getRight() - 5, deleteBtn.getBottom() - 5, 2.0f);
        g.drawLine(deleteBtn.getRight() - 5, deleteBtn.getY() + 5, 
                   deleteBtn.getX() + 5, deleteBtn.getBottom() - 5, 2.0f);
    }
}

void PresetBrowser::PresetCard::mouseEnter(const juce::MouseEvent&)
{
    hovered = true;
    repaint();
}

void PresetBrowser::PresetCard::mouseExit(const juce::MouseEvent&)
{
    hovered = false;
    repaint();
}

void PresetBrowser::PresetCard::mouseDown(const juce::MouseEvent& e)
{
    // Check if clicking delete button
    if (!isFactory && e.getPosition().getX() > getWidth() - 35)
    {
        if (onDelete)
            onDelete(preset);
        return;
    }
    
    if (onClick)
        onClick(preset);
}

// ====================================================================
// PresetBrowser Implementation
// ====================================================================

PresetBrowser::PresetBrowser()
{
    // Search box
    addAndMakeVisible(searchBox);
    searchBox.setTextToShowWhenEmpty("Search presets...", juce::Colours::grey);
    searchBox.setFont(juce::Font(14.0f));
    searchBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff2a2a2a));
    searchBox.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    searchBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0xff444444));
    searchBox.addListener(this);
    
    // Category buttons
    juce::StringArray categories = { "All", "Clean", "Blues", "Rock", "Metal", "Lead", "Ambient", "Bass" };
    
    for (int i = 0; i < 8; ++i)
    {
        addAndMakeVisible(categoryButtons[i]);
        categoryButtons[i].setButtonText(categories[i]);
        categoryButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2a2a2a));
        categoryButtons[i].setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff00E5FF));
        categoryButtons[i].setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        categoryButtons[i].setColour(juce::TextButton::textColourOnId, juce::Colours::black);
        categoryButtons[i].setClickingTogglesState(true);
        categoryButtons[i].setRadioGroupId(2001);
        
        categoryButtons[i].onClick = [this, i, categories]()
        {
            selectCategory(categories[i]);
        };
    }
    
    categoryButtons[0].setToggleState(true, juce::dontSendNotification);
    
    // Save button
    addAndMakeVisible(saveButton);
    saveButton.setButtonText("💾 SAVE PRESET");
    saveButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2ECC71));
    saveButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    saveButton.onClick = [this]()
    {
        showSaveDialog();
    };
    
    // Close button
    addAndMakeVisible(closeButton);
    closeButton.setButtonText("✕");
    closeButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xffE74C3C));
    closeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    closeButton.onClick = [this]()
    {
        if (onClose)
            onClose();
    };
    
    // Viewport for scrolling
    addAndMakeVisible(viewport);
    viewport.setViewedComponent(&presetContainer, false);
    viewport.setScrollBarsShown(true, false);
}

void PresetBrowser::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a1a));
    
    // Header background
    g.setColour(juce::Colour(0xff0a0a0a));
    g.fillRect(0, 0, getWidth(), 140);
    
    // Title
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.setColour(juce::Colours::white);
    g.drawText("Preset Browser", 20, 10, 300, 30, juce::Justification::centredLeft);
    
    // Preset count
    g.setFont(juce::Font(12.0f));
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawText(juce::String(presetCards.size()) + " presets", 
               20, 35, 300, 20, juce::Justification::centredLeft);
}

void PresetBrowser::resized()
{
    auto bounds = getLocalBounds();
    
    // Header area
    auto header = bounds.removeFromTop(140);
    header.removeFromTop(60); // Skip title area
    
    // Close button (top right)
    closeButton.setBounds(getWidth() - 50, 10, 40, 40);
    
    // Save button (below close)
    saveButton.setBounds(getWidth() - 160, 60, 150, 35);
    
    // Search box
    searchBox.setBounds(header.removeFromLeft(250).reduced(20, 10));
    
    // Category buttons
    header.removeFromLeft(20); // spacing
    int buttonWidth = 80;
    int buttonHeight = 28;
    int spacing = 8;
    
    for (int i = 0; i < 8; ++i)
    {
        int x = 290 + (i % 4) * (buttonWidth + spacing);
        int y = 70 + (i / 4) * (buttonHeight + spacing);
        categoryButtons[i].setBounds(x, y, buttonWidth, buttonHeight);
    }
    
    // Viewport for preset cards
    viewport.setBounds(bounds);
    
    updateLayout();
}

void PresetBrowser::textEditorTextChanged(juce::TextEditor& editor)
{
    searchText = editor.getText().toLowerCase();
    filterPresets();
}

void PresetBrowser::setPresetManager(PresetManager* manager)
{
    presetManager = manager;
    loadPresets();
}

void PresetBrowser::loadPresets()
{
    if (!presetManager)
        return;
    
    presetCards.clear();
    
    auto allPresets = presetManager->getAllPresets();
    
    for (const auto& preset : allPresets)
    {
        auto* card = new PresetCard(preset);
        card->onClick = [this](const ChainPreset& p)
        {
            if (onPresetSelected)
                onPresetSelected(p.name);
        };
        
        card->onDelete = [this](const ChainPreset& p)
        {
            if (presetManager)
            {
                // Confirm deletion
                auto options = juce::MessageBoxOptions()
                    .withIconType(juce::MessageBoxIconType::QuestionIcon)
                    .withTitle("Delete Preset")
                    .withMessage("Are you sure you want to delete '" + p.name + "'?")
                    .withButton("Yes")
                    .withButton("No");
                
                juce::AlertWindow::showAsync(options, [this, p](int result)
                {
                    if (result == 1) // Yes clicked
                    {
                        presetManager->deletePreset(p.name);
                        loadPresets();
                    }
                });
            }
        };
        
        presetContainer.addAndMakeVisible(card);
        presetCards.add(card);
    }
    
    filterPresets();
}

void PresetBrowser::filterPresets()
{
    int visibleCount = 0;
    
    for (auto* card : presetCards)
    {
        bool matchesCategory = false;
        if (currentCategory == "All")
        {
            matchesCategory = true;
        }
        else
        {
            // Match against preset.category (primary) OR preset.tags (fallback)
            juce::String catLower = currentCategory.toLowerCase();
            matchesCategory = (card->preset.category.toLowerCase() == catLower)
                           || card->preset.tags.contains(catLower);
        }
        
        bool matchesSearch = (searchText.isEmpty() || 
                             card->preset.name.toLowerCase().contains(searchText) ||
                             card->preset.description.toLowerCase().contains(searchText));
        
        bool shouldBeVisible = matchesCategory && matchesSearch;
        card->setVisible(shouldBeVisible);
        
        if (shouldBeVisible)
            visibleCount++;
    }
    
    updateLayout();
    repaint();
}

void PresetBrowser::updateLayout()
{
    int cardWidth = 200;
    int cardHeight = 140;
    int spacing = 15;
    int padding = 15;
    
    int columns = juce::jmax(1, (viewport.getWidth() - padding * 2) / (cardWidth + spacing));
    int x = padding;
    int y = padding;
    int col = 0;
    
    for (auto* card : presetCards)
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
    presetContainer.setSize(viewport.getWidth(), totalHeight);
}

void PresetBrowser::selectCategory(const juce::String& category)
{
    currentCategory = category;
    filterPresets();
}

void PresetBrowser::showSaveDialog()
{
    if (onSaveNewPreset)
        onSaveNewPreset();
}

#include "FunctionalAmpHead.h"

//==============================================================================
FunctionalAmpHead::FunctionalAmpHead(AudioProcessorValueTreeState& apvts)
    : apvtsRef(apvts),
      gainKnob(HardwareKnob::Style::Skirted),
      bassKnob(HardwareKnob::Style::Skirted),
      midKnob(HardwareKnob::Style::Skirted),
      trebleKnob(HardwareKnob::Style::Skirted),
      presenceKnob(HardwareKnob::Style::Skirted),
      masterKnob(HardwareKnob::Style::Skirted)
{
    // Set knob colors (amber for amp)
    Colour ampColor(0xffffaa00);
    gainKnob.setAccentColor(ampColor);
    bassKnob.setAccentColor(ampColor);
    midKnob.setAccentColor(ampColor);
    trebleKnob.setAccentColor(ampColor);
    presenceKnob.setAccentColor(ampColor);
    masterKnob.setAccentColor(ampColor);
    
    // Add knobs
    addAndMakeVisible(gainKnob);
    addAndMakeVisible(bassKnob);
    addAndMakeVisible(midKnob);
    addAndMakeVisible(trebleKnob);
    addAndMakeVisible(presenceKnob);
    addAndMakeVisible(masterKnob);
    
    // Create APVTS attachments
    gainAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "ampGain", gainKnob);
    bassAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "ampBass", bassKnob);
    midAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "ampMid", midKnob);
    trebleAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "ampTreble", trebleKnob);
    presenceAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "ampPresence", presenceKnob);
    masterAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "ampMaster", masterKnob);
    
    // Setup labels
    auto setupLabel = [this](Label& label, const String& text)
    {
        addAndMakeVisible(label);
        label.setText(text, dontSendNotification);
        label.setFont(Font(10.0f, Font::bold));
        label.setJustificationType(Justification::centred);
        label.setColour(Label::textColourId, Colours::white.withAlpha(0.8f));
    };
    
    setupLabel(gainLabel, "GAIN");
    setupLabel(bassLabel, "BASS");
    setupLabel(midLabel, "MID");
    setupLabel(trebleLabel, "TREBLE");
    setupLabel(presenceLabel, "PRES");
    setupLabel(masterLabel, "MASTER");
    
    // Amp name label
    addAndMakeVisible(ampNameLabel);
    ampNameLabel.setText("ALPHA AMP", dontSendNotification);
    ampNameLabel.setFont(Font(18.0f, Font::bold));
    ampNameLabel.setJustificationType(Justification::centred);
    ampNameLabel.setColour(Label::textColourId, Colour(0xffffaa00));
    
    // Channel selector
    addAndMakeVisible(channelSelector);
    channelSelector.addItem("Clean", 1);
    channelSelector.addItem("Crunch", 2);
    channelSelector.addItem("Lead", 3);
    channelSelector.setSelectedId(2, dontSendNotification);  // Default to Crunch
    channelSelector.setColour(ComboBox::backgroundColourId, Colour(0xff1A1A1A));
    channelSelector.setColour(ComboBox::textColourId, Colours::white);
    channelSelector.setColour(ComboBox::outlineColourId, Colour(0xff404040));
    
    channelAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "ampChannel", channelSelector);
}

//==============================================================================
void FunctionalAmpHead::paint(Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Draw amp chassis
    drawAmpChassis(g, bounds);
    
    // Draw speaker grill at bottom
    auto grillBounds = bounds.removeFromBottom(80.0f).reduced(20.0f, 10.0f);
    drawSpeakerGrill(g, grillBounds);
    
    // Draw power LED
    auto ledBounds = Rectangle<float>(bounds.getX() + 20.0f, bounds.getY() + 20.0f, 12.0f, 12.0f);
    drawPowerLED(g, ledBounds);
}

void FunctionalAmpHead::drawAmpChassis(Graphics& g, Rectangle<float> bounds)
{
    // Shadow
    g.setColour(Colour(0x60000000));
    g.fillRoundedRectangle(bounds.translated(0.0f, 4.0f), 8.0f);
    
    // Main body - metallic gradient
    ColourGradient bodyGrad(Colour(0xFF3A3A3A), bounds.getCentreX(), bounds.getY(),
                           Colour(0xFF1A1A1A), bounds.getCentreX(), bounds.getBottom(),
                           false);
    g.setGradientFill(bodyGrad);
    g.fillRoundedRectangle(bounds, 8.0f);
    
    // Top accent strip (amber glow)
    g.setColour(Colour(0xffffaa00).withAlpha(0.3f));
    g.fillRoundedRectangle(bounds.removeFromTop(6.0f).reduced(10.0f, 0.0f), 3.0f);
    
    // Border
    g.setColour(Colour(0xFF404040));
    g.drawRoundedRectangle(bounds.expanded(0.0f, -6.0f), 8.0f, 2.0f);
    
    // Side vents (decorative)
    g.setColour(Colour(0xFF0A0A0A));
    for (int i = 0; i < 8; ++i)
    {
        float y = 60.0f + i * 20.0f;
        g.fillRect(bounds.getX() + 12.0f, y, 3.0f, 12.0f);
        g.fillRect(bounds.getRight() - 15.0f, y, 3.0f, 12.0f);
    }
}

void FunctionalAmpHead::drawPowerLED(Graphics& g, Rectangle<float> bounds)
{
    auto center = bounds.getCentre();
    float radius = jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    
    bool bypassed = isBypassed();
    
    // LED bezel
    g.setColour(Colour(0xFF303030));
    g.fillEllipse(bounds);
    
    if (!bypassed)
    {
        // Glow effect
        float glowRadius = radius * 2.0f;
        ColourGradient glow(Colour(0xffffaa00).withAlpha(0.6f),
                           center.x, center.y,
                           Colours::transparentBlack,
                           center.x, center.y + glowRadius,
                           true);
        g.setGradientFill(glow);
        g.fillEllipse(center.x - glowRadius, center.y - glowRadius,
                     glowRadius * 2.0f, glowRadius * 2.0f);
        
        // LED lit (amber)
        g.setColour(Colour(0xffffaa00));
        g.fillEllipse(bounds.reduced(2.0f));
    }
    else
    {
        // LED off
        g.setColour(Colour(0xFF1A1A1A));
        g.fillEllipse(bounds.reduced(2.0f));
    }
}

void FunctionalAmpHead::drawSpeakerGrill(Graphics& g, Rectangle<float> bounds)
{
    // Grill background
    g.setColour(Colour(0xFF0A0A0A));
    g.fillRoundedRectangle(bounds, 4.0f);
    
    // Horizontal grill lines
    g.setColour(Colour(0xFF050505));
    int numLines = 8;
    float lineSpacing = bounds.getHeight() / (numLines + 1);
    
    for (int i = 1; i <= numLines; ++i)
    {
        float y = bounds.getY() + i * lineSpacing;
        g.fillRect(bounds.getX() + 10.0f, y - 1.0f, bounds.getWidth() - 20.0f, 2.0f);
    }
}

void FunctionalAmpHead::resized()
{
    auto area = getLocalBounds();
    
    // Top area - name and power LED
    auto topArea = area.removeFromTop(50);
    topArea.removeFromLeft(40);  // Space for LED
    ampNameLabel.setBounds(topArea.reduced(10, 12));
    
    // Channel selector
    auto channelArea = area.removeFromTop(35).reduced(20, 5);
    channelSelector.setBounds(channelArea.withWidth(120).withX(channelArea.getCentreX() - 60));
    
    area.removeFromTop(10);  // Spacing
    
    // Knobs area - 6 knobs in a row
    auto knobArea = area.removeFromTop(90).reduced(15, 0);
    int knobSize = 55;
    int numKnobs = 6;
    int totalWidth = knobSize * numKnobs;
    int spacing = (knobArea.getWidth() - totalWidth) / (numKnobs + 1);
    
    auto placeKnob = [&](HardwareKnob& knob, Label& label)
    {
        int x = knobArea.getX() + spacing;
        knob.setBounds(x, knobArea.getY(), knobSize, knobSize);
        label.setBounds(x - 5, knobArea.getY() + knobSize + 2, knobSize + 10, 15);
        spacing += knobSize + (knobArea.getWidth() - totalWidth) / (numKnobs + 1);
    };
    
    placeKnob(gainKnob, gainLabel);
    placeKnob(bassKnob, bassLabel);
    placeKnob(midKnob, midLabel);
    placeKnob(trebleKnob, trebleLabel);
    placeKnob(presenceKnob, presenceLabel);
    placeKnob(masterKnob, masterLabel);
    
    // Remaining space is for speaker grill (handled in paint)
}

//==============================================================================
String FunctionalAmpHead::getCurrentChannel() const
{
    int selectedId = channelSelector.getSelectedId();
    switch (selectedId)
    {
        case 1: return "Clean";
        case 2: return "Crunch";
        case 3: return "Lead";
        default: return "Unknown";
    }
}

bool FunctionalAmpHead::isBypassed() const
{
    auto* param = apvtsRef.getRawParameterValue("ampBypass");
    return param != nullptr && *param > 0.5f;
}

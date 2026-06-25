#include "FunctionalPedal.h"

//==============================================================================
FunctionalPedal::FunctionalPedal(const String& pedalName,
                                 const String& effectType,
                                 const Colour& categoryColor,
                                 AudioProcessorValueTreeState& apvts,
                                 const String& bypassParamID,
                                 const String& param1ID,
                                 const String& param2ID,
                                 const String& param3ID)
    : name(pedalName),
      type(effectType),
      accentColor(categoryColor),
      apvtsRef(apvts),
      bypassParamID(bypassParamID),
      knob1(HardwareKnob::Style::MetalRounded),
      knob2(HardwareKnob::Style::MetalRounded),
      knob3(HardwareKnob::Style::MetalRounded)
{
    // Customise accent color and styles to match reference screenshots
    if (name == "DEEP HEAT")
    {
        knob1.setAccentColor(Colour(0xffF5B400)); // Yellow tick mark
        knob2.setAccentColor(Colour(0xffF5B400));
        knob3.setAccentColor(Colour(0xffF5B400));
    }
    else if (name == "VOID")
    {
        knob1.setSliderStyle(Slider::LinearHorizontal);
        knob1.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
        knob1.setAccentColor(Colour(0xff8F2FFF)); // Purple tick/glow
        knob2.setAccentColor(Colour(0xff8F2FFF));
        knob3.setAccentColor(Colour(0xff8F2FFF));
    }
    else if (name == "PULSE")
    {
        knob1.setAccentColor(Colour(0xff00FF88)); // Green tick/glow
        knob2.setAccentColor(Colour(0xff00FF88));
        knob3.setAccentColor(Colour(0xff00FF88));
    }
    else
    {
        knob1.setAccentColor(accentColor);
        knob2.setAccentColor(accentColor);
        knob3.setAccentColor(accentColor);
    }
    
    // Add knobs
    addAndMakeVisible(knob1);
    addAndMakeVisible(knob2);
    addAndMakeVisible(knob3);
    
    // Create APVTS attachments
    knob1Attachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
        apvts, param1ID, knob1);
    knob2Attachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
        apvts, param2ID, knob2);
    knob3Attachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
        apvts, param3ID, knob3);
    
    // Setup bypass button (invisible, manual click handling)
    addChildComponent(bypassButton);
    bypassButton.setVisible(false);
    
    bypassAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(
        apvts, bypassParamID, bypassButton);
    
    // Default labels
    knobLabel1 = "PARAM 1";
    knobLabel2 = "PARAM 2";
    knobLabel3 = "PARAM 3";
    
    // Start LED animation timer
    startTimerHz(60);  // 60fps for smooth animation
    
    // Initialize LED state
    ledBrightness = isBypassed() ? 0.0f : 1.0f;
    targetLedBrightness = ledBrightness;
}

//==============================================================================
void FunctionalPedal::paint(Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // 1. Draw pedal enclosure with its unique gradient
    drawPedalEnclosure(g, bounds);
    
    // 2. Draw name at top
    g.setColour(Colours::white);
    
    // Font setup
    if (name == "VOID")
    {
        g.setFont(Font("Georgia", 22.0f, Font::bold | Font::italic));
        g.drawText("VOID", bounds.removeFromTop(45.0f).translated(0, 10), Justification::centred);
    }
    else if (name == "PULSE")
    {
        g.setFont(Font("Helvetica Neue", 20.0f, Font::bold));
        g.drawText("PULSE", bounds.removeFromTop(45.0f).translated(0, 10), Justification::centred);
    }
    else // DEEP HEAT
    {
        g.setFont(Font("Outfit", 18.0f, Font::bold));
        g.drawText("DEEP HEAT", bounds.removeFromTop(45.0f).translated(0, 10), Justification::centred);
    }
    
    // 3. Draw subtitle/description below name
    g.setFont(Font(9.0f, Font::bold));
    g.setColour(Colours::white.withAlpha(0.6f));
    if (name == "VOID")
        g.drawText("ETHEREAL SPACE", 0, 42, getWidth(), 15, Justification::centred);
    else if (name == "PULSE")
        g.drawText("Sine Modulator", 0, 42, getWidth(), 15, Justification::centred);
    else // DEEP HEAT
        g.drawText("OVERDRIVE ENGINE", 0, 42, getWidth(), 15, Justification::centred);
        
    // 4. Draw labels for the knobs
    drawLabels(g);
    
    // 5. Draw footswitch
    if (!footswitchBounds.isEmpty())
    {
        drawFootswitch(g, footswitchBounds.toFloat());
    }
    
    // 6. Draw LED
    Rectangle<float> ledBounds;
    if (name == "DEEP HEAT")
        ledBounds = Rectangle<float>((getWidth() - 10) / 2.0f, 171.0f, 10.0f, 10.0f);
    else if (name == "VOID")
        ledBounds = Rectangle<float>((getWidth() - 10) / 2.0f, 156.0f, 10.0f, 10.0f);
    else // PULSE
        ledBounds = Rectangle<float>((getWidth() - 10) / 2.0f, 171.0f, 10.0f, 10.0f);
        
    drawLED(g, ledBounds);
    
    // Hover effect
    if (isHovered)
    {
        g.setColour(accentColor.withAlpha(0.06f));
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 12.0f);
    }
}

void FunctionalPedal::drawPedalEnclosure(Graphics& g, Rectangle<float> bounds)
{
    // Shadow
    g.setColour(Colour(0x50000000));
    g.fillRoundedRectangle(bounds.translated(0.0f, 4.0f).reduced(2.0f), 12.0f);
    
    // Main body gradient
    ColourGradient bodyGrad;
    if (name == "DEEP HEAT")
    {
        bodyGrad = ColourGradient(Colour(0xffd5983d), bounds.getCentreX(), bounds.getY(),
                                  Colour(0xff724317), bounds.getCentreX(), bounds.getBottom(),
                                  false);
    }
    else if (name == "VOID")
    {
        bodyGrad = ColourGradient(Colour(0xff551188), bounds.getCentreX(), bounds.getY(),
                                  Colour(0xff220044), bounds.getCentreX(), bounds.getBottom(),
                                  false);
    }
    else if (name == "PULSE")
    {
        bodyGrad = ColourGradient(Colour(0xff008877), bounds.getCentreX(), bounds.getY(),
                                  Colour(0xff004433), bounds.getCentreX(), bounds.getBottom(),
                                  false);
    }
    else
    {
        bodyGrad = ColourGradient(accentColor, bounds.getCentreX(), bounds.getY(),
                                  accentColor.darker(0.5f), bounds.getCentreX(), bounds.getBottom(),
                                  false);
    }
    
    g.setGradientFill(bodyGrad);
    g.fillRoundedRectangle(bounds, 12.0f);
    
    // Top highlight (radial/soft flare at top left)
    ColourGradient highlightGrad(Colour(0x18FFFFFF), bounds.getX(), bounds.getY(),
                                 Colours::transparentWhite, bounds.getCentreX(), bounds.getCentreY(),
                                 true);
    g.setGradientFill(highlightGrad);
    g.fillRoundedRectangle(bounds, 12.0f);
    
    // Border
    g.setColour(Colour(0xFF070707).withAlpha(0.6f));
    g.drawRoundedRectangle(bounds, 12.0f, 1.5f);
}

void FunctionalPedal::drawFootswitch(Graphics& g, Rectangle<float> bounds)
{
    // Draw a square footswitch matching the screenshots: a light-grey metallic rounded square button
    auto center = bounds.getCentre();
    float pressOffset = footswitchPressed ? 1.5f : 0.0f;
    auto switchArea = bounds.withSizeKeepingCentre(44, 44).translated(0.0f, pressOffset);
    
    // Switch shadow
    if (!footswitchPressed)
    {
        g.setColour(Colour(0x40000000));
        g.fillRoundedRectangle(switchArea.translated(0, 3), 6.0f);
    }
    
    // Outer metallic bevel
    ColourGradient bevelGrad(Colour(0xFFD2D2D2), switchArea.getX(), switchArea.getY(),
                             Colour(0xFF9E9E9E), switchArea.getX(), switchArea.getBottom(),
                             false);
    g.setGradientFill(bevelGrad);
    g.fillRoundedRectangle(switchArea, 6.0f);
    
    // Inner surface
    auto innerArea = switchArea.reduced(2.0f);
    ColourGradient innerGrad(Colour(0xFFAAAAAA), innerArea.getX(), innerArea.getY(),
                            Colour(0xFF757575), innerArea.getX(), innerArea.getBottom(),
                            false);
    g.setGradientFill(innerGrad);
    g.fillRoundedRectangle(innerArea, 5.0f);
    
    // Center bolt
    auto boltArea = innerArea.reduced(10.0f);
    g.setColour(Colour(0xFF404040));
    g.fillEllipse(boltArea);
    g.setColour(Colour(0xFF757575));
    g.drawEllipse(boltArea, 1.5f);
}

void FunctionalPedal::drawLED(Graphics& g, Rectangle<float> bounds)
{
    auto center = bounds.getCentre();
    float radius = 5.0f; // clean small glowing LED
    
    // Draw a glowing LED of the pedal's accentColor
    if (ledBrightness > 0.01f)
    {
        // Glow
        float glowRadius = radius * 3.0f;
        ColourGradient glow(accentColor.withAlpha(ledBrightness * 0.7f), center.x, center.y,
                            Colours::transparentBlack, center.x + glowRadius, center.y + glowRadius,
                            true);
        g.setGradientFill(glow);
        g.fillEllipse(center.x - glowRadius, center.y - glowRadius, glowRadius * 2.0f, glowRadius * 2.0f);
        
        // Solid center
        g.setColour(Colours::white.overlaidWith(accentColor.withAlpha(0.9f)));
        g.fillEllipse(center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);
    }
    else
    {
        // Off state (dark/faded)
        g.setColour(accentColor.darker(1.5f).withAlpha(0.3f));
        g.fillEllipse(center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);
        
        g.setColour(Colour(0x60000000));
        g.drawEllipse(center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f, 1.0f);
    }
}

void FunctionalPedal::drawLabels(Graphics& g)
{
    g.setColour(Colours::white.withAlpha(0.6f));
    g.setFont(Font(9.0f, Font::bold));
    
    if (name == "DEEP HEAT")
    {
        g.drawText("DRIVE", knob1.getBounds().translated(0, -11), Justification::centred);
        g.drawText("TONE", knob2.getBounds().translated(0, -11), Justification::centred);
        g.drawText("LEVEL", knob3.getBounds().translated(0, -11), Justification::centred);
    }
    else if (name == "VOID")
    {
        g.drawText("DECAY", knob2.getBounds().translated(0, -11), Justification::centred);
        g.drawText("MIX", knob3.getBounds().translated(0, -11), Justification::centred);
    }
    else if (name == "PULSE")
    {
        g.drawText("RATE", knob1.getBounds().translated(0, -11), Justification::centred);
        g.drawText("DEPTH", knob2.getBounds().translated(0, -11), Justification::centred);
    }
    else
    {
        g.drawText(knobLabel1, knob1.getBounds().translated(0, knob1.getHeight() + 2), Justification::centred);
        g.drawText(knobLabel2, knob2.getBounds().translated(0, knob2.getHeight() + 2), Justification::centred);
        g.drawText(knobLabel3, knob3.getBounds().translated(0, knob3.getHeight() + 2), Justification::centred);
    }
}

void FunctionalPedal::resized()
{
    auto bounds = getLocalBounds();
    
    // Title space (45px)
    bounds.removeFromTop(45);
    
    // Subtitle space (15px)
    bounds.removeFromTop(15);
    
    if (name == "DEEP HEAT")
    {
        // 1 large top center knob (DRIVE)
        // 2 small bottom side-by-side knobs (TONE, LEVEL)
        int topKnobSize = 56;
        int bottomKnobSize = 44;
        
        knob1.setBounds((getWidth() - topKnobSize) / 2, 58, topKnobSize, topKnobSize);
        
        int spacing = 12;
        int startX = (getWidth() - (bottomKnobSize * 2 + spacing)) / 2;
        knob2.setBounds(startX, 118, bottomKnobSize, bottomKnobSize);
        knob3.setBounds(startX + bottomKnobSize + spacing, 118, bottomKnobSize, bottomKnobSize);
        
        knob1.setVisible(true);
        knob2.setVisible(true);
        knob3.setVisible(true);
    }
    else if (name == "VOID")
    {
        // Horizontal Shimmer bar on top (representing knob1)
        // 2 small side-by-side knobs (DECAY, MIX) representing knob2 and knob3
        int barW = 120;
        int barH = 18;
        knob1.setBounds((getWidth() - barW) / 2, 62, barW, barH);
        
        int knobSize = 44;
        int spacing = 16;
        int startX = (getWidth() - (knobSize * 2 + spacing)) / 2;
        knob2.setBounds(startX, 98, knobSize, knobSize);
        knob3.setBounds(startX + knobSize + spacing, 98, knobSize, knobSize);
        
        knob1.setVisible(true);
        knob2.setVisible(true);
        knob3.setVisible(true);
    }
    else if (name == "PULSE")
    {
        // 2 knobs vertically stacked: RATE (top) and DEPTH (bottom)
        // knob3 is Mix, let's hide it visuals or place it offscreen
        int knobSize = 46;
        knob1.setBounds((getWidth() - knobSize) / 2, 58, knobSize, knobSize);
        knob2.setBounds((getWidth() - knobSize) / 2, 114, knobSize, knobSize);
        
        knob3.setBounds(-100, -100, 10, 10); // place offscreen
        
        knob1.setVisible(true);
        knob2.setVisible(true);
        knob3.setVisible(false);
    }
    else
    {
        // Default layout
        auto knobArea = bounds.removeFromTop(100).reduced(10);
        int knobSize = 50;
        int spacing = (knobArea.getWidth() - (knobSize * 3)) / 4;
        
        knob1.setBounds(knobArea.removeFromLeft(spacing).removeFromLeft(knobSize)
                       .withSizeKeepingCentre(knobSize, knobSize));
        knobArea.removeFromLeft(spacing);
        knob2.setBounds(knobArea.removeFromLeft(knobSize)
                       .withSizeKeepingCentre(knobSize, knobSize));
        knobArea.removeFromLeft(spacing);
        knob3.setBounds(knobArea.removeFromLeft(knobSize)
                       .withSizeKeepingCentre(knobSize, knobSize));
    }
    
    // Footswitch area at the bottom
    int swSize = 44;
    footswitchBounds = Rectangle<int>((getWidth() - swSize) / 2, getHeight() - 55, swSize, swSize);
}

void FunctionalPedal::mouseDown(const MouseEvent& e)
{
    // Check if footswitch was clicked
    if (footswitchBounds.contains(e.getPosition()))
    {
        footswitchPressed = true;
        repaint();
        
        // Toggle bypass parameter
        auto* param = apvtsRef.getParameter(bypassParamID);
        if (param != nullptr)
        {
            float currentValue = param->getValue();
            param->setValueNotifyingHost(currentValue > 0.5f ? 0.0f : 1.0f);
        }
    }
    else
    {
        // Check if click was on chassis (not inside knobs)
        if (!knob1.getBounds().contains(e.getPosition()) &&
            !knob2.getBounds().contains(e.getPosition()) &&
            !knob3.getBounds().contains(e.getPosition()))
        {
            if (onDragStartCallback != nullptr)
                onDragStartCallback(this);
        }
    }
}

void FunctionalPedal::mouseDrag(const MouseEvent& e)
{
    // Chassis dragging triggers callback
    if (!footswitchPressed)
    {
        if (onDragCallback != nullptr)
            onDragCallback(this, e);
    }
}

void FunctionalPedal::mouseUp(const MouseEvent& e)
{
    if (footswitchPressed)
    {
        footswitchPressed = false;
        repaint();
    }
    else
    {
        if (onDragEndCallback != nullptr)
            onDragEndCallback(this);
    }
}

void FunctionalPedal::mouseEnter(const MouseEvent& e)
{
    isHovered = true;
    repaint();
}

void FunctionalPedal::mouseExit(const MouseEvent& e)
{
    isHovered = false;
    repaint();
}

//==============================================================================
void FunctionalPedal::timerCallback()
{
    // Smooth LED animation
    bool bypassed = isBypassed();
    targetLedBrightness = bypassed ? 0.0f : 1.0f;
    
    // Smooth interpolation
    float delta = targetLedBrightness - ledBrightness;
    ledBrightness += delta * 0.15f;  // 15% per frame
    
    // Repaint if animating
    if (std::abs(delta) > 0.01f)
    {
        repaint();
    }
}

bool FunctionalPedal::isBypassed() const
{
    auto* param = apvtsRef.getRawParameterValue(bypassParamID);
    return param != nullptr && *param > 0.5f;
}

void FunctionalPedal::setKnobLabels(const String& label1, const String& label2, const String& label3)
{
    knobLabel1 = label1;
    knobLabel2 = label2;
    knobLabel3 = label3;
    repaint();
}

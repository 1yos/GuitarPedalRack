#include "EffectModule.h"

//==============================================================================
EffectModule::EffectModule(EffectType type, const String& name, AudioProcessorValueTreeState& state)
    : effectType(type), displayName(name), apvts(state)
{
    // Name label
    addAndMakeVisible(nameLabel);
    nameLabel.setText(displayName, dontSendNotification);
    nameLabel.setFont(Font(14.0f, Font::bold));
    nameLabel.setColour(Label::textColourId, Colours::white);
    nameLabel.setJustificationType(Justification::centred);
    
    // Bypass button (LED style)
    addAndMakeVisible(bypassButton);
    bypassButton.setButtonText("");
    bypassButton.setClickingTogglesState(true);
    bypassButton.onClick = [this]() {
        bypassed = bypassButton.getToggleState();
        if (onBypassToggle) onBypassToggle();
        repaint();
    };
    
    // Remove button
    addAndMakeVisible(removeButton);
    removeButton.setButtonText("✕");
    removeButton.setColour(TextButton::buttonColourId, Colours::transparentBlack);
    removeButton.setColour(TextButton::textColourOffId, Colour(0xff8A8A8A));
    removeButton.onClick = [this]() {
        if (onRemove) onRemove(this);
    };
    
    // Setup knobs for this effect type
    setupKnobs();
}

//==============================================================================
void EffectModule::paint(Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Draw based on effect type
    if (effectType == EffectType::AmpSimulator)
        drawAmpStyle(g);
    else
        drawPedalStyle(g);
    
    // Drag overlay
    if (dragging)
    {
        g.setColour(Colour(0xff00E5FF).withAlpha(0.3f));
        g.fillRoundedRectangle(bounds, 8.0f);
        g.setColour(Colour(0xff00E5FF));
        g.drawRoundedRectangle(bounds, 8.0f, 2.0f);
    }
}

void EffectModule::drawPedalStyle(Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);
    
    // ALWAYS draw a visible background for testing
    g.setColour(Colours::red); // Bright red so we can see it!
    g.fillRoundedRectangle(bounds, 8.0f);
    
    // Background (darker when bypassed)
    Colour bgColor = bypassed ? Colour(0xff0A0A0A) : Colour(0xff1A1A1A);
    g.setColour(bgColor);
    g.fillRoundedRectangle(bounds.reduced(2.0f), 8.0f);
    
    // Effect color accent stripe (top)
    auto accentBounds = bounds.removeFromTop(6.0f);
    g.setColour(getEffectColor());
    g.fillRoundedRectangle(accentBounds.getX(), accentBounds.getY(), 
                           accentBounds.getWidth(), 6.0f, 8.0f);
    
    // Border (brighter when hovered, dimmer when bypassed)
    if (isHovered)
    {
        g.setColour(Colour(0xff3A3A3A));
        g.drawRoundedRectangle(bounds.getX(), bounds.getY(), 
                              bounds.getWidth(), bounds.getHeight(), 8.0f, 1.5f);
    }
    else if (!bypassed)
    {
        g.setColour(Colour(0xff2A2A2A));
        g.drawRoundedRectangle(bounds.getX(), bounds.getY(), 
                              bounds.getWidth(), bounds.getHeight(), 8.0f, 1.0f);
    }
    
    // LED indicator (top right)
    auto ledBounds = Rectangle<float>(bounds.getRight() - 20, bounds.getY() + 12, 10, 10);
    drawLED(g, ledBounds, !bypassed);
    
    // Bypass effect when bypassed
    if (bypassed)
    {
        g.setColour(Colour(0xff0A0A0A).withAlpha(0.7f));
        g.fillRoundedRectangle(bounds, 8.0f);
    }
}

void EffectModule::drawAmpStyle(Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);
    
    // Amp head background (tolex texture style)
    Colour bgColor = bypassed ? Colour(0xff0A0A0A) : Colour(0xff1A1512);
    g.setColour(bgColor);
    g.fillRoundedRectangle(bounds, 6.0f);
    
    // Metal front panel (top strip)
    auto panelBounds = bounds.removeFromTop(40.0f);
    g.setGradientFill(ColourGradient(Colour(0xff2A2A2A), panelBounds.getCentreX(), panelBounds.getY(),
                                      Colour(0xff1A1A1A), panelBounds.getCentreX(), panelBounds.getBottom(),
                                      false));
    g.fillRoundedRectangle(panelBounds, 6.0f);
    
    // Logo/name on metal panel
    g.setColour(getEffectColor());
    g.setFont(Font(16.0f, Font::bold));
    g.drawText(displayName.toUpperCase(), panelBounds, Justification::centred);
    
    // Border
    g.setColour(Colour(0xff2A2A2A));
    g.drawRoundedRectangle(bounds.getX(), bounds.getY(), 
                          bounds.getWidth(), bounds.getHeight(), 6.0f, 1.0f);
    
    // Power LED
    auto ledBounds = Rectangle<float>(bounds.getX() + 10, bounds.getY() + 50, 8, 8);
    drawLED(g, ledBounds, !bypassed);
}

void EffectModule::drawLED(Graphics& g, Rectangle<float> bounds, bool isOn)
{
    Colour ledColor = isOn ? getEffectColor() : Colour(0xff2A2A2A);
    
    // LED glow
    if (isOn)
    {
        g.setGradientFill(ColourGradient(ledColor.withAlpha(0.6f), bounds.getCentre(),
                                         ledColor.withAlpha(0.0f), bounds.getCentre() + Point<float>(8, 8),
                                         true));
        g.fillEllipse(bounds.expanded(6.0f));
    }
    
    // LED body
    g.setColour(ledColor);
    g.fillEllipse(bounds);
    
    // LED highlight
    if (isOn)
    {
        g.setColour(Colours::white.withAlpha(0.4f));
        g.fillEllipse(bounds.reduced(2.0f).translated(-1, -1));
    }
}

Colour EffectModule::getEffectColor() const
{
    switch (effectType)
    {
        case EffectType::NoiseGate:       return Colour(0xff66BB6A); // Green
        case EffectType::TubeOverdrive:   return Colour(0xffFF9800); // Orange
        case EffectType::Distortion:      return Colour(0xffF44336); // Red
        case EffectType::Compressor:      return Colour(0xff2196F3); // Blue
        case EffectType::Chorus:          return Colour(0xff9C27B0); // Purple
        case EffectType::Delay:           return Colour(0xff00BCD4); // Cyan
        case EffectType::Reverb:          return Colour(0xff3F51B5); // Indigo
        case EffectType::AmpSimulator:    return Colour(0xffFFC107); // Amber
        case EffectType::CabinetIR:       return Colour(0xff795548); // Brown
        case EffectType::EQ:              return Colour(0xff607D8B); // Blue Grey
        default:                          return Colour(0xff9E9E9E); // Grey
    }
}

//==============================================================================
void EffectModule::resized()
{
    auto bounds = getLocalBounds().reduced(8);
    
    // Top section: Remove button, name, LED (bypass button is invisible, just for click)
    auto topSection = bounds.removeFromTop(30);
    removeButton.setBounds(topSection.removeFromRight(24).withSizeKeepingCentre(20, 20));
    bypassButton.setBounds(topSection.removeFromRight(20).withSizeKeepingCentre(12, 12));
    nameLabel.setBounds(topSection);
    
    bounds.removeFromTop(5);
    
    // Knobs layout
    if (effectType == EffectType::AmpSimulator)
    {
        // Amp: 2 rows of knobs
        int knobsPerRow = (knobs.size() + 1) / 2;
        int knobSize = jmin(50, bounds.getWidth() / knobsPerRow - 10);
        
        for (int row = 0; row < 2; ++row)
        {
            auto rowBounds = bounds.removeFromTop(knobSize + 20);
            int startIdx = row * knobsPerRow;
            int endIdx = jmin(startIdx + knobsPerRow, knobs.size());
            
            for (int i = startIdx; i < endIdx; ++i)
            {
                int x = bounds.getX() + (i - startIdx) * (bounds.getWidth() / knobsPerRow);
                knobs[i]->slider.setBounds(x, rowBounds.getY(), knobSize, knobSize);
            }
        }
    }
    else
    {
        // Pedal: single row of knobs
        int numKnobs = knobs.size();
        if (numKnobs > 0)
        {
            int knobSize = jmin(55, (bounds.getWidth() - 20) / numKnobs - 5);
            int spacing = (bounds.getWidth() - numKnobs * knobSize) / (numKnobs + 1);
            
            for (int i = 0; i < numKnobs; ++i)
            {
                int x = bounds.getX() + spacing + i * (knobSize + spacing);
                knobs[i]->slider.setBounds(x, bounds.getY() + 10, knobSize, knobSize);
            }
        }
    }
}

//==============================================================================
void EffectModule::mouseDown(const MouseEvent& e)
{
    if (e.mods.isLeftButtonDown() && !removeButton.getBounds().contains(e.getPosition()) 
        && !bypassButton.getBounds().contains(e.getPosition()))
    {
        dragging = true;
        dragStartPos = e.getPosition();
        toFront(true);
    }
}

void EffectModule::mouseDrag(const MouseEvent& e)
{
    if (dragging && onDrag)
    {
        int deltaY = e.getPosition().y - dragStartPos.y;
        onDrag(this, deltaY);
    }
}

//==============================================================================
void EffectModule::setBypassed(bool shouldBeBypassed)
{
    bypassed = shouldBeBypassed;
    bypassButton.setToggleState(shouldBeBypassed, dontSendNotification);
    repaint();
}

//==============================================================================
void EffectModule::setupKnobs()
{
    // Setup knobs based on effect type and connect to APVTS
    
    auto createKnob = [this](const String& paramId, const String& label) {
        auto* knob = new KnobControl();
        knob->label = label;
        knob->slider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        knob->slider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 16);
        knob->slider.setTextValueSuffix(label.containsIgnoreCase("gain") ? " dB" : "");
        
        if (auto* param = apvts.getParameter(paramId))
        {
            knob->attachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, paramId, knob->slider);
        }
        
        addAndMakeVisible(knob->slider);
        knobs.add(knob);
    };
    
    switch (effectType)
    {
        case EffectType::NoiseGate:
            createKnob("gateThreshold", "THRESHOLD");
            createKnob("gateAttack", "ATTACK");
            createKnob("gateRelease", "RELEASE");
            bypassAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(apvts, "gateBypass", bypassButton);
            break;
            
        case EffectType::TubeOverdrive:
            createKnob("deepHeatDrive", "DRIVE");
            createKnob("deepHeatTone", "TONE");
            createKnob("deepHeatLevel", "LEVEL");
            bypassAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(apvts, "deepHeatBypass", bypassButton);
            break;
            
        case EffectType::Distortion:
            createKnob("distDrive", "DRIVE");
            createKnob("distTone", "TONE");
            createKnob("distLevel", "LEVEL");
            bypassAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(apvts, "distBypass", bypassButton);
            break;
            
        case EffectType::Compressor:
            createKnob("compThreshold", "THRESHOLD");
            createKnob("compRatio", "RATIO");
            createKnob("compAttack", "ATTACK");
            createKnob("compRelease", "RELEASE");
            bypassAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(apvts, "compBypass", bypassButton);
            break;
            
        case EffectType::Chorus:
            createKnob("pulseRate", "RATE");
            createKnob("pulseDepth", "DEPTH");
            createKnob("pulseMix", "MIX");
            bypassAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(apvts, "pulseBypass", bypassButton);
            break;
            
        case EffectType::Delay:
            createKnob("delayTime", "TIME");
            createKnob("delayFeedback", "FEEDBACK");
            createKnob("delayMix", "MIX");
            bypassAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(apvts, "delayBypass", bypassButton);
            break;
            
        case EffectType::Reverb:
            createKnob("voidSize", "SIZE");
            createKnob("voidDecay", "DECAY");
            createKnob("voidMix", "MIX");
            bypassAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(apvts, "voidBypass", bypassButton);
            break;
            
        case EffectType::AmpSimulator:
            createKnob("ampGain", "GAIN");
            createKnob("ampBass", "BASS");
            createKnob("ampMid", "MID");
            createKnob("ampTreble", "TREBLE");
            createKnob("ampPresence", "PRES");
            createKnob("ampMaster", "MASTER");
            bypassAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(apvts, "ampBypass", bypassButton);
            break;
            
        case EffectType::CabinetIR:
            createKnob("cabDistance", "DISTANCE");
            createKnob("cabAxis", "AXIS");
            bypassAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(apvts, "cabBypass", bypassButton);
            break;
            
        case EffectType::EQ:
            createKnob("eqLowGain", "LOW");
            createKnob("eqLowMidGain", "LOW-MID");
            createKnob("eqHighMidGain", "HI-MID");
            createKnob("eqHighGain", "HIGH");
            bypassAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(apvts, "eqBypass", bypassButton);
            break;
    }
}

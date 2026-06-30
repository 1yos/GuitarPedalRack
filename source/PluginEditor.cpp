#include "PluginEditor.h"

//==============================================================================
GuitarPedalRackEditor::GuitarPedalRackEditor(GuitarPedalRackProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      deepHeatLED(p.apvts.getRawParameterValue("deepHeatBypass")),
      pulseLED(p.apvts.getRawParameterValue("pulseBypass")),
      voidLED(p.apvts.getRawParameterValue("voidBypass"))
{
    setSize(1280, 720);
    setResizable(false, false);
    
    // ============ TITLE ============
    addAndMakeVisible(titleLabel);
    titleLabel.setText("GUITAR PEDAL RACK", dontSendNotification);
    titleLabel.setFont(Font(36.0f, Font::bold));
    titleLabel.setColour(Label::textColourId, Colour(0xffFFD700));
    titleLabel.setJustificationType(Justification::centred);
    
    // ============ CPU MONITOR ============
    addAndMakeVisible(cpuLabel);
    cpuLabel.setFont(Font(13.0f, Font::bold));
    cpuLabel.setColour(Label::textColourId, Colour(0xff00E5FF));
    
    // ============ GLOBAL INPUT/OUTPUT ============
    addAndMakeVisible(inputGain);
    inputGain.setSliderStyle(Slider::LinearVertical);
    inputGain.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    inputGain.setColour(Slider::thumbColourId, Colour(0xff00E5FF));
    inputGain.setColour(Slider::trackColourId, Colour(0xff2a2a2a));
    inputGain.setColour(Slider::textBoxTextColourId, Colours::white);
    inputAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "globalInputGain", inputGain));
    
    addAndMakeVisible(inputLabel);
    inputLabel.setText("INPUT", dontSendNotification);
    inputLabel.setFont(Font(12.0f, Font::bold));
    inputLabel.setColour(Label::textColourId, Colour(0xff00E5FF));
    inputLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(outputGain);
    outputGain.setSliderStyle(Slider::LinearVertical);
    outputGain.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    outputGain.setColour(Slider::thumbColourId, Colour(0xff00FF00));
    outputGain.setColour(Slider::trackColourId, Colour(0xff2a2a2a));
    outputGain.setColour(Slider::textBoxTextColourId, Colours::white);
    outputAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "globalOutputGain", outputGain));
    
    addAndMakeVisible(outputLabel);
    outputLabel.setText("OUTPUT", dontSendNotification);
    outputLabel.setFont(Font(12.0f, Font::bold));
    outputLabel.setColour(Label::textColourId, Colour(0xff00FF00));
    outputLabel.setJustificationType(Justification::centred);

    
    // ============ PEDAL 1: DEEP HEAT (OVERDRIVE) ============
    deepHeatDrive.setKnobColor(Colour(0xffFF8C00));
    deepHeatTone.setKnobColor(Colour(0xffFF8C00));
    deepHeatLevel.setKnobColor(Colour(0xffFF8C00));
    
    addAndMakeVisible(deepHeatDrive);
    addAndMakeVisible(deepHeatTone);
    addAndMakeVisible(deepHeatLevel);
    driveAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "deepHeatDrive", deepHeatDrive));
    toneAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "deepHeatTone", deepHeatTone));
    levelAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "deepHeatLevel", deepHeatLevel));
    
    addAndMakeVisible(driveLabel);
    driveLabel.setText("DRIVE", dontSendNotification);
    driveLabel.setFont(Font(10.0f, Font::bold));
    driveLabel.setColour(Label::textColourId, Colours::white);
    driveLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(toneLabel);
    toneLabel.setText("TONE", dontSendNotification);
    toneLabel.setFont(Font(10.0f, Font::bold));
    toneLabel.setColour(Label::textColourId, Colours::white);
    toneLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(levelLabel);
    levelLabel.setText("LEVEL", dontSendNotification);
    levelLabel.setFont(Font(10.0f, Font::bold));
    levelLabel.setColour(Label::textColourId, Colours::white);
    levelLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(deepHeatNameLabel);
    deepHeatNameLabel.setText("DEEP HEAT", dontSendNotification);
    deepHeatNameLabel.setFont(Font(16.0f, Font::bold));
    deepHeatNameLabel.setColour(Label::textColourId, Colours::white);
    deepHeatNameLabel.setJustificationType(Justification::centred);
    
    deepHeatLED.setLEDColor(Colour(0xffFF8C00));
    addAndMakeVisible(deepHeatLED);
    
    addAndMakeVisible(deepHeatBypass);
    deepHeatBypass.setButtonText("BYPASS");
    deepHeatBypass.setColour(TextButton::buttonColourId, Colour(0xff1a1a1a));
    deepHeatBypass.setColour(TextButton::buttonOnColourId, Colour(0xffFF3333));
    deepHeatBypass.setClickingTogglesState(true);
    deepHeatBypassAttach.reset(new AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "deepHeatBypass", deepHeatBypass));

    
    // ============ PEDAL 2: PULSE (CHORUS) ============
    pulseRate.setKnobColor(Colour(0xff9B59B6));
    pulseDepth.setKnobColor(Colour(0xff9B59B6));
    pulseMix.setKnobColor(Colour(0xff9B59B6));
    
    addAndMakeVisible(pulseRate);
    addAndMakeVisible(pulseDepth);
    addAndMakeVisible(pulseMix);
    rateAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "pulseRate", pulseRate));
    depthAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "pulseDepth", pulseDepth));
    mixAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "pulseMix", pulseMix));
    
    addAndMakeVisible(rateLabel);
    rateLabel.setText("RATE", dontSendNotification);
    rateLabel.setFont(Font(10.0f, Font::bold));
    rateLabel.setColour(Label::textColourId, Colours::white);
    rateLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(depthLabel);
    depthLabel.setText("DEPTH", dontSendNotification);
    depthLabel.setFont(Font(10.0f, Font::bold));
    depthLabel.setColour(Label::textColourId, Colours::white);
    depthLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(mixLabel);
    mixLabel.setText("MIX", dontSendNotification);
    mixLabel.setFont(Font(10.0f, Font::bold));
    mixLabel.setColour(Label::textColourId, Colours::white);
    mixLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(pulseNameLabel);
    pulseNameLabel.setText("PULSE", dontSendNotification);
    pulseNameLabel.setFont(Font(16.0f, Font::bold));
    pulseNameLabel.setColour(Label::textColourId, Colours::white);
    pulseNameLabel.setJustificationType(Justification::centred);
    
    pulseLED.setLEDColor(Colour(0xff9B59B6));
    addAndMakeVisible(pulseLED);
    
    addAndMakeVisible(pulseBypass);
    pulseBypass.setButtonText("BYPASS");
    pulseBypass.setColour(TextButton::buttonColourId, Colour(0xff1a1a1a));
    pulseBypass.setColour(TextButton::buttonOnColourId, Colour(0xffFF3333));
    pulseBypass.setClickingTogglesState(true);
    pulseBypassAttach.reset(new AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "pulseBypass", pulseBypass));

    
    // ============ PEDAL 3: VOID (REVERB) ============
    voidSize.setKnobColor(Colour(0xff4B0082));
    voidDecay.setKnobColor(Colour(0xff4B0082));
    voidMix2.setKnobColor(Colour(0xff4B0082));
    
    addAndMakeVisible(voidSize);
    addAndMakeVisible(voidDecay);
    addAndMakeVisible(voidMix2);
    sizeAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "voidSize", voidSize));
    decayAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "voidDecay", voidDecay));
    mix2Attach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "voidMix", voidMix2));
    
    addAndMakeVisible(sizeLabel);
    sizeLabel.setText("SIZE", dontSendNotification);
    sizeLabel.setFont(Font(10.0f, Font::bold));
    sizeLabel.setColour(Label::textColourId, Colours::white);
    sizeLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(decayLabel);
    decayLabel.setText("DECAY", dontSendNotification);
    decayLabel.setFont(Font(10.0f, Font::bold));
    decayLabel.setColour(Label::textColourId, Colours::white);
    decayLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(mix2Label);
    mix2Label.setText("MIX", dontSendNotification);
    mix2Label.setFont(Font(10.0f, Font::bold));
    mix2Label.setColour(Label::textColourId, Colours::white);
    mix2Label.setJustificationType(Justification::centred);
    
    addAndMakeVisible(voidNameLabel);
    voidNameLabel.setText("VOID", dontSendNotification);
    voidNameLabel.setFont(Font(16.0f, Font::bold));
    voidNameLabel.setColour(Label::textColourId, Colours::white);
    voidNameLabel.setJustificationType(Justification::centred);
    
    voidLED.setLEDColor(Colour(0xff4B0082));
    addAndMakeVisible(voidLED);
    
    addAndMakeVisible(voidBypass);
    voidBypass.setButtonText("BYPASS");
    voidBypass.setColour(TextButton::buttonColourId, Colour(0xff1a1a1a));
    voidBypass.setColour(TextButton::buttonOnColourId, Colour(0xffFF3333));
    voidBypass.setClickingTogglesState(true);
    voidBypassAttach.reset(new AudioProcessorValueTreeState::ButtonAttachment(audioProcessor.apvts, "voidBypass", voidBypass));

    
    // ============ AMP: ALPHA AMP ============
    Colour ampKnobColor = Colour(0xffC8A060);
    
    ampGain.setKnobColor(ampKnobColor);
    ampBass.setKnobColor(ampKnobColor);
    ampMid.setKnobColor(ampKnobColor);
    ampTreble.setKnobColor(ampKnobColor);
    ampPresence.setKnobColor(ampKnobColor);
    ampMaster.setKnobColor(Colour(0xffFFD700));
    
    addAndMakeVisible(ampGain);
    addAndMakeVisible(ampBass);
    addAndMakeVisible(ampMid);
    addAndMakeVisible(ampTreble);
    addAndMakeVisible(ampPresence);
    addAndMakeVisible(ampMaster);
    
    ampGainAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "ampGain", ampGain));
    ampBassAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "ampBass", ampBass));
    ampMidAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "ampMid", ampMid));
    ampTrebleAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "ampTreble", ampTreble));
    ampPresenceAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "ampPresence", ampPresence));
    ampMasterAttach.reset(new AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "ampMaster", ampMaster));
    
    addAndMakeVisible(ampGainLabel);
    ampGainLabel.setText("GAIN", dontSendNotification);
    ampGainLabel.setFont(Font(11.0f, Font::bold));
    ampGainLabel.setColour(Label::textColourId, Colour(0xffFFD700));
    ampGainLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(ampBassLabel);
    ampBassLabel.setText("BASS", dontSendNotification);
    ampBassLabel.setFont(Font(11.0f, Font::bold));
    ampBassLabel.setColour(Label::textColourId, Colour(0xffFFD700));
    ampBassLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(ampMidLabel);
    ampMidLabel.setText("MID", dontSendNotification);
    ampMidLabel.setFont(Font(11.0f, Font::bold));
    ampMidLabel.setColour(Label::textColourId, Colour(0xffFFD700));
    ampMidLabel.setJustificationType(Justification::centred);

    
    addAndMakeVisible(ampTrebleLabel);
    ampTrebleLabel.setText("TREBLE", dontSendNotification);
    ampTrebleLabel.setFont(Font(11.0f, Font::bold));
    ampTrebleLabel.setColour(Label::textColourId, Colour(0xffFFD700));
    ampTrebleLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(ampPresenceLabel);
    ampPresenceLabel.setText("PRESENCE", dontSendNotification);
    ampPresenceLabel.setFont(Font(11.0f, Font::bold));
    ampPresenceLabel.setColour(Label::textColourId, Colour(0xffFFD700));
    ampPresenceLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(ampMasterLabel);
    ampMasterLabel.setText("MASTER", dontSendNotification);
    ampMasterLabel.setFont(Font(11.0f, Font::bold));
    ampMasterLabel.setColour(Label::textColourId, Colour(0xffFF6B35));
    ampMasterLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(ampNameLabel);
    ampNameLabel.setText("ALPHA AMP", dontSendNotification);
    ampNameLabel.setFont(Font(24.0f, Font::bold));
    ampNameLabel.setColour(Label::textColourId, Colour(0xffFFD700));
    ampNameLabel.setJustificationType(Justification::centred);
    
    addAndMakeVisible(ampChannel);
    ampChannel.addItem("CLEAN", 1);
    ampChannel.addItem("CRUNCH", 2);
    ampChannel.addItem("LEAD", 3);
    ampChannel.setSelectedId(2);
    ampChannel.setColour(ComboBox::backgroundColourId, Colour(0xff1a1a1a));
    ampChannel.setColour(ComboBox::textColourId, Colour(0xffFFD700));
    ampChannel.setColour(ComboBox::outlineColourId, Colour(0xffFFD700));
    ampChannelAttach.reset(new AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.apvts, "ampChannel", ampChannel));
    
    addAndMakeVisible(channelLabel);
    channelLabel.setText("CHANNEL", dontSendNotification);
    channelLabel.setFont(Font(11.0f, Font::bold));
    channelLabel.setColour(Label::textColourId, Colour(0xffFFD700));
    channelLabel.setJustificationType(Justification::centred);
    
    startTimerHz(30);
}

GuitarPedalRackEditor::~GuitarPedalRackEditor()
{
    stopTimer();
}


//==============================================================================
void GuitarPedalRackEditor::paint(Graphics& g)
{
    // Dark studio gradient background
    ColourGradient bgGrad(Colour(0xff0a0a0a), getWidth()/2, 0,
                           Colour(0xff1a1a1a), getWidth()/2, getHeight(), false);
    g.setGradientFill(bgGrad);
    g.fillAll();
    
    // Wooden pedal board floor (bottom section)
    Rectangle<int> floorArea(0, 420, getWidth(), getHeight() - 420);
    ColourGradient woodGrad(Colour(0xff4a2c1a), 0, floorArea.getY(),
                             Colour(0xff2d1810), 0, floorArea.getBottom(), false);
    g.setGradientFill(woodGrad);
    g.fillRect(floorArea);
    
    // Wood grain texture
    g.setColour(Colour(0xff1a0f08).withAlpha(0.3f));
    for (int i = 0; i < 60; ++i)
    {
        int y = floorArea.getY() + (i * 5);
        g.drawLine(0.0f, (float)y, (float)getWidth(), (float)(y + 1), 1.5f);
    }
    
    // Draw realistic pedals
    bool deepHeatBypassed = audioProcessor.apvts.getRawParameterValue("deepHeatBypass")->load() > 0.5f;
    bool pulseBypassed = audioProcessor.apvts.getRawParameterValue("pulseBypass")->load() > 0.5f;
    bool voidBypassed = audioProcessor.apvts.getRawParameterValue("voidBypass")->load() > 0.5f;
    
    // Pedal positions
    int pedalY = 450;
    int pedalW = 220;
    int pedalH = 250;
    int spacing = 70;
    int startX = (getWidth() - (pedalW * 3 + spacing * 2)) / 2;
    
    drawRealisticPedal(g, Rectangle<int>(startX, pedalY, pedalW, pedalH), 
                       "DEEP HEAT", Colour(0xffFF8C00), deepHeatBypassed);
    
    drawRealisticPedal(g, Rectangle<int>(startX + pedalW + spacing, pedalY, pedalW, pedalH), 
                       "PULSE", Colour(0xff9B59B6), pulseBypassed);
    
    drawRealisticPedal(g, Rectangle<int>(startX + (pedalW + spacing) * 2, pedalY, pedalW, pedalH), 
                       "VOID", Colour(0xff4B0082), voidBypassed);
    
    // Draw realistic amp
    drawRealisticAmp(g, Rectangle<int>(100, 120, getWidth() - 200, 260));
}


void GuitarPedalRackEditor::drawRealisticPedal(Graphics& g, Rectangle<int> bounds, 
                                                const String& name, Colour pedalColor, bool isBypassed)
{
    auto b = bounds.toFloat();
    
    // Drop shadow for 3D depth
    g.setColour(Colours::black.withAlpha(0.6f));
    g.fillRoundedRectangle(b.translated(6, 8).reduced(-2), 15.0f);
    
    // Main pedal body - realistic metallic gradient
    ColourGradient bodyGrad(pedalColor.darker(isBypassed ? 1.5f : 0.7f), b.getCentreX(), b.getY(),
                             pedalColor.darker(isBypassed ? 2.0f : 1.3f), b.getCentreX(), b.getBottom(), false);
    g.setGradientFill(bodyGrad);
    g.fillRoundedRectangle(b, 15.0f);
    
    // Glossy top highlight
    g.setColour(Colours::white.withAlpha(isBypassed ? 0.05f : 0.15f));
    g.fillRoundedRectangle(b.reduced(15, 15).withHeight(40), 10.0f);
    
    // Border - metal edge
    g.setColour(isBypassed ? Colour(0xff444444) : pedalColor.brighter(0.4f));
    g.drawRoundedRectangle(b, 15.0f, 3.0f);
    
    // Screws in corners (realistic hardware)
    auto drawScrew = [&g](float x, float y)
    {
        g.setColour(Colour(0xff888888));
        g.fillEllipse(x - 4, y - 4, 8, 8);
        g.setColour(Colour(0xff555555));
        g.drawLine(x - 2, y, x + 2, y, 1.5f);
        g.drawLine(x, y - 2, x, y + 2, 1.5f);
    };
    
    drawScrew(b.getX() + 15, b.getY() + 15);
    drawScrew(b.getRight() - 15, b.getY() + 15);
    drawScrew(b.getX() + 15, b.getBottom() - 15);
    drawScrew(b.getRight() - 15, b.getBottom() - 15);
}


void GuitarPedalRackEditor::drawRealisticAmp(Graphics& g, Rectangle<int> bounds)
{
    auto b = bounds.toFloat();
    
    // Amp shadow
    g.setColour(Colours::black.withAlpha(0.5f));
    g.fillRoundedRectangle(b.translated(4, 6), 10.0f);
    
    // Amp tolex (textured black covering)
    g.setColour(Colour(0xff0a0a0a));
    g.fillRoundedRectangle(b, 10.0f);
    
    // Tolex texture simulation
    g.setColour(Colour(0xff1a1a1a).withAlpha(0.3f));
    for (int i = 0; i < 40; ++i)
    {
        int x = bounds.getX() + (i * 30);
        g.drawLine((float)x, (float)bounds.getY(), (float)x, (float)bounds.getBottom(), 2.0f);
    }
    
    // Metal corner hardware
    auto drawCornerHardware = [&g](float x, float y)
    {
        g.setColour(Colour(0xff999999));
        g.fillRect(x, y, 18.0f, 18.0f);
        g.setColour(Colour(0xff666666));
        g.drawRect(x, y, 18.0f, 18.0f, 1.0f);
        g.setColour(Colour(0xff444444));
        g.fillEllipse(x + 6, y + 6, 6, 6);
    };
    
    drawCornerHardware(b.getX() + 10, b.getY() + 10);
    drawCornerHardware(b.getRight() - 28, b.getY() + 10);
    drawCornerHardware(b.getX() + 10, b.getBottom() - 28);
    drawCornerHardware(b.getRight() - 28, b.getBottom() - 28);
    
    // Control panel inset (gold faceplate)
    Rectangle<float> panel(b.getX() + 30, b.getY() + 50, b.getWidth() - 60, b.getHeight() - 80);
    ColourGradient panelGrad(Colour(0xff2a2520), panel.getCentreX(), panel.getY(),
                              Colour(0xff1a1510), panel.getCentreX(), panel.getBottom(), false);
    g.setGradientFill(panelGrad);
    g.fillRoundedRectangle(panel, 8.0f);
    
    // Panel border (gold)
    g.setColour(Colour(0xffC8A060));
    g.drawRoundedRectangle(panel, 8.0f, 2.5f);
    
    // Amp grill cloth texture (bottom section hint)
    Rectangle<float> grill(panel.getX() + 10, panel.getBottom() - 40, panel.getWidth() - 20, 30);
    g.setColour(Colour(0xff0a0a0a));
    g.fillRoundedRectangle(grill, 4.0f);
    g.setColour(Colour(0xff1a1a1a));
    for (int i = 0; i < 100; ++i)
    {
        int x = (int)grill.getX() + (i * 12);
        g.drawLine((float)x, grill.getY(), (float)x, grill.getBottom(), 1.0f);
    }
}


void GuitarPedalRackEditor::resized()
{
    auto area = getLocalBounds();
    
    // Title
    titleLabel.setBounds(0, 10, getWidth(), 50);
    
    // CPU monitor
    cpuLabel.setBounds(getWidth() - 150, 20, 140, 25);
    
    // Global controls
    inputGain.setBounds(20, 120, 50, 260);
    inputLabel.setBounds(10, 390, 70, 20);
    
    outputGain.setBounds(getWidth() - 70, 120, 50, 260);
    outputLabel.setBounds(getWidth() - 80, 390, 70, 20);
    
    // Pedal layout parameters
    int pedalY = 450;
    int pedalW = 220;
    int pedalH = 250;
    int spacing = 70;
    int startX = (getWidth() - (pedalW * 3 + spacing * 2)) / 2;
    int knobSize = 60;
    int knobSpacing = 70;
    
    // === PEDAL 1: DEEP HEAT ===
    int pedal1X = startX;
    int knobY = pedalY + 70;
    
    deepHeatNameLabel.setBounds(pedal1X, pedalY + 20, pedalW, 30);
    
    deepHeatLED.setBounds(pedal1X + pedalW/2 - 10, pedalY + 55, 20, 20);
    
    deepHeatDrive.setBounds(pedal1X + 20, knobY, knobSize, knobSize);
    driveLabel.setBounds(pedal1X + 15, knobY + knobSize + 5, knobSize + 10, 20);
    
    deepHeatTone.setBounds(pedal1X + pedalW/2 - knobSize/2, knobY, knobSize, knobSize);
    toneLabel.setBounds(pedal1X + pedalW/2 - knobSize/2 - 5, knobY + knobSize + 5, knobSize + 10, 20);
    
    deepHeatLevel.setBounds(pedal1X + pedalW - knobSize - 20, knobY, knobSize, knobSize);
    levelLabel.setBounds(pedal1X + pedalW - knobSize - 25, knobY + knobSize + 5, knobSize + 10, 20);
    
    deepHeatBypass.setBounds(pedal1X + pedalW/2 - 50, pedalY + pedalH - 45, 100, 32);
    
    // === PEDAL 2: PULSE ===
    int pedal2X = startX + pedalW + spacing;
    
    pulseNameLabel.setBounds(pedal2X, pedalY + 20, pedalW, 30);
    
    pulseLED.setBounds(pedal2X + pedalW/2 - 10, pedalY + 55, 20, 20);
    
    pulseRate.setBounds(pedal2X + 20, knobY, knobSize, knobSize);
    rateLabel.setBounds(pedal2X + 15, knobY + knobSize + 5, knobSize + 10, 20);
    
    pulseDepth.setBounds(pedal2X + pedalW/2 - knobSize/2, knobY, knobSize, knobSize);
    depthLabel.setBounds(pedal2X + pedalW/2 - knobSize/2 - 5, knobY + knobSize + 5, knobSize + 10, 20);
    
    pulseMix.setBounds(pedal2X + pedalW - knobSize - 20, knobY, knobSize, knobSize);
    mixLabel.setBounds(pedal2X + pedalW - knobSize - 25, knobY + knobSize + 5, knobSize + 10, 20);
    
    pulseBypass.setBounds(pedal2X + pedalW/2 - 50, pedalY + pedalH - 45, 100, 32);

    
    // === PEDAL 3: VOID ===
    int pedal3X = startX + (pedalW + spacing) * 2;
    
    voidNameLabel.setBounds(pedal3X, pedalY + 20, pedalW, 30);
    
    voidLED.setBounds(pedal3X + pedalW/2 - 10, pedalY + 55, 20, 20);
    
    voidSize.setBounds(pedal3X + 20, knobY, knobSize, knobSize);
    sizeLabel.setBounds(pedal3X + 15, knobY + knobSize + 5, knobSize + 10, 20);
    
    voidDecay.setBounds(pedal3X + pedalW/2 - knobSize/2, knobY, knobSize, knobSize);
    decayLabel.setBounds(pedal3X + pedalW/2 - knobSize/2 - 5, knobY + knobSize + 5, knobSize + 10, 20);
    
    voidMix2.setBounds(pedal3X + pedalW - knobSize - 20, knobY, knobSize, knobSize);
    mix2Label.setBounds(pedal3X + pedalW - knobSize - 25, knobY + knobSize + 5, knobSize + 10, 20);
    
    voidBypass.setBounds(pedal3X + pedalW/2 - 50, pedalY + pedalH - 45, 100, 32);
    
    // === AMP SECTION ===
    int ampX = 100;
    int ampY = 120;
    int ampW = getWidth() - 200;
    int ampH = 260;
    
    ampNameLabel.setBounds(ampX, ampY + 15, ampW, 35);
    
    channelLabel.setBounds(ampX + 30, ampY + 60, 80, 20);
    ampChannel.setBounds(ampX + 30, ampY + 85, 140, 30);
    
    // Amp knobs - top row (EQ stack)
    int ampKnobSize = 70;
    int ampKnobY = ampY + 130;
    int ampKnobSpacing = 95;
    int ampStartX = ampX + ampW/2 - (ampKnobSpacing * 2);
    
    ampGain.setBounds(ampStartX, ampKnobY, ampKnobSize, ampKnobSize);
    ampGainLabel.setBounds(ampStartX - 5, ampKnobY + ampKnobSize + 5, ampKnobSize + 10, 20);
    
    ampBass.setBounds(ampStartX + ampKnobSpacing, ampKnobY, ampKnobSize, ampKnobSize);
    ampBassLabel.setBounds(ampStartX + ampKnobSpacing - 5, ampKnobY + ampKnobSize + 5, ampKnobSize + 10, 20);
    
    ampMid.setBounds(ampStartX + ampKnobSpacing * 2, ampKnobY, ampKnobSize, ampKnobSize);
    ampMidLabel.setBounds(ampStartX + ampKnobSpacing * 2 - 5, ampKnobY + ampKnobSize + 5, ampKnobSize + 10, 20);
    
    ampTreble.setBounds(ampStartX + ampKnobSpacing * 3, ampKnobY, ampKnobSize, ampKnobSize);
    ampTrebleLabel.setBounds(ampStartX + ampKnobSpacing * 3 - 5, ampKnobY + ampKnobSize + 5, ampKnobSize + 10, 20);
    
    ampPresence.setBounds(ampStartX + ampKnobSpacing * 4, ampKnobY, ampKnobSize, ampKnobSize);
    ampPresenceLabel.setBounds(ampStartX + ampKnobSpacing * 4 - 10, ampKnobY + ampKnobSize + 5, ampKnobSize + 20, 20);
    
    // Master volume - larger, positioned separately
    int masterSize = 85;
    ampMaster.setBounds(ampX + ampW - masterSize - 50, ampKnobY - 5, masterSize, masterSize);
    ampMasterLabel.setBounds(ampX + ampW - masterSize - 60, ampKnobY + masterSize, masterSize + 20, 20);
}

void GuitarPedalRackEditor::timerCallback()
{
    float cpuUsage = audioProcessor.getCPUUsage();
    cpuLabel.setText(String::formatted("CPU: %.1f%%", cpuUsage), dontSendNotification);
}

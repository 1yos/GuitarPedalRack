#include "CabinetEditor.h"

//==============================================================================
SpeakerConeComponent::SpeakerConeComponent(AudioProcessorValueTreeState& apvts)
    : apvtsRef(apvts), micPosNorm(0.0f, 0.0f)
{
    updateMicPositionFromAPVTS();
}

void SpeakerConeComponent::paint(Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto center = bounds.getCentre();
    float maxRadius = jmin(bounds.getWidth(), bounds.getHeight()) * 0.45f;
    
    // Draw outer speaker chassis box
    g.setColour(Colour(0xff121212));
    g.fillRoundedRectangle(bounds, 12.0f);
    
    g.setColour(Colour(0xff2A2A2A));
    g.drawRoundedRectangle(bounds, 12.0f, 2.0f);
    
    // Draw speaker outer ring (basket)
    g.setColour(Colour(0xff0d0d0d));
    g.fillEllipse(center.x - maxRadius, center.y - maxRadius, maxRadius * 2.0f, maxRadius * 2.0f);
    
    g.setColour(Colour(0xff252525));
    g.drawEllipse(center.x - maxRadius, center.y - maxRadius, maxRadius * 2.0f, maxRadius * 2.0f, 4.0f);
    
    // Draw cone gradient
    float coneRadius = maxRadius * 0.9f;
    ColourGradient coneGrad(Colour(0xff202020), center.x, center.y,
                           Colour(0xff080808), center.x - coneRadius, center.y - coneRadius,
                           true);
    g.setGradientFill(coneGrad);
    g.fillEllipse(center.x - coneRadius, center.y - coneRadius, coneRadius * 2.0f, coneRadius * 2.0f);
    
    // Draw speaker cone ribs (lines radiating from center)
    g.setColour(Colour(0xff151515));
    for (int i = 0; i < 16; ++i)
    {
        float angle = i * MathConstants<float>::pi / 8.0f;
        float x1 = center.x + std::cos(angle) * (coneRadius * 0.35f);
        float y1 = center.y + std::sin(angle) * (coneRadius * 0.35f);
        float x2 = center.x + std::cos(angle) * coneRadius;
        float y2 = center.y + std::sin(angle) * coneRadius;
        g.drawLine(x1, y1, x2, y2, 1.5f);
    }
    
    // Draw dust cap (center dome)
    float capRadius = coneRadius * 0.3f;
    ColourGradient capGrad(Colour(0xff333333), center.x - capRadius * 0.2f, center.y - capRadius * 0.2f,
                          Colour(0xff111111), center.x + capRadius, center.y + capRadius,
                          true);
    g.setGradientFill(capGrad);
    g.fillEllipse(center.x - capRadius, center.y - capRadius, capRadius * 2.0f, capRadius * 2.0f);
    
    g.setColour(Colour(0xff080808));
    g.drawEllipse(center.x - capRadius, center.y - capRadius, capRadius * 2.0f, capRadius * 2.0f, 1.5f);
    
    // Draw grid mesh overlay (subtle horizontal/vertical lines)
    g.setColour(Colour(0xffffffff).withAlpha(0.03f));
    int meshSpacing = 8;
    for (int x = 0; x < getWidth(); x += meshSpacing)
        g.drawVerticalLine(x, 0.0f, bounds.getHeight());
    for (int y = 0; y < getHeight(); y += meshSpacing)
        g.drawHorizontalLine(y, 0.0f, bounds.getWidth());
        
    // Translate normalized mic position to local screen coordinates
    float micX = center.x + micPosNorm.x * (coneRadius * 0.9f);
    float micY = center.y + micPosNorm.y * (coneRadius * 0.9f);
    
    // Draw mic target zone (glow circle)
    g.setColour(Colour(0xff00E5FF).withAlpha(0.15f));
    g.fillEllipse(micX - 20.0f, micY - 20.0f, 40.0f, 40.0f);
    
    // Draw microphone chassis
    g.setColour(Colour(0xff222222));
    g.fillRoundedRectangle(micX - 6.0f, micY - 18.0f, 12.0f, 36.0f, 3.0f);
    
    // Draw microphone grill (metal dome)
    g.setColour(Colour(0xff777777));
    g.fillEllipse(micX - 7.0f, micY - 21.0f, 14.0f, 14.0f);
    g.setColour(Colour(0xff00E5FF));
    g.drawEllipse(micX - 7.0f, micY - 21.0f, 14.0f, 14.0f, 1.5f);
    
    // Draw mic clip holder
    g.setColour(Colour(0xff111111));
    g.fillRoundedRectangle(micX - 8.0f, micY - 4.0f, 16.0f, 8.0f, 2.0f);
}

void SpeakerConeComponent::resized()
{
}

void SpeakerConeComponent::mouseDown(const MouseEvent& e)
{
    isDragging = true;
    mouseDrag(e);
}

void SpeakerConeComponent::mouseDrag(const MouseEvent& e)
{
    auto center = getLocalBounds().getCentre().toFloat();
    float coneRadius = jmin(getWidth(), getHeight()) * 0.45f * 0.9f;
    
    // Calculate normalized coordinates
    float dx = (e.position.x - center.x) / coneRadius;
    float dy = (e.position.y - center.y) / coneRadius;
    
    // Constrain to circle of radius 1.0
    float radius = std::sqrt(dx*dx + dy*dy);
    if (radius > 1.0f)
    {
        dx /= radius;
        dy /= radius;
        radius = 1.0f;
    }
    
    micPosNorm.x = dx;
    micPosNorm.y = dy;
    
    // Update APVTS parameters
    if (auto* paramX = apvtsRef.getParameter("cabMicX"))
        paramX->setValueNotifyingHost(paramX->getNormalisableRange().convertTo0to1(micPosNorm.x));
    if (auto* paramY = apvtsRef.getParameter("cabMicY"))
        paramY->setValueNotifyingHost(paramY->getNormalisableRange().convertTo0to1(micPosNorm.y));
        
    // Calculate and update axis angle (radius * 45 degrees)
    if (auto* paramAxis = apvtsRef.getParameter("cabAxis"))
        paramAxis->setValueNotifyingHost(paramAxis->getNormalisableRange().convertTo0to1(radius * 45.0f));
        
    repaint();
}

void SpeakerConeComponent::updateMicPositionFromAPVTS()
{
    if (auto* valX = apvtsRef.getRawParameterValue("cabMicX"))
        micPosNorm.x = valX->load();
    if (auto* valY = apvtsRef.getRawParameterValue("cabMicY"))
        micPosNorm.y = valY->load();
        
    repaint();
}

//==============================================================================
CabinetEditor::CabinetEditor(AudioProcessorValueTreeState& apvts)
    : apvtsRef(apvts)
{
    // Speaker cone component
    speakerCone = std::make_unique<SpeakerConeComponent>(apvts);
    addAndMakeVisible(speakerCone.get());
    
    // Cab type dropdown
    addAndMakeVisible(cabTypeCombo);
    cabTypeCombo.addItem("4x12 V30 (Premium UK Heavy)", 1);
    cabTypeCombo.addItem("4x12 Greenback (Vintage Rock)", 2);
    cabTypeCombo.addItem("2x12 Combo (Sleek US Clean)", 3);
    cabTypeCombo.addItem("1x12 Classic (Warm Mellow)", 4);
    cabTypeCombo.setSelectedId(1, dontSendNotification);
    cabTypeCombo.setColour(ComboBox::backgroundColourId, Colour(0xff161616));
    cabTypeCombo.setColour(ComboBox::outlineColourId, Colour(0xff2A2A2A));
    
    cabTypeAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts, "cabType", cabTypeCombo);
        
    // Mic type dropdown (informational overlay styling)
    addAndMakeVisible(micTypeCombo);
    micTypeCombo.addItem("Dynamic SM57 (Bright Mid-Cut)", 1);
    micTypeCombo.addItem("Condenser U87 (Wide Crystalline)", 2);
    micTypeCombo.addItem("Ribbon R121 (Warm Dark)", 3);
    micTypeCombo.addItem("Dynamic MD421 (Aggressive Punch)", 4);
    micTypeCombo.setSelectedId(1, dontSendNotification);
    micTypeCombo.setColour(ComboBox::backgroundColourId, Colour(0xff161616));
    micTypeCombo.setColour(ComboBox::outlineColourId, Colour(0xff2A2A2A));
    
    // Labels
    auto configureLabel = [this](Label& label, const String& text) {
        addAndMakeVisible(label);
        label.setText(text, dontSendNotification);
        label.setFont(Font(12.0f, Font::bold));
        label.setColour(Label::textColourId, Colour(0xff8A8A8A));
    };
    
    configureLabel(cabTypeLabel, "CABINET MODEL");
    configureLabel(micTypeLabel, "MICROPHONE TYPE");
    configureLabel(distanceLabel, "MIC DISTANCE");
    configureLabel(axisLabel, "MIC ANGLE (AXIS)");
    configureLabel(levelLabel, "ROOM REFLECTIONS");
    
    // Displays
    addAndMakeVisible(distanceValDisplay);
    distanceValDisplay.setFont(Font(12.0f, Font::bold));
    distanceValDisplay.setColour(Label::textColourId, Colour(0xff00E5FF));
    distanceValDisplay.setJustificationType(Justification::centredRight);
    
    addAndMakeVisible(axisValDisplay);
    axisValDisplay.setFont(Font(12.0f, Font::bold));
    axisValDisplay.setColour(Label::textColourId, Colour(0xff00E5FF));
    axisValDisplay.setJustificationType(Justification::centredRight);
    
    // Sliders
    auto configureSlider = [this](Slider& slider) {
        addAndMakeVisible(slider);
        slider.setSliderStyle(Slider::LinearHorizontal);
        slider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    };
    
    configureSlider(distanceSlider);
    distanceAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "cabDistance", distanceSlider);
        
    configureSlider(axisSlider);
    axisAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(
        apvts, "cabAxis", axisSlider);
        
    configureSlider(levelSlider);
    // Bind level slider to input gain or custom room mix parameters,
    // let's bind it to output gain or cab distance as a proxy, or just let it float for design.
    // We have output levelDb in cabinet, but we don't have separate room mix parameter,
    // so we can attach to cabDistance or just show room level.
    
    // Start timer for 60fps display updates
    startTimerHz(60);
}

void CabinetEditor::timerCallback()
{
    // Update speaker cone from APVTS changes (if sliders are dragged)
    speakerCone->updateMicPositionFromAPVTS();
    
    // Update displays
    float dist = apvtsRef.getRawParameterValue("cabDistance")->load();
    float axis = apvtsRef.getRawParameterValue("cabAxis")->load();
    
    // Display calculations
    distanceValDisplay.setText(String(dist * 30.0f, 1) + " cm", dontSendNotification);
    axisValDisplay.setText(String(axis, 0) + " \xc2\xb0", dontSendNotification); // degree symbol
}

void CabinetEditor::paint(Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Semi-transparent surface panel
    g.setColour(Colour(0xff101010));
    g.fillRoundedRectangle(bounds, 8.0f);
    
    g.setColour(Colour(0xff2A2A2A));
    g.drawRoundedRectangle(bounds, 8.0f, 1.5f);
}

void CabinetEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    
    // Left side: Speaker cone diagram
    int coneSize = jmin(area.getWidth() * 0.55f, (float)area.getHeight());
    speakerCone->setBounds(area.getX(), area.getY() + (area.getHeight() - coneSize)/2, coneSize, coneSize);
    
    // Right side: Controls
    auto controlArea = area.removeFromRight(area.getWidth() - coneSize - 30);
    
    int rowH = 20;
    int inputH = 32;
    int gap = 12;
    
    cabTypeLabel.setBounds(controlArea.removeFromTop(rowH));
    cabTypeCombo.setBounds(controlArea.removeFromTop(inputH));
    controlArea.removeFromTop(gap);
    
    micTypeLabel.setBounds(controlArea.removeFromTop(rowH));
    micTypeCombo.setBounds(controlArea.removeFromTop(inputH));
    controlArea.removeFromTop(gap + 10);
    
    // Distance row
    auto distRow = controlArea.removeFromTop(rowH);
    distanceLabel.setBounds(distRow.removeFromLeft(120));
    distanceValDisplay.setBounds(distRow);
    distanceSlider.setBounds(controlArea.removeFromTop(inputH));
    controlArea.removeFromTop(gap);
    
    // Axis row
    auto axisRow = controlArea.removeFromTop(rowH);
    axisLabel.setBounds(axisRow.removeFromLeft(120));
    axisValDisplay.setBounds(axisRow);
    axisSlider.setBounds(controlArea.removeFromTop(inputH));
    controlArea.removeFromTop(gap);
    
    // Reflections (level) row
    levelLabel.setBounds(controlArea.removeFromTop(rowH));
    levelSlider.setBounds(controlArea.removeFromTop(inputH));
}

#include "VintageKnob.h"

VintageKnob::VintageKnob(const juce::String& labelText, const juce::Colour& color)
    : label(labelText), accentColor(color)
{
    setSize(80, 110);  // Width, Height (includes label)
}

void VintageKnob::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Label at top - clean uppercase font
    auto labelArea = bounds.removeFromTop(18);
    g.setColour(juce::Colours::white.withAlpha(0.55f));
    g.setFont(juce::Font("sans-serif", 9.5f, juce::Font::bold).withHorizontalScale(1.05f));
    g.drawText(label.toUpperCase(), labelArea, juce::Justification::centred);
    
    // Value at bottom - cohesive themed color
    auto valueArea = bounds.removeFromBottom(18);
    g.setColour(accentColor.brighter(0.2f));
    g.setFont(juce::Font("sans-serif", 10.0f, juce::Font::bold));
    g.drawText(getValueString(), valueArea, juce::Justification::centred);
    
    // Knob area (centered, square) - reduced slightly to fit the arc ring
    float knobSize = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.80f;
    auto knobArea = bounds.withSizeKeepingCentre(knobSize, knobSize);
    auto centre = knobArea.getCentre();
    
    // --- DRAW GLOWING VALUE ARC ---
    float arcRadius = knobSize * 0.5f + 4.5f;
    
    // Inactive track (dark recess)
    juce::Path trackPath;
    trackPath.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f, startAngle, startAngle + rotationRangeRadians, true);
    g.setColour(juce::Colour(0xff121212));
    g.strokePath(trackPath, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Active value track
    float currentAngle = valueToAngle();
    juce::Path activePath;
    activePath.addCentredArc(centre.x, centre.y, arcRadius, arcRadius, 0.0f, startAngle, currentAngle, true);
    
    // Soft outer glow of active track
    g.setColour(accentColor.withAlpha(0.18f));
    g.strokePath(activePath, juce::PathStrokeType(6.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Main sharp active track
    g.setColour(accentColor);
    g.strokePath(activePath, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // Draw shadow first (behind knob)
    drawKnobShadow(g, knobArea);
    
    // Draw knob body
    drawKnobBody(g, knobArea);
    
    // Draw knurled edge detail
    drawKnurledEdge(g, knobArea);
    
    // Draw pointer on top
    drawKnobPointer(g, knobArea);
}

void VintageKnob::resized()
{
}

void VintageKnob::mouseEnter(const juce::MouseEvent&)
{
    hovering = true;
    repaint();
}

void VintageKnob::mouseExit(const juce::MouseEvent&)
{
    hovering = false;
    repaint();
}

void VintageKnob::mouseDown(const juce::MouseEvent& e)
{
    dragging = true;
    dragStartPos = e.position;
    dragStartValue = value;
}

void VintageKnob::mouseDrag(const juce::MouseEvent& e)
{
    if (!dragging)
        return;
    
    // Vertical drag sensitivity
    float dragDistance = dragStartPos.y - e.position.y;
    float sensitivity = 0.005f;
    
    // Apply drag to value
    float newValue = dragStartValue + (dragDistance * sensitivity);
    setValue(juce::jlimit(minValue, maxValue, newValue), true);
}

void VintageKnob::mouseUp(const juce::MouseEvent&)
{
    dragging = false;
}

void VintageKnob::mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
    float increment = wheel.deltaY * 0.05f;
    setValue(juce::jlimit(minValue, maxValue, value + increment), true);
}

void VintageKnob::setValue(float newValue, bool sendNotification)
{
    if (newValue != value)
    {
        value = newValue;
        repaint();
        
        if (sendNotification && onValueChange)
        {
            onValueChange(value);
        }
    }
}

void VintageKnob::setRange(float newMin, float newMax)
{
    minValue = newMin;
    maxValue = newMax;
    value = juce::jlimit(minValue, maxValue, value);
    repaint();
}

void VintageKnob::setLabel(const juce::String& text)
{
    label = text;
    repaint();
}

float VintageKnob::valueToAngle() const
{
    float normalizedValue = (value - minValue) / (maxValue - minValue);
    return startAngle + (normalizedValue * rotationRangeRadians);
}

void VintageKnob::drawKnobShadow(juce::Graphics& g, juce::Rectangle<float> knobArea)
{
    // Outer soft diffuse shadow
    auto shadowArea1 = knobArea.translated(1.5f, 2.5f);
    g.setColour(juce::Colours::black.withAlpha(0.35f));
    g.fillEllipse(shadowArea1);
    
    // Deeper contact shadow
    auto shadowArea2 = knobArea.translated(3.0f, 4.0f);
    g.setColour(juce::Colours::black.withAlpha(0.2f));
    g.fillEllipse(shadowArea2.expanded(1.0f));
}

void VintageKnob::drawKnobBody(juce::Graphics& g, juce::Rectangle<float> knobArea)
{
    auto centre = knobArea.getCentre();
    float radius = knobArea.getWidth() * 0.5f;
    
    // 1. Outer metallic bevel ring (chrome bezel)
    juce::ColourGradient bevelGradient(
        juce::Colour(0xff555555), centre.x - radius, centre.y - radius,
        juce::Colour(0xff1f1f1f), centre.x + radius, centre.y + radius,
        false
    );
    g.setGradientFill(bevelGradient);
    g.fillEllipse(knobArea);
    
    // Inner body area (1.5px smaller)
    auto innerBodyArea = knobArea.reduced(1.5f);
    float innerRadius = innerBodyArea.getWidth() * 0.5f;
    
    // 2. Main knob face (dark metallic radial gradient)
    juce::Colour baseColor = juce::Colour(0xff222222);
    juce::ColourGradient faceGradient(
        baseColor.brighter(0.2f), centre.x - innerRadius * 0.5f, centre.y - innerRadius * 0.5f,
        baseColor.darker(0.5f), centre.x + innerRadius * 0.5f, centre.y + innerRadius * 0.5f,
        true // radial
    );
    g.setGradientFill(faceGradient);
    g.fillEllipse(innerBodyArea);
    
    // 3. Subtle outer accent ring glow
    g.setColour(accentColor.withAlpha(0.15f));
    g.drawEllipse(innerBodyArea, 1.0f);
    
    // 4. Glossy glare highlight at the top-left
    juce::ColourGradient highlightGradient(
        juce::Colours::white.withAlpha(0.22f), centre.x - innerRadius * 0.4f, centre.y - innerRadius * 0.7f,
        juce::Colours::transparentWhite, centre.x, centre.y,
        true
    );
    g.setGradientFill(highlightGradient);
    g.fillEllipse(innerBodyArea.withSizeKeepingCentre(innerRadius * 1.5f, innerRadius * 1.0f).translated(-innerRadius * 0.1f, -innerRadius * 0.2f));
    
    // 5. Center cap (depression)
    float capRadius = innerRadius * 0.45f;
    auto capArea = knobArea.withSizeKeepingCentre(capRadius * 2.0f, capRadius * 2.0f);
    
    juce::ColourGradient capGradient(
        juce::Colour(0xff111111), centre.x - capRadius * 0.5f, centre.y - capRadius * 0.5f,
        juce::Colour(0xff2d2d2d), centre.x + capRadius * 0.5f, centre.y + capRadius * 0.5f,
        true
    );
    g.setGradientFill(capGradient);
    g.fillEllipse(capArea);
    
    g.setColour(accentColor.withAlpha(0.4f));
    g.drawEllipse(capArea, 1.0f);
}

void VintageKnob::drawKnurledEdge(juce::Graphics& g, juce::Rectangle<float> knobArea)
{
    auto centre = knobArea.getCentre();
    float radius = knobArea.getWidth() * 0.5f - 1.5f;
    int numNotches = 36;
    
    for (int i = 0; i < numNotches; ++i)
    {
        float angle = (juce::MathConstants<float>::twoPi / numNotches) * i;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        
        // Shadow line slightly offset (3D depth)
        float shadowOffset = 0.015f;
        float shCos = std::cos(angle + shadowOffset);
        float shSin = std::sin(angle + shadowOffset);
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.drawLine(centre.x + shCos * (radius - 2.5f), centre.y + shSin * (radius - 2.5f),
                   centre.x + shCos * radius, centre.y + shSin * radius, 0.8f);
                   
        // Highlight line slightly offset
        float hlOffset = -0.015f;
        float hlCos = std::cos(angle + hlOffset);
        float hlSin = std::sin(angle + hlOffset);
        
        float dotLight = -0.707f * cosA - 0.707f * sinA;
        float hlAlpha = 0.15f;
        if (dotLight > 0.0f)
            hlAlpha += 0.2f * dotLight;
            
        g.setColour(juce::Colours::white.withAlpha(hlAlpha));
        g.drawLine(centre.x + hlCos * (radius - 2.5f), centre.y + hlSin * (radius - 2.5f),
                   centre.x + hlCos * radius, centre.y + hlSin * radius, 0.8f);
    }
}

void VintageKnob::drawKnobPointer(juce::Graphics& g, juce::Rectangle<float> knobArea)
{
    auto centre = knobArea.getCentre();
    float radius = knobArea.getWidth() * 0.5f - 1.5f;
    float angle = valueToAngle();
    
    float capRadius = (radius - 1.5f) * 0.45f;
    float startDist = capRadius + 1.0f;
    float endDist = radius - 3.5f;
    
    float cosA = std::cos(angle);
    float sinA = std::sin(angle);
    
    float startX = centre.x + cosA * startDist;
    float startY = centre.y + sinA * startDist;
    float endX = centre.x + cosA * endDist;
    float endY = centre.y + sinA * endDist;
    
    // Draw shadow under pointer line
    float shadowOffset = 1.0f;
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawLine(startX + shadowOffset, startY + shadowOffset,
               endX + shadowOffset, endY + shadowOffset, 2.5f);
               
    // Draw main pointer line (clean cream-white and accent colored)
    g.setColour(accentColor.brighter(0.4f));
    g.drawLine(startX, startY, endX, endY, 2.5f);
    
    g.setColour(juce::Colours::white);
    g.drawLine(startX, startY, endX, endY, 1.2f);
}

void VintageKnob::drawValueDisplay(juce::Graphics&)
{
    // Background bubble - legacy, drawn inline in paint() now
}

void VintageKnob::drawLabel(juce::Graphics&)
{
    // Legacy, drawn inline in paint() now
}

juce::String VintageKnob::getValueString() const
{
    if (valueSuffix.equalsIgnoreCase(" dB"))
    {
        return juce::String::formatted("%.1f dB", value);
    }
    if (valueSuffix.equalsIgnoreCase(" ms"))
    {
        if (value >= 100.0f)
            return juce::String::formatted("%d ms", (int)value);
        return juce::String::formatted("%.1f ms", value);
    }
    if (valueSuffix.equalsIgnoreCase(" Hz"))
    {
        if (value >= 1000.0f)
            return juce::String::formatted("%.1f kHz", value / 1000.0f);
        return juce::String::formatted("%d Hz", (int)value);
    }
    if (valueSuffix.equalsIgnoreCase(":1"))
    {
        return juce::String::formatted("%.1f:1", value);
    }
    
    float normalizedValue = (value - minValue) / (maxValue - minValue);
    int percentage = static_cast<int>(normalizedValue * 100.0f);
    
    if (valueSuffix.isNotEmpty())
    {
        return juce::String(percentage) + valueSuffix;
    }
    
    if (minValue == 0.0f && maxValue == 1.0f)
    {
        return juce::String(percentage) + "%";
    }
    
    return juce::String::formatted("%.1f", value);
}

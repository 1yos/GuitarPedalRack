#include "HardwareKnob.h"

//==============================================================================
HardwareKnob::HardwareKnob(Style knobStyle)
    : Slider(Slider::RotaryHorizontalVerticalDrag, Slider::NoTextBox),
      style(knobStyle),
      accentColor(Colour(0xFF00E5FF))  // Cyan default
{
    setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    setRotaryParameters(MathConstants<float>::pi * 1.2f,
                       MathConstants<float>::pi * 2.8f,
                       true);
}

//==============================================================================
void HardwareKnob::paint(Graphics& g)
{
    // LinearHorizontal is used for VOID's Shimmer bar
    if (getSliderStyle() == Slider::LinearHorizontal)
    {
        auto bounds = getLocalBounds().toFloat();
        
        g.setColour(Colour(0xFF150A25)); // Dark purple
        g.fillRoundedRectangle(bounds, 4.0f);
        
        float val = (float)valueToProportionOfLength(getValue());
        auto fillBar = bounds.withWidth(bounds.getWidth() * val);
        g.setColour(Colour(0xFF8F2FFF).withAlpha(0.6f)); // glowing purple
        g.fillRoundedRectangle(fillBar, 4.0f);
        
        g.setColour(Colour(0xFF8F2FFF));
        g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
        
        int percentage = (int)(val * 100);
        g.setColour(Colour(0xFF00E5FF)); // Cyan
        g.setFont(Font(9.0f, Font::bold));
        g.drawText("SHIMMER: " + String(percentage) + "%", bounds, Justification::centred);
        return;
    }

    auto bounds = getLocalBounds().toFloat().reduced(4.0f);
    
    // Calculate rotation angle
    float rotaryStart = MathConstants<float>::pi * 1.2f;
    float rotaryEnd = MathConstants<float>::pi * 2.8f;
    float sliderPos = (float)valueToProportionOfLength(getValue());
    float angle = rotaryStart + sliderPos * (rotaryEnd - rotaryStart);
    
    // Draw knob based on style
    switch (style)
    {
        case Style::MetalRounded:
            paintMetalRounded(g, bounds, angle);
            break;
        case Style::PlasticDome:
            paintPlasticDome(g, bounds, angle);
            break;
        case Style::ChickenHead:
            paintChickenHead(g, bounds, angle);
            break;
        case Style::Skirted:
            paintSkirted(g, bounds, angle);
            break;
    }
    
    // Draw value tooltip if enabled and hovered
    if (showTooltip && (isHovered || isDragging))
    {
        drawValueTooltip(g, bounds);
    }
}

void HardwareKnob::paintMetalRounded(Graphics& g, Rectangle<float> bounds, float angle)
{
    auto center = bounds.getCentre();
    float radius = jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    
    // Cast shadow
    {
        ColourGradient shadow(Colour(0x50000000), center.x, center.y + radius,
                            Colours::transparentBlack, center.x, center.y + radius + 6.0f,
                            true);
        g.setGradientFill(shadow);
        g.fillEllipse(center.x - radius, center.y - radius + 2.0f,
                     radius * 2.0f, radius * 2.0f);
    }
    
    // Outer silver rim
    g.setColour(Colour(0xFFCCCCCC));
    g.fillEllipse(center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);
    
    // Inner dark brushed metal body
    float bodyRadius = radius * 0.88f;
    ColourGradient metalGrad(Colour(0xFF333333), center.x, center.y - bodyRadius * 0.5f,
                            Colour(0xFF1E1E1E), center.x, center.y + bodyRadius * 0.5f,
                            false);
    g.setGradientFill(metalGrad);
    g.fillEllipse(center.x - bodyRadius, center.y - bodyRadius, bodyRadius * 2.0f, bodyRadius * 2.0f);
    
    // Specular radial highlights
    g.setColour(Colour(0x20FFFFFF));
    g.drawEllipse(center.x - bodyRadius + 1.0f, center.y - bodyRadius + 1.0f,
                  bodyRadius * 2.0f - 2.0f, bodyRadius * 2.0f - 2.0f, 1.0f);
    
    // Indicator line (styled notch)
    float lineStart = bodyRadius * 0.2f;
    float lineEnd = bodyRadius * 0.9f;
    
    float x1 = center.x + std::cos(angle - MathConstants<float>::halfPi) * lineStart;
    float y1 = center.y + std::sin(angle - MathConstants<float>::halfPi) * lineStart;
    float x2 = center.x + std::cos(angle - MathConstants<float>::halfPi) * lineEnd;
    float y2 = center.y + std::sin(angle - MathConstants<float>::halfPi) * lineEnd;
    
    // Notch shadow
    g.setColour(Colours::black.withAlpha(0.6f));
    g.drawLine(x1 + 1.0f, y1 + 1.0f, x2 + 1.0f, y2 + 1.0f, 2.5f);
    
    // Notch indicator
    g.setColour(accentColor);
    g.drawLine(x1, y1, x2, y2, 2.0f);
}

void HardwareKnob::paintPlasticDome(Graphics& g, Rectangle<float> bounds, float angle)
{
    auto center = bounds.getCentre();
    float radius = jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    
    // Shadow
    g.setColour(Colour(0x30000000));
    g.fillEllipse(center.x - radius + 2.0f, center.y - radius + 3.0f,
                 radius * 2.0f, radius * 2.0f);
    
    // Plastic body
    g.setColour(Colour(0xFF2A2A2A));
    g.fillEllipse(center.x - radius, center.y - radius,
                 radius * 2.0f, radius * 2.0f);
    
    // Soft highlight
    ColourGradient highlight(Colour(0x20FFFFFF), center.x - radius * 0.3f, center.y - radius * 0.3f,
                            Colours::transparentWhite, center.x, center.y,
                            false);
    g.setGradientFill(highlight);
    g.fillEllipse(center.x - radius * 0.8f, center.y - radius * 0.8f,
                 radius * 1.6f, radius * 1.6f);
    
    // Indicator line
    drawIndicatorLine(g, bounds, angle);
}

void HardwareKnob::paintChickenHead(Graphics& g, Rectangle<float> bounds, float angle)
{
    auto center = bounds.getCentre();
    float radius = jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    
    // Shadow
    g.setColour(Colour(0x30000000));
    g.fillEllipse(center.x - radius * 0.6f + 2.0f, center.y - radius * 0.6f + 3.0f,
                 radius * 1.2f, radius * 1.2f);
    
    // Center circle body
    g.setColour(Colour(0xFF1A1A1A));
    g.fillEllipse(center.x - radius * 0.6f, center.y - radius * 0.6f,
                 radius * 1.2f, radius * 1.2f);
    
    // Pointer
    Path pointer;
    float pointerLength = radius * 0.9f;
    float pointerWidth = radius * 0.15f;
    
    pointer.addTriangle(0.0f, -pointerLength,
                       -pointerWidth, -radius * 0.4f,
                       pointerWidth, -radius * 0.4f);
    
    pointer.applyTransform(AffineTransform::rotation(angle).translated(center.x, center.y));
    
    g.setColour(accentColor);
    g.fillPath(pointer);
    
    // Pointer outline
    g.setColour(accentColor.darker(0.3f));
    g.strokePath(pointer, PathStrokeType(1.5f));
    
    // Center dot
    g.setColour(Colour(0xFF404040));
    g.fillEllipse(center.x - radius * 0.25f, center.y - radius * 0.25f,
                 radius * 0.5f, radius * 0.5f);
}

void HardwareKnob::paintSkirted(Graphics& g, Rectangle<float> bounds, float angle)
{
    auto center = bounds.getCentre();
    float radius = jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    
    // Shadow
    g.setColour(Colour(0x30000000));
    g.fillEllipse(center.x - radius + 2.0f, center.y - radius + 3.0f,
                 radius * 2.0f, radius * 2.0f);
    
    // Skirt (lower part)
    float skirtRadius = radius * 0.95f;
    g.setColour(Colour(0xFF252525));
    g.fillEllipse(center.x - skirtRadius, center.y - skirtRadius,
                 skirtRadius * 2.0f, skirtRadius * 2.0f);
    
    // Top cap
    float capRadius = radius * 0.7f;
    ColourGradient capGrad(Colour(0xFF353535), center.x, center.y - capRadius,
                          Colour(0xFF202020), center.x, center.y + capRadius,
                          false);
    g.setGradientFill(capGrad);
    g.fillEllipse(center.x - capRadius, center.y - capRadius,
                 capRadius * 2.0f, capRadius * 2.0f);
    
    // Indicator line on top
    drawIndicatorLine(g, bounds.reduced(radius * 0.3f), angle);
    
    // Grip lines (decorative)
    g.setColour(Colour(0xFF151515));
    for (int i = 0; i < 12; ++i)
    {
        float lineAngle = (float)i * MathConstants<float>::pi / 6.0f;
        float x1 = center.x + std::cos(lineAngle) * (skirtRadius - 4.0f);
        float y1 = center.y + std::sin(lineAngle) * (skirtRadius - 4.0f);
        float x2 = center.x + std::cos(lineAngle) * skirtRadius;
        float y2 = center.y + std::sin(lineAngle) * skirtRadius;
        g.drawLine(x1, y1, x2, y2, 1.5f);
    }
}

void HardwareKnob::drawIndicatorLine(Graphics& g, Rectangle<float> bounds, float angle)
{
    auto center = bounds.getCentre();
    float radius = jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    
    float lineStart = radius * 0.25f;
    float lineEnd = radius * 0.75f;
    
    float x1 = center.x + std::cos(angle - MathConstants<float>::halfPi) * lineStart;
    float y1 = center.y + std::sin(angle - MathConstants<float>::halfPi) * lineStart;
    float x2 = center.x + std::cos(angle - MathConstants<float>::halfPi) * lineEnd;
    float y2 = center.y + std::sin(angle - MathConstants<float>::halfPi) * lineEnd;
    
    // Indicator line shadow
    g.setColour(Colours::black.withAlpha(0.5f));
    g.drawLine(x1 + 1.0f, y1 + 1.0f, x2 + 1.0f, y2 + 1.0f, 3.0f);
    
    // Indicator line
    g.setColour(accentColor);
    g.drawLine(x1, y1, x2, y2, 2.5f);
    
    // Indicator dot at end
    g.fillEllipse(x2 - 3.0f, y2 - 3.0f, 6.0f, 6.0f);
}

void HardwareKnob::drawValueTooltip(Graphics& g, Rectangle<float> bounds)
{
    // Get current value as string
    String valueText = String(getValue(), 2);
    
    // Add suffix if available
    if (getTextValueSuffix().isNotEmpty())
        valueText += " " + getTextValueSuffix();
    
    // Tooltip box
    Rectangle<float> tooltipBounds(0.0f, -30.0f, 60.0f, 20.0f);
    tooltipBounds.setCentre(bounds.getCentreX(), tooltipBounds.getCentreY());
    
    // Background
    g.setColour(Colour(0xE0000000));
    g.fillRoundedRectangle(tooltipBounds, 4.0f);
    
    // Border
    g.setColour(accentColor.withAlpha(0.8f));
    g.drawRoundedRectangle(tooltipBounds, 4.0f, 1.0f);
    
    // Text
    g.setColour(Colours::white);
    g.setFont(12.0f);
    g.drawText(valueText, tooltipBounds, Justification::centred);
}

//==============================================================================
void HardwareKnob::mouseDown(const MouseEvent& e)
{
    if (getSliderStyle() == Slider::LinearHorizontal)
    {
        Slider::mouseDown(e);
        return;
    }
    
    Slider::mouseDown(e);
    lastMousePos = e.getPosition();
    isDragging = true;
    repaint();
}

void HardwareKnob::mouseDrag(const MouseEvent& e)
{
    if (getSliderStyle() == Slider::LinearHorizontal)
    {
        Slider::mouseDrag(e);
        return;
    }
    
    isDragging = true;
    
    int deltaY = lastMousePos.y - e.getPosition().y;
    
    double sensitivity = e.mods.isShiftDown() ? fineDragSensitivity : dragSensitivity;
    double valueRange = getMaximum() - getMinimum();
    double newValue = getValue() + (deltaY * sensitivity * valueRange);
    
    setValue(jlimit(getMinimum(), getMaximum(), newValue), sendNotificationSync);
    
    lastMousePos = e.getPosition();
    repaint();
}

void HardwareKnob::mouseUp(const MouseEvent& e)
{
    if (getSliderStyle() == Slider::LinearHorizontal)
    {
        Slider::mouseUp(e);
        return;
    }
    
    Slider::mouseUp(e);
    isDragging = false;
    repaint();
}

void HardwareKnob::mouseDoubleClick(const MouseEvent& e)
{
    if (getSliderStyle() == Slider::LinearHorizontal)
    {
        Slider::mouseDoubleClick(e);
        return;
    }
    
    setValue(getDoubleClickReturnValue(), sendNotificationSync);
    repaint();
}

void HardwareKnob::mouseWheelMove(const MouseEvent& e, const MouseWheelDetails& wheel)
{
    if (getSliderStyle() == Slider::LinearHorizontal)
    {
        Slider::mouseWheelMove(e, wheel);
        return;
    }
    
    double sensitivity = e.mods.isShiftDown() ? fineDragSensitivity * 10.0 : dragSensitivity * 10.0;
    double valueRange = getMaximum() - getMinimum();
    double newValue = getValue() + (wheel.deltaY * sensitivity * valueRange);
    
    setValue(jlimit(getMinimum(), getMaximum(), newValue), sendNotificationSync);
    repaint();
}

void HardwareKnob::mouseEnter(const MouseEvent& e)
{
    Slider::mouseEnter(e);
    isHovered = true;
    repaint();
}

void HardwareKnob::mouseExit(const MouseEvent& e)
{
    Slider::mouseExit(e);
    isHovered = false;
    repaint();
}

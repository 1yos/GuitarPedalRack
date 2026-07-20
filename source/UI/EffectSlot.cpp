#include "EffectSlot.h"

EffectSlot::EffectSlot(const juce::String& name, const juce::String& cat)
    : effectName(name), category(cat)
{
    setSize(200, 80);
}

void EffectSlot::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);
    
    // Shadow for depth
    if (!dragging)
    {
        g.setColour(juce::Colours::black.withAlpha(0.4f));
        g.fillRoundedRectangle(bounds.translated(2, 3), 8.0f);
    }
    
    // Main background with category color
    auto categoryColor = getCategoryColor();
    
    if (bypassed)
    {
        categoryColor = categoryColor.darker(1.5f).withSaturation(0.3f);
    }
    else if (selected)
    {
        categoryColor = categoryColor.brighter(0.3f);
    }
    else if (hovered)
    {
        categoryColor = categoryColor.brighter(0.15f);
    }
    
    // Gradient background
    juce::ColourGradient gradient(
        categoryColor.brighter(0.2f), bounds.getCentreX(), bounds.getY(),
        categoryColor.darker(0.4f), bounds.getCentreX(), bounds.getBottom(),
        false
    );
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, 8.0f);
    
    // Border
    g.setColour(selected ? juce::Colours::white : categoryColor.brighter(0.5f));
    g.drawRoundedRectangle(bounds, 8.0f, selected ? 3.0f : 2.0f);
    
    // Glossy top highlight
    g.setColour(juce::Colours::white.withAlpha(bypassed ? 0.05f : 0.15f));
    g.fillRoundedRectangle(bounds.reduced(10, 10).withHeight(20), 6.0f);
    
    // Icon area
    auto iconBounds = bounds.removeFromLeft(60).reduced(12);
    auto icon = getCategoryIcon();
    g.setColour(juce::Colours::white.withAlpha(bypassed ? 0.3f : 0.8f));
    g.fillPath(icon, icon.getTransformToScaleToFit(iconBounds, true));
    
    // Effect name
    auto textBounds = bounds.reduced(8, 4);
    g.setColour(juce::Colours::white.withAlpha(bypassed ? 0.4f : 1.0f));
    g.setFont(juce::Font(16.0f, juce::Font::bold));
    g.drawText(effectName, textBounds.removeFromTop(24), juce::Justification::left);
    
    // Category label
    g.setFont(juce::Font(11.0f));
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawText(category, textBounds.removeFromTop(16), juce::Justification::left);
    
    // CPU usage indicator
    if (cpuUsage > 0.01f)
    {
        auto cpuBounds = textBounds.removeFromBottom(12).reduced(2);
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.fillRoundedRectangle(cpuBounds, 2.0f);
        
        auto cpuFill = cpuBounds.removeFromLeft(cpuBounds.getWidth() * juce::jmin(cpuUsage, 1.0f));
        auto cpuColor = cpuUsage < 0.5f ? juce::Colours::green :
                       cpuUsage < 0.8f ? juce::Colours::orange :
                       juce::Colours::red;
        g.setColour(cpuColor.withAlpha(0.7f));
        g.fillRoundedRectangle(cpuFill, 2.0f);
    }
    
    // LED indicator (bypass status)
    auto ledBounds = juce::Rectangle<float>(bounds.getRight() - 25, bounds.getY() + 10, 12, 12);
    
    if (!bypassed)
    {
        // Glow effect when active
        g.setColour(categoryColor.withAlpha(0.4f));
        g.fillEllipse(ledBounds.expanded(4));
        
        g.setColour(categoryColor.brighter(0.8f));
        g.fillEllipse(ledBounds);
        
        g.setColour(juce::Colours::white.withAlpha(0.8f));
        g.fillEllipse(ledBounds.reduced(4));
    }
    else
    {
        g.setColour(juce::Colour(0xff2a2a2a));
        g.fillEllipse(ledBounds);
    }
    
    // Remove button (X)
    removeButtonArea = juce::Rectangle<int>(
        (int)(bounds.getRight() - 22),
        (int)(bounds.getBottom() - 22),
        18, 18
    );
    
    if (hovered)
    {
        g.setColour(juce::Colours::red.withAlpha(0.7f));
        g.fillRoundedRectangle(removeButtonArea.toFloat(), 3.0f);
        g.setColour(juce::Colours::white);
        g.drawLine(removeButtonArea.getX() + 5, removeButtonArea.getY() + 5,
                   removeButtonArea.getRight() - 5, removeButtonArea.getBottom() - 5, 2.0f);
        g.drawLine(removeButtonArea.getRight() - 5, removeButtonArea.getY() + 5,
                   removeButtonArea.getX() + 5, removeButtonArea.getBottom() - 5, 2.0f);
    }
    
    // Drag handle (grip lines)
    dragHandleArea = juce::Rectangle<int>(
        (int)(bounds.getX() + 8),
        (int)(bounds.getCentreY() - 15),
        6, 30
    );
    
    if (!bypassed)
    {
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        for (int i = 0; i < 5; ++i)
        {
            float y = dragHandleArea.getY() + i * 6.0f;
            g.fillRect(dragHandleArea.getX(), (int)y, dragHandleArea.getWidth(), 2);
        }
    }
}

void EffectSlot::resized()
{
    // Button areas calculated in paint for accurate positioning
}

void EffectSlot::mouseEnter(const juce::MouseEvent&)
{
    hovered = true;
    repaint();
}

void EffectSlot::mouseExit(const juce::MouseEvent&)
{
    hovered = false;
    repaint();
}

void EffectSlot::mouseDown(const juce::MouseEvent& event)
{
    if (removeButtonArea.contains(event.getPosition()))
    {
        if (onRemoveClicked)
            onRemoveClicked(this);
        return;
    }
    
    // Check if clicking the main area (not remove button)
    auto bounds = getLocalBounds().reduced(4);
    if (bounds.contains(event.getPosition()))
    {
        // Double-click toggles bypass
        if (event.getNumberOfClicks() == 2)
        {
            setBypassed(!bypassed);
            if (onBypassToggled)
                onBypassToggled(this);
        }
        else
        {
            // Single click selects
            if (onSelected)
                onSelected(this);
        }
    }
}

void EffectSlot::mouseUp(const juce::MouseEvent&)
{
    if (dragging)
    {
        stopDragging();
        if (onDragEnded)
            onDragEnded(this);
    }
}

void EffectSlot::mouseDrag(const juce::MouseEvent& event)
{
    // Start dragging if moved enough
    if (!dragging && event.getDistanceFromDragStart() > 10)
    {
        // Check if drag started from drag handle area
        if (dragHandleArea.contains(event.getMouseDownPosition()))
        {
            startDragging();
            if (onDragStarted)
                onDragStarted(this, event.getPosition());
        }
    }
    
    if (dragging && onDragMoved)
    {
        onDragMoved(this, event.getPosition());
    }
}

void EffectSlot::setBypassed(bool shouldBypass)
{
    bypassed = shouldBypass;
    repaint();
}

void EffectSlot::setSelected(bool shouldSelect)
{
    selected = shouldSelect;
    repaint();
}

void EffectSlot::setCpuUsage(float usage)
{
    cpuUsage = usage;
    repaint();
}

void EffectSlot::startDragging()
{
    dragging = true;
    setAlpha(0.7f);
    repaint();
}

void EffectSlot::stopDragging()
{
    dragging = false;
    setAlpha(1.0f);
    repaint();
}

juce::Colour EffectSlot::getCategoryColor() const
{
    if (category == "Drive") return juce::Colour(0xffFF8C00);
    if (category == "Modulation") return juce::Colour(0xff9B59B6);
    if (category == "Delay") return juce::Colour(0xff3498DB);
    if (category == "Reverb") return juce::Colour(0xff4B0082);
    if (category == "Filter") return juce::Colour(0xff2ECC71);
    if (category == "Dynamics") return juce::Colour(0xffE74C3C);
    if (category == "EQ") return juce::Colour(0xffF39C12);
    if (category == "Pitch") return juce::Colour(0xff1ABC9C);
    if (category == "Amp") return juce::Colour(0xffC8A060);
    if (category == "Cabinet") return juce::Colour(0xff95A5A6);
    return juce::Colours::grey;
}

juce::Path EffectSlot::getCategoryIcon() const
{
    juce::Path icon;
    
    if (category == "Drive")
    {
        // Lightning bolt
        icon.startNewSubPath(20, 0);
        icon.lineTo(8, 20);
        icon.lineTo(15, 20);
        icon.lineTo(10, 40);
        icon.lineTo(22, 20);
        icon.lineTo(15, 20);
        icon.closeSubPath();
    }
    else if (category == "Modulation")
    {
        // Wave
        icon.startNewSubPath(0, 20);
        icon.quadraticTo(10, 0, 20, 20);
        icon.quadraticTo(30, 40, 40, 20);
    }
    else if (category == "Delay" || category == "Reverb")
    {
        // Echo ripples
        icon.addEllipse(10, 10, 20, 20);
        icon.addEllipse(5, 5, 30, 30);
        icon.addEllipse(0, 0, 40, 40);
    }
    else if (category == "Filter")
    {
        // Funnel shape
        icon.startNewSubPath(0, 0);
        icon.lineTo(40, 0);
        icon.lineTo(25, 30);
        icon.lineTo(15, 30);
        icon.closeSubPath();
    }
    else if (category == "Dynamics")
    {
        // Compression curve
        icon.startNewSubPath(0, 40);
        icon.quadraticTo(20, 20, 40, 10);
    }
    else if (category == "EQ")
    {
        // Frequency bars
        for (int i = 0; i < 5; ++i)
        {
            float height = 10 + (i % 3) * 10;
            icon.addRectangle(i * 9, 40 - height, 6, height);
        }
    }
    else if (category == "Pitch")
    {
        // Musical note
        icon.addEllipse(5, 25, 10, 15);
        icon.addRectangle(13, 5, 3, 25);
        icon.addRectangle(13, 5, 10, 3);
    }
    else if (category == "Amp")
    {
        // Amp head silhouette
        icon.addRoundedRectangle(0, 10, 40, 25, 3.0f);
        icon.addRectangle(5, 0, 30, 10);
    }
    else if (category == "Cabinet")
    {
        // Speaker cone
        icon.addEllipse(8, 8, 24, 24);
        icon.addEllipse(14, 14, 12, 12);
    }
    else
    {
        // Generic gear icon
        icon.addEllipse(10, 10, 20, 20);
    }
    
    return icon;
}

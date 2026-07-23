#include "PedalSlot.h"
#include "Materials.h"
#include "WornTextures.h"

PedalSlot::PedalSlot(const juce::String& name, const juce::String& cat)
    : effectName(name), category(cat)
{
    setSize(155, 210); // Compact boutique size matching reference image
    startTimerHz(30);
}

void PedalSlot::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Hover lift animation
    if (hovered && !dragging)
        hoverLift = juce::jmin(1.0f, hoverLift + 0.15f);
    else
        hoverLift = juce::jmax(0.0f, hoverLift - 0.15f);
    
    float liftAmount = hoverLift * 3.0f;
    bounds = bounds.translated(0, -liftAmount);
    
    // Multi-layer drop shadow
    if (!dragging)
    {
        for (int i = 0; i < 4; ++i)
        {
            float alpha  = (selected ? 0.30f : 0.20f) / (i + 1);
            float offset = (selected ? 7.0f  : 4.0f) + i * 3.0f + liftAmount;
            float blur   = 4.0f + i * 2.0f;
            g.setColour(juce::Colours::black.withAlpha(alpha));
            juce::Path sp;
            sp.addRoundedRectangle(bounds.translated(0, offset).expanded(blur), 8.0f);
            g.fillPath(sp);
        }
    }
    
    // --- ENCLOSURE ---
    auto enc = bounds.reduced(3);
    drawBoutiquePedal(g, enc);
    
    auto area = enc.reduced(10, 10);
    auto enclosureColor = getEnclosureColor();
    bool isDark = enclosureColor.getBrightness() < 0.45f;
    auto textColor = isDark ? juce::Colours::white : juce::Colour(0xff1a1a1a);
    auto dimColor  = textColor.withAlpha(bypassed ? 0.4f : 0.65f);
    
    // --- BRAND PLATE ---
    g.setFont(juce::Font(9.0f, juce::Font::plain));
    g.setColour(dimColor.withAlpha(bypassed ? 0.3f : 0.55f));
    g.drawText("AETHER", area.removeFromTop(13), juce::Justification::centred);
    
    // --- EFFECT NAME ---
    auto nameArea = area.removeFromTop(30);
    g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.setColour(textColor.withAlpha(bypassed ? 0.5f : 0.92f));
    g.drawText(effectName.toUpperCase(), nameArea, juce::Justification::centred, true);
    
    // --- KNOB LABELS ROW ---
    auto labelRow = area.removeFromTop(14);
    {
        float kw = area.getWidth() / 3.0f;
        juce::StringArray labels { getFirstKnobLabel(), getSecondKnobLabel(), getThirdKnobLabel() };
        g.setFont(juce::Font(7.5f, juce::Font::bold));
        g.setColour(dimColor);
        for (int i = 0; i < 3; ++i)
        {
            auto lb = juce::Rectangle<float>(labelRow.getX() + i * kw, labelRow.getY(), kw, 14);
            g.drawText(labels[i], lb, juce::Justification::centred);
        }
    }
    
    // --- 3 KNOBS ---
    {
        auto knobRow = area.removeFromTop(40);
        float kSize = 28.0f;
        float kw = knobRow.getWidth() / 3.0f;
        auto accentColor = getCategoryColor();

        for (int i = 0; i < 3; ++i)
        {
            float kx = knobRow.getX() + i * kw + (kw - kSize) * 0.5f;
            float ky = knobRow.getY() + (knobRow.getHeight() - kSize) * 0.5f;
            auto kb = juce::Rectangle<float>(kx, ky, kSize, kSize);

            // Store bounds for interaction (convert to local ints)
            if (i == 0) knob1Bounds = kb;
            else if (i == 1) knob2Bounds = kb;
            else            knob3Bounds = kb;

            float val = (i == 0) ? knob1Value : (i == 1) ? knob2Value : knob3Value;

            // Active knob glow
            if (activeKnob == i)
            {
                g.setColour(accentColor.withAlpha(0.25f));
                g.fillEllipse(kb.expanded(5));
            }

            Materials::drawMachinedKnob(g, kb, val, accentColor, true);

            // Tick marks
            for (int t = 0; t < 11; ++t)
            {
                float ang = -juce::MathConstants<float>::pi * 0.75f +
                            (t / 10.0f) * juce::MathConstants<float>::pi * 1.5f;
                float r1 = kSize * 0.58f, r2 = r1 + 3.0f;
                float cx = kb.getCentreX(), cy = kb.getCentreY();
                g.setColour(isDark ? juce::Colours::white.withAlpha(0.28f)
                                   : juce::Colours::black.withAlpha(0.22f));
                g.drawLine(cx + std::cos(ang) * r1, cy + std::sin(ang) * r1,
                           cx + std::cos(ang) * r2, cy + std::sin(ang) * r2,
                           t == 5 ? 1.4f : 0.7f);
            }
        }
    }
    
    // --- VALUE TOOLTIP while dragging a knob ---
    if (activeKnob >= 0)
    {
        auto kb = (activeKnob == 0) ? knob1Bounds : (activeKnob == 1) ? knob2Bounds : knob3Bounds;
        auto tip = kb.withY(kb.getY() - 28).withHeight(20).withWidth(46)
                     .withCentre({ kb.getCentreX(), kb.getY() - 18 });
        g.setColour(juce::Colours::black.withAlpha(0.88f));
        g.fillRoundedRectangle(tip, 4.0f);
        g.setColour(getCategoryColor().withAlpha(0.85f));
        g.drawRoundedRectangle(tip, 4.0f, 1.2f);
        g.setFont(juce::Font(11.0f, juce::Font::bold));
        g.setColour(juce::Colours::white);
        float val = (activeKnob == 0) ? knob1Value : (activeKnob == 1) ? knob2Value : knob3Value;
        g.drawText(juce::String((int)(val * 100)) + "%", tip, juce::Justification::centred);
    }
    
    area.removeFromTop(4);
    
    // --- LED ---
    ledArea = juce::Rectangle<float>(bounds.getCentreX() - 5.5f,
                                     area.getY(),
                                     11.0f, 11.0f);
    Materials::drawGlassLED(g, ledArea, getCategoryColor(), ledBrightness, !bypassed);
    area.removeFromTop(18);
    
    // --- FOOTSWITCH ---
    footswitchArea = juce::Rectangle<float>(bounds.getCentreX() - 16.0f,
                                            bounds.getBottom() - 44.0f,
                                            32.0f, 32.0f);
    drawProfessionalFootswitch(g, footswitchArea);
    
    // --- SCREWS ---
    drawScrews(g, enc);
    
    // --- JACKS ---
    drawJacks(g);
    
    // --- SELECTION GLOW ---
    if (selected)
    {
        auto c = getCategoryColor();
        g.setColour(c.withAlpha(0.70f));
        g.drawRoundedRectangle(bounds.reduced(2), 9.0f, 2.5f);
        g.setColour(c.withAlpha(0.30f));
        g.drawRoundedRectangle(bounds.expanded(2), 11.0f, 1.8f);
    }
    
    // --- REMOVE BUTTON (hover only) ---
    if (hovered && !dragging)
    {
        removeButtonArea = juce::Rectangle<float>(bounds.getRight() - 22, bounds.getY() + 6, 16, 16);
        g.setColour(juce::Colours::black.withAlpha(0.80f));
        g.fillRoundedRectangle(removeButtonArea, 3.0f);
        g.setColour(juce::Colours::red.withAlpha(0.9f));
        g.drawRoundedRectangle(removeButtonArea, 3.0f, 1.2f);
        float cx = removeButtonArea.getCentreX(), cy = removeButtonArea.getCentreY(), s = 4.5f;
        g.setColour(juce::Colours::red.brighter(0.3f));
        g.drawLine(cx-s, cy-s, cx+s, cy+s, 1.8f);
        g.drawLine(cx+s, cy-s, cx-s, cy+s, 1.8f);
    }
}

void PedalSlot::drawBoutiquePedal(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    auto enclosureColor = getEnclosureColor();
    Materials::drawPowderCoatedMetal(g, bounds, enclosureColor, 8.0f);
    
    // Ambient occlusion in corners
    juce::ColourGradient ao(
        juce::Colours::black.withAlpha(0.35f), bounds.getCentreX(), bounds.getCentreY(),
        juce::Colours::transparentBlack, bounds.getCentreX(), bounds.getCentreY(), true);
    g.setGradientFill(ao);
    g.fillRoundedRectangle(bounds.reduced(2), 6.0f);
    
    // Dark edge
    g.setColour(juce::Colours::black.withAlpha(0.55f));
    g.drawRoundedRectangle(bounds, 8.0f, 1.8f);
}

void PedalSlot::drawBrandPlate(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    // Unused - brand is drawn inline in paint()
    juce::ignoreUnused(g, bounds);
}

void PedalSlot::drawProfessionalKnobs(juce::Graphics& g, juce::Rectangle<float> area, bool isDark)
{
    // Unused - knobs are drawn inline in paint() for compact layout
    juce::ignoreUnused(g, area, isDark);
}

void PedalSlot::drawProfessionalFootswitch(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    auto center = bounds.getCentre();
    
    // Metal housing ring
    juce::ColourGradient housing(
        juce::Colour(0xff5a5a5a), center.x, bounds.getY(),
        juce::Colour(0xff2a2a2a), center.x, bounds.getBottom(),
        false
    );
    g.setGradientFill(housing);
    g.fillEllipse(bounds);
    
    // Housing rim highlight
    g.setColour(juce::Colour(0xff7a7a7a));
    g.drawEllipse(bounds, 1.5f);
    
    // Soft rubber button (using Materials system)
    auto buttonBounds = bounds.reduced(6);
    if (footswitchPressed)
        buttonBounds = buttonBounds.translated(0, 2).reduced(0, 1);
    
    Materials::drawSoftRubber(g, buttonBounds, true);
    
    // Subtle indicator line on rubber
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    g.drawLine(center.x, buttonBounds.getY() + 8, center.x, buttonBounds.getBottom() - 8, 2.0f);
}

void PedalSlot::drawScrews(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    float inset = 12.0f;
    
    // Four corner screws
    Materials::drawStainlessSteelScrew(g, juce::Point<float>(bounds.getX() + inset, bounds.getY() + inset), 10.0f);
    Materials::drawStainlessSteelScrew(g, juce::Point<float>(bounds.getRight() - inset, bounds.getY() + inset), 10.0f);
    Materials::drawStainlessSteelScrew(g, juce::Point<float>(bounds.getX() + inset, bounds.getBottom() - inset), 10.0f);
    Materials::drawStainlessSteelScrew(g, juce::Point<float>(bounds.getRight() - inset, bounds.getBottom() - inset), 10.0f);
}

void PedalSlot::drawRubberFeet(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    // Four rubber feet underneath (visible from slight perspective)
    float footSize = 6.0f;
    float inset = 18.0f;
    
    auto drawFoot = [&](juce::Point<float> pos)
    {
        g.setColour(juce::Colour(0xff0a0a0a));
        g.fillEllipse(pos.x - footSize * 0.5f, pos.y - footSize * 0.5f, footSize, footSize);
        
        g.setColour(juce::Colours::black.withAlpha(0.6f));
        g.fillEllipse(pos.x - footSize * 0.3f, pos.y - footSize * 0.3f, footSize * 0.6f, footSize * 0.6f);
    };
    
    drawFoot(juce::Point<float>(bounds.getX() + inset, bounds.getBottom() + 2));
    drawFoot(juce::Point<float>(bounds.getRight() - inset, bounds.getBottom() + 2));
}

void PedalSlot::drawWornEnclosure(juce::Graphics& g, juce::Rectangle<float> bounds)
{
    auto enclosureColor = getEnclosureColor();
    
    // Use worn texture system (aged/scratched appearance)
    float wearAmount = 0.55f; // Medium wear (looks used but not destroyed)
    int seed = effectName.hashCode(); // Unique wear pattern per pedal
    
    WornTextures::drawWornPaint(g, bounds, enclosureColor, wearAmount, seed);
    
    // Dark outline
    g.setColour(juce::Colours::black.withAlpha(0.4f));
    g.drawRoundedRectangle(bounds, 7.0f, 1.5f);
}

void PedalSlot::drawJacks(juce::Graphics& g)
{
    auto drawJack = [&g](juce::Point<float> pos, juce::Colour color, bool bypassed)
    {
        float jackSize = 10.0f;
        
        // Jack shadow
        juce::ColourGradient shadow(
            juce::Colours::black.withAlpha(0.6f), pos.x, pos.y,
            juce::Colours::transparentBlack, pos.x, pos.y,
            true
        );
        g.setGradientFill(shadow);
        g.fillEllipse(pos.x - jackSize * 0.7f, pos.y - jackSize * 0.7f, jackSize * 1.4f, jackSize * 1.4f);
        
        // Jack barrel (metal housing)
        juce::ColourGradient housing(
            juce::Colour(0xff5a5a5a), pos.x, pos.y - jackSize * 0.5f,
            juce::Colour(0xff1a1a1a), pos.x, pos.y + jackSize * 0.5f,
            false
        );
        g.setGradientFill(housing);
        g.fillEllipse(pos.x - jackSize * 0.5f, pos.y - jackSize * 0.5f, jackSize, jackSize);
        
        // Metal threads detail
        g.setColour(juce::Colour(0xff3a3a3a));
        g.drawEllipse(pos.x - jackSize * 0.4f, pos.y - jackSize * 0.4f, jackSize * 0.8f, jackSize * 0.8f, 0.5f);
        
        // Jack tip (colored or grey if bypassed)
        float tipSize = jackSize * 0.6f;
        juce::Colour tipColor = bypassed ? juce::Colour(0xff2a2a2a) : color;
        
        juce::ColourGradient tip(
            tipColor.brighter(0.2f), pos.x, pos.y - tipSize * 0.5f,
            tipColor.darker(0.3f), pos.x, pos.y + tipSize * 0.5f,
            false
        );
        g.setGradientFill(tip);
        g.fillEllipse(pos.x - tipSize * 0.5f, pos.y - tipSize * 0.5f, tipSize, tipSize);
        
        // Specular highlight
        if (!bypassed)
        {
            g.setColour(juce::Colours::white.withAlpha(0.5f));
            g.fillEllipse(pos.x - tipSize * 0.3f, pos.y - tipSize * 0.4f, 
                        tipSize * 0.4f, tipSize * 0.25f);
        }
        
        // Metal rim highlight
        g.setColour(juce::Colour(0xff7a7a7a));
        g.drawEllipse(pos.x - jackSize * 0.5f, pos.y - jackSize * 0.5f, jackSize, jackSize, 1.2f);
    };
    
    auto categoryColor = getCategoryColor();
    drawJack(inputJackPos, categoryColor, bypassed);
    drawJack(outputJackPos, categoryColor, bypassed);
}

void PedalSlot::resized()
{
    auto bounds = getLocalBounds().toFloat();
    inputJackPos  = { bounds.getX() + 8,        bounds.getY() + bounds.getHeight() * 0.58f };
    outputJackPos = { bounds.getRight() - 8,     bounds.getY() + bounds.getHeight() * 0.58f };
}

// Rest of the implementation (mouseEnter, mouseExit, etc.) remains similar to boutique version
// but with updated bounds handling for compact size

void PedalSlot::mouseEnter(const juce::MouseEvent&)
{
    hovered = true;
    repaint();
}

void PedalSlot::mouseExit(const juce::MouseEvent&)
{
    hovered = false;
    repaint();
}

void PedalSlot::mouseDown(const juce::MouseEvent& event)
{
    if (removeButtonArea.contains(event.position))
    {
        if (onRemoveClicked)
            onRemoveClicked(this);
        return;
    }
    
    if (footswitchArea.contains(event.position))
    {
        footswitchPressed = true;
        bypassed = !bypassed;
        ledBrightness = bypassed ? 0.0f : 1.0f;
        
        if (onBypassToggled)
            onBypassToggled(this);
        
        repaint();
        return;
    }
    
    // Check knobs
    if (knob1Bounds.contains(event.position))
    {
        activeKnob = 0;
        dragStartValue = knob1Value;
        dragStartPos = event.position.toInt();
        repaint();
        return;
    }
    else if (knob2Bounds.contains(event.position))
    {
        activeKnob = 1;
        dragStartValue = knob2Value;
        dragStartPos = event.position.toInt();
        repaint();
        return;
    }
    else if (knob3Bounds.contains(event.position))
    {
        activeKnob = 2;
        dragStartValue = knob3Value;
        dragStartPos = event.position.toInt();
        repaint();
        return;
    }
    
    if (onSelected)
        onSelected(this);
    
    if (onDragStarted)
        onDragStarted(this, event.position.toInt());
}

void PedalSlot::mouseUp(const juce::MouseEvent&)
{
    if (footswitchPressed)
    {
        footswitchPressed = false;
        repaint();
    }
    
    if (activeKnob >= 0)
    {
        activeKnob = -1;
        repaint();
    }
    
    if (dragging && onDragEnded)
    {
        onDragEnded(this);
    }
}

void PedalSlot::mouseDrag(const juce::MouseEvent& event)
{
    if (activeKnob >= 0)
    {
        float pixelDrag = dragStartPos.y - event.position.y;
        float sensitivity = event.mods.isShiftDown() ? 0.001f : 0.005f;
        float newValue = juce::jlimit(0.0f, 1.0f, dragStartValue + (pixelDrag * sensitivity));
        
        if (activeKnob == 0) knob1Value = newValue;
        else if (activeKnob == 1) knob2Value = newValue;
        else if (activeKnob == 2) knob3Value = newValue;
        
        repaint();
        return;
    }
    
    if (!dragging && event.getDistanceFromDragStart() > 10)
    {
        startDragging();
    }
    
    if (dragging && onDragMoved)
    {
        onDragMoved(this, event.position.toInt());
    }
}

void PedalSlot::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    if (knob1Bounds.contains(event.position))
    {
        knob1Value = juce::jlimit(0.0f, 1.0f, knob1Value + wheel.deltaY * 0.1f);
        repaint();
    }
    else if (knob2Bounds.contains(event.position))
    {
        knob2Value = juce::jlimit(0.0f, 1.0f, knob2Value + wheel.deltaY * 0.1f);
        repaint();
    }
    else if (knob3Bounds.contains(event.position))
    {
        knob3Value = juce::jlimit(0.0f, 1.0f, knob3Value + wheel.deltaY * 0.1f);
        repaint();
    }
}

void PedalSlot::mouseDoubleClick(const juce::MouseEvent& event)
{
    // Reset knob to default (50%) on double-click
    if (knob1Bounds.contains(event.position))
    {
        knob1Value = 0.5f;
        repaint();
    }
    else if (knob2Bounds.contains(event.position))
    {
        knob2Value = 0.5f;
        repaint();
    }
    else if (knob3Bounds.contains(event.position))
    {
        knob3Value = 0.5f;
        repaint();
    }
}

void PedalSlot::timerCallback()
{
    if (!bypassed)
    {
        ledPhase += 0.03f;
        if (ledPhase > juce::MathConstants<float>::twoPi)
            ledPhase -= juce::MathConstants<float>::twoPi;
        
        ledBrightness = 0.8f + 0.2f * std::sin(ledPhase);
        repaint();
    }
    
    if (hovered || hoverLift > 0.0f)
    {
        repaint();
    }
}

void PedalSlot::setBypassed(bool shouldBypass)
{
    bypassed = shouldBypass;
    ledBrightness = bypassed ? 0.0f : 1.0f;
    repaint();
}

void PedalSlot::setSelected(bool shouldSelect)
{
    selected = shouldSelect;
    repaint();
}

void PedalSlot::setCpuUsage(float usage)
{
    cpuUsage = usage;
}

void PedalSlot::startDragging()
{
    dragging = true;
    setAlpha(0.7f);
    repaint();
}

void PedalSlot::stopDragging()
{
    dragging = false;
    setAlpha(1.0f);
    repaint();
}

juce::Point<float> PedalSlot::getInputJackPosition() const
{
    return getLocalPoint(nullptr, inputJackPos);
}

juce::Point<float> PedalSlot::getOutputJackPosition() const
{
    return getLocalPoint(nullptr, outputJackPos);
}

juce::Colour PedalSlot::getCategoryColor() const
{
    // Match reference image colors exactly
    if (category == "Drive" || category == "Overdrive") return juce::Colour(0xffD4A349); // Mustard yellow
    if (category == "Distortion" || category == "Fuzz") return juce::Colour(0xff8B3A3A); // Deep red
    if (category == "Modulation" || category == "Chorus") return juce::Colour(0xff6B7C4E); // Olive green
    if (category == "Delay") return juce::Colour(0xff3A7A7A); // Teal/cyan
    if (category == "Reverb") return juce::Colour(0xff5A4A7A); // Deep purple
    if (category == "Filter" || category == "EQ") return juce::Colour(0xff1a1a1a); // Matte black (EQ has screen)
    if (category == "Dynamics" || category == "Compressor") return juce::Colour(0xff1a1a1a); // Matte black
    if (category == "Cabinet" || category == "Amp") return juce::Colour(0xff7A6A4A); // Dark tan
    if (category == "Pitch") return juce::Colour(0xff4A6A7A); // Steel blue
    return juce::Colour(0xff2a2a2a);
}

juce::Colour PedalSlot::getEnclosureColor() const
{
    auto categoryColor = getCategoryColor();
    
    if (bypassed)
        return categoryColor.darker(1.2f).withSaturation(0.25f);
    
    return categoryColor;
}

juce::String PedalSlot::getFirstKnobLabel() const
{
    juce::String n = effectName.toLowerCase();
    if (n.contains("gate") || n.contains("noisegate")) return "THRESH";
    if (n.contains("comp") || n.contains("compressor")) return "THRESH";
    if (n.contains("limiter")) return "THRESH";
    if (n.contains("overdrive") || n.contains("drive") || n.contains("distortion")
        || n.contains("fuzz") || n.contains("rat") || n.contains("muff")) return "GAIN";
    if (n.contains("delay") || n.contains("echo")) return "TIME";
    if (n.contains("reverb") || n.contains("plate") || n.contains("spring") || n.contains("hall")) return "SIZE";
    if (n.contains("chorus") || n.contains("flanger") || n.contains("phaser")
        || n.contains("tremolo") || n.contains("vibrato")) return "RATE";
    if (n.contains("tonestack") || n.contains("amp") || n.contains("eq")) return "BASS";
    if (n.contains("cabinet") || n.contains("cab")) return "MIX";
    if (n.contains("pitch") || n.contains("octav") || n.contains("whammy")) return "PITCH";
    if (n.contains("wah") || n.contains("filter")) return "FREQ";
    return "LEVEL";
}

juce::String PedalSlot::getSecondKnobLabel() const
{
    juce::String n = effectName.toLowerCase();
    if (n.contains("gate") || n.contains("noisegate")) return "ATTACK";
    if (n.contains("comp") || n.contains("compressor")) return "RATIO";
    if (n.contains("limiter")) return "RELEASE";
    if (n.contains("overdrive") || n.contains("drive") || n.contains("distortion")
        || n.contains("fuzz") || n.contains("rat") || n.contains("muff")) return "TONE";
    if (n.contains("delay") || n.contains("echo")) return "FDBK";
    if (n.contains("reverb") || n.contains("plate") || n.contains("spring") || n.contains("hall")) return "DECAY";
    if (n.contains("chorus") || n.contains("flanger") || n.contains("phaser")
        || n.contains("tremolo") || n.contains("vibrato")) return "DEPTH";
    if (n.contains("tonestack") || n.contains("amp") || n.contains("eq")) return "MID";
    if (n.contains("cabinet") || n.contains("cab")) return "LO CUT";
    if (n.contains("pitch") || n.contains("octav") || n.contains("whammy")) return "DIRECT";
    if (n.contains("wah") || n.contains("filter")) return "Q";
    return "TONE";
}

juce::String PedalSlot::getThirdKnobLabel() const
{
    juce::String n = effectName.toLowerCase();
    if (n.contains("gate") || n.contains("noisegate")) return "RELEASE";
    if (n.contains("comp") || n.contains("compressor")) return "MAKEUP";
    if (n.contains("limiter")) return "MAKEUP";
    if (n.contains("overdrive") || n.contains("drive") || n.contains("distortion")
        || n.contains("fuzz") || n.contains("rat") || n.contains("muff")) return "LEVEL";
    if (n.contains("delay") || n.contains("echo")) return "MIX";
    if (n.contains("reverb") || n.contains("plate") || n.contains("spring") || n.contains("hall")) return "MIX";
    if (n.contains("chorus") || n.contains("flanger") || n.contains("phaser")
        || n.contains("tremolo") || n.contains("vibrato")) return "MIX";
    if (n.contains("tonestack") || n.contains("amp") || n.contains("eq")) return "TREBLE";
    if (n.contains("cabinet") || n.contains("cab")) return "HI CUT";
    if (n.contains("pitch") || n.contains("octav") || n.contains("whammy")) return "MIX";
    if (n.contains("wah") || n.contains("filter")) return "MIX";
    return "MIX";
}


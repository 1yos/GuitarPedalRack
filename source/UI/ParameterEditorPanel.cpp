#include "ParameterEditorPanel.h"
#include "../PluginProcessor.h"

//==============================================================================
// Parameter Info Structure
struct ParameterInfo
{
    juce::String name;
    juce::String label;
    float min;
    float max;
    float defaultValue;
    juce::String suffix;
};

//==============================================================================
// Parameter Definitions for Each Effect Type
static juce::Array<ParameterInfo> getParameterInfoForEffect(const juce::String& effectName)
{
    juce::Array<ParameterInfo> params;
    
    // Normalize effect name for comparison
    juce::String normalized = effectName.toLowerCase().removeCharacters(" -");
    
    // DRIVE / OVERDRIVE EFFECTS
    if (normalized.contains("tubescreamer") || normalized.contains("ts808") || 
        normalized.contains("ts9") || normalized.contains("tubedrive") ||
        normalized.contains("tubeover"))
    {
        params.add({"drive", "DRIVE", 0.0f, 1.0f, 0.5f, ""});
        params.add({"tone", "TONE", 0.0f, 1.0f, 0.5f, ""});
        params.add({"level", "LEVEL", 0.0f, 1.0f, 0.7f, ""});
    }
    else if (normalized.contains("klon") || normalized.contains("centaur"))
    {
        params.add({"gain", "GAIN", 0.0f, 1.0f, 0.5f, ""});
        params.add({"treble", "TREBLE", 0.0f, 1.0f, 0.5f, ""});
        params.add({"output", "OUTPUT", 0.0f, 1.0f, 0.7f, ""});
    }
    else if (normalized.contains("blues") || normalized.contains("bd2"))
    {
        params.add({"drive", "DRIVE", 0.0f, 1.0f, 0.5f, ""});
        params.add({"tone", "TONE", 0.0f, 1.0f, 0.5f, ""});
        params.add({"level", "LEVEL", 0.0f, 1.0f, 0.7f, ""});
    }
    else if (normalized.contains("overdrive"))
    {
        params.add({"drive", "DRIVE", 0.0f, 1.0f, 0.5f, ""});
        params.add({"tone", "TONE", 0.0f, 1.0f, 0.5f, ""});
        params.add({"level", "LEVEL", 0.0f, 1.0f, 0.7f, ""});
    }
    
    // DISTORTION EFFECTS
    else if (normalized.contains("ds1") || normalized.contains("rat") || 
             normalized.contains("distortion") || normalized.contains("metal"))
    {
        params.add({"distortion", "DIST", 0.0f, 1.0f, 0.5f, ""});
        params.add({"tone", "TONE", 0.0f, 1.0f, 0.5f, ""});
        params.add({"level", "LEVEL", 0.0f, 1.0f, 0.7f, ""});
    }
    
    // FUZZ EFFECTS
    else if (normalized.contains("fuzz") || normalized.contains("muff"))
    {
        params.add({"fuzz", "FUZZ", 0.0f, 1.0f, 0.6f, ""});
        params.add({"tone", "TONE", 0.0f, 1.0f, 0.5f, ""});
        params.add({"volume", "VOLUME", 0.0f, 1.0f, 0.7f, ""});
    }
    
    // MODULATION EFFECTS
    else if (normalized.contains("chorus"))
    {
        params.add({"rate", "RATE", 0.1f, 10.0f, 2.0f, " Hz"});
        params.add({"depth", "DEPTH", 0.0f, 1.0f, 0.5f, ""});
        params.add({"mix", "MIX", 0.0f, 1.0f, 0.5f, ""});
    }
    else if (normalized.contains("flanger"))
    {
        params.add({"rate", "RATE", 0.1f, 10.0f, 0.5f, " Hz"});
        params.add({"depth", "DEPTH", 0.0f, 1.0f, 0.7f, ""});
        params.add({"feedback", "FDBK", 0.0f, 1.0f, 0.5f, ""});
    }
    else if (normalized.contains("phaser"))
    {
        params.add({"rate", "RATE", 0.1f, 10.0f, 1.0f, " Hz"});
        params.add({"depth", "DEPTH", 0.0f, 1.0f, 0.6f, ""});
        params.add({"feedback", "FDBK", 0.0f, 1.0f, 0.4f, ""});
    }
    else if (normalized.contains("tremolo"))
    {
        params.add({"rate", "RATE", 0.5f, 20.0f, 4.0f, " Hz"});
        params.add({"depth", "DEPTH", 0.0f, 1.0f, 0.5f, ""});
        params.add({"shape", "SHAPE", 0.0f, 1.0f, 0.0f, ""});
    }
    else if (normalized.contains("vibrato"))
    {
        params.add({"rate", "RATE", 0.5f, 15.0f, 5.0f, " Hz"});
        params.add({"depth", "DEPTH", 0.0f, 1.0f, 0.4f, ""});
        params.add({"mix", "MIX", 0.0f, 1.0f, 1.0f, ""});
    }
    else if (normalized.contains("ring"))
    {
        params.add({"frequency", "FREQ", 20.0f, 5000.0f, 400.0f, " Hz"});
        params.add({"mix", "MIX", 0.0f, 1.0f, 0.5f, ""});
        params.add({"tone", "TONE", 0.0f, 1.0f, 0.5f, ""});
    }
    
    // DELAY EFFECTS
    else if (normalized.contains("delay"))
    {
        params.add({"time", "TIME", 10.0f, 2000.0f, 500.0f, " ms"});
        params.add({"feedback", "FDBK", 0.0f, 1.0f, 0.3f, ""});
        params.add({"mix", "MIX", 0.0f, 1.0f, 0.4f, ""});
    }
    else if (normalized.contains("echo"))
    {
        params.add({"time", "TIME", 50.0f, 2000.0f, 400.0f, " ms"});
        params.add({"feedback", "FDBK", 0.0f, 1.0f, 0.5f, ""});
        params.add({"mix", "MIX", 0.0f, 1.0f, 0.5f, ""});
    }
    
    // REVERB EFFECTS
    else if (normalized.contains("reverb"))
    {
        params.add({"size", "SIZE", 0.0f, 1.0f, 0.5f, ""});
        params.add({"decay", "DECAY", 0.0f, 1.0f, 0.5f, ""});
        params.add({"mix", "MIX", 0.0f, 1.0f, 0.3f, ""});
    }
    
    // FILTER EFFECTS
    else if (normalized.contains("wah"))
    {
        params.add({"frequency", "FREQ", 300.0f, 2500.0f, 800.0f, " Hz"});
        params.add({"resonance", "Q", 0.5f, 10.0f, 4.0f, ""});
        params.add({"mix", "MIX", 0.0f, 1.0f, 1.0f, ""});
    }
    else if (normalized.contains("filter"))
    {
        params.add({"cutoff", "CUTOFF", 20.0f, 20000.0f, 1000.0f, " Hz"});
        params.add({"resonance", "Q", 0.1f, 10.0f, 1.0f, ""});
        params.add({"mix", "MIX", 0.0f, 1.0f, 1.0f, ""});
    }
    
    // DYNAMICS EFFECTS
    else if (normalized.contains("compressor"))
    {
        params.add({"threshold", "THRESH", -60.0f, 0.0f, -20.0f, " dB"});
        params.add({"ratio", "RATIO", 1.0f, 20.0f, 4.0f, ":1"});
        params.add({"attack", "ATTACK", 0.1f, 100.0f, 10.0f, " ms"});
    }
    else if (normalized.contains("gate") || normalized.contains("noise"))
    {
        params.add({"threshold", "THRESH", -80.0f, 0.0f, -40.0f, " dB"});
        params.add({"attack", "ATTACK", 0.1f, 50.0f, 1.0f, " ms"});
        params.add({"release", "RELEASE", 10.0f, 1000.0f, 100.0f, " ms"});
    }
    else if (normalized.contains("limiter"))
    {
        params.add({"threshold", "THRESH", -20.0f, 0.0f, -3.0f, " dB"});
        params.add({"release", "RELEASE", 10.0f, 1000.0f, 100.0f, " ms"});
        params.add({"output", "OUTPUT", -20.0f, 20.0f, 0.0f, " dB"});
    }
    
    // EQ / UTILITY
    else if (normalized.contains("eq"))
    {
        params.add({"low", "LOW", -12.0f, 12.0f, 0.0f, " dB"});
        params.add({"mid", "MID", -12.0f, 12.0f, 0.0f, " dB"});
        params.add({"high", "HIGH", -12.0f, 12.0f, 0.0f, " dB"});
    }
    
    // AMP / CABINET
    else if (normalized.contains("amp"))
    {
        params.add({"gain", "GAIN", 0.0f, 1.0f, 0.5f, ""});
        params.add({"bass", "BASS", 0.0f, 1.0f, 0.5f, ""});
        params.add({"middle", "MID", 0.0f, 1.0f, 0.5f, ""});
    }
    else if (normalized.contains("cabinet") || normalized.contains("cab"))
    {
        params.add({"mix", "MIX", 0.0f, 1.0f, 1.0f, ""});
        params.add({"lowcut", "LOW CUT", 20.0f, 500.0f, 80.0f, " Hz"});
        params.add({"highcut", "HI CUT", 2000.0f, 20000.0f, 8000.0f, " Hz"});
    }
    
    // PITCH EFFECTS
    else if (normalized.contains("octaver") || normalized.contains("pitch"))
    {
        params.add({"pitch", "PITCH", -24.0f, 24.0f, 0.0f, " st"});
        params.add({"mix", "MIX", 0.0f, 1.0f, 0.5f, ""});
        params.add({"tone", "TONE", 0.0f, 1.0f, 0.5f, ""});
    }
    else if (normalized.contains("harmonizer"))
    {
        params.add({"interval", "INTERVAL", -12.0f, 12.0f, 5.0f, " st"});
        params.add({"mix", "MIX", 0.0f, 1.0f, 0.5f, ""});
        params.add({"detune", "DETUNE", 0.0f, 50.0f, 0.0f, " ct"});
    }
    
    return params;
}

//==============================================================================

ParameterEditorPanel::ParameterEditorPanel()
{
    // Title (effect name)
    addAndMakeVisible(titleLabel);
    titleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.95f));
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    
    // Category (subtitle)
    addAndMakeVisible(categoryLabel);
    categoryLabel.setFont(juce::Font(11.0f));
    categoryLabel.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.6f));
    categoryLabel.setJustificationType(juce::Justification::centredLeft);
    
    // Close button (top right corner)
    addAndMakeVisible(closeButton);
    closeButton.setButtonText("✕");
    closeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    closeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white.withAlpha(0.7f));
    closeButton.onClick = [this]()
    {
        if (onClose)
            onClose();
    };
    
    // Bypass button (simple toggle)
    addAndMakeVisible(bypassButton);
    bypassButton.setButtonText("BYPASS");
    bypassButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff2a2a2a));
    bypassButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xffE74C3C));
    bypassButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white.withAlpha(0.8f));
    bypassButton.setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    bypassButton.setClickingTogglesState(true);
    bypassButton.onClick = [this]()
    {
        if (onBypassToggled)
            onBypassToggled(bypassButton.getToggleState());
        updateBypassButton();
    };
}

void ParameterEditorPanel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    
    // Dark professional background (matching reference)
    g.setColour(juce::Colour(0xff0e0e0e));
    g.fillAll();
    
    // Subtle top border color matching the category
    auto categoryColor = getCategoryColor();
    g.setColour(categoryColor.withAlpha(0.5f));
    g.fillRect(0, 0, getWidth(), 3);
    
    // If no effect selected, show message
    if (!currentEffect)
    {
        g.setFont(juce::Font(15.0f));
        g.setColour(juce::Colours::white.withAlpha(0.35f));
        g.drawText("No effect selected", bounds, juce::Justification::centred);
        
        g.setFont(juce::Font(11.0f));
        g.setColour(juce::Colours::white.withAlpha(0.25f));
        g.drawText("Select a pedal on the board to configure parameters", 
                  bounds.translated(0, 25), juce::Justification::centred);
        return;
    }
    
    // ============ LEFT PANEL: MINI PEDAL PREVIEW ============
    auto miniPedalBounds = juce::Rectangle<float>(25.0f, 15.0f, 55.0f, 100.0f);
    g.setColour(categoryColor.darker(0.3f));
    g.fillRoundedRectangle(miniPedalBounds, 4.0f);
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawRoundedRectangle(miniPedalBounds, 4.0f, 1.2f);
    
    // Title inside mini pedal
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.setColour(categoryColor.getBrightness() < 0.5f ? juce::Colours::white.withAlpha(0.8f) : juce::Colours::black.withAlpha(0.8f));
    g.drawText(effectName.substring(0, 4).toUpperCase(), miniPedalBounds.removeFromTop(20), juce::Justification::centred);
    
    // Mini LED
    bool isBypassed = bypassButton.getToggleState();
    g.setColour(isBypassed ? categoryColor.darker(2.0f) : categoryColor.brighter(1.0f));
    g.fillEllipse(miniPedalBounds.getCentreX() - 3.5f, 40.0f, 7.0f, 7.0f);
    
    // Mini footswitch
    g.setColour(juce::Colour(0xff888888));
    g.fillEllipse(miniPedalBounds.getCentreX() - 8.0f, 82.0f, 16.0f, 16.0f);
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.drawEllipse(miniPedalBounds.getCentreX() - 8.0f, 82.0f, 16.0f, 16.0f, 0.8f);
    
    // ON Light Indicator
    auto onBounds = juce::Rectangle<float>(22.0f, 128.0f, 60.0f, 22.0f);
    bool isOn = !isBypassed;
    g.setColour(juce::Colour(0xff161616));
    g.fillRoundedRectangle(onBounds, 3.0f);
    g.setColour(juce::Colours::white.withAlpha(0.08f));
    g.drawRoundedRectangle(onBounds, 3.0f, 1.0f);
    
    auto bulb = juce::Rectangle<float>(28.0f, 135.0f, 8.0f, 8.0f);
    g.setColour(isOn ? juce::Colour(0xff39ff14) : juce::Colour(0xff114400));
    g.fillEllipse(bulb);
    if (isOn)
    {
        g.setColour(juce::Colour(0xff39ff14).withAlpha(0.25f));
        g.fillEllipse(bulb.expanded(1.5f));
    }
    
    g.setFont(juce::Font(9.0f, juce::Font::bold));
    g.setColour(isOn ? juce::Colours::white : juce::Colours::white.withAlpha(0.4f));
    g.drawText("ON", onBounds.translated(12.0f, 0.0f), juce::Justification::centred);
    
    // ============ CENTER-LEFT: DROPDOWN DECORATION ============
    auto dropdownBounds = juce::Rectangle<float>(105.0f, 45.0f, 120.0f, 25.0f);
    g.setColour(juce::Colour(0xff161616));
    g.fillRoundedRectangle(dropdownBounds, 3.0f);
    g.setColour(juce::Colours::white.withAlpha(0.08f));
    g.drawRoundedRectangle(dropdownBounds, 3.0f, 1.0f);
    
    g.setFont(juce::Font(10.0f));
    g.setColour(juce::Colours::white.withAlpha(0.8f));
    juce::String modelText = "Default Mode";
    if (effectCategory.contains("Comp")) modelText = "Studio Comp";
    else if (effectCategory.contains("Drive")) modelText = "Vintage OD";
    else if (effectCategory.contains("Delay")) modelText = "Digital Delay";
    else if (effectCategory.contains("Reverb")) modelText = "Plate Reverb";
    else if (effectCategory.contains("EQ")) modelText = "Graphic EQ";
    g.drawText(modelText, dropdownBounds.reduced(8, 0), juce::Justification::centredLeft);
    
    g.setColour(juce::Colours::white.withAlpha(0.4f));
    g.drawText("▼", dropdownBounds.removeFromRight(15.0f), juce::Justification::centred);
    
    // ============ CENTER-RIGHT: VISUALIZER GRAPH ============
    auto graphBounds = juce::Rectangle<float>(getWidth() - 390.0f, 15.0f, 220.0f, 135.0f);
    
    // Background and grid
    g.setColour(juce::Colour(0xff050505));
    g.fillRect(graphBounds);
    
    g.setColour(juce::Colour(0xff181818));
    g.drawRect(graphBounds, 1.2f);
    
    // Grid lines every 30px
    g.setColour(juce::Colour(0xff152515));
    for (float gx = graphBounds.getX() + 36.6f; gx < graphBounds.getRight(); gx += 36.6f)
        g.drawVerticalLine((int)gx, graphBounds.getY(), graphBounds.getBottom());
    for (float gy = graphBounds.getY() + 27.0f; gy < graphBounds.getBottom(); gy += 27.0f)
        g.drawHorizontalLine((int)gy, graphBounds.getX(), graphBounds.getRight());
    
    // Labeled dB ticks on left
    g.setFont(juce::Font(7.0f));
    g.setColour(juce::Colours::white.withAlpha(0.35f));
    g.drawText("0", graphBounds.getX() - 12.0f, graphBounds.getY() - 4.0f, 10.0f, 8.0f, juce::Justification::centredRight);
    g.drawText("-12", graphBounds.getX() - 15.0f, graphBounds.getY() + 27.0f - 4.0f, 13.0f, 8.0f, juce::Justification::centredRight);
    g.drawText("-24", graphBounds.getX() - 15.0f, graphBounds.getY() + 54.0f - 4.0f, 13.0f, 8.0f, juce::Justification::centredRight);
    g.drawText("-36", graphBounds.getX() - 15.0f, graphBounds.getY() + 81.0f - 4.0f, 13.0f, 8.0f, juce::Justification::centredRight);
    g.drawText("-48", graphBounds.getX() - 15.0f, graphBounds.getBottom() - 8.0f, 13.0f, 8.0f, juce::Justification::centredRight);
    
    // Plot curve (Transfer Curve for Compressor, or EQ response, or default wave)
    juce::Path plot;
    bool isCompressor = effectCategory.contains("Comp") || effectName.contains("Komp");
    
    if (isCompressor)
    {
        // Read threshold and ratio values
        float thresholdVal = -20.0f;
        float ratioVal = 4.0f;
        
        for (auto* k : paramKnobs)
        {
            if (k->getLabel().contains("THRESH")) thresholdVal = k->getValue();
            else if (k->getLabel().contains("RATIO")) ratioVal = k->getValue();
        }
        
        float w = graphBounds.getWidth();
        float h = graphBounds.getHeight();
        
        for (float x = 0.0f; x <= w; x += 2.0f)
        {
            float inDb = -48.0f + (x / w) * 48.0f;
            float outDb = inDb;
            
            if (inDb > thresholdVal)
            {
                outDb = thresholdVal + (inDb - thresholdVal) / ratioVal;
            }
            
            float y = h - ((outDb - (-48.0f)) / 48.0f) * h;
            y = juce::jlimit(0.0f, h, y);
            
            if (x == 0.0f)
                plot.startNewSubPath(graphBounds.getX(), graphBounds.getY() + y);
            else
                plot.lineTo(graphBounds.getX() + x, graphBounds.getY() + y);
        }
    }
    else
    {
        // Draw static stylized audio wave (default visualizer)
        float w = graphBounds.getWidth();
        float h = graphBounds.getHeight();
        plot.startNewSubPath(graphBounds.getX(), graphBounds.getCentreY());
        for (float x = 0.0f; x <= w; x += 3.0f)
        {
            float sine = std::sin(x * 0.08f) * std::cos(x * 0.02f);
            plot.lineTo(graphBounds.getX() + x, graphBounds.getCentreY() + sine * (h * 0.35f));
        }
    }
    
    g.setColour(juce::Colour(0xff39ff14)); // Glowing neon green curve
    g.strokePath(plot, juce::PathStrokeType(1.8f));
    
    // ============ RIGHT PANEL: IN/GR/OUT METERS ============
    auto metersBounds = juce::Rectangle<float>(getWidth() - 145.0f, 15.0f, 125.0f, 135.0f);
    
    // Background slot
    g.setColour(juce::Colour(0xff050505));
    g.fillRoundedRectangle(metersBounds, 3.0f);
    g.setColour(juce::Colour(0xff1a1a1a));
    g.drawRoundedRectangle(metersBounds, 3.0f, 1.0f);
    
    // Text labels
    g.setFont(juce::Font(8.0f, juce::Font::bold));
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawText("IN", metersBounds.getX() + 5, metersBounds.getY() + 5, 30, 10, juce::Justification::centred);
    g.drawText("GR", metersBounds.getX() + 45, metersBounds.getY() + 5, 30, 10, juce::Justification::centred);
    g.drawText("OUT", metersBounds.getX() + 85, metersBounds.getY() + 5, 30, 10, juce::Justification::centred);
    
    // Get peak levels
    float peakIn = 0.0f;
    float peakOut = 0.0f;
    float gainRed = 0.0f;
    
    if (processor != nullptr)
    {
        peakIn = processor->getInputLevel();
        peakOut = processor->getOutputLevel();
    }
    
    if (isCompressor && currentEffect != nullptr)
    {
        // Try dynamic cast to Compressor
        auto* comp = dynamic_cast<Compressor*>(currentEffect);
        if (comp != nullptr)
        {
            gainRed = comp->getGainReduction(); // Returns absolute positive dB reduction (e.g. 6.0 for -6dB)
        }
    }
    
    // Draw meters
    auto drawMeterColumn = [&](float centerX, float val, bool isGR)
    {
        float startY = metersBounds.getY() + 20.0f;
        float height = 100.0f;
        float width = 6.0f;
        int segments = 10;
        float segHeight = height / segments;
        
        for (int i = 0; i < segments; ++i)
        {
            auto segment = juce::Rectangle<float>(
                centerX - width * 0.5f,
                isGR ? (startY + i * segHeight + 1.0f) : (startY + (segments - 1 - i) * segHeight + 1.0f),
                width,
                segHeight - 1.5f
            );
            
            bool active = false;
            if (isGR)
            {
                // Gain reduction active downward (0dB to 24dB)
                float reductionDbThreshold = i * (24.0f / segments);
                active = gainRed > reductionDbThreshold && gainRed > 0.1f;
            }
            else
            {
                float lvlThreshold = i / (float)segments;
                active = val > lvlThreshold;
            }
            
            juce::Colour segCol;
            if (active)
            {
                if (isGR)
                    segCol = (i > 7) ? juce::Colours::red : juce::Colours::orange;
                else
                    segCol = (i > 8) ? juce::Colours::red : ((i > 6) ? juce::Colours::orange : juce::Colour(0xff39ff14));
            }
            else
            {
                segCol = juce::Colour(0xff112211); // Dim/inactive
            }
            
            g.setColour(segCol);
            g.fillRoundedRectangle(segment, 1.0f);
            
            if (active)
            {
                g.setColour(segCol.withAlpha(0.2f));
                g.fillRoundedRectangle(segment.expanded(1.5f), 1.5f);
            }
        }
    };
    
    drawMeterColumn(metersBounds.getX() + 20.0f, peakIn, false);
    drawMeterColumn(metersBounds.getX() + 60.0f, gainRed, true);
    drawMeterColumn(metersBounds.getX() + 100.0f, peakOut, false);
}

void ParameterEditorPanel::resized()
{
    auto bounds = getLocalBounds();
    
    if (!currentEffect)
        return;
    
    // Top-left label and dropdown
    titleLabel.setBounds(105, 15, 130, 25);
    
    // Hide standard bypass button visually but keep it responsive at (22, 128, 60, 22) overlaid on ON light
    bypassButton.setBounds(22, 128, 60, 22);
    bypassButton.setButtonText(""); // Hide text
    
    // Parameters knobs section
    if (paramKnobs.isEmpty())
        return;
    
    int numKnobs = paramKnobs.size();
    int startX = 240;
    int knobWidth = 72;
    int knobSpacing = 16;
    int knobY = 22;
    
    for (int i = 0; i < numKnobs; ++i)
    {
        paramKnobs[i]->setBounds(startX + i * (knobWidth + knobSpacing), knobY, knobWidth, 110);
    }
}

void ParameterEditorPanel::setEffect(AudioModule* effect, const juce::String& name, const juce::String& category)
{
    currentEffect = effect;
    effectName = name;
    effectCategory = category;
    
    titleLabel.setText(effectName.toUpperCase(), juce::dontSendNotification);
    categoryLabel.setText(category, juce::dontSendNotification);
    
    createParameterControls();
    updateBypassButton();
    
    resized();
    repaint();
}

void ParameterEditorPanel::clearEffect()
{
    currentEffect = nullptr;
    effectName = "";
    effectCategory = "";
    
    paramKnobs.clear();
    
    titleLabel.setText("", juce::dontSendNotification);
    categoryLabel.setText("", juce::dontSendNotification);
    
    repaint();
}

void ParameterEditorPanel::createParameterControls()
{
    // Clear existing knobs
    paramKnobs.clear();
    
    if (!currentEffect)
        return;
    
    // Get parameter info for this effect
    auto params = getParameterInfoForEffect(effectName);
    
    if (params.isEmpty())
    {
        DBG("No parameters found for effect: " + effectName);
        return;
    }
    
    // Create a knob for each parameter
    for (const auto& param : params)
    {
        auto* knob = paramKnobs.add(new VintageKnob(param.label, getCategoryColor()));
        addAndMakeVisible(knob);
        
        // Set range and initial value
        knob->setRange(param.min, param.max);
        knob->setValue(param.defaultValue, false);
        knob->setValueSuffix(param.suffix);
        
        // Connect to the effect's parameter setter
        knob->onValueChange = [this, paramName = param.name](float value)
        {
            setEffectParameter(paramName, value);
        };
    }
}

void ParameterEditorPanel::updateBypassButton()
{
    if (currentEffect)
    {
        bypassButton.setToggleState(currentEffect->isBypassed(), juce::dontSendNotification);
    }
}

juce::Colour ParameterEditorPanel::getCategoryColor() const
{
    if (effectCategory == "Drive" || effectCategory == "Overdrive") return juce::Colour(0xffD4A349);
    if (effectCategory == "Distortion" || effectCategory == "Fuzz") return juce::Colour(0xff8B3A3A);
    if (effectCategory == "Modulation" || effectCategory == "Chorus") return juce::Colour(0xff6B7C4E);
    if (effectCategory == "Delay") return juce::Colour(0xff3A7A7A);
    if (effectCategory == "Reverb") return juce::Colour(0xff5A4A7A);
    if (effectCategory == "Filter" || effectCategory == "EQ") return juce::Colour(0xff00E5FF);
    if (effectCategory == "Dynamics" || effectCategory == "Compressor") return juce::Colour(0xff4A7A4A);
    if (effectCategory == "Cabinet" || effectCategory == "Amp") return juce::Colour(0xff7A6A4A);
    return juce::Colour(0xff00E5FF);
}

void ParameterEditorPanel::setEffectParameter(const juce::String& paramName, float value)
{
    if (!currentEffect)
        return;
    
    // Normalize parameter name for comparison
    juce::String normalized = paramName.toLowerCase().removeCharacters(" -_");
    
    // Try to cast to specific effect types and call their setter methods
    // This is a simple approach - each effect type has its own setter methods
    
    // DRIVE / OVERDRIVE
    if (auto* ts = dynamic_cast<TubeScreamer808*>(currentEffect))
    {
        if (normalized == "drive") ts->setDrive(value);
        else if (normalized == "tone") ts->setTone(value);
        else if (normalized == "level") ts->setLevel(value);
    }
    else if (auto* ts = dynamic_cast<TubeScreamerTS9*>(currentEffect))
    {
        if (normalized == "drive") ts->setDrive(value);
        else if (normalized == "tone") ts->setTone(value);
        else if (normalized == "level") ts->setLevel(value);
    }
    else if (auto* od = dynamic_cast<TubeOverdrive*>(currentEffect))
    {
        if (normalized == "drive") od->setDrive(value);
        else if (normalized == "tone") od->setTone(value);
        else if (normalized == "level") od->setLevel(value);
    }
    
    // DISTORTION
    else if (auto* dist = dynamic_cast<Distortion*>(currentEffect))
    {
        if (normalized == "distortion" || normalized == "dist") dist->setDrive(value);
        else if (normalized == "tone") dist->setTone(value);
        else if (normalized == "level") dist->setLevel(value);
    }
    
    // MODULATION
    else if (auto* chorus = dynamic_cast<Chorus*>(currentEffect))
    {
        if (normalized == "rate") chorus->setRate(value);
        else if (normalized == "depth") chorus->setDepth(value);
        else if (normalized == "mix") chorus->setMix(value);
    }
    
    // TIME EFFECTS
    else if (auto* delay = dynamic_cast<Delay*>(currentEffect))
    {
        if (normalized == "time") delay->setTime(value);
        else if (normalized == "feedback" || normalized == "fdbk") delay->setFeedback(value);
        else if (normalized == "mix") delay->setMix(value);
    }
    // Note: Reverb is juce::Reverb (built-in), not our custom class
    
    // DYNAMICS
    else if (auto* comp = dynamic_cast<Compressor*>(currentEffect))
    {
        if (normalized == "threshold" || normalized == "thresh") comp->setThreshold(value);
        else if (normalized == "ratio") comp->setRatio(value);
        else if (normalized == "attack") comp->setAttack(value);
    }
    else if (auto* gate = dynamic_cast<NoiseGate*>(currentEffect))
    {
        if (normalized == "threshold" || normalized == "thresh") gate->setThreshold(value);
        else if (normalized == "attack") gate->setAttack(value);
        else if (normalized == "release") gate->setRelease(value);
    }
    
    // UTILITY
    else if (auto* eq = dynamic_cast<EQ*>(currentEffect))
    {
        if (normalized == "low") eq->setBandGain(0, value);
        else if (normalized == "mid" || normalized == "middle") eq->setBandGain(1, value);
        else if (normalized == "high") eq->setBandGain(2, value);
    }
    
    // AMP / CABINET
    else if (auto* amp = dynamic_cast<AmpSimulator*>(currentEffect))
    {
        if (normalized == "gain") amp->setGain(value);
        else if (normalized == "bass") amp->setBass(value);
        else if (normalized == "middle" || normalized == "mid") amp->setMid(value);
    }
    // Note: CabinetIR doesn't have setMix method
    
    DBG("Parameter changed: " + paramName + " = " + juce::String(value));
}

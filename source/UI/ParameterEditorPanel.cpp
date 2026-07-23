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
    juce::String n = effectName.toLowerCase().removeCharacters(" -_");

    // â”€â”€ DRIVE / OVERDRIVE â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    if (n.contains("tubescreamer") || n.contains("ts808") || n.contains("ts9") || n.contains("tubeover"))
    {
        params.add({"drive","DRIVE",0.0f,1.0f,0.5f,""});
        params.add({"tone","TONE",0.0f,1.0f,0.5f,""});
        params.add({"level","LEVEL",0.0f,1.0f,0.7f,""});
    }
    else if (n.contains("klon"))
    {
        params.add({"gain","GAIN",0.0f,1.0f,0.5f,""});
        params.add({"treble","TREBLE",0.0f,1.0f,0.5f,""});
        params.add({"output","OUTPUT",0.0f,1.0f,0.7f,""});
    }
    else if (n.contains("timmy"))
    {
        params.add({"gain","GAIN",0.0f,1.0f,0.4f,""});
        params.add({"bass","BASS",0.0f,1.0f,0.5f,""});
        params.add({"treble","TREBLE",0.0f,1.0f,0.5f,""});
        params.add({"volume","VOLUME",0.0f,1.0f,0.8f,""});
    }
    else if (n.contains("bluesdriver") || n.contains("bd2"))
    {
        params.add({"gain","GAIN",0.0f,1.0f,0.5f,""});
        params.add({"tone","TONE",0.0f,1.0f,0.5f,""});
        params.add({"level","LEVEL",0.0f,1.0f,0.7f,""});
    }
    else if (n.contains("metalzone") || n.contains("mt2"))
    {
        params.add({"distortion","DIST",0.0f,1.0f,0.7f,""});
        params.add({"treble","TREBLE",0.0f,1.0f,0.5f,""});
        params.add({"bass","BASS",0.0f,1.0f,0.5f,""});
        params.add({"level","LEVEL",0.0f,1.0f,0.7f,""});
    }
    else if (n.contains("procor") || n.contains("rat"))
    {
        params.add({"distortion","DIST",0.0f,1.0f,0.5f,""});
        params.add({"filter","FILTER",0.0f,1.0f,0.5f,""});
        params.add({"volume","VOLUME",0.0f,1.0f,0.7f,""});
    }
    else if (n.contains("bossds1") || n.contains("ds1"))
    {
        params.add({"dist","DIST",0.0f,1.0f,0.5f,""});
        params.add({"tone","TONE",0.0f,1.0f,0.5f,""});
        params.add({"level","LEVEL",0.0f,1.0f,0.7f,""});
    }
    else if (n.contains("bosssd1") || n.contains("sd1"))
    {
        params.add({"drive","DRIVE",0.0f,1.0f,0.5f,""});
        params.add({"tone","TONE",0.0f,1.0f,0.5f,""});
        params.add({"level","LEVEL",0.0f,1.0f,0.7f,""});
    }
    else if (n.contains("bigmuff") || n.contains("muff"))
    {
        params.add({"sustain","SUSTAIN",0.0f,1.0f,0.6f,""});
        params.add({"tone","TONE",0.0f,1.0f,0.5f,""});
        params.add({"volume","VOLUME",0.0f,1.0f,0.7f,""});
    }
    else if (n.contains("fuzz"))
    {
        params.add({"fuzz","FUZZ",0.0f,1.0f,0.6f,""});
        params.add({"volume","VOLUME",0.0f,1.0f,0.7f,""});
    }
    else if (n.contains("distortion"))
    {
        params.add({"distortion","DIST",0.0f,1.0f,0.5f,""});
        params.add({"tone","TONE",0.0f,1.0f,0.5f,""});
        params.add({"level","LEVEL",0.0f,1.0f,0.7f,""});
    }
    else if (n.contains("overdrive"))
    {
        params.add({"drive","DRIVE",0.0f,1.0f,0.5f,""});
        params.add({"tone","TONE",0.0f,1.0f,0.5f,""});
        params.add({"level","LEVEL",0.0f,1.0f,0.7f,""});
    }
    
    // â”€â”€ MODULATION â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (n.contains("chorus"))
    {
        params.add({"rate", "RATE", 0.1f, 10.0f, 2.0f, " Hz"});
        params.add({"depth", "DEPTH", 0.0f, 1.0f, 0.5f, ""});
        params.add({"mix", "MIX", 0.0f, 1.0f, 0.5f, ""});
    }
    else if (n.contains("flanger"))
    {
        params.add({"rate", "RATE", 0.1f, 10.0f, 0.5f, " Hz"});
        params.add({"depth", "DEPTH", 0.0f, 1.0f, 0.7f, ""});
        params.add({"feedback", "FDBK", 0.0f, 1.0f, 0.5f, ""});
        params.add({"mix","MIX",0.0f,1.0f,0.5f,""});
    }
    else if (n.contains("phaser"))
    {
        params.add({"rate", "RATE", 0.1f, 10.0f, 1.0f, " Hz"});
        params.add({"depth", "DEPTH", 0.0f, 1.0f, 0.6f, ""});
        params.add({"feedback", "FDBK", 0.0f, 1.0f, 0.4f, ""});
    }
    else if (n.contains("tremolo"))
    {
        params.add({"rate", "RATE", 0.5f, 20.0f, 4.0f, " Hz"});
        params.add({"depth", "DEPTH", 0.0f, 1.0f, 0.5f, ""});
    }
    else if (n.contains("vibrato"))
    {
        params.add({"rate", "RATE", 0.5f, 15.0f, 5.0f, " Hz"});
        params.add({"depth", "DEPTH", 0.0f, 1.0f, 0.4f, ""});
    }
    else if (n.contains("rotary"))
    {
        params.add({"speed","SPEED",0.0f,1.0f,0.5f,""});
        params.add({"separation","SEP",0.0f,1.0f,0.6f,""});
    }
    else if (n.contains("ring"))
    {
        params.add({"frequency", "FREQ", 20.0f, 5000.0f, 400.0f, " Hz"});
        params.add({"mix", "MIX", 0.0f, 1.0f, 0.5f, ""});
    }
    
    // â”€â”€ DELAY â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (n.contains("pingpong"))
    {
        params.add({"time","TIME",0.0f,1.0f,0.5f,""});
        params.add({"feedback","FDBK",0.0f,1.0f,0.4f,""});
        params.add({"width","WIDTH",0.0f,1.0f,1.0f,""});
        params.add({"mix","MIX",0.0f,1.0f,0.4f,""});
    }
    else if (n.contains("tapedelay") || (n.contains("tape") && !n.contains("talkbox")))
    {
        params.add({"time","TIME",0.0f,1.0f,0.4f,""});
        params.add({"feedback","FDBK",0.0f,1.0f,0.5f,""});
        params.add({"wow","WOW",0.0f,1.0f,0.3f,""});
        params.add({"flutter","FLUTTER",0.0f,1.0f,0.2f,""});
        params.add({"mix","MIX",0.0f,1.0f,0.4f,""});
    }
    else if (n.contains("analogdelay"))
    {
        params.add({"time","TIME",0.0f,1.0f,0.35f,""});
        params.add({"feedback","FDBK",0.0f,1.0f,0.5f,""});
        params.add({"modulation","MOD",0.0f,1.0f,0.3f,""});
        params.add({"mix","MIX",0.0f,1.0f,0.5f,""});
    }
    else if (n.contains("delay") || n.contains("echo"))
    {
        params.add({"time","TIME",10.0f,2000.0f,500.0f," ms"});
        params.add({"feedback","FDBK",0.0f,1.0f,0.3f,""});
        params.add({"mix","MIX",0.0f,1.0f,0.4f,""});
    }
    // â”€â”€ REVERB â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (n.contains("spring"))
    {
        params.add({"decay","DECAY",0.0f,1.0f,0.6f,""});
        params.add({"tone","TONE",0.0f,1.0f,0.5f,""});
        params.add({"mix","MIX",0.0f,1.0f,0.4f,""});
    }
    else if (n.contains("reverb") || n.contains("plate") || n.contains("hall") || n.contains("room"))
    {
        params.add({"size","SIZE",0.0f,1.0f,0.5f,""});
        params.add({"decay","DECAY",0.0f,1.0f,0.5f,""});
        params.add({"mix","MIX",0.0f,1.0f,0.3f,""});
    }
    // â”€â”€ FILTER / WAH â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (n.contains("wah") || n.contains("autowah"))
    {
        params.add({"frequency","FREQ",300.0f,2500.0f,800.0f," Hz"});
        params.add({"resonance","Q",0.0f,1.0f,0.7f,""});
        params.add({"mix","MIX",0.0f,1.0f,1.0f,""});
    }
    else if (n.contains("envelopefilter") || n.contains("envelope"))
    {
        params.add({"sensitivity","SENS",0.0f,1.0f,0.7f,""});
        params.add({"attack","ATK",0.0f,1.0f,0.3f,""});
        params.add({"decay","DEC",0.0f,1.0f,0.5f,""});
        params.add({"resonance","Q",0.0f,1.0f,0.6f,""});
    }
    else if (n.contains("talkbox"))
    {
        params.add({"formant","FORMANT",0.0f,1.0f,0.5f,""});
        params.add({"mix","MIX",0.0f,1.0f,0.8f,""});
        params.add({"quality","QUALITY",0.0f,1.0f,0.6f,""});
    }
    else if (n.contains("filter"))
    {
        params.add({"frequency","FREQ",0.0f,1.0f,0.5f,""});
        params.add({"resonance","Q",0.0f,1.0f,0.3f,""});
    }
    
    // â”€â”€ DYNAMICS â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (n.contains("deesser"))
    {
        params.add({"threshold","THRESH",0.0f,1.0f,0.7f,""});
        params.add({"frequency","FREQ",0.0f,1.0f,0.6f,""});
        params.add({"ratio","RATIO",0.0f,1.0f,0.5f,""});
    }
    else if (n.contains("multiband"))
    {
        params.add({"lowthreshold","LO THR",0.0f,1.0f,0.6f,""});
        params.add({"midthreshold","MID THR",0.0f,1.0f,0.6f,""});
        params.add({"highthreshold","HI THR",0.0f,1.0f,0.6f,""});
        params.add({"ratio","RATIO",0.0f,1.0f,0.5f,""});
    }
    else if (n.contains("compressor") || n.contains("optical") || n.contains("vca") || n.contains("fetcomp") || n.contains("tubecomp"))
    {
        params.add({"threshold","THRESH",0.0f,1.0f,0.65f,""});
        params.add({"ratio","RATIO",0.0f,1.0f,0.3f,""});
        params.add({"attack","ATTACK",0.0f,1.0f,0.1f,""});
        params.add({"release","RELEASE",0.0f,1.0f,0.3f,""});
        params.add({"makeup","MAKEUP",0.0f,1.0f,0.4f,""});
    }
    else if (n.contains("limiter"))
    {
        params.add({"threshold","THRESH",0.0f,1.0f,0.8f,""});
        params.add({"release","RELEASE",0.0f,1.0f,0.5f,""});
        params.add({"makeup","MAKEUP",0.0f,1.0f,0.0f,""});
    }
    else if (n.contains("expander"))
    {
        params.add({"threshold","THRESH",0.0f,1.0f,0.3f,""});
        params.add({"ratio","RATIO",0.0f,1.0f,0.5f,""});
        params.add({"attack","ATTACK",0.0f,1.0f,0.3f,""});
        params.add({"release","RELEASE",0.0f,1.0f,0.5f,""});
    }
    else if (n.contains("gate") || n.contains("noise"))
    {
        params.add({"threshold","THRESH",-80.0f,0.0f,-40.0f," dB"});
        params.add({"attack","ATTACK",0.1f,50.0f,1.0f," ms"});
        params.add({"release","RELEASE",10.0f,1000.0f,100.0f," ms"});
    }
    // â”€â”€ EQ / TONESTACK â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (n.contains("fender"))
    {
        params.add({"bass","BASS",0.0f,1.0f,0.5f,""});
        params.add({"mid","MID",0.0f,1.0f,0.5f,""});
        params.add({"treble","TREBLE",0.0f,1.0f,0.5f,""});
    }
    else if (n.contains("marshall"))
    {
        params.add({"bass","BASS",0.0f,1.0f,0.5f,""});
        params.add({"mid","MID",0.0f,1.0f,0.5f,""});
        params.add({"treble","TREBLE",0.0f,1.0f,0.5f,""});
        params.add({"presence","PRESENCE",0.0f,1.0f,0.5f,""});
    }
    else if (n.contains("eq"))
    {
        params.add({"low","LOW",-12.0f,12.0f,0.0f," dB"});
        params.add({"mid","MID",-12.0f,12.0f,0.0f," dB"});
        params.add({"high","HIGH",-12.0f,12.0f,0.0f," dB"});
    }
    // â”€â”€ AMP / CABINET â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (n.contains("amp"))
    {
        params.add({"gain","GAIN",0.0f,1.0f,0.5f,""});
        params.add({"bass","BASS",0.0f,1.0f,0.5f,""});
        params.add({"middle","MID",0.0f,1.0f,0.5f,""});
    }
    else if (n.contains("cabinet") || n.contains("cab"))
    {
        params.add({"mix","MIX",0.0f,1.0f,1.0f,""});
        params.add({"lowcut","LOW CUT",20.0f,500.0f,80.0f," Hz"});
        params.add({"highcut","HI CUT",2000.0f,20000.0f,8000.0f," Hz"});
    }
    // â”€â”€ PITCH â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (n.contains("harmonizer"))
    {
        params.add({"interval","INTERVAL",0.0f,1.0f,0.33f,""});
        params.add({"key","KEY",0.0f,1.0f,0.5f,""});
        params.add({"mix","MIX",0.0f,1.0f,0.5f,""});
    }
    else if (n.contains("octaver") || n.contains("octave"))
    {
        params.add({"octavelevel","OCTAVE",0.0f,1.0f,0.7f,""});
        params.add({"direct","DIRECT",0.0f,1.0f,0.5f,""});
    }
    else if (n.contains("pitch") || n.contains("whammy"))
    {
        params.add({"pitch","PITCH",-12.0f,12.0f,0.0f," st"});
        params.add({"formant","FORMANT",0.0f,1.0f,0.5f,""});
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
    closeButton.setButtonText("âœ•");
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
    g.drawText("â–¼", dropdownBounds.removeFromRight(15.0f), juce::Justification::centred);
    
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
    
    auto params = getParameterInfoForEffect(effectName);
    
    if (params.isEmpty())
    {
        DBG("No parameters found for effect: " + effectName);
        return;
    }
    
    for (const auto& param : params)
    {
        auto* knob = paramKnobs.add(new VintageKnob(param.label, getCategoryColor()));
        addAndMakeVisible(knob);
        
        knob->setRange(param.min, param.max);
        knob->setValueSuffix(param.suffix);
        
        // FIX: Read the actual current value from the effect instead of hardcoded default.
        // Query via the same setter path in reverse — use AudioModule::getParameterValue
        // to pull from APVTS pointer, then fall back to the hardcoded default.
        float currentVal = param.defaultValue;
        {
            // Try to get live value from the parameter pointer attached to the module
            auto* ptr = currentEffect->getParameterPointer(param.name);
            if (ptr != nullptr)
            {
                // The pointer holds the raw APVTS value. Map it back to the knob range
                // using the same normalisable range logic.
                float rawVal = ptr->load();
                // Only use it if it's within the declared range (sanity check)
                if (rawVal >= param.min && rawVal <= param.max)
                    currentVal = rawVal;
            }
        }
        
        knob->setValue(currentVal, false);
        
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
    if (!currentEffect) return;
    juce::String p = paramName.toLowerCase().removeCharacters(" -_");

    // â”€â”€ DRIVE â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    if (auto* e = dynamic_cast<TubeScreamer808*>(currentEffect))
    {
        if (p=="drive") e->setDrive(value);
        else if (p=="tone") e->setTone(value);
        else if (p=="level") e->setLevel(value);
    }
    else if (auto* e = dynamic_cast<TubeScreamerTS9*>(currentEffect))
    {
        if (p=="drive") e->setDrive(value);
        else if (p=="tone") e->setTone(value);
        else if (p=="level") e->setLevel(value);
    }
    else if (auto* e = dynamic_cast<TubeScreamerMini*>(currentEffect))
    {
        if (p=="drive"||p=="overdrive") e->setOverdrive(value);
        else if (p=="tone") e->setTone(value);
    }
    else if (auto* e = dynamic_cast<TubeOverdrive*>(currentEffect))
    {
        if (p=="drive") e->setDrive(value);
        else if (p=="tone") e->setTone(value);
        else if (p=="level") e->setLevel(value);
    }
    else if (auto* e = dynamic_cast<KlonCentaur*>(currentEffect))
    {
        if (p=="gain") e->setGain(value);
        else if (p=="treble") e->setTreble(value);
        else if (p=="output") e->setOutput(value);
    }
    else if (auto* e = dynamic_cast<TimmyOverdrive*>(currentEffect))
    {
        if (p=="gain") e->setGain(value);
        else if (p=="bass") e->setBass(value);
        else if (p=="treble") e->setTreble(value);
        else if (p=="volume") e->setVolume(value);
    }
    else if (auto* e = dynamic_cast<BluesDriver*>(currentEffect))
    {
        if (p=="gain") e->setGain(value);
        else if (p=="tone") e->setTone(value);
        else if (p=="level") e->setLevel(value);
    }
    else if (auto* e = dynamic_cast<MetalZone*>(currentEffect))
    {
        if (p=="distortion"||p=="dist") e->setDistortion(value);
        else if (p=="treble") e->setTreble(value);
        else if (p=="bass") e->setBass(value);
        else if (p=="level") e->setLevel(value);
    }
    else if (auto* e = dynamic_cast<ProCoRAT2*>(currentEffect))
    {
        if (p=="distortion"||p=="dist") e->setDistortion(value);
        else if (p=="filter") e->setFilter(value);
        else if (p=="volume") e->setVolume(value);
    }
    else if (auto* e = dynamic_cast<TurboRAT*>(currentEffect))
    {
        if (p=="distortion"||p=="dist") e->setDistortion(value);
        else if (p=="tone") e->setTone(value);
        else if (p=="volume") e->setVolume(value);
    }
    else if (auto* e = dynamic_cast<ProCoRAT*>(currentEffect))
    {
        if (p=="distortion"||p=="dist") e->setDistortion(value);
        else if (p=="filter") e->setFilter(value);
        else if (p=="volume") e->setVolume(value);
    }
    else if (auto* e = dynamic_cast<BossDS1*>(currentEffect))
    {
        if (p=="dist") e->setDist(value);
        else if (p=="tone") e->setTone(value);
        else if (p=="level") e->setLevel(value);
    }
    else if (auto* e = dynamic_cast<BossSD1*>(currentEffect))
    {
        if (p=="drive") e->setDrive(value);
        else if (p=="tone") e->setTone(value);
        else if (p=="level") e->setLevel(value);
    }
    else if (auto* e = dynamic_cast<BigMuffRussian*>(currentEffect))
    {
        if (p=="sustain") e->setSustain(value);
        else if (p=="tone") e->setTone(value);
        else if (p=="volume") e->setVolume(value);
    }
    else if (auto* e = dynamic_cast<BigMuffPi*>(currentEffect))
    {
        if (p=="sustain") e->setSustain(value);
        else if (p=="tone") e->setTone(value);
        else if (p=="volume") e->setVolume(value);
    }
    else if (auto* e = dynamic_cast<FuzzFaceSilicon*>(currentEffect))
    {
        if (p=="fuzz") e->setFuzz(value);
        else if (p=="volume") e->setVolume(value);
    }
    else if (auto* e = dynamic_cast<FuzzFace*>(currentEffect))
    {
        if (p=="fuzz") e->setFuzz(value);
        else if (p=="volume") e->setVolume(value);
    }
    else if (auto* e = dynamic_cast<Distortion*>(currentEffect))
    {
        if (p=="distortion"||p=="dist") e->setDrive(value);
        else if (p=="tone") e->setTone(value);
        else if (p=="level") e->setLevel(value);
    }
    // â”€â”€ MODULATION â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (auto* e = dynamic_cast<ChorusEnsemble*>(currentEffect))
    {
        if (p=="rate") e->setRate(value);
        else if (p=="depth") e->setDepth(value);
        else if (p=="voices") e->setVoices(value);
        else if (p=="mix") e->setMix(value);
    }
    else if (auto* e = dynamic_cast<Chorus*>(currentEffect))
    {
        if (p=="rate") e->setRate(value);
        else if (p=="depth") e->setDepth(value);
        else if (p=="mix") e->setMix(value);
    }
    else if (auto* e = dynamic_cast<FlangerBasic*>(currentEffect))
    {
        if (p=="rate") e->setRate(value);
        else if (p=="depth") e->setDepth(value);
        else if (p=="feedback") e->setFeedback(value);
        else if (p=="mix") e->setMix(value);
    }
    else if (auto* e = dynamic_cast<Phaser4Stage*>(currentEffect))
    {
        if (p=="rate") e->setRate(value);
        else if (p=="depth") e->setDepth(value);
        else if (p=="feedback") e->setFeedback(value);
    }
    else if (auto* e = dynamic_cast<Phaser90*>(currentEffect))
    {
        if (p=="rate") e->setRate(value);
    }
    else if (auto* e = dynamic_cast<TremoloOptical*>(currentEffect))
    {
        if (p=="rate") e->setRate(value);
        else if (p=="depth") e->setDepth(value);
    }
    else if (auto* e = dynamic_cast<VibratoClassic*>(currentEffect))
    {
        if (p=="rate") e->setRate(value);
        else if (p=="depth") e->setDepth(value);
    }
    else if (auto* e = dynamic_cast<RotarySpeaker*>(currentEffect))
    {
        if (p=="speed") e->setSpeed(value);
        else if (p=="separation"||p=="sep") e->setSeparation(value);
    }
    // â”€â”€ DELAY â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (auto* e = dynamic_cast<PingPongDelay*>(currentEffect))
    {
        if (p=="time") e->setTime(value);
        else if (p=="feedback"||p=="fdbk") e->setFeedback(value);
        else if (p=="width") e->setStereoWidth(value);
        else if (p=="mix") e->setMix(value);
    }
    else if (auto* e = dynamic_cast<TapeDelay*>(currentEffect))
    {
        if (p=="time") e->setTime(value);
        else if (p=="feedback"||p=="fdbk") e->setFeedback(value);
        else if (p=="wow") e->setWow(value);
        else if (p=="flutter") e->setFlutter(value);
        else if (p=="mix") e->setMix(value);
    }
    else if (auto* e = dynamic_cast<AnalogDelay*>(currentEffect))
    {
        if (p=="time") e->setTime(value);
        else if (p=="feedback"||p=="fdbk") e->setFeedback(value);
        else if (p=="modulation"||p=="mod") e->setModulation(value);
        else if (p=="mix") e->setMix(value);
    }
    else if (auto* e = dynamic_cast<Delay*>(currentEffect))
    {
        if (p=="time") e->setTime(value);
        else if (p=="feedback"||p=="fdbk") e->setFeedback(value);
        else if (p=="mix") e->setMix(value);
    }
    // â”€â”€ REVERB â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (auto* e = dynamic_cast<SpringReverb*>(currentEffect))
    {
        if (p=="decay") e->setDecay(value);
        else if (p=="tone") e->setTone(value);
        else if (p=="mix") e->setMix(value);
    }
    else if (auto* e = dynamic_cast<PlateReverb*>(currentEffect))
    {
        if (p=="size") e->setSize(value);
        else if (p=="decay") e->setDamping(value);
        else if (p=="mix") e->setMix(value);
    }
    // â”€â”€ DYNAMICS â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (auto* e = dynamic_cast<DeEsser*>(currentEffect))
    {
        if (p=="threshold") e->setThreshold(value);
        else if (p=="frequency"||p=="freq") e->setFrequency(value);
        else if (p=="ratio") e->setRatio(value);
    }
    else if (auto* e = dynamic_cast<MultibandCompressor*>(currentEffect))
    {
        if (p=="lowthreshold"||p=="lothr") e->setLowThreshold(value);
        else if (p=="midthreshold"||p=="midthr") e->setMidThreshold(value);
        else if (p=="highthreshold"||p=="hithr") e->setHighThreshold(value);
        else if (p=="ratio") e->setRatio(value);
    }
    else if (auto* e = dynamic_cast<FETCompressor*>(currentEffect))
    {
        if (p=="threshold") e->setThreshold(value);
        else if (p=="ratio") e->setRatio(value);
        else if (p=="attack") e->setAttack(value);
        else if (p=="release") e->setRelease(value);
        else if (p=="makeup") e->setMakeup(value);
    }
    else if (auto* e = dynamic_cast<OpticalCompressor*>(currentEffect))
    {
        if (p=="threshold") e->setThreshold(value);
        else if (p=="ratio") e->setRatio(value);
        else if (p=="attack") e->setAttack(value);
        else if (p=="release") e->setRelease(value);
        else if (p=="makeup") e->setMakeup(value);
    }
    else if (auto* e = dynamic_cast<VCACompressor*>(currentEffect))
    {
        if (p=="threshold") e->setThreshold(value);
        else if (p=="ratio") e->setRatio(value);
        else if (p=="attack") e->setAttack(value);
        else if (p=="release") e->setRelease(value);
        else if (p=="makeup") e->setMakeup(value);
    }
    else if (auto* e = dynamic_cast<TubeCompressor*>(currentEffect))
    {
        if (p=="threshold") e->setThreshold(value);
        else if (p=="ratio") e->setRatio(value);
        else if (p=="attack") e->setAttack(value);
        else if (p=="release") e->setRelease(value);
        else if (p=="makeup") e->setMakeup(value);
    }
    else if (auto* e = dynamic_cast<Limiter*>(currentEffect))
    {
        if (p=="threshold") e->setThreshold(value);
        else if (p=="release") e->setRelease(value);
        else if (p=="makeup") e->setMakeup(value);
    }
    else if (auto* e = dynamic_cast<Expander*>(currentEffect))
    {
        if (p=="threshold") e->setThreshold(value);
        else if (p=="ratio") e->setRatio(value);
        else if (p=="attack") e->setAttack(value);
        else if (p=="release") e->setRelease(value);
    }
    else if (auto* e = dynamic_cast<Compressor*>(currentEffect))
    {
        if (p=="threshold") e->setThreshold(value);
        else if (p=="ratio") e->setRatio(value);
        else if (p=="attack") e->setAttack(value);
        else if (p=="release") e->setRelease(value);
        else if (p=="makeup") e->setMakeupGain(value);
    }
    else if (auto* e = dynamic_cast<NoiseGate*>(currentEffect))
    {
        if (p=="threshold") e->setThreshold(value);
        else if (p=="attack") e->setAttack(value);
        else if (p=="release") e->setRelease(value);
    }
    // â”€â”€ FILTER â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (auto* e = dynamic_cast<WahPedal*>(currentEffect))
    {
        if (p=="frequency"||p=="freq") e->setPosition(juce::jmap(value, 300.0f, 2500.0f, 0.0f, 1.0f));
        else if (p=="resonance"||p=="q") e->setResonance(value);
    }
    else if (auto* e = dynamic_cast<AutoWah*>(currentEffect))
    {
        if (p=="frequency"||p=="freq") e->setFrequency(value);
        else if (p=="resonance"||p=="q") e->setResonance(value);
        else if (p=="sensitivity"||p=="sens") e->setSensitivity(value);
        else if (p=="mix") e->setMix(value);
    }
    else if (auto* e = dynamic_cast<EnvelopeFilter*>(currentEffect))
    {
        if (p=="sensitivity"||p=="sens") e->setSensitivity(value);
        else if (p=="attack"||p=="atk") e->setAttack(value);
        else if (p=="decay"||p=="dec") e->setDecay(value);
        else if (p=="resonance"||p=="q") e->setResonance(value);
    }
    else if (auto* e = dynamic_cast<TalkBox*>(currentEffect))
    {
        if (p=="formant") e->setFormant(value);
        else if (p=="mix") e->setMix(value);
        else if (p=="quality") e->setQuality(value);
    }
    else if (auto* e = dynamic_cast<LowPassFilter*>(currentEffect))
    {
        if (p=="frequency"||p=="freq"||p=="cutoff") e->setFrequency(value);
        else if (p=="resonance"||p=="q") e->setResonance(value);
    }
    // â”€â”€ PITCH â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (auto* e = dynamic_cast<Harmonizer*>(currentEffect))
    {
        if (p=="interval") e->setInterval(value);
        else if (p=="key") e->setKey(value);
        else if (p=="mix") e->setMix(value);
    }
    else if (auto* e = dynamic_cast<OctaverUp*>(currentEffect))
    {
        if (p=="octavelevel"||p=="octave") e->setOctaveLevel(value);
        else if (p=="direct") e->setDirect(value);
    }
    else if (auto* e = dynamic_cast<OctaverDown*>(currentEffect))
    {
        if (p=="octavelevel"||p=="octave") e->setOctaveLevel(value);
        else if (p=="direct") e->setDirect(value);
    }
    else if (auto* e = dynamic_cast<PitchShifter*>(currentEffect))
    {
        if (p=="pitch") e->setPitch(value);
        else if (p=="formant") e->setFormant(value);
    }
    // â”€â”€ EQ / TONESTACK â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (auto* e = dynamic_cast<ToneStackFender*>(currentEffect))
    {
        if (p=="bass") e->setBass(value);
        else if (p=="mid") e->setMid(value);
        else if (p=="treble") e->setTreble(value);
    }
    else if (auto* e = dynamic_cast<ToneStackMarshall*>(currentEffect))
    {
        if (p=="bass") e->setBass(value);
        else if (p=="mid") e->setMid(value);
        else if (p=="treble") e->setTreble(value);
        else if (p=="presence") e->setPresence(value);
    }
    else if (auto* e = dynamic_cast<EQ*>(currentEffect))
    {
        if (p=="low") e->setBandGain(0, value);
        else if (p=="mid"||p=="middle") e->setBandGain(1, value);
        else if (p=="high") e->setBandGain(2, value);
    }
    // â”€â”€ AMP / CABINET â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    else if (auto* e = dynamic_cast<CabinetIR*>(currentEffect))
    {
        if (p=="mix")        e->setMicPosition(value);
        else if (p=="lowcut")  e->setRoomMix(value);
        else if (p=="highcut") e->setOutputLevel((value - 0.5f) * 20.0f);
    }
    else if (auto* e = dynamic_cast<AmpSimulator*>(currentEffect))
    {
        if (p=="gain") e->setGain(value);
        else if (p=="bass") e->setBass(value);
        else if (p=="middle"||p=="mid") e->setMid(value);
        else if (p=="treble") e->setTreble(value);
        else if (p=="presence") e->setPresence(value);
    }

    DBG("Parameter changed: " + paramName + " = " + juce::String(value));
}


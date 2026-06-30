#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"

using namespace juce;

class GuitarPedalRackEditor : public AudioProcessorEditor
{
public:
    GuitarPedalRackEditor(GuitarPedalRackProcessor& p)
        : AudioProcessorEditor(&p), audioProcessor(p)
    {
        setSize(800, 600);
        
        addAndMakeVisible(testLabel);
        testLabel.setText("Guitar Pedal Rack - Test Build", dontSendNotification);
        testLabel.setFont(Font(24.0f, Font::bold));
        testLabel.setColour(Label::textColourId, Colours::white);
        testLabel.setJustificationType(Justification::centred);
    }

    ~GuitarPedalRackEditor() override {}

    void paint(Graphics& g) override
    {
        g.fillAll(Colour(0xff1A1A1A));
    }

    void resized() override
    {
        testLabel.setBounds(getLocalBounds());
    }

private:
    GuitarPedalRackProcessor& audioProcessor;
    Label testLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuitarPedalRackEditor)
};

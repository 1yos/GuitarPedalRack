#pragma once

#include "../JuceHeader.h"

using namespace juce;

//==============================================================================
/**
 * Visual representation of the signal chain with pedal cards
 */
class PedalboardView : public Component
{
public:
    PedalboardView();
    ~PedalboardView() override = default;

    void paint(Graphics& g) override;
    void resized() override;

private:
    void drawPedalCard(Graphics& g, int x, int y, int width, int height,
                      const String& name, const Colour& color, const StringArray& knobs);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PedalboardView)
};

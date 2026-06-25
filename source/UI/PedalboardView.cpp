#include "PedalboardView.h"

PedalboardView::PedalboardView()
{
    setSize(900, 600);
}

void PedalboardView::paint(Graphics& g)
{
    // Background with subtle texture
    g.fillAll(Colour(0xff0d0d0d));
    
    // Draw pedal cards
    auto area = getLocalBounds().reduced(20);
    int pedalWidth = 180;
    int pedalHeight = 280;
    int gap = 20;
    int x = area.getX();
    int y = area.getY() + 50;
    
    // Pedal cards with different colors (like the inspiration images)
    struct PedalCard {
        String name;
        Colour color;
        StringArray knobs;
    };
    
    PedalCard pedal1 = {"NOISE GATE", Colour(0xff2a4a2a), {"THRESHOLD", "RELEASE"}};
    PedalCard pedal2 = {"TUBE DRIVE", Colour(0xffaa7733), {"DRIVE", "TONE", "LEVEL"}};
    PedalCard pedal3 = {"AMP SIM", Colour(0xff4a2a4a), {"GAIN", "BASS", "MID", "TREBLE"}};
    PedalCard pedal4 = {"CABINET", Colour(0xff2a4a4a), {"MIC POS", "ROOM"}};
    
    Array<PedalCard> pedals;
    pedals.add(pedal1);
    pedals.add(pedal2);
    pedals.add(pedal3);
    pedals.add(pedal4);
    
    for (int i = 0; i < pedals.size() && x + pedalWidth < area.getRight(); ++i)
    {
        drawPedalCard(g, x, y, pedalWidth, pedalHeight, pedals[i].name, pedals[i].color, pedals[i].knobs);
        x += pedalWidth + gap;
    }
    
    // Title at top
    g.setColour(Colours::white);
    g.setFont(Font(24.0f, Font::bold));
    g.drawText("Signal Chain", area.getX(), area.getY(), area.getWidth(), 40, Justification::centredLeft);
}

void PedalboardView::drawPedalCard(Graphics& g, int x, int y, int width, int height, 
                                   const String& name, const Colour& color, const StringArray& knobs)
{
    // Card background with rounded corners
    Rectangle<float> cardBounds(x, y, width, height);
    
    // Shadow
    g.setColour(Colours::black.withAlpha(0.5f));
    g.fillRoundedRectangle(cardBounds.translated(3, 3), 8.0f);
    
    // Main card
    g.setColour(color);
    g.fillRoundedRectangle(cardBounds, 8.0f);
    
    // Border/highlight
    g.setColour(color.brighter(0.3f));
    g.drawRoundedRectangle(cardBounds, 8.0f, 1.5f);
    
    // Name label at top
    g.setColour(Colours::white);
    g.setFont(Font(14.0f, Font::bold));
    g.drawText(name, x, y + 15, width, 20, Justification::centred);
    
    // Draw knob placeholders
    int knobSize = 55;
    int knobY = y + 60;
    int knobSpacing = (width - knobs.size() * knobSize) / (knobs.size() + 1);
    int knobX = x + knobSpacing;
    
    for (const auto& knobLabel : knobs)
    {
        // Knob circle
        g.setColour(Colour(0xff2a2a2a));
        g.fillEllipse(knobX, knobY, knobSize, knobSize);
        
        // Knob highlight
        g.setColour(Colour(0xff00d4ff).withAlpha(0.3f));
        g.drawEllipse(knobX, knobY, knobSize, knobSize, 2.0f);
        
        // Pointer/indicator
        g.setColour(Colour(0xff00d4ff));
        Path pointer;
        pointer.addRectangle(knobX + knobSize/2 - 1.5f, knobY + 5, 3, knobSize * 0.4f);
        g.fillPath(pointer);
        
        // Label below knob
        g.setColour(Colours::white.withAlpha(0.7f));
        g.setFont(Font(9.0f));
        g.drawText(knobLabel, knobX - 10, knobY + knobSize + 5, knobSize + 20, 15, Justification::centred);
        
        knobY += knobSize + 45;
        if (knobY > y + height - 80)
        {
            knobY = y + 60;
            knobX += knobSize + knobSpacing;
        }
    }
    
    // Bypass button at bottom
    Rectangle<float> bypassButton(x + 20, y + height - 45, width - 40, 30);
    g.setColour(Colour(0xffffffff).withAlpha(0.9f));
    g.fillRoundedRectangle(bypassButton, 4.0f);
    
    g.setColour(Colour(0xff0d0d0d));
    g.setFont(Font(11.0f, Font::bold));
    g.drawText("ACTIVE", bypassButton.toNearestInt(), Justification::centred);
}

void PedalboardView::resized()
{
    // Layout will be handled in paint for now
}

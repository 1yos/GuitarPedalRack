#pragma once

#include "../JuceHeader.h"

/**
 * WornTextures - Aged and worn pedal surface effects
 * 
 * Creates realistic wear patterns for vintage/used pedals:
 * - Edge wear (paint worn off at corners)
 * - Random scratches and scuffs
 * - Surface patina and aging
 * - Color variation from use
 */
namespace WornTextures
{
    /**
     * Draw aged/worn paint surface
     * Creates realistic wear patterns on pedal enclosure
     */
    inline void drawWornPaint(juce::Graphics& g, 
                             juce::Rectangle<float> bounds,
                             juce::Colour baseColor,
                             float wearAmount = 0.5f, // 0=pristine, 1=heavily worn
                             int seed = 12345)
    {
        juce::Random rand(seed);
        
        // Base color layer
        g.setColour(baseColor);
        g.fillRoundedRectangle(bounds, 7.0f);
        
        // 1. EDGE WEAR (paint worn off at corners and edges)
        auto drawEdgeWear = [&](juce::Rectangle<float> area, float intensity)
        {
            // Bare metal showing through (lighter, desaturated)
            juce::Colour wornMetal = juce::Colour(0xff7a7a7a).overlaidWith(baseColor.withAlpha(0.3f));
            
            for (int i = 0; i < (int)(intensity * 20); ++i)
            {
                float size = rand.nextFloat() * 15.0f + 5.0f;
                float x = area.getX() + rand.nextFloat() * area.getWidth();
                float y = area.getY() + rand.nextFloat() * area.getHeight();
                float alpha = rand.nextFloat() * 0.6f + 0.2f;
                
                g.setColour(wornMetal.withAlpha(alpha));
                g.fillEllipse(x - size/2, y - size/2, size, size);
            }
        };
        
        // Apply edge wear to corners and edges
        float edgeWear = wearAmount * 0.8f;
        
        // Top-left corner
        drawEdgeWear(juce::Rectangle<float>(bounds.getX(), bounds.getY(), 25, 25), edgeWear);
        
        // Top-right corner
        drawEdgeWear(juce::Rectangle<float>(bounds.getRight() - 25, bounds.getY(), 25, 25), edgeWear);
        
        // Bottom-left corner
        drawEdgeWear(juce::Rectangle<float>(bounds.getX(), bounds.getBottom() - 25, 25, 25), edgeWear * 1.2f);
        
        // Bottom-right corner
        drawEdgeWear(juce::Rectangle<float>(bounds.getRight() - 25, bounds.getBottom() - 25, 25, 25), edgeWear * 1.2f);
        
        // Top edge
        drawEdgeWear(juce::Rectangle<float>(bounds.getX() + 30, bounds.getY(), bounds.getWidth() - 60, 15), edgeWear * 0.5f);
        
        // Bottom edge
        drawEdgeWear(juce::Rectangle<float>(bounds.getX() + 30, bounds.getBottom() - 15, bounds.getWidth() - 60, 15), edgeWear * 0.6f);
        
        // 2. RANDOM SCRATCHES (linear marks from use)
        int numScratches = (int)(wearAmount * 25);
        for (int i = 0; i < numScratches; ++i)
        {
            float x1 = bounds.getX() + rand.nextFloat() * bounds.getWidth();
            float y1 = bounds.getY() + rand.nextFloat() * bounds.getHeight();
            float angle = rand.nextFloat() * juce::MathConstants<float>::twoPi;
            float length = rand.nextFloat() * 30.0f + 10.0f;
            float x2 = x1 + std::cos(angle) * length;
            float y2 = y1 + std::sin(angle) * length;
            
            // Dark scratch
            g.setColour(juce::Colours::black.withAlpha(rand.nextFloat() * 0.3f + 0.1f));
            g.drawLine(x1, y1, x2, y2, rand.nextFloat() * 1.5f + 0.5f);
            
            // Light highlight next to scratch
            g.setColour(juce::Colours::white.withAlpha(rand.nextFloat() * 0.15f + 0.05f));
            g.drawLine(x1 + 0.5f, y1 + 0.5f, x2 + 0.5f, y2 + 0.5f, 0.5f);
        }
        
        // 3. SCUFF MARKS (small worn areas)
        int numScuffs = (int)(wearAmount * 15);
        for (int i = 0; i < numScuffs; ++i)
        {
            float x = bounds.getX() + rand.nextFloat() * bounds.getWidth();
            float y = bounds.getY() + rand.nextFloat() * bounds.getHeight();
            float size = rand.nextFloat() * 12.0f + 4.0f;
            
            // Darker scuff mark
            g.setColour(baseColor.darker(0.3f).withAlpha(rand.nextFloat() * 0.4f + 0.2f));
            g.fillEllipse(x - size/2, y - size/2, size, size);
        }
        
        // 4. SURFACE PATINA (overall aging)
        // Random color variation across surface
        for (int i = 0; i < (int)(wearAmount * 30); ++i)
        {
            float x = bounds.getX() + rand.nextFloat() * bounds.getWidth();
            float y = bounds.getY() + rand.nextFloat() * bounds.getHeight();
            float size = rand.nextFloat() * 20.0f + 10.0f;
            
            // Slight darkening or lightening
            bool darken = rand.nextBool();
            juce::Colour patina = darken ? 
                baseColor.darker(rand.nextFloat() * 0.15f) :
                baseColor.brighter(rand.nextFloat() * 0.1f);
            
            g.setColour(patina.withAlpha(rand.nextFloat() * 0.15f + 0.05f));
            g.fillEllipse(x - size/2, y - size/2, size, size);
        }
        
        // 5. DIRT/GRIME accumulation (darker areas)
        int numGrime = (int)(wearAmount * 20);
        for (int i = 0; i < numGrime; ++i)
        {
            float x = bounds.getX() + rand.nextFloat() * bounds.getWidth();
            float y = bounds.getY() + rand.nextFloat() * bounds.getHeight();
            float size = rand.nextFloat() * 15.0f + 5.0f;
            
            g.setColour(juce::Colours::black.withAlpha(rand.nextFloat() * 0.12f + 0.03f));
            g.fillEllipse(x - size/2, y - size/2, size, size);
        }
    }
    
    /**
     * Draw chrome/metal footswitch (silver, mechanical)
     */
    inline void drawChromeFootswitch(juce::Graphics& g,
                                    juce::Rectangle<float> bounds,
                                    bool pressed = false)
    {
        auto center = bounds.getCentre();
        float pressOffset = pressed ? 2.0f : 0.0f;
        auto buttonBounds = bounds.reduced(3).translated(0, pressOffset);
        
        // Switch well (dark recess)
        juce::ColourGradient well(
            juce::Colour(0xff0a0a0a), center.x, bounds.getY(),
            juce::Colour(0xff1a1a1a), center.x, bounds.getBottom(),
            false
        );
        g.setGradientFill(well);
        g.fillEllipse(bounds);
        
        // Chrome metal button
        juce::ColourGradient chrome(
            juce::Colour(0xffcccccc), center.x, buttonBounds.getY(),
            juce::Colour(0xff7a7a7a), center.x, buttonBounds.getBottom(),
            false
        );
        g.setGradientFill(chrome);
        g.fillEllipse(buttonBounds);
        
        // Top highlight (specular)
        auto highlightBounds = buttonBounds.reduced(buttonBounds.getWidth() * 0.25f);
        highlightBounds = highlightBounds.withHeight(highlightBounds.getHeight() * 0.4f);
        
        juce::ColourGradient highlight(
            juce::Colours::white.withAlpha(0.7f), center.x, highlightBounds.getCentreY(),
            juce::Colours::transparentWhite, center.x, highlightBounds.getCentreY(),
            true
        );
        g.setGradientFill(highlight);
        g.fillEllipse(highlightBounds);
        
        // Circular rim
        g.setColour(juce::Colour(0xff4a4a4a));
        g.drawEllipse(buttonBounds, 1.5f);
        
        // Outer rim highlight
        g.setColour(juce::Colour(0xffaaaaaa));
        g.drawEllipse(buttonBounds.reduced(1), 0.8f);
    }
    
    /**
     * Draw black plastic knob with white indicator
     */
    inline void drawBlackPlasticKnob(juce::Graphics& g,
                                    juce::Rectangle<float> bounds,
                                    float value, // 0 to 1
                                    bool hasGrip = true)
    {
        auto center = bounds.getCentre();
        float radius = bounds.getWidth() * 0.5f;
        
        // Shadow under knob
        juce::ColourGradient shadow(
            juce::Colours::black.withAlpha(0.5f), center.x, center.y,
            juce::Colours::transparentBlack, center.x, center.y,
            true
        );
        g.setGradientFill(shadow);
        g.fillEllipse(bounds.expanded(2).translated(0, 1));
        
        // Black plastic body
        juce::ColourGradient body(
            juce::Colour(0xff2a2a2a), center.x, bounds.getY(),
            juce::Colour(0xff0a0a0a), center.x, bounds.getBottom(),
            false
        );
        g.setGradientFill(body);
        g.fillEllipse(bounds);
        
        // Grip texture (if enabled)
        if (hasGrip)
        {
            int numGrips = 20;
            for (int i = 0; i < numGrips; ++i)
            {
                float angle = (i / (float)numGrips) * juce::MathConstants<float>::twoPi;
                float x = center.x + std::cos(angle) * radius * 0.75f;
                float y = center.y + std::sin(angle) * radius * 0.75f;
                
                g.setColour(juce::Colours::black.withAlpha(0.4f));
                g.fillEllipse(x - 0.8f, y - 1.2f, 1.6f, 2.4f);
            }
        }
        
        // White indicator line (rotating)
        float angle = -juce::MathConstants<float>::pi * 0.75f + 
                     (value * juce::MathConstants<float>::pi * 1.5f);
        
        float lineLength = radius * 0.65f;
        float x1 = center.x;
        float y1 = center.y;
        float x2 = center.x + std::cos(angle) * lineLength;
        float y2 = center.y + std::sin(angle) * lineLength;
        
        g.setColour(juce::Colours::white);
        g.drawLine(x1, y1, x2, y2, 2.5f);
        
        // Subtle top highlight
        auto highlightBounds = bounds.reduced(radius * 0.3f);
        highlightBounds = highlightBounds.withHeight(highlightBounds.getHeight() * 0.35f);
        
        juce::ColourGradient highlight(
            juce::Colours::white.withAlpha(0.15f), center.x, highlightBounds.getCentreY(),
            juce::Colours::transparentWhite, center.x, highlightBounds.getCentreY(),
            true
        );
        g.setGradientFill(highlight);
        g.fillEllipse(highlightBounds);
        
        // Rim
        g.setColour(juce::Colours::black);
        g.drawEllipse(bounds, 1.0f);
    }
}

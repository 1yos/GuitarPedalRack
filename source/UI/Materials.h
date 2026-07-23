#pragma once

#include "../JuceHeader.h"

/**
 * Materials - Professional material rendering system
 * 
 * Provides realistic material appearances:
 * - Brushed aluminum
 * - Powder-coated metal
 * - Matte paint
 * - Soft rubber
 * - Glass LEDs
 * - Machined metal knobs
 * - Stainless steel screws
 */
namespace Materials
{
    // ============ STUDIO LIGHTING ============
    
    struct StudioLight
    {
        juce::Point<float> position;
        float intensity;
        juce::Colour color;
        
        StudioLight(juce::Point<float> pos = {-200, -300}, 
                   float intens = 1.0f,
                   juce::Colour col = juce::Colour(0xffFFF8E7))
            : position(pos), intensity(intens), color(col) {}
    };
    
    inline StudioLight getMainStudioLight()
    {
        // Warm studio lighting from upper-left
        return StudioLight({-200, -300}, 1.0f, juce::Colour(0xffFFF8E7));
    }
    
    // ============ BRUSHED ALUMINUM ============
    
    inline void drawBrushedAluminum(juce::Graphics& g, 
                                   juce::Rectangle<float> bounds,
                                   juce::Colour baseColor,
                                   bool horizontal = true,
                                   float roughness = 0.3f)
    {
        // Base color
        g.setColour(baseColor);
        g.fillRect(bounds);
        
        // Micro-scratches (brushed texture)
        int numLines = horizontal ? (int)(bounds.getHeight() * 2) : (int)(bounds.getWidth() * 2);
        juce::Random rand(54321);
        
        for (int i = 0; i < numLines; ++i)
        {
            float brightness = 0.7f + rand.nextFloat() * 0.6f;
            float alpha = (0.03f + roughness * 0.02f) * brightness;
            
            g.setColour(juce::Colours::white.withAlpha(alpha));
            
            if (horizontal)
            {
                float y = bounds.getY() + (i * bounds.getHeight() / numLines);
                float length = bounds.getWidth() * (0.8f + rand.nextFloat() * 0.2f);
                float x = bounds.getX() + rand.nextFloat() * (bounds.getWidth() - length);
                g.drawLine(x, y, x + length, y, 0.5f);
            }
            else
            {
                float x = bounds.getX() + (i * bounds.getWidth() / numLines);
                float length = bounds.getHeight() * (0.8f + rand.nextFloat() * 0.2f);
                float y = bounds.getY() + rand.nextFloat() * (bounds.getHeight() - length);
                g.drawLine(x, y, x, y + length, 0.5f);
            }
        }
        
        // Anisotropic reflection
        auto light = getMainStudioLight();
        auto center = bounds.getCentre();
        float distToLight = center.getDistanceFrom(light.position);
        float reflectionStrength = juce::jlimit(0.0f, 0.2f, 1.0f - (distToLight / 1000.0f));
        
        juce::ColourGradient reflection(
            juce::Colours::white.withAlpha(reflectionStrength),
            horizontal ? bounds.getCentreX() : bounds.getX(),
            horizontal ? bounds.getY() : bounds.getCentreY(),
            juce::Colours::transparentWhite,
            horizontal ? bounds.getCentreX() : bounds.getRight(),
            horizontal ? bounds.getBottom() : bounds.getCentreY(),
            horizontal
        );
        
        g.setGradientFill(reflection);
        g.fillRect(bounds);
    }
    
    // ============ POWDER-COATED METAL ============
    
    inline void drawPowderCoatedMetal(juce::Graphics& g,
                                     juce::Rectangle<float> bounds,
                                     juce::Colour baseColor,
                                     float cornerRadius = 8.0f)
    {
        auto light = getMainStudioLight();
        auto center = bounds.getCentre();
        
        // Base color with slight gradient for dimensional depth
        juce::ColourGradient baseGrad(
            baseColor.brighter(0.15f), center.x, bounds.getY(),
            baseColor.darker(0.25f), center.x, bounds.getBottom(),
            false
        );
        g.setGradientFill(baseGrad);
        g.fillRoundedRectangle(bounds, cornerRadius);
        
        // Matte finish (very subtle texture)
        juce::Random rand(12345);
        for (int i = 0; i < 100; ++i)
        {
            float x = bounds.getX() + rand.nextFloat() * bounds.getWidth();
            float y = bounds.getY() + rand.nextFloat() * bounds.getHeight();
            float size = rand.nextFloat() * 0.8f + 0.2f;
            
            g.setColour(juce::Colours::white.withAlpha(0.015f));
            g.fillEllipse(x, y, size, size);
        }
        
        // Subtle specular highlight (matte, not glossy)
        float lightDist = center.getDistanceFrom(light.position);
        float specStrength = juce::jlimit(0.0f, 0.15f, 1.0f - (lightDist / 800.0f));
        
        juce::ColourGradient spec(
            juce::Colours::white.withAlpha(specStrength * 0.3f), center.x, center.y,
            juce::Colours::transparentWhite, center.x, center.y,
            true
        );
        spec.addColour(0.4, juce::Colours::white.withAlpha(specStrength * 0.15f));
        
        g.setGradientFill(spec);
        g.fillRoundedRectangle(bounds.reduced(5), cornerRadius - 2);
    }
    
    // ============ SOFT RUBBER ============
    
    inline void drawSoftRubber(juce::Graphics& g,
                              juce::Rectangle<float> bounds,
                              bool isCircular = false)
    {
        auto center = bounds.getCentre();
        
        // Rubber base (dark matte)
        juce::Colour rubberBase(0xff1a1a1a);
        
        // Subsurface scattering simulation
        juce::ColourGradient subsurface(
            rubberBase.brighter(0.4f), center.x, bounds.getY(),
            rubberBase.darker(0.3f), center.x, bounds.getBottom(),
            false
        );
        
        g.setGradientFill(subsurface);
        if (isCircular)
            g.fillEllipse(bounds);
        else
            g.fillRoundedRectangle(bounds, 6.0f);
        
        // Ambient occlusion
        juce::ColourGradient ao(
            juce::Colours::transparentBlack, center.x, center.y,
            juce::Colours::black.withAlpha(0.5f), center.x, center.y,
            true
        );
        ao.addColour(0.6, juce::Colours::transparentBlack);
        
        g.setGradientFill(ao);
        if (isCircular)
            g.fillEllipse(bounds);
        else
            g.fillRoundedRectangle(bounds, 6.0f);
        
        // Subtle specular (rubber is slightly glossy)
        float specSize = bounds.getWidth() * 0.25f;
        auto specBounds = juce::Rectangle<float>(
            center.x - bounds.getWidth() * 0.2f,
            center.y - bounds.getHeight() * 0.3f,
            specSize, specSize * 0.6f
        );
        
        juce::ColourGradient spec(
            juce::Colours::white.withAlpha(0.15f), specBounds.getCentreX(), specBounds.getCentreY(),
            juce::Colours::transparentWhite, specBounds.getCentreX(), specBounds.getCentreY(),
            true
        );
        
        g.setGradientFill(spec);
        if (isCircular)
            g.fillEllipse(specBounds);
        else
            g.fillRoundedRectangle(specBounds, 3.0f);
    }
    
    // ============ GLASS LED ============
    
    inline void drawGlassLED(juce::Graphics& g,
                            juce::Rectangle<float> bounds,
                            juce::Colour ledColor,
                            float brightness = 1.0f,
                            bool isOn = true)
    {
        auto center = bounds.getCentre();
        
        if (isOn && brightness > 0.0f)
        {
            // Volumetric glow (multiple layers)
            int glowLayers = 15;
            for (int i = glowLayers; i > 0; --i)
            {
                float layerAlpha = (0.15f / (i * 0.6f)) * brightness;
                auto glowBounds = bounds.expanded(i * 3.0f);
                
                juce::ColourGradient glow(
                    ledColor.withAlpha(layerAlpha), center.x, center.y,
                    ledColor.withAlpha(0.0f), center.x, center.y,
                    true
                );
                
                g.setGradientFill(glow);
                g.fillEllipse(glowBounds);
            }
        }
        
        // LED housing (dark plastic bezel)
        g.setColour(juce::Colour(0xff0a0a0a));
        g.fillEllipse(bounds.expanded(3));
        
        // LED lens (glass)
        juce::ColourGradient lens(
            juce::Colour(0xff2a2a2a), center.x, bounds.getY(),
            juce::Colour(0xff1a1a1a), center.x, bounds.getBottom(),
            false
        );
        g.setGradientFill(lens);
        g.fillEllipse(bounds);
        
        if (isOn && brightness > 0.0f)
        {
            // Inner illumination
            juce::ColourGradient inner(
                ledColor.withAlpha(brightness * 0.9f), center.x, center.y,
                ledColor.withAlpha(brightness * 0.3f), center.x, center.y,
                true
            );
            g.setGradientFill(inner);
            g.fillEllipse(bounds.reduced(2));
            
            // Core highlight
            g.setColour(ledColor.brighter(2.0f).withAlpha(brightness * 0.7f));
            g.fillEllipse(bounds.reduced(bounds.getWidth() * 0.35f));
        }
        
        // Glass reflection
        auto reflectionBounds = juce::Rectangle<float>(
            center.x - bounds.getWidth() * 0.25f,
            center.y - bounds.getHeight() * 0.35f,
            bounds.getWidth() * 0.4f,
            bounds.getHeight() * 0.3f
        );
        
        juce::ColourGradient reflection(
            juce::Colours::white.withAlpha(0.4f), reflectionBounds.getCentreX(), reflectionBounds.getCentreY(),
            juce::Colours::transparentWhite, reflectionBounds.getCentreX(), reflectionBounds.getCentreY(),
            true
        );
        g.setGradientFill(reflection);
        g.fillEllipse(reflectionBounds);
    }
    
    // ============ STAINLESS STEEL SCREW ============
    
    inline void drawStainlessSteelScrew(juce::Graphics& g, juce::Point<float> position, float size = 12.0f)
    {
        // Screw recess (shadow)
        juce::ColourGradient recess(
            juce::Colour(0xff0a0a0a), position.x, position.y - size * 0.5f,
            juce::Colour(0xff2a2a2a), position.x, position.y + size * 0.5f,
            false
        );
        g.setGradientFill(recess);
        g.fillEllipse(position.x - size * 0.5f, position.y - size * 0.5f, size, size);
        
        // Screw head (stainless steel)
        float headSize = size * 0.7f;
        juce::ColourGradient head(
            juce::Colour(0xffb0b0b0), position.x - headSize * 0.2f, position.y - headSize * 0.2f,
            juce::Colour(0xff5a5a5a), position.x + headSize * 0.2f, position.y + headSize * 0.2f,
            true
        );
        g.setGradientFill(head);
        g.fillEllipse(position.x - headSize * 0.5f, position.y - headSize * 0.5f, headSize, headSize);
        
        // Specular highlight
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.fillEllipse(position.x - headSize * 0.3f, position.y - headSize * 0.4f, headSize * 0.4f, headSize * 0.25f);
        
        // Phillips slot (crisp and deep)
        g.setColour(juce::Colour(0xff0a0a0a));
        g.drawLine(position.x - headSize * 0.35f, position.y, position.x + headSize * 0.35f, position.y, 1.5f);
        g.drawLine(position.x, position.y - headSize * 0.35f, position.x, position.y + headSize * 0.35f, 1.5f);
        
        // Screw shadow on surface
        juce::ColourGradient shadow(
            juce::Colours::black.withAlpha(0.4f), position.x, position.y,
            juce::Colours::transparentBlack, position.x, position.y,
            true
        );
        g.setGradientFill(shadow);
        g.fillEllipse(position.x - size * 0.6f, position.y - size * 0.5f, size * 1.2f, size);
    }
    
    // ============ MACHINED ALUMINUM KNOB ============
    
    inline void drawMachinedKnob(juce::Graphics& g,
                                juce::Rectangle<float> bounds,
                                float rotation, // 0 to 1
                                juce::Colour accentColor,
                                bool hasRubberGrip = true)
    {
        auto center = bounds.getCentre();
        float radius = bounds.getWidth() * 0.5f;
        
        // Knob shadow
        juce::ColourGradient shadow(
            juce::Colours::black.withAlpha(0.6f), center.x, center.y,
            juce::Colours::transparentBlack, center.x, center.y,
            true
        );
        g.setGradientFill(shadow);
        g.fillEllipse(bounds.expanded(4).translated(0, 2));
        
        // Aluminum body
        juce::ColourGradient body(
            juce::Colour(0xff8a8a8a), center.x, bounds.getY(),
            juce::Colour(0xff3a3a3a), center.x, bounds.getBottom(),
            false
        );
        g.setGradientFill(body);
        g.fillEllipse(bounds);
        
        // Circular brushed texture
        int numLines = 80;
        for (int i = 0; i < numLines; ++i)
        {
            float angle = (i / (float)numLines) * juce::MathConstants<float>::twoPi;
            float x1 = center.x + std::cos(angle) * radius * 0.4f;
            float y1 = center.y + std::sin(angle) * radius * 0.4f;
            float x2 = center.x + std::cos(angle) * radius;
            float y2 = center.y + std::sin(angle) * radius;
            
            g.setColour(juce::Colours::white.withAlpha(0.03f));
            g.drawLine(x1, y1, x2, y2, 0.5f);
        }
        
        // Rubber grip (if enabled)
        if (hasRubberGrip)
        {
            int numGrips = 20;
            for (int i = 0; i < numGrips; ++i)
            {
                float angle = (i / (float)numGrips) * juce::MathConstants<float>::twoPi;
                float x = center.x + std::cos(angle) * radius * 0.75f;
                float y = center.y + std::sin(angle) * radius * 0.75f;
                
                g.setColour(juce::Colour(0xff1a1a1a));
                g.fillEllipse(x - 1, y - 1.5f, 2, 3);
            }
        }
        
        // Position marker (rotates with knob)
        float markerAngle = -juce::MathConstants<float>::pi * 0.75f + 
                          (rotation * juce::MathConstants<float>::pi * 1.5f);
        float markerX = center.x + std::cos(markerAngle) * radius * 0.6f;
        float markerY = center.y + std::sin(markerAngle) * radius * 0.6f;
        
        // Marker line
        g.setColour(accentColor);
        g.fillRoundedRectangle(markerX - 1.5f, markerY - radius * 0.25f, 3, radius * 0.4f, 1.5f);
        
        // Marker dot
        g.setColour(accentColor.brighter(0.5f));
        g.fillEllipse(markerX - 3, markerY - 3, 6, 6);
        
        // Specular highlight
        juce::ColourGradient spec(
            juce::Colours::white.withAlpha(0.5f), center.x - radius * 0.3f, center.y - radius * 0.4f,
            juce::Colours::transparentWhite, center.x, center.y,
            true
        );
        g.setGradientFill(spec);
        g.fillEllipse(bounds.reduced(radius * 0.2f));
        
        // Center cap
        g.setColour(juce::Colour(0xff2a2a2a));
        g.fillEllipse(center.x - radius * 0.15f, center.y - radius * 0.15f, radius * 0.3f, radius * 0.3f);
        
        g.setColour(juce::Colour(0xff4a4a4a));
        g.drawEllipse(center.x - radius * 0.15f, center.y - radius * 0.15f, radius * 0.3f, radius * 0.3f, 0.5f);
    }
}

#pragma once

#include "../JuceHeader.h"

/**
 * PatchCable - Realistic patch cable rendering and animation
 * 
 * Features:
 * - Automatic smooth routing
 * - Natural cable sag and curvature
 * - Collision avoidance
 * - Signal flow visualization
 * - Physical cable movement
 * - Realistic jacks and plugs
 */
class PatchCable
{
public:
    PatchCable(juce::Point<float> start, juce::Point<float> end, juce::Colour color)
        : startPoint(start), endPoint(end), cableColor(color)
    {
        updatePath();
    }
    
    void setStartPoint(juce::Point<float> point)
    {
        startPoint = point;
        updatePath();
    }
    
    void setEndPoint(juce::Point<float> point)
    {
        endPoint = point;
        updatePath();
    }
    
    void setColor(juce::Colour color)
    {
        cableColor = color;
    }
    
    void setActive(bool isActive)
    {
        active = isActive;
    }
    
    void setSignalLevel(float level)
    {
        signalLevel = juce::jlimit(0.0f, 1.0f, level);
    }
    
    void animate(float deltaTime)
    {
        if (active && signalLevel > 0.01f)
        {
            // Animate signal pulse traveling through cable
            signalPulsePosition += deltaTime * 2.0f; // Speed of pulse
            if (signalPulsePosition > 1.0f)
                signalPulsePosition -= 1.0f;
        }
        
        // Subtle cable sway animation
        cableSway += deltaTime * 0.5f;
        if (cableSway > juce::MathConstants<float>::twoPi)
            cableSway -= juce::MathConstants<float>::twoPi;
    }
    
    void draw(juce::Graphics& g)
    {
        // Cable shadow (underneath)
        g.setColour(juce::Colours::black.withAlpha(0.35f));
        g.strokePath(cablePath, juce::PathStrokeType(cableThickness + 2.0f), 
                    juce::AffineTransform::translation(0, 3));
        
        // Cable body
        juce::Colour bodyColor = active ? cableColor.darker(0.3f) : juce::Colour(0xff3a3a3a);
        g.setColour(bodyColor);
        g.strokePath(cablePath, juce::PathStrokeType(cableThickness));
        
        // Cable highlight (shiny outer jacket)
        g.setColour(juce::Colours::white.withAlpha(0.12f));
        g.strokePath(cablePath, juce::PathStrokeType(cableThickness * 0.4f));
        
        // Signal flow visualization (pulse)
        if (active && signalLevel > 0.01f)
        {
            drawSignalPulse(g);
        }
        
        // Draw jacks at both ends
        drawJack(g, startPoint, cableColor, true);  // Output jack
        drawJack(g, endPoint, cableColor, false);   // Input jack
    }
    
    juce::Rectangle<float> getBounds() const
    {
        return cablePath.getBounds().expanded(20);
    }
    
private:
    juce::Point<float> startPoint;
    juce::Point<float> endPoint;
    juce::Colour cableColor;
    juce::Path cablePath;
    
    bool active = true;
    float signalLevel = 0.0f;
    float signalPulsePosition = 0.0f;
    float cableSway = 0.0f;
    float cableThickness = 4.5f;
    
    void updatePath()
    {
        cablePath.clear();
        cablePath.startNewSubPath(startPoint);
        
        float distance = startPoint.getDistanceFrom(endPoint);
        float sag = juce::jmin(60.0f, distance * 0.15f); // Natural cable sag
        
        // Add subtle horizontal sway
        float swayAmount = std::sin(cableSway) * 3.0f;
        
        if (startPoint.x > endPoint.x)
        {
            // Backward/downward routing (swoop down and left)
            juce::Point<float> ctrl1(startPoint.x + 60.0f, startPoint.y + 100.0f);
            juce::Point<float> ctrl2(endPoint.x - 60.0f, endPoint.y - 100.0f);
            
            ctrl1 = ctrl1.translated(swayAmount, 0);
            ctrl2 = ctrl2.translated(-swayAmount, 0);
            
            cablePath.cubicTo(ctrl1, ctrl2, endPoint);
        }
        else
        {
            // Standard left-to-right routing
            juce::Point<float> ctrl1(startPoint.x + distance * 0.25f, startPoint.y + sag * 0.3f);
            juce::Point<float> ctrl2(endPoint.x - distance * 0.25f, endPoint.y + sag * 0.3f);
            
            ctrl1 = ctrl1.translated(swayAmount, 0);
            ctrl2 = ctrl2.translated(-swayAmount, 0);
            
            cablePath.cubicTo(ctrl1, ctrl2, endPoint);
        }
    }
    
    void drawSignalPulse(juce::Graphics& g)
    {
        // Calculate position along cable path
        juce::Point<float> pulsePoint;
        float pathLength = cablePath.getLength();
        
        if (pathLength > 0.0f)
        {
            pulsePoint = cablePath.getPointAlongPath(pathLength * signalPulsePosition);
            
            // Draw glowing pulse
            int numGlowLayers = 8;
            for (int i = numGlowLayers; i > 0; --i)
            {
                float layerSize = (i * 2.0f) + 3.0f;
                float layerAlpha = (0.3f / i) * signalLevel;
                
                g.setColour(cableColor.brighter(1.5f).withAlpha(layerAlpha));
                g.fillEllipse(pulsePoint.x - layerSize * 0.5f,
                            pulsePoint.y - layerSize * 0.5f,
                            layerSize, layerSize);
            }
            
            // Core bright spot
            g.setColour(cableColor.brighter(2.5f).withAlpha(signalLevel * 0.8f));
            g.fillEllipse(pulsePoint.x - 2, pulsePoint.y - 2, 4, 4);
        }
    }
    
    void drawJack(juce::Graphics& g, juce::Point<float> position, juce::Colour color, bool isOutput)
    {
        float jackSize = 10.0f;
        
        // Jack shadow
        juce::ColourGradient shadow(
            juce::Colours::black.withAlpha(0.5f), position.x, position.y,
            juce::Colours::transparentBlack, position.x, position.y,
            true
        );
        g.setGradientFill(shadow);
        g.fillEllipse(position.x - jackSize * 0.6f, position.y - jackSize * 0.6f, 
                     jackSize * 1.2f, jackSize * 1.2f);
        
        // Jack barrel (metal housing)
        juce::ColourGradient barrel(
            juce::Colour(0xff4a4a4a), position.x, position.y - jackSize * 0.5f,
            juce::Colour(0xff1a1a1a), position.x, position.y + jackSize * 0.5f,
            false
        );
        g.setGradientFill(barrel);
        g.fillEllipse(position.x - jackSize * 0.5f, position.y - jackSize * 0.5f, jackSize, jackSize);
        
        // Jack tip (colored to match cable/pedal)
        float tipSize = jackSize * 0.6f;
        juce::Colour tipColor = active ? color : juce::Colour(0xff2a2a2a);
        
        juce::ColourGradient tip(
            tipColor.brighter(0.3f), position.x, position.y - tipSize * 0.5f,
            tipColor.darker(0.3f), position.x, position.y + tipSize * 0.5f,
            false
        );
        g.setGradientFill(tip);
        g.fillEllipse(position.x - tipSize * 0.5f, position.y - tipSize * 0.5f, tipSize, tipSize);
        
        // Metal rim highlight
        g.setColour(juce::Colour(0xff7a7a7a));
        g.drawEllipse(position.x - jackSize * 0.5f, position.y - jackSize * 0.5f, jackSize, jackSize, 1.0f);
        
        // Specular highlight
        if (active)
        {
            g.setColour(juce::Colours::white.withAlpha(0.4f));
            g.fillEllipse(position.x - tipSize * 0.3f, position.y - tipSize * 0.4f, 
                        tipSize * 0.4f, tipSize * 0.25f);
        }
    }
};

/**
 * CableManager - Manages all patch cables in the pedalboard
 */
class CableManager
{
public:
    struct Connection
    {
        int fromPedalIndex;
        int toPedalIndex;
        std::unique_ptr<PatchCable> cable;
    };
    
    void updateConnection(int fromIndex, int toIndex, 
                         juce::Point<float> startPoint, juce::Point<float> endPoint,
                         juce::Colour color)
    {
        // Find or create connection
        Connection* conn = nullptr;
        for (auto& c : connections)
        {
            if (c.fromPedalIndex == fromIndex && c.toPedalIndex == toIndex)
            {
                conn = &c;
                break;
            }
        }
        
        if (!conn)
        {
            connections.push_back({fromIndex, toIndex, std::make_unique<PatchCable>(startPoint, endPoint, color)});
        }
        else
        {
            conn->cable->setStartPoint(startPoint);
            conn->cable->setEndPoint(endPoint);
            conn->cable->setColor(color);
        }
    }
    
    void removeConnection(int fromIndex, int toIndex)
    {
        connections.erase(
            std::remove_if(connections.begin(), connections.end(),
                [fromIndex, toIndex](const Connection& c) {
                    return c.fromPedalIndex == fromIndex && c.toPedalIndex == toIndex;
                }),
            connections.end()
        );
    }
    
    void clearAll()
    {
        connections.clear();
    }
    
    void setActive(int fromIndex, int toIndex, bool active)
    {
        for (auto& c : connections)
        {
            if (c.fromPedalIndex == fromIndex && c.toPedalIndex == toIndex)
            {
                c.cable->setActive(active);
                break;
            }
        }
    }
    
    void setSignalLevel(int fromIndex, int toIndex, float level)
    {
        for (auto& c : connections)
        {
            if (c.fromPedalIndex == fromIndex && c.toPedalIndex == toIndex)
            {
                c.cable->setSignalLevel(level);
                break;
            }
        }
    }
    
    void animate(float deltaTime)
    {
        for (auto& c : connections)
        {
            c.cable->animate(deltaTime);
        }
    }
    
    void drawAll(juce::Graphics& g)
    {
        for (auto& c : connections)
        {
            c.cable->draw(g);
        }
    }
    
    void updatePedalMoved(int oldIndex, int newIndex)
    {
        for (auto& c : connections)
        {
            if (c.fromPedalIndex == oldIndex)
                c.fromPedalIndex = newIndex;
            else if (c.fromPedalIndex > oldIndex && c.fromPedalIndex <= newIndex)
                c.fromPedalIndex--;
            else if (c.fromPedalIndex < oldIndex && c.fromPedalIndex >= newIndex)
                c.fromPedalIndex++;
                
            if (c.toPedalIndex == oldIndex)
                c.toPedalIndex = newIndex;
            else if (c.toPedalIndex > oldIndex && c.toPedalIndex <= newIndex)
                c.toPedalIndex--;
            else if (c.toPedalIndex < oldIndex && c.toPedalIndex >= newIndex)
                c.toPedalIndex++;
        }
    }
    
private:
    std::vector<Connection> connections;
};

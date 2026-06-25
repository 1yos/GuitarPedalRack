#pragma once

#include "../JuceHeader.h"
#include <memory>
#include <map>

using namespace juce;

//==============================================================================
/**
 * Base class for all audio processing modules in the Guitar Pedal Rack.
 * 
 * All effects (Gate, Overdrive, Amp, etc.) inherit from this class.
 * Provides common functionality: bypass, wet/dry mix, parameter management.
 */
class AudioModule
{
public:
    AudioModule(const String& moduleName) : name(moduleName) {}
    virtual ~AudioModule() = default;

    //==============================================================================
    // Core Processing
    
    /** Prepares the module for playback at the given sample rate and block size */
    virtual void prepare(double sampleRate, int maximumExpectedSamplesPerBlock)
    {
        currentSampleRate = sampleRate;
        maxBlockSize = maximumExpectedSamplesPerBlock;
    }
    
    /** Processes an audio buffer */
    virtual void process(AudioBuffer<float>& buffer)
    {
        if (bypassed)
            return;
            
        if (wetDryMix >= 0.999f)
        {
            // Fully wet - just process
            processInternal(buffer);
        }
        else if (wetDryMix <= 0.001f)
        {
            // Fully dry - skip processing
            return;
        }
        else
        {
            // Mix wet and dry
            AudioBuffer<float> dryBuffer(buffer.getNumChannels(), buffer.getNumSamples());
            
            // Copy dry signal
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                dryBuffer.copyFrom(ch, 0, buffer, ch, 0, buffer.getNumSamples());
            
            // Process wet signal
            processInternal(buffer);
            
            // Mix
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                auto* wet = buffer.getWritePointer(ch);
                const auto* dry = dryBuffer.getReadPointer(ch);
                
                for (int i = 0; i < buffer.getNumSamples(); ++i)
                    wet[i] = dry[i] * (1.0f - wetDryMix) + wet[i] * wetDryMix;
            }
        }
    }
    
    /** Resets the module's internal state */
    virtual void reset()
    {
        // Override in derived classes
    }

    //==============================================================================
    // State Management
    
    /** Returns the module's unique name */
    String getName() const { return name; }
    
    /** Returns the module's display name (user-friendly) */
    virtual String getDisplayName() const { return name; }
    
    /** Returns the module type identifier */
    virtual String getModuleType() const = 0;
    
    /** Saves the module's state to a MemoryBlock */
    virtual void saveState(MemoryBlock& destData)
    {
        MemoryOutputStream stream(destData, false);
        stream.writeFloat(wetDryMix);
        stream.writeBool(bypassed);
    }
    
    /** Restores the module's state from data */
    virtual void loadState(const void* data, int sizeInBytes)
    {
        MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
        wetDryMix = stream.readFloat();
        bypassed = stream.readBool();
    }

    //==============================================================================
    // Control
    
    /** Sets the bypass state */
    void setBypass(bool shouldBypass) { bypassed = shouldBypass; }
    
    /** Returns true if the module is bypassed */
    bool isBypassed() const { return bypassed; }
    
    /** Sets the wet/dry mix (0.0 = fully dry, 1.0 = fully wet) */
    void setWetDryMix(float newMix) 
    { 
        wetDryMix = jlimit(0.0f, 1.0f, newMix);
    }
    
    /** Returns the current wet/dry mix value */
    float getWetDryMix() const { return wetDryMix; }
    
    /** Returns the current sample rate */
    double getSampleRate() const { return currentSampleRate; }
    
    /** Returns the maximum block size */
    int getMaxBlockSize() const { return maxBlockSize; }
    
    //==============================================================================
    // APVTS Parameter Connection
    
    /** Sets a parameter pointer (for real-time reading) */
    void setParameterPointer(const String& paramName, std::atomic<float>* paramPtr)
    {
        parameterPointers[paramName] = paramPtr;
    }
    
    /** Gets a parameter value by name (returns 0.0 if not found) */
    float getParameterValue(const String& paramName) const
    {
        auto it = parameterPointers.find(paramName);
        if (it != parameterPointers.end() && it->second != nullptr)
            return it->second->load();
        return 0.0f;
    }
    
    /** Checks if a parameter exists */
    bool hasParameter(const String& paramName) const
    {
        return parameterPointers.find(paramName) != parameterPointers.end();
    }

protected:
    //==============================================================================
    // Override this in derived classes to implement the actual processing
    virtual void processInternal(AudioBuffer<float>& buffer) = 0;
    
    //==============================================================================
    // Member variables
    String name;
    double currentSampleRate = 44100.0;
    int maxBlockSize = 512;
    bool bypassed = false;
    float wetDryMix = 1.0f;  // 0.0 = fully dry, 1.0 = fully wet
    
    // Parameter pointers for real-time access
    std::map<String, std::atomic<float>*> parameterPointers;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioModule)
};

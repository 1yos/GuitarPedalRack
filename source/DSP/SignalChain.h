#pragma once

#include "../JuceHeader.h"
#include "AudioModule.h"
#include <vector>
#include <memory>

using namespace juce;

//==============================================================================
/**
 * SignalChain manages a series of audio processing modules.
 * 
 * Modules are processed in order (serial chain).
 * Future versions will support parallel routing.
 */
class SignalChain
{
public:
    SignalChain() = default;
    ~SignalChain() = default;

    //==============================================================================
    // Module Management
    
    /** Adds a module to the end of the chain */
    void addModule(std::unique_ptr<AudioModule> module)
    {
        if (module != nullptr)
        {
            if (isPrepared)
                module->prepare(currentSampleRate, maxBlockSize);
            
            modules.push_back(std::move(module));
        }
    }
    
    /** Inserts a module at a specific position */
    void insertModule(int index, std::unique_ptr<AudioModule> module)
    {
        if (module != nullptr && index >= 0 && index <= (int)modules.size())
        {
            if (isPrepared)
                module->prepare(currentSampleRate, maxBlockSize);
            
            modules.insert(modules.begin() + index, std::move(module));
        }
    }
    
    /** Removes a module at the specified index */
    void removeModule(int index)
    {
        if (index >= 0 && index < (int)modules.size())
            modules.erase(modules.begin() + index);
    }
    
    /** Moves a module from one position to another */
    void moveModule(int fromIndex, int toIndex)
    {
        if (fromIndex >= 0 && fromIndex < (int)modules.size() &&
            toIndex >= 0 && toIndex < (int)modules.size() &&
            fromIndex != toIndex)
        {
            auto module = std::move(modules[fromIndex]);
            modules.erase(modules.begin() + fromIndex);
            
            if (toIndex > fromIndex)
                --toIndex;
            
            modules.insert(modules.begin() + toIndex, std::move(module));
        }
    }
    
    /** Removes all modules from the chain */
    void clearChain()
    {
        modules.clear();
    }
    
    /** Returns the number of modules in the chain */
    int getNumModules() const
    {
        return static_cast<int>(modules.size());
    }
    
    /** Returns a pointer to a specific module */
    AudioModule* getModule(int index)
    {
        if (index >= 0 && index < (int)modules.size())
            return modules[index].get();
        return nullptr;
    }

    //==============================================================================
    // Processing
    
    /** Prepares all modules in the chain */
    void prepare(double sampleRate, int maximumExpectedSamplesPerBlock)
    {
        currentSampleRate = sampleRate;
        maxBlockSize = maximumExpectedSamplesPerBlock;
        isPrepared = true;
        
        for (auto& module : modules)
        {
            if (module != nullptr)
                module->prepare(sampleRate, maximumExpectedSamplesPerBlock);
        }
    }
    
    /** Processes audio through all modules in the chain */
    void process(AudioBuffer<float>& buffer)
    {
        // Safety check: validate buffer
        if (buffer.getNumSamples() == 0 || buffer.getNumChannels() == 0)
            return;
        
        // Process each module in series
        for (auto& module : modules)
        {
            if (module != nullptr && !module->isBypassed())
            {
                // Safety: sanitize buffer before each module
                sanitizeBuffer(buffer);
                module->process(buffer);
            }
        }
        
        // Final safety check
        sanitizeBuffer(buffer);
    }
    
    /** Resets all modules in the chain */
    void reset()
    {
        for (auto& module : modules)
        {
            if (module != nullptr)
                module->reset();
        }
    }

    //==============================================================================
    // State Management
    
    /** Saves the entire chain configuration */
    void saveState(MemoryBlock& destData)
    {
        MemoryOutputStream stream(destData, false);
        
        // Write number of modules
        stream.writeInt((int)modules.size());
        
        // Write each module's type and state
        for (auto& module : modules)
        {
            if (module != nullptr)
            {
                stream.writeString(module->getModuleType());
                
                MemoryBlock moduleData;
                module->saveState(moduleData);
                
                stream.writeInt((int)moduleData.getSize());
                stream.write(moduleData.getData(), moduleData.getSize());
            }
        }
    }
    
    /** Loads the chain configuration (modules must be recreated by caller) */
    void loadState(const void* data, int sizeInBytes)
    {
        MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
        
        int numModules = stream.readInt();
        
        // Note: This only loads state for existing modules
        // Module creation must be handled by the caller based on module types
        for (int i = 0; i < numModules && i < (int)modules.size(); ++i)
        {
            String moduleType = stream.readString();
            int moduleDataSize = stream.readInt();
            
            if (modules[i] != nullptr)
            {
                std::vector<char> moduleData(moduleDataSize);
                stream.read(moduleData.data(), moduleDataSize);
                modules[i]->loadState(moduleData.data(), moduleDataSize);
            }
            else
            {
                stream.skipNextBytes(moduleDataSize);
            }
        }
    }

private:
    std::vector<std::unique_ptr<AudioModule>> modules;
    double currentSampleRate = 44100.0;
    int maxBlockSize = 512;
    bool isPrepared = false;
    
    /** Sanitizes buffer by removing NaN, Inf, and extreme values */
    void sanitizeBuffer(AudioBuffer<float>& buffer)
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                if (std::isnan(data[i]) || std::isinf(data[i]))
                    data[i] = 0.0f;
                else if (data[i] > 10.0f)
                    data[i] = 1.0f;
                else if (data[i] < -10.0f)
                    data[i] = -1.0f;
            }
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SignalChain)
};

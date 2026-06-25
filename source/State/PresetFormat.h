#pragma once

#include "../JuceHeader.h"
#include <map>

using namespace juce;

//==============================================================================
/**
 * Data structures for preset management
 */

struct ModulePreset
{
    String moduleType;          // e.g., "NoiseGate", "TubeOverdrive"
    bool bypassed = false;
    float wetDryMix = 1.0f;
    MemoryBlock state;          // Module-specific state
};

struct ChainPreset
{
    String name;
    String author = "Factory";
    String description;
    StringArray tags;           // e.g., "rock", "blues", "lead"
    
    Array<ModulePreset> modules;
    std::map<String, float> parameterValues;
    
    Time dateCreated;
    Time dateModified;
    
    // Metadata
    String category = "Other";  // Genre, Feel, Performance
    int rating = 0;             // 0-5 stars
};

struct PresetLibrary
{
    Array<ChainPreset> presets;
    
    void sortByName()
    {
        std::sort(presets.begin(), presets.end(),
                 [](const ChainPreset& a, const ChainPreset& b) {
                     return a.name < b.name;
                 });
    }
    
    void sortByDate()
    {
        std::sort(presets.begin(), presets.end(),
                 [](const ChainPreset& a, const ChainPreset& b) {
                     return a.dateModified > b.dateModified;
                 });
    }
    
    ChainPreset* findPresetByName(const String& name)
    {
        for (auto& preset : presets)
        {
            if (preset.name == name)
                return &preset;
        }
        return nullptr;
    }
    
    Array<ChainPreset*> searchPresets(const String& query, const StringArray& tagFilter)
    {
        Array<ChainPreset*> results;
        
        for (auto& preset : presets)
        {
            bool matchesQuery = query.isEmpty() || 
                               preset.name.containsIgnoreCase(query) ||
                               preset.description.containsIgnoreCase(query);
            
            bool matchesTags = tagFilter.isEmpty();
            if (!matchesTags)
            {
                for (const auto& tag : tagFilter)
                {
                    if (preset.tags.contains(tag))
                    {
                        matchesTags = true;
                        break;
                    }
                }
            }
            
            if (matchesQuery && matchesTags)
                results.add(&preset);
        }
        
        return results;
    }
};

#pragma once

#include "JuceHeader.h"
#include "AudioModule.h"
#include <vector>
#include <map>
#include <memory>

using namespace juce;

//==============================================================================
/**
 * Effect Library - Registry of all 150+ effects
 * 
 * Central repository for:
 * - Effect metadata (name, category, color, description)
 * - Effect creation (factory pattern)
 * - Effect categorization
 * - Search and filtering
 */

//==============================================================================
// Effect Categories
enum class EffectCategory
{
    Drive,          // Overdrive, Distortion, Fuzz (18 effects)
    Modulation,     // Chorus, Flanger, Phaser, etc. (20 effects)
    Delay,          // All delay types (15 effects)
    Reverb,         // All reverb types (12 effects)
    Filter,         // Wah, Filter, EQ (10 effects)
    Dynamics,       // Compressor, Gate, Limiter (10 effects)
    EQ,             // Parametric, Graphic EQ (8 effects)
    Pitch,          // Pitch shifter, Harmonizer (10 effects)
    Amp,            // Amp simulators (15 amps)
    Cabinet,        // Cabinet IRs (12 types)
    Special         // Looper, Synth, etc. (15 effects)
};

//==============================================================================
// Effect Descriptor - Metadata for each effect
struct EffectDescriptor
{
    String id;                      // Unique ID: "tubeOverdrive"
    String displayName;             // Display: "Tube Overdrive"
    String manufacturer;            // "Boss", "Ibanez", etc.
    EffectCategory category;        // Category
    Colour pedalColor;              // Visual color for UI
    String description;             // Help text
    StringArray tags;               // Search tags
    float estimatedCPUCost;         // Relative CPU cost (0.0-1.0)
    bool isPremium;                 // Premium/Pro effect
    int version;                    // Effect version
    
    // Parameter information
    struct ParameterInfo
    {
        String id;
        String name;
        float minValue;
        float maxValue;
        float defaultValue;
        String units;
    };
    
    std::vector<ParameterInfo> parameters;
};

//==============================================================================
/**
 * Effect Library Singleton
 */
class EffectLibrary
{
public:
    //==============================================================================
    // Singleton Access
    static EffectLibrary& getInstance();
    
    //==============================================================================
    // Effect Registry
    
    /** Register an effect type */
    void registerEffect(const EffectDescriptor& descriptor,
                       std::function<std::unique_ptr<AudioModule>()> factory);
    
    /** Get all registered effects */
    std::vector<EffectDescriptor> getAllEffects() const;
    
    /** Get effects in category */
    std::vector<EffectDescriptor> getEffectsInCategory(EffectCategory category) const;
    
    /** Get effect descriptor by ID */
    const EffectDescriptor* getEffectDescriptor(const String& effectId) const;
    
    /** Get all categories */
    std::vector<EffectCategory> getAllCategories() const;
    
    /** Get category name */
    static String getCategoryName(EffectCategory category);
    
    /** Get category color */
    static Colour getCategoryColor(EffectCategory category);
    
    //==============================================================================
    // Effect Creation
    
    /** Create effect instance by ID */
    std::unique_ptr<AudioModule> createEffect(const String& effectId);
    
    //==============================================================================
    // Search & Filter
    
    /** Search effects by keyword */
    std::vector<EffectDescriptor> searchEffects(const String& keyword) const;
    
    /** Filter effects by tags */
    std::vector<EffectDescriptor> filterByTags(const StringArray& tags) const;
    
    /** Get featured/recommended effects */
    std::vector<EffectDescriptor> getFeaturedEffects() const;
    
    //==============================================================================
    // Initialization
    
    /** Initialize library with all 150+ effects */
    void initializeLibrary();
    
    /** Get library statistics */
    int getTotalEffectCount() const { return static_cast<int>(effectRegistry.size()); }
    int getEffectCountInCategory(EffectCategory category) const;

private:
    EffectLibrary();
    ~EffectLibrary() = default;
    
    // Disable copy
    EffectLibrary(const EffectLibrary&) = delete;
    EffectLibrary& operator=(const EffectLibrary&) = delete;
    
    //==============================================================================
    // Registration Helpers
    void registerDriveEffects();
    void registerModulationEffects();
    void registerDelayEffects();
    void registerReverbEffects();
    void registerFilterEffects();
    void registerDynamicsEffects();
    void registerEQEffects();
    void registerPitchEffects();
    void registerAmpEffects();
    void registerCabinetEffects();
    void registerSpecialEffects();
    
    //==============================================================================
    // Internal Storage
    struct EffectRegistration
    {
        EffectDescriptor descriptor;
        std::function<std::unique_ptr<AudioModule>()> factory;
    };
    
    std::map<String, EffectRegistration> effectRegistry;
    std::map<EffectCategory, std::vector<String>> categoryIndex;
    
    bool initialized = false;
};

//==============================================================================
// Helper Macros for Effect Registration

#define REGISTER_EFFECT(id, name, category, color, factory) \
    registerEffect( \
        { id, name, "", category, color, "", {}, 0.5f, false, 1, {} }, \
        factory \
    )

#define REGISTER_EFFECT_WITH_DESC(descriptor, factory) \
    registerEffect(descriptor, factory)

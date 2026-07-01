#include "EffectLibrary.h"

// Include all effect headers
#include "Drive/TubeOverdrive.h"
#include "Drive/Distortion.h"
#include "Modulation/Chorus.h"
#include "TimeFX/Delay.h"
#include "TimeFX/Reverb.h"
#include "Dynamics/NoiseGate.h"
#include "Dynamics/Compressor.h"
#include "Amp/AmpSimulator.h"
#include "Cabinet/CabinetIR.h"
#include "Utility/EQ.h"

//==============================================================================
// Singleton Instance

EffectLibrary& EffectLibrary::getInstance()
{
    static EffectLibrary instance;
    
    if (!instance.initialized)
    {
        instance.initializeLibrary();
        instance.initialized = true;
    }
    
    return instance;
}

//==============================================================================
// Constructor

EffectLibrary::EffectLibrary()
{
}

//==============================================================================
// Effect Registry

void EffectLibrary::registerEffect(const EffectDescriptor& descriptor,
                                   std::function<std::unique_ptr<AudioModule>()> factory)
{
    if (effectRegistry.find(descriptor.id) != effectRegistry.end())
    {
        DBG("Warning: Effect '" + descriptor.id + "' already registered!");
        return;
    }
    
    EffectRegistration registration;
    registration.descriptor = descriptor;
    registration.factory = factory;
    
    effectRegistry[descriptor.id] = registration;
    
    // Add to category index
    categoryIndex[descriptor.category].push_back(descriptor.id);
}

std::vector<EffectDescriptor> EffectLibrary::getAllEffects() const
{
    std::vector<EffectDescriptor> effects;
    effects.reserve(effectRegistry.size());
    
    for (const auto& pair : effectRegistry)
    {
        effects.push_back(pair.second.descriptor);
    }
    
    return effects;
}

std::vector<EffectDescriptor> EffectLibrary::getEffectsInCategory(EffectCategory category) const
{
    std::vector<EffectDescriptor> effects;
    
    auto it = categoryIndex.find(category);
    if (it != categoryIndex.end())
    {
        for (const auto& effectId : it->second)
        {
            auto regIt = effectRegistry.find(effectId);
            if (regIt != effectRegistry.end())
            {
                effects.push_back(regIt->second.descriptor);
            }
        }
    }
    
    return effects;
}

const EffectDescriptor* EffectLibrary::getEffectDescriptor(const String& effectId) const
{
    auto it = effectRegistry.find(effectId);
    if (it != effectRegistry.end())
        return &it->second.descriptor;
    
    return nullptr;
}

std::vector<EffectCategory> EffectLibrary::getAllCategories() const
{
    std::vector<EffectCategory> categories;
    
    for (const auto& pair : categoryIndex)
    {
        categories.push_back(pair.first);
    }
    
    return categories;
}

//==============================================================================
// Category Helpers

String EffectLibrary::getCategoryName(EffectCategory category)
{
    switch (category)
    {
        case EffectCategory::Drive:       return "Drive & Distortion";
        case EffectCategory::Modulation:  return "Modulation";
        case EffectCategory::Delay:       return "Delay";
        case EffectCategory::Reverb:      return "Reverb";
        case EffectCategory::Filter:      return "Filter & Wah";
        case EffectCategory::Dynamics:    return "Dynamics";
        case EffectCategory::EQ:          return "EQ";
        case EffectCategory::Pitch:       return "Pitch & Harmony";
        case EffectCategory::Amp:         return "Amplifiers";
        case EffectCategory::Cabinet:     return "Cabinets";
        case EffectCategory::Special:     return "Special Effects";
        default:                          return "Unknown";
    }
}

Colour EffectLibrary::getCategoryColor(EffectCategory category)
{
    switch (category)
    {
        case EffectCategory::Drive:       return Colour(255, 120, 50);   // Orange
        case EffectCategory::Modulation:  return Colour(150, 100, 255);  // Purple
        case EffectCategory::Delay:       return Colour(100, 200, 255);  // Light Blue
        case EffectCategory::Reverb:      return Colour(80, 120, 255);   // Deep Blue
        case EffectCategory::Filter:      return Colour(100, 255, 150);  // Green
        case EffectCategory::Dynamics:    return Colour(255, 200, 100);  // Yellow
        case EffectCategory::EQ:          return Colour(180, 180, 180);  // Gray
        case EffectCategory::Pitch:       return Colour(255, 100, 200);  // Pink
        case EffectCategory::Amp:         return Colour(200, 50, 50);    // Red
        case EffectCategory::Cabinet:     return Colour(120, 80, 60);    // Brown
        case EffectCategory::Special:     return Colour(200, 150, 255);  // Violet
        default:                          return Colours::white;
    }
}

//==============================================================================
// Effect Creation

std::unique_ptr<AudioModule> EffectLibrary::createEffect(const String& effectId)
{
    auto it = effectRegistry.find(effectId);
    if (it != effectRegistry.end())
    {
        return it->second.factory();
    }
    
    DBG("Error: Effect '" + effectId + "' not found in library!");
    return nullptr;
}

//==============================================================================
// Search & Filter

std::vector<EffectDescriptor> EffectLibrary::searchEffects(const String& keyword) const
{
    std::vector<EffectDescriptor> results;
    String lowerKeyword = keyword.toLowerCase();
    
    for (const auto& pair : effectRegistry)
    {
        const auto& desc = pair.second.descriptor;
        
        // Search in name
        if (desc.displayName.toLowerCase().contains(lowerKeyword))
        {
            results.push_back(desc);
            continue;
        }
        
        // Search in manufacturer
        if (desc.manufacturer.toLowerCase().contains(lowerKeyword))
        {
            results.push_back(desc);
            continue;
        }
        
        // Search in description
        if (desc.description.toLowerCase().contains(lowerKeyword))
        {
            results.push_back(desc);
            continue;
        }
        
        // Search in tags
        for (const auto& tag : desc.tags)
        {
            if (tag.toLowerCase().contains(lowerKeyword))
            {
                results.push_back(desc);
                break;
            }
        }
    }
    
    return results;
}

std::vector<EffectDescriptor> EffectLibrary::filterByTags(const StringArray& tags) const
{
    std::vector<EffectDescriptor> results;
    
    for (const auto& pair : effectRegistry)
    {
        const auto& desc = pair.second.descriptor;
        
        // Check if effect has any of the specified tags
        for (const auto& tag : tags)
        {
            if (desc.tags.contains(tag))
            {
                results.push_back(desc);
                break;
            }
        }
    }
    
    return results;
}

std::vector<EffectDescriptor> EffectLibrary::getFeaturedEffects() const
{
    // Return a curated list of featured effects
    std::vector<String> featured = {
        "tubeOverdrive",
        "chorus",
        "tapDelay",
        "plateReverb",
        "ts808",
        "univibe",
        "ce2Chorus",
        "dl4Delay"
    };
    
    std::vector<EffectDescriptor> results;
    
    for (const auto& effectId : featured)
    {
        auto it = effectRegistry.find(effectId);
        if (it != effectRegistry.end())
        {
            results.push_back(it->second.descriptor);
        }
    }
    
    return results;
}

//==============================================================================
// Statistics

int EffectLibrary::getEffectCountInCategory(EffectCategory category) const
{
    auto it = categoryIndex.find(category);
    if (it != categoryIndex.end())
        return static_cast<int>(it->second.size());
    
    return 0;
}

//==============================================================================
// Library Initialization

void EffectLibrary::initializeLibrary()
{
    DBG("Initializing Effect Library...");
    
    registerDriveEffects();
    registerModulationEffects();
    registerDelayEffects();
    registerReverbEffects();
    registerFilterEffects();
    registerDynamicsEffects();
    registerEQEffects();
    registerPitchEffects();
    registerAmpEffects();
    registerCabinetEffects();
    registerSpecialEffects();
    
    DBG("Effect Library initialized with " + String(getTotalEffectCount()) + " effects");
}

//==============================================================================
// Effect Registration Functions

void EffectLibrary::registerDriveEffects()
{
    // 1. Tube Overdrive (TS808 style)
    {
        EffectDescriptor desc;
        desc.id = "tubeOverdrive";
        desc.displayName = "Tube Overdrive";
        desc.manufacturer = "Classic";
        desc.category = EffectCategory::Drive;
        desc.pedalColor = Colour(255, 140, 50);
        desc.description = "Classic TS808 Tube Screamer style overdrive with mid-boost";
        desc.tags = {"overdrive", "ts808", "tube", "screamer", "blues", "classic"};
        desc.estimatedCPUCost = 0.3f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<TubeOverdrive>(); });
    }
    
    // 2. Distortion
    {
        EffectDescriptor desc;
        desc.id = "distortion";
        desc.displayName = "Distortion";
        desc.manufacturer = "Classic";
        desc.category = EffectCategory::Drive;
        desc.pedalColor = Colour(200, 50, 50);
        desc.description = "High-gain distortion with aggressive clipping";
        desc.tags = {"distortion", "heavy", "metal", "gain", "crunch"};
        desc.estimatedCPUCost = 0.3f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<Distortion>(); });
    }
    
    // TODO: Add remaining 16 drive effects in future phases
    // - RAT Distortion, Big Muff, Fuzz Face, Blues Driver, etc.
}

void EffectLibrary::registerModulationEffects()
{
    // 1. Chorus
    {
        EffectDescriptor desc;
        desc.id = "chorus";
        desc.displayName = "Analog Chorus";
        desc.manufacturer = "Classic";
        desc.category = EffectCategory::Modulation;
        desc.pedalColor = Colour(150, 100, 255);
        desc.description = "BBD analog-style chorus with multiple voices";
        desc.tags = {"chorus", "modulation", "analog", "bbd", "shimmer"};
        desc.estimatedCPUCost = 0.4f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<Chorus>(); });
    }
    
    // TODO: Add remaining 19 modulation effects in future phases
    // - Flanger, Phaser, Tremolo, Vibrato, Univibe, Rotary, etc.
}

void EffectLibrary::registerDelayEffects()
{
    // 1. Delay
    {
        EffectDescriptor desc;
        desc.id = "delay";
        desc.displayName = "Digital Delay";
        desc.manufacturer = "Classic";
        desc.category = EffectCategory::Delay;
        desc.pedalColor = Colour(100, 200, 255);
        desc.description = "Clean digital delay with multiple modes";
        desc.tags = {"delay", "echo", "repeat", "digital", "tape", "analog"};
        desc.estimatedCPUCost = 0.5f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<Delay>(); });
    }
    
    // TODO: Add remaining 14 delay effects in future phases
    // - Tape Delay, Analog Delay, Ping Pong, Multi-Tap, etc.
}

void EffectLibrary::registerReverbEffects()
{
    // 1. Reverb
    {
        EffectDescriptor desc;
        desc.id = "reverb";
        desc.displayName = "Studio Reverb";
        desc.manufacturer = "Classic";
        desc.category = EffectCategory::Reverb;
        desc.pedalColor = Colour(80, 120, 255);
        desc.description = "Algorithmic reverb with room, hall, and plate modes";
        desc.tags = {"reverb", "room", "hall", "plate", "ambience", "space"};
        desc.estimatedCPUCost = 0.7f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<ReverbEffect>(); });
    }
    
    // TODO: Add remaining 11 reverb effects in future phases
    // - Plate, Spring, Chamber, Shimmer, Convolution, etc.
}

void EffectLibrary::registerFilterEffects()
{
    // TODO: Add 10 filter effects in future phases
    // - Auto Wah, Envelope Filter, Wah Pedal, Low Pass, High Pass, etc.
}

void EffectLibrary::registerDynamicsEffects()
{
    // 1. Noise Gate
    {
        EffectDescriptor desc;
        desc.id = "noiseGate";
        desc.displayName = "Noise Gate";
        desc.manufacturer = "Studio";
        desc.category = EffectCategory::Dynamics;
        desc.pedalColor = Colour(255, 200, 100);
        desc.description = "Professional noise gate for eliminating unwanted noise";
        desc.tags = {"gate", "noise", "suppressor", "cleanup"};
        desc.estimatedCPUCost = 0.2f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<NoiseGate>(); });
    }
    
    // 2. Compressor
    {
        EffectDescriptor desc;
        desc.id = "compressor";
        desc.displayName = "Studio Compressor";
        desc.manufacturer = "Studio";
        desc.category = EffectCategory::Dynamics;
        desc.pedalColor = Colour(255, 200, 100);
        desc.description = "Transparent compressor for evening out dynamics";
        desc.tags = {"compressor", "dynamics", "sustain", "leveling"};
        desc.estimatedCPUCost = 0.3f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<Compressor>(); });
    }
    
    // TODO: Add remaining 8 dynamics effects in future phases
    // - Limiter, Expander, Multiband Compressor, etc.
}

void EffectLibrary::registerEQEffects()
{
    // 1. Parametric EQ
    {
        EffectDescriptor desc;
        desc.id = "parametricEQ";
        desc.displayName = "Parametric EQ";
        desc.manufacturer = "Studio";
        desc.category = EffectCategory::EQ;
        desc.pedalColor = Colour(180, 180, 180);
        desc.description = "Multi-band parametric equalizer";
        desc.tags = {"eq", "equalizer", "tone", "shaping"};
        desc.estimatedCPUCost = 0.4f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<EQ>(); });
    }
    
    // TODO: Add remaining 7 EQ effects in future phases
    // - Graphic EQ, Bass/Mid/Treble, Tilt EQ, etc.
}

void EffectLibrary::registerPitchEffects()
{
    // TODO: Add 10 pitch effects in future phases
    // - Pitch Shifter, Harmonizer, Octaver, Whammy, etc.
}

void EffectLibrary::registerAmpEffects()
{
    // 1. Amp Simulator
    {
        EffectDescriptor desc;
        desc.id = "ampSimulator";
        desc.displayName = "Alpha Amp";
        desc.manufacturer = "Alpha";
        desc.category = EffectCategory::Amp;
        desc.pedalColor = Colour(200, 50, 50);
        desc.description = "Multi-channel amp simulator with 3 channels";
        desc.tags = {"amp", "amplifier", "preamp", "gain", "distortion"};
        desc.estimatedCPUCost = 0.6f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<AmpSimulator>(); });
    }
    
    // TODO: Add remaining 14 amp models in future phases
    // - Marshall, Fender, Vox, Mesa, etc.
}

void EffectLibrary::registerCabinetEffects()
{
    // 1. Cabinet IR
    {
        EffectDescriptor desc;
        desc.id = "cabinetIR";
        desc.displayName = "Cabinet Simulator";
        desc.manufacturer = "Studio";
        desc.category = EffectCategory::Cabinet;
        desc.pedalColor = Colour(120, 80, 60);
        desc.description = "Cabinet simulator with impulse responses";
        desc.tags = {"cabinet", "cab", "speaker", "ir", "impulse"};
        desc.estimatedCPUCost = 0.5f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<CabinetIR>(); });
    }
    
    // TODO: Add remaining 11 cabinet types in future phases
    // - Various speaker configurations and microphone placements
}

void EffectLibrary::registerSpecialEffects()
{
    // TODO: Add 15 special effects in future phases
    // - Looper, Ring Modulator, Bit Crusher, Sample & Hold, etc.
}

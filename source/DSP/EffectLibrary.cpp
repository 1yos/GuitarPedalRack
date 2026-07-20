#include "EffectLibrary.h"

// Include all effect headers
#include "Drive/TubeOverdrive.h"
#include "Drive/Distortion.h"
#include "Drive/TubeScreamer808.h"
#include "Drive/ProCoRAT.h"
#include "Drive/BossDS1.h"
#include "Drive/BigMuffPi.h"
#include "Drive/KlonCentaur.h"
#include "Drive/BossSD1.h"
#include "Drive/FuzzFace.h"
#include "Drive/BluesDriver.h"
#include "Drive/TubeScreamerTS9.h"
#include "Drive/TubeScreamerMini.h"
#include "Drive/ProCoRAT2.h"
#include "Drive/TurboRAT.h"
#include "Drive/BigMuffRussian.h"
#include "Drive/FuzzFaceSilicon.h"
#include "Drive/MetalZone.h"
#include "Drive/TimmyOverdrive.h"
#include "Modulation/Chorus.h"
#include "Modulation/ChorusEnsemble.h"
#include "Modulation/ChorusDimension.h"
#include "Modulation/BossCE2.h"
#include "Modulation/BossCE1.h"
#include "Modulation/Phaser90.h"
#include "Modulation/Phaser4Stage.h"
#include "Modulation/Phaser6Stage.h"
#include "Modulation/FlangerBasic.h"
#include "Modulation/FlangerElectric.h"
#include "Modulation/TremoloOptical.h"
#include "Modulation/TremoloHarmonic.h"
#include "Modulation/VibratoClassic.h"
#include "Modulation/VibratoVintage.h"
#include "Modulation/RingModulator.h"
#include "Modulation/RingModAnalog.h"
#include "Modulation/RotarySpeaker.h"
#include "TimeFX/Delay.h"
#include "TimeFX/TapeDelay.h"
#include "TimeFX/AnalogDelay.h"
#include "TimeFX/PingPongDelay.h"
#include "TimeFX/MultiTapDelay.h"
#include "TimeFX/ReverseDelay.h"
#include "TimeFX/SlapbackDelay.h"
#include "TimeFX/DualDelay.h"
#include "TimeFX/DottedDelay.h"
#include "TimeFX/RhythmicDelay.h"
#include "TimeFX/MemoryMan.h"
#include "TimeFX/Echoplex.h"
#include "TimeFX/SpaceEcho.h"
#include "TimeFX/BossDD3.h"
#include "TimeFX/Reverb.h"
#include "TimeFX/PlateReverb.h"
#include "TimeFX/SpringReverb.h"
#include "TimeFX/HallReverb.h"
#include "TimeFX/RoomReverb.h"
#include "TimeFX/ChamberReverb.h"
#include "TimeFX/ShimmerReverb.h"
#include "TimeFX/ReverseReverb.h"
#include "TimeFX/GatedReverb.h"
#include "TimeFX/ModulatedReverb.h"
#include "Dynamics/NoiseGate.h"
#include "Dynamics/Compressor.h"
#include "Dynamics/FETCompressor.h"
#include "Dynamics/VCACompressor.h"
#include "Dynamics/OpticalCompressor.h"
#include "Dynamics/TubeCompressor.h"
#include "Dynamics/Limiter.h"
#include "Dynamics/Expander.h"
#include "Dynamics/MultibandCompressor.h"
#include "Dynamics/DeEsser.h"
#include "Amp/AmpSimulator.h"
#include "Cabinet/CabinetIR.h"
#include "Utility/EQ.h"
#include "Utility/GraphicEQ10Band.h"
#include "Utility/GraphicEQ31Band.h"
#include "Utility/ShelvingEQ.h"
#include "Utility/HighPassEQ.h"
#include "Utility/LowPassEQ.h"
#include "Utility/ToneStackFender.h"
#include "Utility/ToneStackMarshall.h"
#include "Filter/AutoWah.h"
#include "Filter/EnvelopeFilter.h"
#include "Filter/WahPedal.h"
#include "Filter/TalkBox.h"
#include "Filter/LowPassFilter.h"
#include "Filter/HighPassFilter.h"
#include "Filter/BandPassFilter.h"
#include "Filter/NotchFilter.h"
#include "Filter/FormantFilter.h"
#include "Filter/StateVariableFilter.h"
#include "Pitch/PitchShifter.h"
#include "Pitch/Harmonizer.h"
#include "Pitch/OctaverUp.h"
#include "Pitch/OctaverDown.h"
#include "Pitch/WhammyPedal.h"
#include "Pitch/DetuneChorus.h"
#include "Pitch/PitchCorrection.h"
#include "Pitch/FormantShifter.h"
#include "Pitch/PitchVibrato.h"
#include "Pitch/IntelligentHarmony.h"
#include "Filter/TalkBox.h"
#include "Filter/LowPassFilter.h"
#include "Filter/HighPassFilter.h"
#include "Filter/BandPassFilter.h"
#include "Filter/NotchFilter.h"
#include "Filter/FormantFilter.h"
#include "Filter/StateVariableFilter.h"

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
    
    // 3. Tube Screamer 808
    {
        EffectDescriptor desc;
        desc.id = "tubeScreamer808";
        desc.displayName = "TS-808 Tube Screamer";
        desc.manufacturer = "Ibanez";
        desc.category = EffectCategory::Drive;
        desc.pedalColor = Colour(80, 180, 80);
        desc.description = "Authentic TS-808 Tube Screamer with asymmetric clipping and mid-hump";
        desc.tags = {"ts808", "tubescreamer", "ibanez", "overdrive", "blues", "rock", "mid-boost"};
        desc.estimatedCPUCost = 0.35f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<TubeScreamer808>(); });
    }
    
    // 4. ProCo RAT
    {
        EffectDescriptor desc;
        desc.id = "proCoRAT";
        desc.displayName = "ProCo RAT";
        desc.manufacturer = "ProCo";
        desc.category = EffectCategory::Drive;
        desc.pedalColor = Colour(40, 40, 40);
        desc.description = "Aggressive hard-clipping distortion with filter control";
        desc.tags = {"rat", "proco", "distortion", "hard", "aggressive", "filter"};
        desc.estimatedCPUCost = 0.30f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<ProCoRAT>(); });
    }
    
    // 5. Boss DS-1
    {
        EffectDescriptor desc;
        desc.id = "bossDS1";
        desc.displayName = "Boss DS-1 Distortion";
        desc.manufacturer = "Boss";
        desc.category = EffectCategory::Drive;
        desc.pedalColor = Colour(255, 140, 0);
        desc.description = "Hard-edged distortion with asymmetric clipping and active tone control";
        desc.tags = {"boss", "ds1", "distortion", "orange", "hard", "rock"};
        desc.estimatedCPUCost = 0.35f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<BossDS1>(); });
    }
    
    // 6. Big Muff Pi
    {
        EffectDescriptor desc;
        desc.id = "bigMuffPi";
        desc.displayName = "Big Muff Pi";
        desc.manufacturer = "Electro-Harmonix";
        desc.category = EffectCategory::Drive;
        desc.pedalColor = Colour(150, 150, 200);
        desc.description = "Thick, saturated fuzz with sustain and scooped mids";
        desc.tags = {"bigmuff", "muff", "fuzz", "ehx", "sustain", "scooped"};
        desc.estimatedCPUCost = 0.40f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<BigMuffPi>(); });
    }
    
    // 7. Klon Centaur
    {
        EffectDescriptor desc;
        desc.id = "klonCentaur";
        desc.displayName = "Klon Centaur";
        desc.manufacturer = "Klon";
        desc.category = EffectCategory::Drive;
        desc.pedalColor = Colour(200, 180, 130);
        desc.description = "Legendary transparent overdrive with clean blend and treble boost";
        desc.tags = {"klon", "centaur", "transparent", "overdrive", "germanium", "boutique"};
        desc.estimatedCPUCost = 0.30f;
        desc.isPremium = true;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<KlonCentaur>(); });
    }
    
    // 8. Boss SD-1 Super Overdrive
    {
        EffectDescriptor desc;
        desc.id = "bossSD1";
        desc.displayName = "Boss SD-1 Super Overdrive";
        desc.manufacturer = "Boss";
        desc.category = EffectCategory::Drive;
        desc.pedalColor = Colour(255, 200, 50);
        desc.description = "TS-style overdrive with harder clipping and more gain";
        desc.tags = {"boss", "sd1", "overdrive", "yellow", "mid", "tubescreamer"};
        desc.estimatedCPUCost = 0.30f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<BossSD1>(); });
    }
    
    // 9. Fuzz Face
    {
        EffectDescriptor desc;
        desc.id = "fuzzFace";
        desc.displayName = "Fuzz Face (Germanium)";
        desc.manufacturer = "Dallas Arbiter";
        desc.category = EffectCategory::Drive;
        desc.pedalColor = Colour(200, 200, 200);
        desc.description = "Classic two-transistor germanium fuzz with warm, vintage character";
        desc.tags = {"fuzzface", "fuzz", "germanium", "vintage", "hendrix", "classic"};
        desc.estimatedCPUCost = 0.35f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<FuzzFace>(); });
    }
    
    // 10. Boss BD-2 Blues Driver
    {
        EffectDescriptor desc;
        desc.id = "bluesDriver";
        desc.displayName = "Boss BD-2 Blues Driver";
        desc.manufacturer = "Boss";
        desc.category = EffectCategory::Drive;
        desc.pedalColor = Colour(100, 150, 255);
        desc.description = "Warm, tube-like overdrive for blues and classic rock";
        desc.tags = {"boss", "bd2", "blues", "overdrive", "warm", "tube", "vintage"};
        desc.estimatedCPUCost = 0.35f;
        desc.isPremium = false;
        desc.version = 1;
        
        registerEffect(desc, []() { return std::make_unique<BluesDriver>(); });
    }
    
    // 11-18: Remaining drive effects
    registerEffect({"tubeScreamerTS9", "Tube Screamer TS9", "Ibanez", EffectCategory::Drive, Colour(100, 200, 100), "TS9 variant with harder clipping", {"ts9", "tubescreamer", "overdrive"}, 0.30f, false, 1, {}}, []() { return std::make_unique<TubeScreamerTS9>(); });
    registerEffect({"tubeScreamerMini", "Tube Screamer Mini", "Ibanez", EffectCategory::Drive, Colour(90, 170, 90), "Compact TS variant", {"tsmini", "tubescreamer", "compact"}, 0.28f, false, 1, {}}, []() { return std::make_unique<TubeScreamerMini>(); });
    registerEffect({"proCoRAT2", "ProCo RAT 2", "ProCo", EffectCategory::Drive, Colour(50, 50, 50), "RAT variant with more gain", {"rat2", "proco", "distortion"}, 0.30f, false, 1, {}}, []() { return std::make_unique<ProCoRAT2>(); });
    registerEffect({"turboRAT", "Turbo RAT", "ProCo", EffectCategory::Drive, Colour(200, 30, 30), "High-gain RAT with LED clipping", {"turborat", "rat", "led", "high-gain"}, 0.32f, false, 1, {}}, []() { return std::make_unique<TurboRAT>(); });
    registerEffect({"bigMuffRussian", "Big Muff Russian", "Electro-Harmonix", EffectCategory::Drive, Colour(120, 120, 180), "Warmer Big Muff variant", {"bigmuff", "russian", "fuzz", "warm"}, 0.38f, false, 1, {}}, []() { return std::make_unique<BigMuffRussian>(); });
    registerEffect({"fuzzFaceSilicon", "Fuzz Face Silicon", "Dallas Arbiter", EffectCategory::Drive, Colour(180, 180, 180), "Brighter silicon fuzz", {"fuzzface", "silicon", "fuzz", "bright"}, 0.35f, false, 1, {}}, []() { return std::make_unique<FuzzFaceSilicon>(); });
    registerEffect({"metalZone", "Boss MT-2 Metal Zone", "Boss", EffectCategory::Drive, Colour(255, 50, 150), "Extreme high-gain metal distortion", {"metalzone", "mt2", "metal", "high-gain"}, 0.40f, false, 1, {}}, []() { return std::make_unique<MetalZone>(); });
    registerEffect({"timmyOverdrive", "Timmy Overdrive", "Paul Cochrane", EffectCategory::Drive, Colour(240, 240, 200), "Transparent overdrive with bass/treble controls", {"timmy", "transparent", "overdrive", "boutique"}, 0.30f, true, 1, {}}, []() { return std::make_unique<TimmyOverdrive>(); });
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
    
    // 2-5: Chorus effects
    registerEffect({"chorusEnsemble", "Chorus Ensemble", "Classic", EffectCategory::Modulation, Colour(170, 120, 255), "Multi-voice ensemble chorus with 3 LFOs", {"chorus", "ensemble", "multi-voice", "rich"}, 0.45f, false, 1, {}}, []() { return std::make_unique<ChorusEnsemble>(); });
    registerEffect({"chorusDimension", "Chorus Dimension", "Roland", EffectCategory::Modulation, Colour(130, 80, 220), "Stereo widening dimension chorus", {"dimension", "chorus", "stereo", "wide"}, 0.40f, false, 1, {}}, []() { return std::make_unique<ChorusDimension>(); });
    registerEffect({"bossCE2", "Boss CE-2 Chorus", "Boss", EffectCategory::Modulation, Colour(100, 180, 255), "Classic analog chorus pedal", {"boss", "ce2", "chorus", "analog", "vintage"}, 0.38f, false, 1, {}}, []() { return std::make_unique<BossCE2>(); });
    registerEffect({"bossCE1", "Boss CE-1", "Boss", EffectCategory::Modulation, Colour(120, 200, 255), "Vintage chorus/vibrato ensemble", {"boss", "ce1", "chorus", "vibrato", "ensemble"}, 0.42f, false, 1, {}}, []() { return std::make_unique<BossCE1>(); });
    
    // 6-8: Phaser effects
    registerEffect({"phaser90", "Phaser 90", "MXR", EffectCategory::Modulation, Colour(255, 140, 50), "Classic 4-stage phaser", {"phaser", "mxr", "90", "phase", "sweep"}, 0.35f, false, 1, {}}, []() { return std::make_unique<Phaser90>(); });
    registerEffect({"phaser4Stage", "Phaser 4-Stage", "Classic", EffectCategory::Modulation, Colour(240, 120, 40), "Smooth 4-stage phaser with feedback", {"phaser", "4-stage", "smooth"}, 0.36f, false, 1, {}}, []() { return std::make_unique<Phaser4Stage>(); });
    registerEffect({"phaser6Stage", "Phaser 6-Stage", "Classic", EffectCategory::Modulation, Colour(230, 100, 30), "Deep 6-stage phaser", {"phaser", "6-stage", "deep"}, 0.40f, false, 1, {}}, []() { return std::make_unique<Phaser6Stage>(); });
    
    // 9-10: Flanger effects
    registerEffect({"flangerBasic", "Flanger Basic", "Classic", EffectCategory::Modulation, Colour(100, 220, 180), "Classic flanger with feedback", {"flanger", "jet", "sweep"}, 0.38f, false, 1, {}}, []() { return std::make_unique<FlangerBasic>(); });
    registerEffect({"flangerElectric", "Electric Flanger", "Electro-Harmonix", EffectCategory::Modulation, Colour(80, 200, 160), "Through-zero flanging effect", {"flanger", "electric", "through-zero"}, 0.42f, false, 1, {}}, []() { return std::make_unique<FlangerElectric>(); });
    
    // 11-12: Tremolo effects
    registerEffect({"tremoloOptical", "Tremolo Optical", "Classic", EffectCategory::Modulation, Colour(255, 100, 100), "Smooth optical tremolo", {"tremolo", "optical", "amplitude"}, 0.28f, false, 1, {}}, []() { return std::make_unique<TremoloOptical>(); });
    registerEffect({"tremoloHarmonic", "Tremolo Harmonic", "Vintage", EffectCategory::Modulation, Colour(240, 80, 80), "Harmonic tremolo with dual modulation", {"tremolo", "harmonic", "brownface"}, 0.32f, false, 1, {}}, []() { return std::make_unique<TremoloHarmonic>(); });
    
    // 13-14: Vibrato effects
    registerEffect({"vibratoClassic", "Vibrato Classic", "Classic", EffectCategory::Modulation, Colour(150, 255, 200), "Pure pitch vibrato", {"vibrato", "pitch", "modulation"}, 0.35f, false, 1, {}}, []() { return std::make_unique<VibratoClassic>(); });
    registerEffect({"vibratoVintage", "Vibrato Vintage", "Vintage", EffectCategory::Modulation, Colour(130, 235, 180), "Vintage vibrato with triangle/sine wave", {"vibrato", "vintage", "warble"}, 0.37f, false, 1, {}}, []() { return std::make_unique<VibratoVintage>(); });
    
    // 15-16: Ring Modulator effects
    registerEffect({"ringModulator", "Ring Modulator", "Classic", EffectCategory::Modulation, Colour(200, 50, 200), "Classic ring modulation effect", {"ring", "modulator", "metallic", "bell"}, 0.30f, false, 1, {}}, []() { return std::make_unique<RingModulator>(); });
    registerEffect({"ringModAnalog", "Ring Mod Analog", "Vintage", EffectCategory::Modulation, Colour(180, 40, 180), "Analog ring mod with waveshaping", {"ring", "analog", "carrier"}, 0.32f, false, 1, {}}, []() { return std::make_unique<RingModAnalog>(); });
    
    // 17: Rotary Speaker
    registerEffect({"rotarySpeaker", "Rotary Speaker", "Leslie", EffectCategory::Modulation, Colour(140, 100, 200), "Leslie-style rotary speaker simulation", {"rotary", "leslie", "doppler", "speaker"}, 0.45f, false, 1, {}}, []() { return std::make_unique<RotarySpeaker>(); });
}

void EffectLibrary::registerDelayEffects()
{
    // 1. Digital Delay (Phase 1)
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
    
    // 2-15: New delay effects
    registerEffect({"tapeDelay", "Tape Delay", "Classic", EffectCategory::Delay, Colour(160, 140, 120), "Vintage tape delay with wow & flutter", {"tape", "delay", "vintage", "wow", "flutter"}, 0.52f, false, 1, {}}, []() { return std::make_unique<TapeDelay>(); });
    registerEffect({"analogDelay", "Analog Delay", "Classic", EffectCategory::Delay, Colour(120, 180, 220), "BBD-style analog delay with modulation", {"analog", "delay", "bbd", "modulation", "warm"}, 0.48f, false, 1, {}}, []() { return std::make_unique<AnalogDelay>(); });
    registerEffect({"pingPongDelay", "Ping Pong Delay", "Classic", EffectCategory::Delay, Colour(80, 220, 180), "Stereo ping-pong delay effect", {"pingpong", "delay", "stereo", "bounce"}, 0.50f, false, 1, {}}, []() { return std::make_unique<PingPongDelay>(); });
    registerEffect({"multiTapDelay", "Multi-Tap Delay", "Classic", EffectCategory::Delay, Colour(140, 160, 240), "Multiple delay taps for rhythmic effects", {"multitap", "delay", "rhythm", "complex"}, 0.55f, false, 1, {}}, []() { return std::make_unique<MultiTapDelay>(); });
    registerEffect({"reverseDelay", "Reverse Delay", "Creative", EffectCategory::Delay, Colour(200, 100, 255), "Reverse playback delay effect", {"reverse", "delay", "backwards", "psychedelic"}, 0.53f, false, 1, {}}, []() { return std::make_unique<ReverseDelay>(); });
    registerEffect({"slapbackDelay", "Slapback Delay", "Vintage", EffectCategory::Delay, Colour(220, 180, 140), "Short rockabilly-style slapback echo", {"slapback", "delay", "rockabilly", "elvis"}, 0.35f, false, 1, {}}, []() { return std::make_unique<SlapbackDelay>(); });
    registerEffect({"dualDelay", "Dual Delay", "Classic", EffectCategory::Delay, Colour(100, 200, 200), "Independent left/right delay times", {"dual", "delay", "stereo", "independent"}, 0.51f, false, 1, {}}, []() { return std::make_unique<DualDelay>(); });
    registerEffect({"dottedDelay", "Dotted Eighth Delay", "Rhythmic", EffectCategory::Delay, Colour(180, 140, 240), "Dotted eighth note delay timing", {"dotted", "delay", "eighth", "u2", "edge"}, 0.46f, false, 1, {}}, []() { return std::make_unique<DottedDelay>(); });
    registerEffect({"rhythmicDelay", "Rhythmic Delay", "Creative", EffectCategory::Delay, Colour(240, 120, 180), "Pattern-based rhythmic delay", {"rhythmic", "delay", "pattern", "groove"}, 0.49f, false, 1, {}}, []() { return std::make_unique<RhythmicDelay>(); });
    registerEffect({"memoryMan", "Memory Man", "Electro-Harmonix", EffectCategory::Delay, Colour(150, 150, 200), "Classic analog delay with chorus modulation", {"memoryman", "ehx", "analog", "modulation"}, 0.54f, true, 1, {}}, []() { return std::make_unique<MemoryMan>(); });
    registerEffect({"echoplex", "Echoplex EP-3", "Maestro", EffectCategory::Delay, Colour(200, 160, 120), "Legendary tape echo with preamp", {"echoplex", "tape", "maestro", "vintage"}, 0.52f, true, 1, {}}, []() { return std::make_unique<Echoplex>(); });
    registerEffect({"spaceEcho", "Roland RE-201 Space Echo", "Roland", EffectCategory::Delay, Colour(140, 120, 100), "Iconic tape delay with spring reverb", {"spaceecho", "roland", "re201", "tape", "spring"}, 0.57f, true, 1, {}}, []() { return std::make_unique<SpaceEcho>(); });
    registerEffect({"bossDD3", "Boss DD-3 Digital Delay", "Boss", EffectCategory::Delay, Colour(100, 220, 240), "Digital delay with hold mode", {"boss", "dd3", "digital", "hold"}, 0.47f, false, 1, {}}, []() { return std::make_unique<BossDD3>(); });
}

void EffectLibrary::registerReverbEffects()
{
    // 1. Studio Reverb (Phase 1)
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
    
    // 2-11: New reverb effects
    registerEffect({"plateReverb", "Plate Reverb", "Studio", EffectCategory::Reverb, Colour(120, 140, 255), "Dense, bright plate reverb", {"plate", "reverb", "studio", "dense", "bright"}, 0.68f, false, 1, {}}, []() { return std::make_unique<PlateReverb>(); });
    registerEffect({"springReverb", "Spring Reverb", "Vintage", EffectCategory::Reverb, Colour(180, 140, 100), "Metallic spring reverb tank", {"spring", "reverb", "surf", "metallic", "vintage"}, 0.55f, false, 1, {}}, []() { return std::make_unique<SpringReverb>(); });
    registerEffect({"hallReverb", "Hall Reverb", "Studio", EffectCategory::Reverb, Colour(100, 160, 255), "Spacious concert hall reverb", {"hall", "reverb", "large", "spacious", "concert"}, 0.75f, false, 1, {}}, []() { return std::make_unique<HallReverb>(); });
    registerEffect({"roomReverb", "Room Reverb", "Studio", EffectCategory::Reverb, Colour(140, 180, 220), "Small to medium room ambience", {"room", "reverb", "ambience", "natural"}, 0.60f, false, 1, {}}, []() { return std::make_unique<RoomReverb>(); });
    registerEffect({"chamberReverb", "Chamber Reverb", "Studio", EffectCategory::Reverb, Colour(160, 140, 200), "Classic echo chamber sound", {"chamber", "reverb", "echo", "studio"}, 0.65f, false, 1, {}}, []() { return std::make_unique<ChamberReverb>(); });
    registerEffect({"shimmerReverb", "Shimmer Reverb", "Creative", EffectCategory::Reverb, Colour(200, 140, 255), "Ethereal reverb with octave-up", {"shimmer", "reverb", "octave", "ambient", "ethereal"}, 0.72f, false, 1, {}}, []() { return std::make_unique<ShimmerReverb>(); });
    registerEffect({"reverseReverb", "Reverse Reverb", "Creative", EffectCategory::Reverb, Colour(255, 120, 200), "Backwards swell reverb effect", {"reverse", "reverb", "backwards", "swell"}, 0.58f, false, 1, {}}, []() { return std::make_unique<ReverseReverb>(); });
    registerEffect({"gatedReverb", "Gated Reverb", "Studio", EffectCategory::Reverb, Colour(220, 100, 180), "80s-style gated reverb", {"gated", "reverb", "80s", "drums", "phil collins"}, 0.66f, false, 1, {}}, []() { return std::make_unique<GatedReverb>(); });
    registerEffect({"modulatedReverb", "Modulated Reverb", "Creative", EffectCategory::Reverb, Colour(180, 180, 255), "Chorus-modulated reverb", {"modulated", "reverb", "chorus", "warble"}, 0.70f, false, 1, {}}, []() { return std::make_unique<ModulatedReverb>(); });
}

void EffectLibrary::registerFilterEffects()
{
    // 1-10: Filter effects
    registerEffect({"autoWah", "Auto Wah", "Classic", EffectCategory::Filter, Colour(255, 180, 100), "Envelope-controlled wah effect", {"autowah", "wah", "envelope", "funk", "sweep"}, 0.32f, false, 1, {}}, []() { return std::make_unique<AutoWah>(); });
    registerEffect({"envelopeFilter", "Envelope Filter", "Mu-Tron", EffectCategory::Filter, Colour(255, 200, 120), "Touch-sensitive filter sweep", {"envelope", "filter", "mutron", "funk", "quack"}, 0.35f, false, 1, {}}, []() { return std::make_unique<EnvelopeFilter>(); });
    registerEffect({"wahPedal", "Wah Pedal", "Cry Baby", EffectCategory::Filter, Colour(200, 100, 100), "Classic wah-wah pedal", {"wah", "crybaby", "sweep", "vox", "hendrix"}, 0.30f, false, 1, {}}, []() { return std::make_unique<WahPedal>(); });
    registerEffect({"talkBox", "Talk Box", "Heil", EffectCategory::Filter, Colour(220, 140, 180), "Voice synthesis effect", {"talkbox", "vocoder", "formant", "voice", "peter frampton"}, 0.40f, false, 1, {}}, []() { return std::make_unique<TalkBox>(); });
    registerEffect({"lowPassFilter", "Low Pass Filter", "Classic", EffectCategory::Filter, Colour(100, 150, 255), "Smooth low-pass filtering", {"lowpass", "lpf", "filter", "smooth"}, 0.28f, false, 1, {}}, []() { return std::make_unique<LowPassFilter>(); });
    registerEffect({"highPassFilter", "High Pass Filter", "Classic", EffectCategory::Filter, Colour(255, 150, 100), "Clean high-pass filtering", {"highpass", "hpf", "filter", "rumble"}, 0.28f, false, 1, {}}, []() { return std::make_unique<HighPassFilter>(); });
    registerEffect({"bandPassFilter", "Band Pass Filter", "Classic", EffectCategory::Filter, Colour(150, 255, 150), "Telephone/radio effect", {"bandpass", "bpf", "filter", "telephone"}, 0.30f, false, 1, {}}, []() { return std::make_unique<BandPassFilter>(); });
    registerEffect({"notchFilter", "Notch Filter", "Classic", EffectCategory::Filter, Colour(180, 180, 255), "Narrow frequency rejection", {"notch", "filter", "rejection", "hum"}, 0.29f, false, 1, {}}, []() { return std::make_unique<NotchFilter>(); });
    registerEffect({"formantFilter", "Formant Filter", "Vintage", EffectCategory::Filter, Colour(255, 140, 200), "Vowel sound formant synthesis", {"formant", "vowel", "voice", "synthesis"}, 0.38f, false, 1, {}}, []() { return std::make_unique<FormantFilter>(); });
    registerEffect({"stateVariableFilter", "State Variable Filter", "Moog", EffectCategory::Filter, Colour(200, 120, 255), "Multi-mode state variable filter", {"svf", "filter", "moog", "multimode"}, 0.33f, false, 1, {}}, []() { return std::make_unique<StateVariableFilter>(); });
}

void EffectLibrary::registerDynamicsEffects()
{
    // 1. Noise Gate (Phase 1)
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
    
    // 2. Compressor (Phase 1)
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
    
    // 3-10: New dynamics effects (Phase 2)
    registerEffect({"fetCompressor", "FET Compressor", "1176-Style", EffectCategory::Dynamics, Colour(240, 180, 80), "Fast FET compressor for aggressive peak control", {"fet", "compressor", "1176", "fast", "peak"}, 0.32f, false, 1, {}}, []() { return std::make_unique<FETCompressor>(); });
    registerEffect({"vcaCompressor", "VCA Compressor", "dbx-Style", EffectCategory::Dynamics, Colour(220, 190, 90), "Clean VCA compressor for transparent dynamics", {"vca", "compressor", "dbx", "transparent", "clean"}, 0.31f, false, 1, {}}, []() { return std::make_unique<VCACompressor>(); });
    registerEffect({"opticalCompressor", "Optical Compressor", "LA-2A Style", EffectCategory::Dynamics, Colour(200, 170, 100), "Smooth optical compressor with natural release", {"optical", "compressor", "la2a", "smooth", "musical"}, 0.33f, false, 1, {}}, []() { return std::make_unique<OpticalCompressor>(); });
    registerEffect({"tubeCompressor", "Tube Compressor", "Vintage", EffectCategory::Dynamics, Colour(230, 160, 70), "Warm tube compressor with harmonic color", {"tube", "compressor", "vintage", "warm", "color"}, 0.34f, false, 1, {}}, []() { return std::make_unique<TubeCompressor>(); });
    registerEffect({"limiter", "Brick Wall Limiter", "Studio", EffectCategory::Dynamics, Colour(255, 100, 100), "Transparent limiter for peak protection", {"limiter", "brickwall", "peak", "protection", "ceiling"}, 0.28f, false, 1, {}}, []() { return std::make_unique<Limiter>(); });
    registerEffect({"expander", "Expander", "Studio", EffectCategory::Dynamics, Colour(180, 220, 140), "Upward expander for increasing dynamic range", {"expander", "dynamics", "range", "upward"}, 0.30f, false, 1, {}}, []() { return std::make_unique<Expander>(); });
    registerEffect({"multibandCompressor", "Multiband Compressor", "Studio", EffectCategory::Dynamics, Colour(255, 180, 120), "3-band multiband compressor for frequency-specific control", {"multiband", "compressor", "crossover", "frequency", "mastering"}, 0.42f, false, 1, {}}, []() { return std::make_unique<MultibandCompressor>(); });
    registerEffect({"deEsser", "De-Esser", "Studio", EffectCategory::Dynamics, Colour(200, 200, 240), "Intelligent sibilance reduction for vocals and guitars", {"deesser", "sibilance", "vocals", "highs", "tame"}, 0.29f, false, 1, {}}, []() { return std::make_unique<DeEsser>(); });
}

void EffectLibrary::registerEQEffects()
{
    // 1. Parametric EQ (Phase 1)
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
    
    // 2-8: New EQ effects (Phase 2)
    registerEffect({"graphicEQ10Band", "Graphic EQ (10-Band)", "Studio", EffectCategory::EQ, Colour(200, 200, 200), "10-band graphic equalizer with ISO frequencies", {"graphic", "eq", "10band", "iso", "live"}, 0.45f, false, 1, {}}, []() { return std::make_unique<GraphicEQ10Band>(); });
    registerEffect({"graphicEQ31Band", "Graphic EQ (31-Band)", "Studio", EffectCategory::EQ, Colour(220, 220, 220), "31-band 1/3 octave graphic equalizer", {"graphic", "eq", "31band", "octave", "mastering"}, 0.65f, false, 1, {}}, []() { return std::make_unique<GraphicEQ31Band>(); });
    registerEffect({"shelvingEQ", "Shelving EQ", "Studio", EffectCategory::EQ, Colour(160, 180, 200), "Low and high shelf equalizer", {"shelving", "eq", "bass", "treble", "tilt"}, 0.35f, false, 1, {}}, []() { return std::make_unique<ShelvingEQ>(); });
    registerEffect({"highPassEQ", "High-Pass Filter EQ", "Studio", EffectCategory::EQ, Colour(255, 200, 150), "High-pass filter with resonance (20Hz-2kHz)", {"highpass", "hpf", "eq", "rumble", "cut"}, 0.32f, false, 1, {}}, []() { return std::make_unique<HighPassEQ>(); });
    registerEffect({"lowPassEQ", "Low-Pass Filter EQ", "Studio", EffectCategory::EQ, Colour(150, 200, 255), "Low-pass filter with resonance (200Hz-20kHz)", {"lowpass", "lpf", "eq", "smooth", "roll-off"}, 0.32f, false, 1, {}}, []() { return std::make_unique<LowPassEQ>(); });
    registerEffect({"toneStackFender", "Fender Tone Stack", "Fender", EffectCategory::EQ, Colour(240, 200, 140), "Classic Fender Bassman/Twin tone stack", {"fender", "tonestack", "bassman", "twin", "vintage"}, 0.38f, false, 1, {}}, []() { return std::make_unique<ToneStackFender>(); });
    registerEffect({"toneStackMarshall", "Marshall Tone Stack", "Marshall", EffectCategory::EQ, Colour(140, 140, 140), "Classic Marshall JCM800 tone stack with presence", {"marshall", "tonestack", "jcm800", "british", "rock"}, 0.40f, false, 1, {}}, []() { return std::make_unique<ToneStackMarshall>(); });
}

void EffectLibrary::registerPitchEffects()
{
    // 1-10: Pitch effects
    registerEffect({"pitchShifter", "Pitch Shifter", "Studio", EffectCategory::Pitch, Colour(255, 100, 200), "Basic pitch shifter (±12 semitones)", {"pitch", "shifter", "transpose", "semitone"}, 0.55f, false, 1, {}}, []() { return std::make_unique<PitchShifter>(); });
    registerEffect({"harmonizer", "Harmonizer", "Studio", EffectCategory::Pitch, Colour(255, 120, 220), "Intelligent harmonizer with musical intervals", {"harmonizer", "harmony", "interval", "music"}, 0.58f, false, 1, {}}, []() { return std::make_unique<Harmonizer>(); });
    registerEffect({"octaverUp", "Octaver Up", "Classic", EffectCategory::Pitch, Colour(255, 140, 180), "One octave up effect", {"octaver", "octave", "up", "high"}, 0.48f, false, 1, {}}, []() { return std::make_unique<OctaverUp>(); });
    registerEffect({"octaverDown", "Octaver Down", "Classic", EffectCategory::Pitch, Colour(220, 100, 160), "One octave down effect", {"octaver", "octave", "down", "bass"}, 0.42f, false, 1, {}}, []() { return std::make_unique<OctaverDown>(); });
    registerEffect({"whammyPedal", "Whammy Pedal", "DigiTech", EffectCategory::Pitch, Colour(255, 80, 150), "Expression-controlled pitch bender", {"whammy", "pitch", "bend", "expression", "digitech"}, 0.60f, false, 1, {}}, []() { return std::make_unique<WhammyPedal>(); });
    registerEffect({"detuneChorus", "Detune Chorus", "Studio", EffectCategory::Pitch, Colour(200, 150, 255), "Multi-voice detuned chorus", {"detune", "chorus", "voices", "thick"}, 0.52f, false, 1, {}}, []() { return std::make_unique<DetuneChorus>(); });
    registerEffect({"pitchCorrection", "Pitch Correction", "Studio", EffectCategory::Pitch, Colour(180, 180, 255), "Auto-tune style pitch correction", {"autotune", "correction", "pitch", "tune"}, 0.50f, false, 1, {}}, []() { return std::make_unique<PitchCorrection>(); });
    registerEffect({"formantShifter", "Formant Shifter", "Studio", EffectCategory::Pitch, Colour(240, 120, 200), "Formant and pitch shifter", {"formant", "voice", "shift", "gender"}, 0.56f, false, 1, {}}, []() { return std::make_unique<FormantShifter>(); });
    registerEffect({"pitchVibrato", "Pitch Vibrato", "Classic", EffectCategory::Pitch, Colour(255, 160, 220), "Pure pitch modulation vibrato", {"vibrato", "pitch", "modulation", "warble"}, 0.45f, false, 1, {}}, []() { return std::make_unique<PitchVibrato>(); });
    registerEffect({"intelligentHarmony", "Intelligent Harmony", "Advanced", EffectCategory::Pitch, Colour(255, 100, 240), "Key-aware intelligent harmonizer", {"harmony", "intelligent", "key", "scale", "smart"}, 0.62f, false, 1, {}}, []() { return std::make_unique<IntelligentHarmony>(); });
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

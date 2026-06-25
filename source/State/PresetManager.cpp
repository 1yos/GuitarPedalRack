#include "PresetManager.h"

PresetManager::PresetManager()
{
    initializePresetFolder();
    loadFactoryPresets();
    loadUserPresets();
}

void PresetManager::initializePresetFolder()
{
    // Get user application data folder
    File appData = File::getSpecialLocation(File::userApplicationDataDirectory);
    userPresetFolder = appData.getChildFile("GuitarPedalRack").getChildFile("Presets");
    
    if (!userPresetFolder.exists())
        userPresetFolder.createDirectory();
}

static void populatePresetParameters(ChainPreset& preset)
{
    // Set default parameter values for ALL parameters first
    auto setParam = [&](const String& id, float val) {
        preset.parameterValues[id] = val;
    };
    
    // Globals
    setParam("globalInputGain", 0.0f);
    setParam("globalOutputGain", 0.0f);
    setParam("globalBypass", 0.0f);
    
    // Deep Heat (Overdrive)
    setParam("deepHeatDrive", 0.4f);
    setParam("deepHeatTone", 0.5f);
    setParam("deepHeatLevel", 0.7f);
    setParam("deepHeatBypass", 1.0f); // Default bypassed
    
    // Void (Reverb)
    setParam("voidSize", 0.5f);
    setParam("voidDecay", 0.4f);
    setParam("voidMix", 0.3f);
    setParam("voidBypass", 1.0f); // Default bypassed
    
    // Pulse (Chorus)
    setParam("pulseRate", 1.0f);
    setParam("pulseDepth", 0.5f);
    setParam("pulseMix", 0.4f);
    setParam("pulseBypass", 1.0f); // Default bypassed
    
    // Amp
    setParam("ampGain", 5.0f);
    setParam("ampBass", 5.0f);
    setParam("ampMid", 5.0f);
    setParam("ampTreble", 5.0f);
    setParam("ampPresence", 5.0f);
    setParam("ampMaster", 7.0f);
    setParam("ampChannel", 1.0f); // Default Crunch
    setParam("ampBypass", 0.0f); // Default active
    
    // Cab
    setParam("cabMicX", 0.0f);
    setParam("cabMicY", 0.0f);
    setParam("cabDistance", 0.2f);
    setParam("cabAxis", 14.0f);
    setParam("cabType", 0.0f); // Default V30
    setParam("cabBypass", 0.0f); // Default active
    
    // Morph
    setParam("morphPosition", 0.5f);
    
    // Customize based on category / name
    String cat = preset.category;
    String name = preset.name.toLowerCase();
    
    if (cat == "Metal" || name.contains("metal") || name.contains("djent") || name.contains("doom") || name.contains("thrash"))
    {
        setParam("ampChannel", 2.0f); // Lead
        setParam("ampGain", 7.5f);
        setParam("ampBass", 6.0f);
        setParam("ampMid", 3.8f); // Scooped
        setParam("ampTreble", 6.5f);
        setParam("ampPresence", 6.0f);
        
        setParam("deepHeatBypass", 0.0f); // Active
        setParam("deepHeatDrive", 0.15f);
        setParam("deepHeatTone", 0.6f);
        setParam("deepHeatLevel", 0.8f);
        
        setParam("cabType", 0.0f); // 4x12 V30
        setParam("cabMicX", 0.25f);
    }
    else if (cat == "Ambient" || name.contains("ambient") || name.contains("shimmer") || name.contains("space") || name.contains("dream"))
    {
        setParam("ampChannel", 0.0f); // Clean
        setParam("ampGain", 3.0f);
        setParam("ampBass", 5.0f);
        setParam("ampMid", 6.0f);
        setParam("ampTreble", 5.5f);
        
        setParam("voidBypass", 0.0f); // Active
        setParam("voidSize", 0.85f);
        setParam("voidDecay", 0.8f);
        setParam("voidMix", 0.55f);
        
        setParam("pulseBypass", 0.0f); // Active
        setParam("pulseRate", 1.2f);
        setParam("pulseDepth", 0.6f);
        setParam("pulseMix", 0.4f);
        
        setParam("cabType", 2.0f); // 2x12 Combo
    }
    else if (cat == "Clean" || cat == "Jazz" || name.contains("clean") || name.contains("jazz") || name.contains("funk"))
    {
        setParam("ampChannel", 0.0f); // Clean
        setParam("ampGain", 3.5f);
        setParam("ampBass", 5.0f);
        setParam("ampMid", 5.0f);
        setParam("ampTreble", 6.0f);
        
        setParam("voidBypass", 0.0f); // Active
        setParam("voidSize", 0.4f);
        setParam("voidDecay", 0.3f);
        setParam("voidMix", 0.2f);
        
        if (name.contains("chorus"))
        {
            setParam("pulseBypass", 0.0f); // Active
            setParam("pulseRate", 1.0f);
            setParam("pulseDepth", 0.5f);
            setParam("pulseMix", 0.4f);
        }
        
        setParam("cabType", 3.0f); // 1x12 Classic
    }
    else if (cat == "Blues" || name.contains("blues") || name.contains("srv") || name.contains("texas"))
    {
        setParam("ampChannel", 1.0f); // Crunch
        setParam("ampGain", 4.2f);
        setParam("ampBass", 5.5f);
        setParam("ampMid", 5.5f);
        setParam("ampTreble", 5.0f);
        
        setParam("deepHeatBypass", 0.0f); // Active
        setParam("deepHeatDrive", 0.35f);
        setParam("deepHeatTone", 0.5f);
        setParam("deepHeatLevel", 0.65f);
        
        setParam("voidBypass", 0.0f); // Active
        setParam("voidSize", 0.4f);
        setParam("voidDecay", 0.3f);
        setParam("voidMix", 0.2f);
    }
    else // Rock / Lead / Default
    {
        setParam("ampChannel", 1.0f); // Crunch
        setParam("ampGain", 6.0f);
        setParam("ampBass", 5.0f);
        setParam("ampMid", 5.0f);
        setParam("ampTreble", 6.0f);
        
        setParam("deepHeatBypass", 0.0f); // Active
        setParam("deepHeatDrive", 0.45f);
        setParam("deepHeatTone", 0.5f);
        
        setParam("voidBypass", 0.0f); // Active
        setParam("voidSize", 0.5f);
        setParam("voidDecay", 0.35f);
        setParam("voidMix", 0.18f);
    }
}

void PresetManager::loadFactoryPresets()
{
    library.presets.clear();
    
    // Clean & Jazz (10)
    library.presets.add(createJazzCleanPreset());
    library.presets.add(createCountryCleanPreset());
    library.presets.add(createFunkRhythmPreset());
    library.presets.add(createCleanChorusPreset());
    library.presets.add(createSparkleCleanPreset());
    library.presets.add(createWarmJazzPreset());
    library.presets.add(createChickenPickinPreset());
    library.presets.add(createBrightCleanPreset());
    library.presets.add(createCrystalCleanPreset());
    library.presets.add(createCleanBluesPreset());
    
    // Rock (15)
    library.presets.add(createClassicRockPreset());
    library.presets.add(createCrunchRhythmPreset());
    library.presets.add(createBluesRockPreset());
    library.presets.add(createHardRockRhythmPreset());
    library.presets.add(createAltRockPreset());
    library.presets.add(createPowerChordsPreset());
    library.presets.add(createVintageRockPreset());
    library.presets.add(createBritishCrunchPreset());
    library.presets.add(createModernRockPreset());
    library.presets.add(createGrungeRhythmPreset());
    library.presets.add(createIndieRockPreset());
    library.presets.add(createGarageRockPreset());
    library.presets.add(createPunkRockPreset());
    library.presets.add(createStadiumRockPreset());
    library.presets.add(createClassicRockLeadPreset());
    
    // Metal (15)
    library.presets.add(createHighGainMetalPreset());
    library.presets.add(createThrashMetalPreset());
    library.presets.add(createDjentRhythmPreset());
    library.presets.add(createDeathMetalPreset());
    library.presets.add(createMetalcorePreset());
    library.presets.add(createNuMetalPreset());
    library.presets.add(createProgMetalPreset());
    library.presets.add(createBlackMetalPreset());
    library.presets.add(createDoomMetalPreset());
    library.presets.add(createPowerMetalPreset());
    library.presets.add(createModernMetalLeadPreset());
    library.presets.add(createTightRhythmPreset());
    library.presets.add(createScoopedMetalPreset());
    library.presets.add(createAggressiveMetalPreset());
    library.presets.add(createExtremeMetalPreset());
    
    // Blues (10)
    library.presets.add(createTexasBluesPreset());
    library.presets.add(createChicagoBluesPreset());
    library.presets.add(createDeltaBluesPreset());
    library.presets.add(createBluesLeadPreset());
    library.presets.add(createSmoothLeadPreset());
    library.presets.add(createBBKingTonePreset());
    library.presets.add(createSRVTonePreset());
    library.presets.add(createBluesBreakerPreset());
    library.presets.add(createOverdrivenBluesPreset());
    library.presets.add(createSoulfulLeadPreset());
    
    // Ambient & Experimental (10)
    library.presets.add(createAmbientPadPreset());
    library.presets.add(createShimmerVerbPreset());
    library.presets.add(createDreamscapePreset());
    library.presets.add(createSpaceDelayPreset());
    library.presets.add(createReverbSwellPreset());
    library.presets.add(createPostRockPreset());
    library.presets.add(createSoundscapePreset());
    library.presets.add(createEtherealPreset());
    library.presets.add(createCinematicPreset());
    library.presets.add(createExperimentalPreset());
    
    // Lead Tones (10)
    library.presets.add(createSingingLeadPreset());
    library.presets.add(createScreamingLeadPreset());
    library.presets.add(createSustainLeadPreset());
    library.presets.add(createVintageLeadPreset());
    library.presets.add(createModernLeadPreset());
    library.presets.add(createSoloBoostPreset());
    library.presets.add(createWarmLeadPreset());
    library.presets.add(createBrightLeadPreset());
    library.presets.add(createDelayLeadPreset());
    library.presets.add(createLegendaryLeadPreset());
    
    // Populate parameters
    for (auto& preset : library.presets)
    {
        populatePresetParameters(preset);
    }
}

void PresetManager::loadUserPresets()
{
    Array<File> files;
    userPresetFolder.findChildFiles(files, File::findFiles, false, "*.gpr");
    
    for (const auto& file : files)
    {
        std::unique_ptr<XmlElement> xml(XmlDocument::parse(file));
        if (xml != nullptr && xml->hasTagName("GUITAR_PEDAL_RACK_PRESET"))
        {
            ChainPreset preset;
            preset.name = xml->getStringAttribute("name");
            preset.category = xml->getStringAttribute("category");
            preset.description = xml->getStringAttribute("description");
            preset.author = xml->getStringAttribute("author");
            preset.dateCreated = file.getCreationTime();
            preset.dateModified = file.getLastModificationTime();
            
            if (auto* paramsXml = xml->getChildByName("PARAMETERS"))
            {
                for (int i = 0; i < paramsXml->getNumChildElements(); ++i)
                {
                    if (auto* pXml = paramsXml->getChildElement(i))
                    {
                        if (pXml->hasTagName("PARAM"))
                        {
                            String id = pXml->getStringAttribute("id");
                            float val = (float)pXml->getDoubleAttribute("value");
                            preset.parameterValues[id] = val;
                        }
                    }
                }
            }
            
            // Add or overwrite in library
            auto* existing = library.findPresetByName(preset.name);
            if (existing != nullptr)
                *existing = preset;
            else
                library.presets.add(preset);
        }
    }
}

bool PresetManager::savePreset(const ChainPreset& preset)
{
    auto* existing = library.findPresetByName(preset.name);
    if (existing != nullptr)
        *existing = preset;
    else
        library.presets.add(preset);
        
    File presetFile = userPresetFolder.getChildFile(preset.name + ".gpr");
    
    std::unique_ptr<XmlElement> xml(new XmlElement("GUITAR_PEDAL_RACK_PRESET"));
    xml->setAttribute("name", preset.name);
    xml->setAttribute("category", preset.category);
    xml->setAttribute("description", preset.description);
    xml->setAttribute("author", preset.author);
    
    auto* paramsXml = xml->createNewChildElement("PARAMETERS");
    for (const auto& pair : preset.parameterValues)
    {
        auto* pXml = paramsXml->createNewChildElement("PARAM");
        pXml->setAttribute("id", pair.first);
        pXml->setAttribute("value", pair.second);
    }
    
    return xml->writeTo(presetFile);
}

bool PresetManager::loadPreset(const String& name, ChainPreset& outPreset)
{
    auto* preset = library.findPresetByName(name);
    if (preset != nullptr)
    {
        outPreset = *preset;
        return true;
    }
    return false;
}

bool PresetManager::deletePreset(const String& name)
{
    for (int i = 0; i < library.presets.size(); ++i)
    {
        if (library.presets[i].name == name)
        {
            library.presets.remove(i);
            
            // Delete file if it exists
            File presetFile = userPresetFolder.getChildFile(name + ".gpr");
            if (presetFile.exists())
                presetFile.deleteFile();
            
            return true;
        }
    }
    return false;
}

bool PresetManager::exportPreset(const ChainPreset& preset, const File& file)
{
    // TODO: Serialize to JSON format
    return false;
}

bool PresetManager::importPreset(const File& file, ChainPreset& outPreset)
{
    // TODO: Deserialize from JSON format
    return false;
}

StringArray PresetManager::getAllTags() const
{
    StringArray allTags;
    
    for (const auto& preset : library.presets)
    {
        for (const auto& tag : preset.tags)
        {
            if (!allTags.contains(tag))
                allTags.add(tag);
        }
    }
    
    allTags.sort(true);
    return allTags;
}

StringArray PresetManager::getAllCategories() const
{
    StringArray categories;
    categories.add("Rock");
    categories.add("Metal");
    categories.add("Blues");
    categories.add("Jazz");
    categories.add("Clean");
    categories.add("Lead");
    categories.add("Rhythm");
    return categories;
}

//==============================================================================
// Factory Preset Implementations

ChainPreset PresetManager::createCleanBluesPreset()
{
    ChainPreset preset;
    preset.name = "Clean Blues";
    preset.author = "Factory";
    preset.description = "Warm clean tone with subtle compression, perfect for blues rhythm";
    preset.tags.add("blues");
    preset.tags.add("clean");
    preset.tags.add("rhythm");
    preset.category = "Blues";
    preset.dateCreated = Time::getCurrentTime();
    preset.dateModified = preset.dateCreated;
    
    // TODO: Add actual module configurations
    // For now, define the chain structure
    
    return preset;
}

ChainPreset PresetManager::createClassicRockPreset()
{
    ChainPreset preset;
    preset.name = "Classic Rock Rhythm";
    preset.author = "Factory";
    preset.description = "AC/DC style crunch tone - perfect for power chords";
    preset.tags.add("rock");
    preset.tags.add("crunch");
    preset.tags.add("rhythm");
    preset.category = "Rock";
    preset.dateCreated = Time::getCurrentTime();
    preset.dateModified = preset.dateCreated;
    
    return preset;
}

ChainPreset PresetManager::createHighGainMetalPreset()
{
    ChainPreset preset;
    preset.name = "Modern Metal";
    preset.author = "Factory";
    preset.description = "Tight high-gain tone for modern metal riffing";
    preset.tags.add("metal");
    preset.tags.add("high-gain");
    preset.tags.add("rhythm");
    preset.category = "Metal";
    preset.dateCreated = Time::getCurrentTime();
    preset.dateModified = preset.dateCreated;
    
    return preset;
}

ChainPreset PresetManager::createCrunchRhythmPreset()
{
    ChainPreset preset;
    preset.name = "Crunch Rhythm";
    preset.author = "Factory";
    preset.description = "Mid-gain crunch for classic rock rhythm playing";
    preset.tags.add("rock");
    preset.tags.add("crunch");
    preset.tags.add("rhythm");
    preset.category = "Rock";
    preset.dateCreated = Time::getCurrentTime();
    preset.dateModified = preset.dateCreated;
    
    return preset;
}

ChainPreset PresetManager::createSmoothLeadPreset()
{
    ChainPreset preset;
    preset.name = "Smooth Lead";
    preset.author = "Factory";
    preset.description = "Singing lead tone with compression and delay";
    preset.tags.add("lead");
    preset.tags.add("rock");
    preset.tags.add("blues");
    preset.category = "Lead";
    preset.dateCreated = Time::getCurrentTime();
    preset.dateModified = preset.dateCreated;
    
    return preset;
}

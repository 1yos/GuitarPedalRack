#pragma once

#include "../JuceHeader.h"
#include "PresetFormat.h"

using namespace juce;

//==============================================================================
/**
 * Manages preset loading, saving, and factory presets
 */
class PresetManager
{
public:
    PresetManager();
    ~PresetManager() = default;

    //==============================================================================
    // Preset Management
    
    /** Loads factory presets */
    void loadFactoryPresets();
    
    /** Saves a preset to the user preset folder */
    bool savePreset(const ChainPreset& preset);
    
    /** Loads a preset by name */
    bool loadPreset(const String& name, ChainPreset& outPreset);
    
    /** Deletes a user preset */
    bool deletePreset(const String& name);
    
    /** Exports a preset to a file */
    bool exportPreset(const ChainPreset& preset, const File& file);
    
    /** Imports a preset from a file */
    bool importPreset(const File& file, ChainPreset& outPreset);

    //==============================================================================
    // Preset Library Access
    
    /** Returns all available presets */
    Array<ChainPreset>& getAllPresets() { return library.presets; }
    const Array<ChainPreset>& getAllPresets() const { return library.presets; }
    
    /** Searches presets by query and tags */
    Array<ChainPreset*> searchPresets(const String& query, const StringArray& tags = {})
    {
        return library.searchPresets(query, tags);
    }
    
    /** Returns all unique tags */
    StringArray getAllTags() const;
    
    /** Returns all categories */
    StringArray getAllCategories() const;

    //==============================================================================
    // Factory Preset Creators
    
    // Clean & Jazz (10)
    static ChainPreset createCleanBluesPreset();
    static ChainPreset createJazzCleanPreset();
    static ChainPreset createCountryCleanPreset();
    static ChainPreset createFunkRhythmPreset();
    static ChainPreset createCleanChorusPreset();
    static ChainPreset createSparkleCleanPreset();
    static ChainPreset createWarmJazzPreset();
    static ChainPreset createChickenPickinPreset();
    static ChainPreset createBrightCleanPreset();
    static ChainPreset createCrystalCleanPreset();
    
    // Rock (15)
    static ChainPreset createClassicRockPreset();
    static ChainPreset createCrunchRhythmPreset();
    static ChainPreset createBluesRockPreset();
    static ChainPreset createHardRockRhythmPreset();
    static ChainPreset createAltRockPreset();
    static ChainPreset createPowerChordsPreset();
    static ChainPreset createVintageRockPreset();
    static ChainPreset createBritishCrunchPreset();
    static ChainPreset createModernRockPreset();
    static ChainPreset createGrungeRhythmPreset();
    static ChainPreset createIndieRockPreset();
    static ChainPreset createGarageRockPreset();
    static ChainPreset createPunkRockPreset();
    static ChainPreset createStadiumRockPreset();
    static ChainPreset createClassicRockLeadPreset();
    
    // Metal (15)
    static ChainPreset createHighGainMetalPreset();
    static ChainPreset createThrashMetalPreset();
    static ChainPreset createDjentRhythmPreset();
    static ChainPreset createDeathMetalPreset();
    static ChainPreset createMetalcorePreset();
    static ChainPreset createNuMetalPreset();
    static ChainPreset createProgMetalPreset();
    static ChainPreset createBlackMetalPreset();
    static ChainPreset createDoomMetalPreset();
    static ChainPreset createPowerMetalPreset();
    static ChainPreset createModernMetalLeadPreset();
    static ChainPreset createTightRhythmPreset();
    static ChainPreset createScoopedMetalPreset();
    static ChainPreset createAggressiveMetalPreset();
    static ChainPreset createExtremeMetalPreset();
    
    // Blues (10)
    static ChainPreset createTexasBluesPreset();
    static ChainPreset createChicagoBluesPreset();
    static ChainPreset createDeltaBluesPreset();
    static ChainPreset createBluesLeadPreset();
    static ChainPreset createSmoothLeadPreset();
    static ChainPreset createBBKingTonePreset();
    static ChainPreset createSRVTonePreset();
    static ChainPreset createBluesBreakerPreset();
    static ChainPreset createOverdrivenBluesPreset();
    static ChainPreset createSoulfulLeadPreset();
    
    // Ambient & Experimental (10)
    static ChainPreset createAmbientPadPreset();
    static ChainPreset createShimmerVerbPreset();
    static ChainPreset createDreamscapePreset();
    static ChainPreset createSpaceDelayPreset();
    static ChainPreset createReverbSwellPreset();
    static ChainPreset createPostRockPreset();
    static ChainPreset createSoundscapePreset();
    static ChainPreset createEtherealPreset();
    static ChainPreset createCinematicPreset();
    static ChainPreset createExperimentalPreset();
    
    // Lead Tones (10)
    static ChainPreset createSingingLeadPreset();
    static ChainPreset createScreamingLeadPreset();
    static ChainPreset createSustainLeadPreset();
    static ChainPreset createVintageLeadPreset();
    static ChainPreset createModernLeadPreset();
    static ChainPreset createSoloBoostPreset();
    static ChainPreset createWarmLeadPreset();
    static ChainPreset createBrightLeadPreset();
    static ChainPreset createDelayLeadPreset();
    static ChainPreset createLegendaryLeadPreset();

private:
    PresetLibrary library;
    File userPresetFolder;
    
    void initializePresetFolder();
    void loadUserPresets();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};

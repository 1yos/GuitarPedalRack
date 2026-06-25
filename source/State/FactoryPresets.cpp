#include "PresetManager.h"

using namespace juce;

static void initPreset(ChainPreset& preset, const String& name, const String& cat, const String& desc)
{
    preset.name = name;
    preset.author = "Factory";
    preset.description = desc;
    preset.category = cat;
    preset.dateCreated = Time::getCurrentTime();
    preset.dateModified = preset.dateCreated;
}

// Helper macro for creating presets
#define CREATE_PRESET(name, cat, desc) \
    ChainPreset preset; \
    initPreset(preset, name, cat, desc);

//==============================================================================
// CLEAN & JAZZ PRESETS
//==============================================================================

ChainPreset PresetManager::createJazzCleanPreset()
{
    CREATE_PRESET("Jazz Clean", "Jazz", "Smooth, warm jazz tone with natural dynamics");
    preset.tags.add("jazz");
    preset.tags.add("clean");
    preset.tags.add("warm");
    return preset;
}

ChainPreset PresetManager::createCountryCleanPreset()
{
    CREATE_PRESET("Country Clean", "Country", "Bright, snappy clean with slapback for classic country");
    preset.tags.add("country");
    preset.tags.add("clean");
    preset.tags.add("bright");
    preset.tags.add("twang");
    return preset;
}

ChainPreset PresetManager::createFunkRhythmPreset()
{
    CREATE_PRESET("Funk Rhythm", "Funk", "Tight, percussive funk rhythm with scooped mids")
    preset.tags.add("funk");
    preset.tags.add("rhythm");
    preset.tags.add("percussive");
    preset.tags.add("tight");
    return preset;
}

ChainPreset PresetManager::createCleanChorusPreset()
{
    CREATE_PRESET("Clean Chorus", "Clean", "Lush chorus for clean arpeggios and rhythm")
    preset.tags.add("clean");
    preset.tags.add("chorus");
    preset.tags.add("lush");
    preset.tags.add("80s");
    return preset;
}

ChainPreset PresetManager::createSparkleCleanPreset()
{
    CREATE_PRESET("Sparkle Clean", "Pop", "Bright, shimmering clean for pop and indie")
    preset.tags.add("clean");
    preset.tags.add("bright");
    preset.tags.add("sparkle");
    preset.tags.add("pop");
    return preset;
}


ChainPreset PresetManager::createWarmJazzPreset()
{
    CREATE_PRESET("Warm Jazz", "Jazz", "Dark, smooth jazz tone with ribbon mic character")
    preset.tags.add("jazz");
    preset.tags.add("warm");
    preset.tags.add("mellow");
    return preset;
}

ChainPreset PresetManager::createChickenPickinPreset()
{
    CREATE_PRESET("Chicken Pickin'", "Country", "Super tight, percussive clean for chicken pickin'")
    preset.tags.add("country");
    preset.tags.add("chicken pickin");
    preset.tags.add("bright");
    preset.tags.add("percussive");
    return preset;
}

ChainPreset PresetManager::createBrightCleanPreset()
{
    CREATE_PRESET("Bright Clean", "Clean", "Crystal clear, bright clean tone")
    preset.tags.add("clean");
    preset.tags.add("bright");
    preset.tags.add("clear");
    return preset;
}

ChainPreset PresetManager::createCrystalCleanPreset()
{
    CREATE_PRESET("Crystal Clean", "Ambient", "Pristine, crystalline clean with space")
    preset.tags.add("clean");
    preset.tags.add("crystal");
    preset.tags.add("pristine");
    return preset;
}

//==============================================================================
// ROCK PRESETS
//==============================================================================

ChainPreset PresetManager::createBluesRockPreset()
{
    CREATE_PRESET("Blues Rock", "Blues Rock", "Classic blues rock with sustain and warmth")
    preset.tags.add("blues rock");
    preset.tags.add("crunch");
    preset.tags.add("classic");
    return preset;
}

ChainPreset PresetManager::createHardRockRhythmPreset()
{
    CREATE_PRESET("Hard Rock Rhythm", "Hard Rock", "Aggressive hard rock rhythm tone")
    preset.tags.add("hard rock");
    preset.tags.add("heavy");
    preset.tags.add("rhythm");
    return preset;
}

ChainPreset PresetManager::createAltRockPreset()
{
    CREATE_PRESET("Alt Rock", "Alt Rock", "90s alternative rock rhythm tone")
    preset.tags.add("alternative");
    preset.tags.add("rock");
    preset.tags.add("90s");
    return preset;
}

ChainPreset PresetManager::createPowerChordsPreset()
{
    CREATE_PRESET("Power Chords", "Rock", "Tight, aggressive tone for power chords")
    preset.tags.add("power chords");
    preset.tags.add("tight");
    preset.tags.add("aggressive");
    return preset;
}


ChainPreset PresetManager::createVintageRockPreset()
{
    CREATE_PRESET("Vintage Rock", "Rock", "Warm vintage rock tone with tape echo")
    preset.tags.add("vintage");
    preset.tags.add("classic");
    preset.tags.add("70s");
    return preset;
}

ChainPreset PresetManager::createBritishCrunchPreset()
{
    CREATE_PRESET("British Crunch", "Rock", "Classic British Marshall-style crunch")
    preset.tags.add("british");
    preset.tags.add("marshall");
    preset.tags.add("crunch");
    return preset;
}

ChainPreset PresetManager::createModernRockPreset()
{
    CREATE_PRESET("Modern Rock", "Rock", "Tight, modern rock rhythm tone")
    preset.tags.add("modern");
    preset.tags.add("rock");
    preset.tags.add("tight");
    return preset;
}

ChainPreset PresetManager::createGrungeRhythmPreset()
{
    CREATE_PRESET("Grunge Rhythm", "Grunge", "Heavy, scooped grunge tone (Nirvana-style)")
    preset.tags.add("grunge");
    preset.tags.add("90s");
    preset.tags.add("heavy");
    return preset;
}

ChainPreset PresetManager::createIndieRockPreset()
{
    CREATE_PRESET("Indie Rock", "Indie", "Jangly indie rock with chorus and delay")
    preset.tags.add("indie");
    preset.tags.add("jangly");
    preset.tags.add("bright");
    return preset;
}

ChainPreset PresetManager::createGarageRockPreset()
{
    CREATE_PRESET("Garage Rock", "Garage Rock", "Raw, garage rock tone with character")
    preset.tags.add("garage");
    preset.tags.add("lo-fi");
    preset.tags.add("raw");
    return preset;
}

ChainPreset PresetManager::createPunkRockPreset()
{
    CREATE_PRESET("Punk Rock", "Punk", "Aggressive, in-your-face punk tone")
    preset.tags.add("punk");
    preset.tags.add("aggressive");
    preset.tags.add("fast");
    return preset;
}

ChainPreset PresetManager::createStadiumRockPreset()
{
    CREATE_PRESET("Stadium Rock", "Rock", "Big, anthemic rock tone for arenas")
    preset.tags.add("arena");
    preset.tags.add("big");
    preset.tags.add("anthemic");
    return preset;
}

ChainPreset PresetManager::createClassicRockLeadPreset()
{
    CREATE_PRESET("Classic Rock Lead", "Rock", "Singing lead tone for classic rock solos")
    preset.tags.add("rock");
    preset.tags.add("lead");
    preset.tags.add("singing");
    return preset;
}


//==============================================================================
// METAL PRESETS
//==============================================================================

ChainPreset PresetManager::createThrashMetalPreset()
{
    CREATE_PRESET("Thrash Metal", "Thrash Metal", "Aggressive, fast thrash metal rhythm")
    preset.tags.add("thrash");
    preset.tags.add("aggressive");
    preset.tags.add("fast");
    return preset;
}

ChainPreset PresetManager::createDjentRhythmPreset()
{
    CREATE_PRESET("Djent Rhythm", "Djent", "Super tight, surgical djent tone for palm-muted riffs")
    preset.tags.add("djent");
    preset.tags.add("tight");
    preset.tags.add("modern");
    preset.tags.add("palm-mute");
    return preset;
}

ChainPreset PresetManager::createDeathMetalPreset()
{
    CREATE_PRESET("Death Metal", "Death Metal", "Brutal, crushing death metal tone")
    preset.tags.add("death metal");
    preset.tags.add("brutal");
    preset.tags.add("heavy");
    return preset;
}

ChainPreset PresetManager::createMetalcorePreset()
{
    CREATE_PRESET("Metalcore", "Metalcore", "Modern metalcore rhythm tone")
    preset.tags.add("metalcore");
    preset.tags.add("modern");
    preset.tags.add("heavy");
    return preset;
}

ChainPreset PresetManager::createNuMetalPreset()
{
    CREATE_PRESET("Nu Metal", "Nu Metal", "Scooped nu-metal tone with punch")
    preset.tags.add("nu metal");
    preset.tags.add("scooped");
    preset.tags.add("modern");
    return preset;
}

ChainPreset PresetManager::createProgMetalPreset()
{
    CREATE_PRESET("Prog Metal", "Prog Metal", "Articulate progressive metal tone")
    preset.tags.add("progressive");
    preset.tags.add("metal");
    preset.tags.add("technical");
    return preset;
}

ChainPreset PresetManager::createBlackMetalPreset()
{
    CREATE_PRESET("Black Metal", "Black Metal", "Raw, tremolo-picked black metal tone")
    preset.tags.add("black metal");
    preset.tags.add("raw");
    preset.tags.add("dark");
    return preset;
}

ChainPreset PresetManager::createDoomMetalPreset()
{
    CREATE_PRESET("Doom Metal", "Doom Metal", "Heavy, sludgy doom metal tone")
    preset.tags.add("doom");
    preset.tags.add("heavy");
    preset.tags.add("sludge");
    return preset;
}

ChainPreset PresetManager::createPowerMetalPreset()
{
    CREATE_PRESET("Power Metal", "Power Metal", "Bright, epic power metal tone")
    preset.tags.add("power metal");
    preset.tags.add("epic");
    preset.tags.add("bright");
    return preset;
}


ChainPreset PresetManager::createModernMetalLeadPreset()
{
    CREATE_PRESET("Modern Metal Lead", "Metal", "Articulate modern metal lead tone")
    preset.tags.add("metal");
    preset.tags.add("lead");
    preset.tags.add("modern");
    return preset;
}

ChainPreset PresetManager::createTightRhythmPreset()
{
    CREATE_PRESET("Tight Rhythm", "Metal", "Super tight rhythm tone for fast riffs")
    preset.tags.add("metal");
    preset.tags.add("tight");
    preset.tags.add("rhythm");
    return preset;
}

ChainPreset PresetManager::createScoopedMetalPreset()
{
    CREATE_PRESET("Scooped Metal", "Metal", "Heavily scooped metal tone")
    preset.tags.add("metal");
    preset.tags.add("scooped");
    preset.tags.add("heavy");
    return preset;
}

ChainPreset PresetManager::createAggressiveMetalPreset()
{
    CREATE_PRESET("Aggressive Metal", "Metal", "In-your-face aggressive metal")
    preset.tags.add("metal");
    preset.tags.add("aggressive");
    preset.tags.add("heavy");
    return preset;
}

ChainPreset PresetManager::createExtremeMetalPreset()
{
    CREATE_PRESET("Extreme Metal", "Metal", "Maximum gain extreme metal tone")
    preset.tags.add("extreme");
    preset.tags.add("metal");
    preset.tags.add("brutal");
    return preset;
}

//==============================================================================
// BLUES PRESETS
//==============================================================================

ChainPreset PresetManager::createTexasBluesPreset()
{
    CREATE_PRESET("Texas Blues", "Blues", "Warm Texas blues rhythm tone")
    preset.tags.add("texas");
    preset.tags.add("blues");
    preset.tags.add("warm");
    return preset;
}

ChainPreset PresetManager::createChicagoBluesPreset()
{
    CREATE_PRESET("Chicago Blues", "Blues", "Classic Chicago blues tone")
    preset.tags.add("chicago");
    preset.tags.add("blues");
    preset.tags.add("classic");
    return preset;
}

ChainPreset PresetManager::createDeltaBluesPreset()
{
    CREATE_PRESET("Delta Blues", "Blues", "Raw delta blues tone")
    preset.tags.add("delta");
    preset.tags.add("blues");
    preset.tags.add("raw");
    return preset;
}

ChainPreset PresetManager::createBluesLeadPreset()
{
    CREATE_PRESET("Blues Lead", "Blues", "Expressive blues lead with sustain")
    preset.tags.add("blues");
    preset.tags.add("lead");
    preset.tags.add("expressive");
    return preset;
}


ChainPreset PresetManager::createBBKingTonePreset()
{
    CREATE_PRESET("BB King Tone", "Blues", "Smooth, singing blues lead (BB King-inspired)")
    preset.tags.add("blues");
    preset.tags.add("lead");
    preset.tags.add("bb king");
    preset.tags.add("smooth");
    return preset;
}

ChainPreset PresetManager::createSRVTonePreset()
{
    CREATE_PRESET("SRV Tone", "Blues", "Texas blues with bite (Stevie Ray Vaughan-inspired)")
    preset.tags.add("blues");
    preset.tags.add("srv");
    preset.tags.add("texas");
    preset.tags.add("bite");
    return preset;
}

ChainPreset PresetManager::createBluesBreakerPreset()
{
    CREATE_PRESET("Blues Breaker", "Blues", "Classic blues breaker overdrive tone")
    preset.tags.add("blues");
    preset.tags.add("blues breaker");
    preset.tags.add("classic");
    return preset;
}

ChainPreset PresetManager::createOverdrivenBluesPreset()
{
    CREATE_PRESET("Overdriven Blues", "Blues", "Pushed blues overdrive for lead")
    preset.tags.add("blues");
    preset.tags.add("overdrive");
    preset.tags.add("lead");
    return preset;
}

ChainPreset PresetManager::createSoulfulLeadPreset()
{
    CREATE_PRESET("Soulful Lead", "Blues", "Warm, soulful lead tone")
    preset.tags.add("blues");
    preset.tags.add("lead");
    preset.tags.add("soulful");
    preset.tags.add("warm");
    return preset;
}

//==============================================================================
// AMBIENT & EXPERIMENTAL PRESETS
//==============================================================================

ChainPreset PresetManager::createAmbientPadPreset()
{
    CREATE_PRESET("Ambient Pad", "Ambient", "Lush ambient pad with long reverb")
    preset.tags.add("ambient");
    preset.tags.add("pad");
    preset.tags.add("spacious");
    return preset;
}

ChainPreset PresetManager::createShimmerVerbPreset()
{
    CREATE_PRESET("Shimmer Verb", "Ambient", "Ethereal shimmer reverb for ambient textures")
    preset.tags.add("ambient");
    preset.tags.add("shimmer");
    preset.tags.add("ethereal");
    return preset;
}

ChainPreset PresetManager::createDreamscapePreset()
{
    CREATE_PRESET("Dreamscape", "Ambient", "Dreamy soundscape with chorus and delay")
    preset.tags.add("ambient");
    preset.tags.add("dreamy");
    preset.tags.add("soundscape");
    return preset;
}

ChainPreset PresetManager::createSpaceDelayPreset()
{
    CREATE_PRESET("Space Delay", "Ambient", "Long delay with feedback for space textures")
    preset.tags.add("ambient");
    preset.tags.add("delay");
    preset.tags.add("space");
    return preset;
}


ChainPreset PresetManager::createReverbSwellPreset()
{
    CREATE_PRESET("Reverb Swell", "Ambient", "Volume swell with massive reverb")
    preset.tags.add("ambient");
    preset.tags.add("swell");
    preset.tags.add("reverb");
    return preset;
}

ChainPreset PresetManager::createPostRockPreset()
{
    CREATE_PRESET("Post Rock", "Post Rock", "Post-rock ambient guitar with delay and reverb")
    preset.tags.add("post rock");
    preset.tags.add("ambient");
    preset.tags.add("atmospheric");
    return preset;
}

ChainPreset PresetManager::createSoundscapePreset()
{
    CREATE_PRESET("Soundscape", "Ambient", "Textural soundscape for ambient music")
    preset.tags.add("ambient");
    preset.tags.add("soundscape");
    preset.tags.add("texture");
    return preset;
}

ChainPreset PresetManager::createEtherealPreset()
{
    CREATE_PRESET("Ethereal", "Ambient", "Otherworldly ethereal guitar tones")
    preset.tags.add("ambient");
    preset.tags.add("ethereal");
    preset.tags.add("otherworldly");
    return preset;
}

ChainPreset PresetManager::createCinematicPreset()
{
    CREATE_PRESET("Cinematic", "Cinematic", "Epic cinematic guitar for film scores")
    preset.tags.add("cinematic");
    preset.tags.add("epic");
    preset.tags.add("film");
    return preset;
}

ChainPreset PresetManager::createExperimentalPreset()
{
    CREATE_PRESET("Experimental", "Experimental", "Weird and wonderful experimental sounds")
    preset.tags.add("experimental");
    preset.tags.add("weird");
    preset.tags.add("creative");
    return preset;
}

//==============================================================================
// LEAD TONE PRESETS
//==============================================================================

ChainPreset PresetManager::createSingingLeadPreset()
{
    CREATE_PRESET("Singing Lead", "Lead", "Smooth singing lead with compression and sustain")
    preset.tags.add("lead");
    preset.tags.add("singing");
    preset.tags.add("smooth");
    return preset;
}

ChainPreset PresetManager::createScreamingLeadPreset()
{
    CREATE_PRESET("Screaming Lead", "Lead", "High-gain screaming lead tone")
    preset.tags.add("lead");
    preset.tags.add("screaming");
    preset.tags.add("high-gain");
    return preset;
}

ChainPreset PresetManager::createSustainLeadPreset()
{
    CREATE_PRESET("Sustain Lead", "Lead", "Endless sustain for long notes")
    preset.tags.add("lead");
    preset.tags.add("sustain");
    preset.tags.add("compression");
    return preset;
}


ChainPreset PresetManager::createVintageLeadPreset()
{
    CREATE_PRESET("Vintage Lead", "Lead", "Classic vintage lead tone")
    preset.tags.add("lead");
    preset.tags.add("vintage");
    preset.tags.add("classic");
    return preset;
}

ChainPreset PresetManager::createModernLeadPreset()
{
    CREATE_PRESET("Modern Lead", "Lead", "Tight, articulate modern lead tone")
    preset.tags.add("lead");
    preset.tags.add("modern");
    preset.tags.add("articulate");
    return preset;
}

ChainPreset PresetManager::createSoloBoostPreset()
{
    CREATE_PRESET("Solo Boost", "Lead", "Extra volume and presence for solos")
    preset.tags.add("lead");
    preset.tags.add("boost");
    preset.tags.add("solo");
    return preset;
}

ChainPreset PresetManager::createWarmLeadPreset()
{
    CREATE_PRESET("Warm Lead", "Lead", "Warm, smooth lead tone")
    preset.tags.add("lead");
    preset.tags.add("warm");
    preset.tags.add("smooth");
    return preset;
}

ChainPreset PresetManager::createBrightLeadPreset()
{
    CREATE_PRESET("Bright Lead", "Lead", "Bright, cutting lead tone")
    preset.tags.add("lead");
    preset.tags.add("bright");
    preset.tags.add("cutting");
    return preset;
}

ChainPreset PresetManager::createDelayLeadPreset()
{
    CREATE_PRESET("Delay Lead", "Lead", "Lead tone with dotted eighth delay")
    preset.tags.add("lead");
    preset.tags.add("delay");
    preset.tags.add("dotted eighth");
    return preset;
}

ChainPreset PresetManager::createLegendaryLeadPreset()
{
    CREATE_PRESET("Legendary Lead", "Lead", "Epic legendary lead tone for unforgettable solos")
    preset.tags.add("lead");
    preset.tags.add("legendary");
    preset.tags.add("epic");
    return preset;
}

#undef CREATE_PRESET

#include "PresetManager.h"
using namespace juce;

// ─── helpers ────────────────────────────────────────────────────────────────

static ModulePreset mkMod(const String& type, bool bypassed = false)
{
    ModulePreset m;
    m.moduleType = type;
    m.bypassed   = bypassed;
    m.wetDryMix  = 1.0f;
    return m;
}

static void initPreset(ChainPreset& p, const String& name,
                       const String& cat, const String& desc)
{
    p.name         = name;
    p.author       = "Factory";
    p.description  = desc;
    p.category     = cat;
    p.dateCreated  = Time::getCurrentTime();
    p.dateModified = p.dateCreated;
}

// ─── CLEAN & JAZZ ────────────────────────────────────────────────────────────

ChainPreset PresetManager::createJazzCleanPreset()
{
    ChainPreset p;
    initPreset(p, "Jazz Clean", "Jazz", "Smooth, warm jazz tone with natural dynamics");
    p.tags = {"jazz", "clean", "warm"};
    p.modules.add(mkMod("Compressor"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["ampChannel"]    = 0.0f;
    p.parameterValues["ampGain"]       = 3.0f;
    p.parameterValues["ampBass"]       = 6.0f;
    p.parameterValues["ampMid"]        = 5.0f;
    p.parameterValues["ampTreble"]     = 4.0f;
    p.parameterValues["voidSize"]      = 0.3f;
    p.parameterValues["voidDecay"]     = 0.25f;
    p.parameterValues["voidMix"]       = 0.15f;
    p.parameterValues["voidBypass"]    = 0.0f;
    p.parameterValues["compThreshold"] = -18.0f;
    p.parameterValues["compRatio"]     = 3.0f;
    return p;
}

ChainPreset PresetManager::createCountryCleanPreset()
{
    ChainPreset p;
    initPreset(p, "Country Clean", "Country", "Bright twangy clean with slapback");
    p.tags = {"country", "clean", "bright", "twang"};
    p.modules.add(mkMod("SlapbackDelay"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["ampChannel"] = 0.0f;
    p.parameterValues["ampGain"]    = 4.0f;
    p.parameterValues["ampBass"]    = 4.0f;
    p.parameterValues["ampMid"]     = 6.0f;
    p.parameterValues["ampTreble"]  = 7.0f;
    p.parameterValues["voidSize"]   = 0.25f;
    p.parameterValues["voidMix"]    = 0.12f;
    return p;
}

ChainPreset PresetManager::createFunkRhythmPreset()
{
    ChainPreset p;
    initPreset(p, "Funk Rhythm", "Funk", "Tight percussive funk rhythm with scooped mids");
    p.tags = {"funk", "rhythm", "percussive", "tight"};
    p.modules.add(mkMod("NoiseGate"));
    p.modules.add(mkMod("Compressor"));
    p.modules.add(mkMod("AutoWah"));
    p.parameterValues["ampChannel"] = 0.0f;
    p.parameterValues["ampGain"]    = 3.5f;
    p.parameterValues["ampBass"]    = 3.0f;
    p.parameterValues["ampMid"]     = 4.0f;
    p.parameterValues["ampTreble"]  = 7.0f;
    return p;
}

ChainPreset PresetManager::createCleanChorusPreset()
{
    ChainPreset p;
    initPreset(p, "Clean Chorus", "Clean", "Lush chorus for clean arpeggios");
    p.tags = {"clean", "chorus", "lush"};
    p.modules.add(mkMod("Chorus"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["pulseRate"]  = 1.5f;
    p.parameterValues["pulseDepth"] = 0.55f;
    p.parameterValues["pulseMix"]   = 0.45f;
    p.parameterValues["pulseBypass"]= 0.0f;
    p.parameterValues["ampChannel"] = 0.0f;
    p.parameterValues["ampGain"]    = 3.0f;
    p.parameterValues["voidSize"]   = 0.4f;
    p.parameterValues["voidMix"]    = 0.2f;
    p.parameterValues["voidBypass"] = 0.0f;
    return p;
}

ChainPreset PresetManager::createSparkleCleanPreset()
{
    ChainPreset p;
    initPreset(p, "Sparkle Clean", "Pop", "Bright shimmering clean for pop and indie");
    p.tags = {"clean", "bright", "sparkle", "pop"};
    p.modules.add(mkMod("Compressor"));
    p.modules.add(mkMod("Delay"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["ampChannel"]  = 0.0f;
    p.parameterValues["ampGain"]     = 3.5f;
    p.parameterValues["ampTreble"]   = 7.5f;
    p.parameterValues["delayTime"]   = 300.0f;
    p.parameterValues["delayFeedback"]= 0.25f;
    p.parameterValues["delayMix"]    = 0.2f;
    p.parameterValues["voidSize"]    = 0.4f;
    p.parameterValues["voidMix"]     = 0.2f;
    p.parameterValues["voidBypass"]  = 0.0f;
    return p;
}

ChainPreset PresetManager::createWarmJazzPreset()
{
    ChainPreset p;
    initPreset(p, "Warm Jazz", "Jazz", "Dark, smooth jazz tone with ribbon character");
    p.tags = {"jazz", "warm", "mellow"};
    p.modules.add(mkMod("Compressor"));
    p.parameterValues["ampChannel"] = 0.0f;
    p.parameterValues["ampGain"]    = 2.5f;
    p.parameterValues["ampBass"]    = 7.0f;
    p.parameterValues["ampMid"]     = 5.0f;
    p.parameterValues["ampTreble"]  = 3.0f;
    return p;
}

ChainPreset PresetManager::createChickenPickinPreset()
{
    ChainPreset p;
    initPreset(p, "Chicken Pickin", "Country", "Classic country hybrid picking tone");
    p.tags = {"country", "chicken", "picking", "strat"};
    p.modules.add(mkMod("Compressor"));
    p.modules.add(mkMod("Delay"));
    p.parameterValues["ampChannel"]   = 0.0f;
    p.parameterValues["ampGain"]      = 4.5f;
    p.parameterValues["ampTreble"]    = 7.0f;
    p.parameterValues["delayTime"]    = 250.0f;
    p.parameterValues["delayFeedback"]= 0.2f;
    p.parameterValues["delayMix"]     = 0.15f;
    return p;
}

ChainPreset PresetManager::createBrightCleanPreset()
{
    ChainPreset p;
    initPreset(p, "Bright Clean", "Clean", "Crystal clear bright clean tone");
    p.tags = {"clean", "bright", "crystal"};
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["ampChannel"] = 0.0f;
    p.parameterValues["ampGain"]    = 3.0f;
    p.parameterValues["ampTreble"]  = 8.0f;
    p.parameterValues["voidSize"]   = 0.35f;
    p.parameterValues["voidMix"]    = 0.15f;
    p.parameterValues["voidBypass"] = 0.0f;
    return p;
}

ChainPreset PresetManager::createCrystalCleanPreset()
{
    ChainPreset p;
    initPreset(p, "Crystal Clean", "Clean", "Pristine transparent clean, pure guitar sound");
    p.tags = {"clean", "pure", "transparent"};
    p.modules.add(mkMod("Compressor"));
    p.parameterValues["ampChannel"] = 0.0f;
    p.parameterValues["ampGain"]    = 2.5f;
    p.parameterValues["ampBass"]    = 5.0f;
    p.parameterValues["ampMid"]     = 5.0f;
    p.parameterValues["ampTreble"]  = 6.0f;
    return p;
}

ChainPreset PresetManager::createCleanBluesPreset()
{
    ChainPreset p;
    initPreset(p, "Clean Blues", "Blues", "Warm clean tone with subtle compression for blues rhythm");
    p.tags = {"blues", "clean", "rhythm", "warm"};
    p.modules.add(mkMod("Compressor"));
    p.modules.add(mkMod("TubeOverdrive", true)); // bypassed - available if needed
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["ampChannel"]   = 0.0f;
    p.parameterValues["ampGain"]      = 4.0f;
    p.parameterValues["ampBass"]      = 5.5f;
    p.parameterValues["ampMid"]       = 5.5f;
    p.parameterValues["ampTreble"]    = 5.5f;
    p.parameterValues["voidSize"]     = 0.35f;
    p.parameterValues["voidDecay"]    = 0.3f;
    p.parameterValues["voidMix"]      = 0.18f;
    p.parameterValues["voidBypass"]   = 0.0f;
    return p;
}

// ─── ROCK ─────────────────────────────────────────────────────────────────────

ChainPreset PresetManager::createClassicRockPreset()
{
    ChainPreset p;
    initPreset(p, "Classic Rock Rhythm", "Rock", "AC/DC style crunch tone for power chords");
    p.tags = {"rock", "crunch", "rhythm", "classic"};
    p.modules.add(mkMod("TubeOverdrive"));
    p.modules.add(mkMod("Delay"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["deepHeatDrive"] = 0.55f;
    p.parameterValues["deepHeatTone"]  = 0.55f;
    p.parameterValues["deepHeatLevel"] = 0.7f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 1.0f;
    p.parameterValues["ampGain"]       = 6.0f;
    p.parameterValues["ampBass"]       = 5.0f;
    p.parameterValues["ampMid"]        = 5.5f;
    p.parameterValues["ampTreble"]     = 6.0f;
    p.parameterValues["delayTime"]     = 400.0f;
    p.parameterValues["delayFeedback"] = 0.25f;
    p.parameterValues["delayMix"]      = 0.15f;
    p.parameterValues["voidSize"]      = 0.4f;
    p.parameterValues["voidMix"]       = 0.15f;
    p.parameterValues["voidBypass"]    = 0.0f;
    return p;
}

ChainPreset PresetManager::createCrunchRhythmPreset()
{
    ChainPreset p;
    initPreset(p, "Crunch Rhythm", "Rock", "Mid-gain crunch for classic rock rhythm");
    p.tags = {"rock", "crunch", "rhythm"};
    p.modules.add(mkMod("NoiseGate"));
    p.modules.add(mkMod("TubeOverdrive"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["deepHeatDrive"] = 0.45f;
    p.parameterValues["deepHeatTone"]  = 0.5f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 1.0f;
    p.parameterValues["ampGain"]       = 5.5f;
    p.parameterValues["voidMix"]       = 0.12f;
    p.parameterValues["voidBypass"]    = 0.0f;
    return p;
}

ChainPreset PresetManager::createBluesRockPreset()
{
    ChainPreset p;
    initPreset(p, "Blues Rock", "Rock", "SRV-inspired blues rock with bite and sustain");
    p.tags = {"blues", "rock", "srv", "lead"};
    p.modules.add(mkMod("NoiseGate"));
    p.modules.add(mkMod("TubeOverdrive"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["deepHeatDrive"] = 0.6f;
    p.parameterValues["deepHeatTone"]  = 0.6f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 1.0f;
    p.parameterValues["ampGain"]       = 5.0f;
    p.parameterValues["ampBass"]       = 6.0f;
    p.parameterValues["ampMid"]        = 6.0f;
    p.parameterValues["ampTreble"]     = 5.5f;
    p.parameterValues["voidSize"]      = 0.4f;
    p.parameterValues["voidMix"]       = 0.18f;
    p.parameterValues["voidBypass"]    = 0.0f;
    return p;
}

ChainPreset PresetManager::createHardRockRhythmPreset()
{
    ChainPreset p;
    initPreset(p, "Hard Rock Rhythm", "Rock", "Tight high-gain rhythm for hard rock riffing");
    p.tags = {"rock", "hard", "rhythm", "gain"};
    p.modules.add(mkMod("NoiseGate"));
    p.modules.add(mkMod("TubeOverdrive"));
    p.modules.add(mkMod("Delay"));
    p.parameterValues["deepHeatDrive"] = 0.75f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 2.0f;
    p.parameterValues["ampGain"]       = 7.0f;
    p.parameterValues["delayTime"]     = 350.0f;
    p.parameterValues["delayFeedback"] = 0.2f;
    p.parameterValues["delayMix"]      = 0.1f;
    return p;
}

ChainPreset PresetManager::createAltRockPreset()
{
    ChainPreset p;
    initPreset(p, "Alt Rock", "Rock", "Alternative rock with chorus shimmer");
    p.tags = {"rock", "alternative", "chorus", "indie"};
    p.modules.add(mkMod("TubeOverdrive"));
    p.modules.add(mkMod("Chorus"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["deepHeatDrive"] = 0.4f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["pulseRate"]     = 1.0f;
    p.parameterValues["pulseDepth"]    = 0.4f;
    p.parameterValues["pulseMix"]      = 0.3f;
    p.parameterValues["pulseBypass"]   = 0.0f;
    p.parameterValues["ampChannel"]    = 1.0f;
    p.parameterValues["ampGain"]       = 5.0f;
    p.parameterValues["voidSize"]      = 0.5f;
    p.parameterValues["voidMix"]       = 0.2f;
    p.parameterValues["voidBypass"]    = 0.0f;
    return p;
}

ChainPreset PresetManager::createPowerChordsPreset()
{
    ChainPreset p;
    initPreset(p, "Power Chords", "Rock", "Massive power chord tone with tight low end");
    p.tags = {"rock", "power", "heavy", "chords"};
    p.modules.add(mkMod("NoiseGate"));
    p.modules.add(mkMod("TubeOverdrive"));
    p.parameterValues["deepHeatDrive"] = 0.7f;
    p.parameterValues["deepHeatTone"]  = 0.45f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 2.0f;
    p.parameterValues["ampGain"]       = 7.5f;
    p.parameterValues["ampBass"]       = 6.5f;
    p.parameterValues["ampMid"]        = 4.0f;
    p.parameterValues["ampTreble"]     = 6.0f;
    return p;
}

ChainPreset PresetManager::createVintageRockPreset()
{
    ChainPreset p;
    initPreset(p, "Vintage Rock", "Rock", "50s-60s vintage rock with warm breakup");
    p.tags = {"rock", "vintage", "retro", "50s"};
    p.modules.add(mkMod("TubeOverdrive"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["deepHeatDrive"] = 0.35f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 1.0f;
    p.parameterValues["ampGain"]       = 5.0f;
    p.parameterValues["voidSize"]      = 0.45f;
    p.parameterValues["voidMix"]       = 0.25f;
    p.parameterValues["voidBypass"]    = 0.0f;
    return p;
}

ChainPreset PresetManager::createBritishCrunchPreset()
{
    ChainPreset p;
    initPreset(p, "British Crunch", "Rock", "Marshall-style British crunch");
    p.tags = {"rock", "british", "marshall", "crunch"};
    p.modules.add(mkMod("NoiseGate"));
    p.modules.add(mkMod("TubeOverdrive"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["deepHeatDrive"] = 0.5f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 1.0f;
    p.parameterValues["ampGain"]       = 6.5f;
    p.parameterValues["ampMid"]        = 6.5f;
    p.parameterValues["voidMix"]       = 0.12f;
    p.parameterValues["voidBypass"]    = 0.0f;
    return p;
}

ChainPreset PresetManager::createModernRockPreset()
{
    ChainPreset p;
    initPreset(p, "Modern Rock", "Rock", "Contemporary rock with punchy mids");
    p.tags = {"rock", "modern", "punchy"};
    p.modules.add(mkMod("NoiseGate"));
    p.modules.add(mkMod("TubeOverdrive"));
    p.modules.add(mkMod("Delay"));
    p.parameterValues["deepHeatDrive"] = 0.65f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 2.0f;
    p.parameterValues["ampGain"]       = 7.0f;
    p.parameterValues["delayTime"]     = 380.0f;
    p.parameterValues["delayMix"]      = 0.12f;
    return p;
}

ChainPreset PresetManager::createGrungeRhythmPreset()
{
    ChainPreset p;
    initPreset(p, "Grunge Rhythm", "Rock", "Seattle-style grunge with raw distortion");
    p.tags = {"rock", "grunge", "seattle", "raw"};
    p.modules.add(mkMod("NoiseGate"));
    p.modules.add(mkMod("TubeOverdrive"));
    p.parameterValues["deepHeatDrive"] = 0.8f;
    p.parameterValues["deepHeatTone"]  = 0.4f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 2.0f;
    p.parameterValues["ampGain"]       = 8.0f;
    p.parameterValues["ampBass"]       = 6.0f;
    p.parameterValues["ampMid"]        = 3.5f;
    p.parameterValues["ampTreble"]     = 6.5f;
    return p;
}

ChainPreset PresetManager::createIndieRockPreset()
{
    ChainPreset p;
    initPreset(p, "Indie Rock", "Rock", "Jangly indie rock with reverb wash");
    p.tags = {"rock", "indie", "jangly", "reverb"};
    p.modules.add(mkMod("TubeOverdrive"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["deepHeatDrive"] = 0.3f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 1.0f;
    p.parameterValues["ampGain"]       = 4.5f;
    p.parameterValues["voidSize"]      = 0.55f;
    p.parameterValues["voidDecay"]     = 0.5f;
    p.parameterValues["voidMix"]       = 0.3f;
    p.parameterValues["voidBypass"]    = 0.0f;
    return p;
}

ChainPreset PresetManager::createGarageRockPreset()
{
    ChainPreset p;
    initPreset(p, "Garage Rock", "Rock", "Raw garage rock with ragged edges");
    p.tags = {"rock", "garage", "raw", "punk"};
    p.modules.add(mkMod("TubeOverdrive"));
    p.parameterValues["deepHeatDrive"] = 0.7f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 1.0f;
    p.parameterValues["ampGain"]       = 7.0f;
    return p;
}

ChainPreset PresetManager::createPunkRockPreset()
{
    ChainPreset p;
    initPreset(p, "Punk Rock", "Rock", "Fast, aggressive punk rhythm tone");
    p.tags = {"rock", "punk", "aggressive", "fast"};
    p.modules.add(mkMod("NoiseGate"));
    p.modules.add(mkMod("TubeOverdrive"));
    p.parameterValues["deepHeatDrive"] = 0.75f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 2.0f;
    p.parameterValues["ampGain"]       = 7.5f;
    p.parameterValues["ampBass"]       = 5.0f;
    p.parameterValues["ampMid"]        = 5.0f;
    p.parameterValues["ampTreble"]     = 7.0f;
    return p;
}

ChainPreset PresetManager::createStadiumRockPreset()
{
    ChainPreset p;
    initPreset(p, "Stadium Rock", "Rock", "Big arena rock sound with massive reverb");
    p.tags = {"rock", "stadium", "arena", "big"};
    p.modules.add(mkMod("NoiseGate"));
    p.modules.add(mkMod("TubeOverdrive"));
    p.modules.add(mkMod("Chorus"));
    p.modules.add(mkMod("Delay"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["deepHeatDrive"] = 0.55f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["pulseRate"]     = 0.8f;
    p.parameterValues["pulseDepth"]    = 0.3f;
    p.parameterValues["pulseBypass"]   = 0.0f;
    p.parameterValues["ampChannel"]    = 1.0f;
    p.parameterValues["ampGain"]       = 6.0f;
    p.parameterValues["delayTime"]     = 450.0f;
    p.parameterValues["delayFeedback"] = 0.35f;
    p.parameterValues["delayMix"]      = 0.2f;
    p.parameterValues["voidSize"]      = 0.7f;
    p.parameterValues["voidDecay"]     = 0.6f;
    p.parameterValues["voidMix"]       = 0.3f;
    p.parameterValues["voidBypass"]    = 0.0f;
    return p;
}

ChainPreset PresetManager::createClassicRockLeadPreset()
{
    ChainPreset p;
    initPreset(p, "Classic Rock Lead", "Rock", "Singing classic rock lead with long sustain");
    p.tags = {"rock", "lead", "solo", "classic"};
    p.modules.add(mkMod("NoiseGate"));
    p.modules.add(mkMod("TubeOverdrive"));
    p.modules.add(mkMod("Delay"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["deepHeatDrive"] = 0.65f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 2.0f;
    p.parameterValues["ampGain"]       = 7.5f;
    p.parameterValues["delayTime"]     = 420.0f;
    p.parameterValues["delayFeedback"] = 0.3f;
    p.parameterValues["delayMix"]      = 0.2f;
    p.parameterValues["voidSize"]      = 0.5f;
    p.parameterValues["voidMix"]       = 0.2f;
    p.parameterValues["voidBypass"]    = 0.0f;
    return p;
}

// ─── METAL ────────────────────────────────────────────────────────────────────

static ChainPreset makeMetalPreset(const String& name, const String& desc,
    const StringArray& tags, float drive, float gain, float bass, float mid, float treble)
{
    ChainPreset p;
    p.name = name; p.author = "Factory"; p.description = desc;
    p.category = "Metal"; p.tags = tags;
    p.dateCreated = p.dateModified = Time::getCurrentTime();
    p.modules.add(mkMod("NoiseGate"));
    p.modules.add(mkMod("TubeOverdrive"));
    p.modules.add(mkMod("Delay"));
    p.parameterValues["deepHeatDrive"] = drive;
    p.parameterValues["deepHeatTone"]  = 0.6f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 2.0f;
    p.parameterValues["ampGain"]       = gain;
    p.parameterValues["ampBass"]       = bass;
    p.parameterValues["ampMid"]        = mid;
    p.parameterValues["ampTreble"]     = treble;
    p.parameterValues["ampPresence"]   = 6.0f;
    p.parameterValues["delayTime"]     = 360.0f;
    p.parameterValues["delayFeedback"] = 0.15f;
    p.parameterValues["delayMix"]      = 0.08f;
    return p;
}

ChainPreset PresetManager::createHighGainMetalPreset()   { return makeMetalPreset("Modern Metal","Tight high-gain for modern metal riffing",{"metal","high-gain","rhythm"},0.15f,7.5f,6.0f,3.8f,6.5f); }
ChainPreset PresetManager::createThrashMetalPreset()     { return makeMetalPreset("Thrash Metal","Aggressive thrash rhythm with tight low end",{"metal","thrash","fast"},0.2f,8.0f,5.5f,3.5f,7.0f); }
ChainPreset PresetManager::createDjentRhythmPreset()     { return makeMetalPreset("Djent Rhythm","Ultra-tight djent rhythm with palm mutes",{"metal","djent","modern"},0.1f,8.5f,7.0f,3.0f,6.0f); }
ChainPreset PresetManager::createDeathMetalPreset()      { return makeMetalPreset("Death Metal","Brutal death metal with maximum distortion",{"metal","death","brutal"},0.25f,9.0f,7.0f,3.0f,6.5f); }
ChainPreset PresetManager::createMetalcorePreset()       { return makeMetalPreset("Metalcore","Metalcore breakdown tone",{"metal","metalcore","breakdown"},0.18f,8.0f,6.5f,3.5f,6.5f); }
ChainPreset PresetManager::createNuMetalPreset()         { return makeMetalPreset("Nu Metal","90s nu metal tone",{"metal","nu-metal","90s"},0.2f,7.5f,7.0f,4.0f,5.5f); }
ChainPreset PresetManager::createProgMetalPreset()       { return makeMetalPreset("Prog Metal","Progressive metal with clarity",{"metal","prog","technical"},0.15f,7.5f,5.5f,4.5f,6.5f); }
ChainPreset PresetManager::createBlackMetalPreset()      { return makeMetalPreset("Black Metal","Tremolo picking black metal tone",{"metal","black","kvlt"},0.2f,8.0f,4.0f,4.0f,8.0f); }
ChainPreset PresetManager::createDoomMetalPreset()       { return makeMetalPreset("Doom Metal","Slow crushing doom riffs",{"metal","doom","slow"},0.3f,7.0f,8.0f,5.0f,5.0f); }
ChainPreset PresetManager::createPowerMetalPreset()      { return makeMetalPreset("Power Metal","Melodic power metal rhythm",{"metal","power","melodic"},0.2f,7.5f,5.5f,4.0f,6.5f); }
ChainPreset PresetManager::createModernMetalLeadPreset() { return makeMetalPreset("Modern Metal Lead","Screaming modern metal lead",{"metal","lead","solo"},0.15f,8.5f,5.5f,5.0f,7.0f); }
ChainPreset PresetManager::createTightRhythmPreset()     { return makeMetalPreset("Tight Rhythm","Super tight rhythm for chugging",{"metal","tight","rhythm"},0.1f,8.0f,6.0f,3.5f,6.0f); }
ChainPreset PresetManager::createScoopedMetalPreset()    { return makeMetalPreset("Scooped Metal","Classic scooped mids metal tone",{"metal","scooped","mids"},0.2f,8.5f,7.0f,2.5f,7.5f); }
ChainPreset PresetManager::createAggressiveMetalPreset() { return makeMetalPreset("Aggressive Metal","In-your-face aggressive metal",{"metal","aggressive","brutal"},0.22f,8.5f,6.5f,4.0f,7.0f); }
ChainPreset PresetManager::createExtremeMetalPreset()    { return makeMetalPreset("Extreme Metal","Ultimate extreme metal saturation",{"metal","extreme","chaos"},0.25f,9.5f,7.0f,3.0f,7.0f); }

// ─── BLUES ────────────────────────────────────────────────────────────────────

static ChainPreset makeBluesPreset(const String& name, const String& desc,
    const StringArray& tags, float drive, float gain, float reverb = 0.2f)
{
    ChainPreset p;
    p.name = name; p.author = "Factory"; p.description = desc;
    p.category = "Blues"; p.tags = tags;
    p.dateCreated = p.dateModified = Time::getCurrentTime();
    p.modules.add(mkMod("TubeOverdrive"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["deepHeatDrive"] = drive;
    p.parameterValues["deepHeatTone"]  = 0.55f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 1.0f;
    p.parameterValues["ampGain"]       = gain;
    p.parameterValues["ampBass"]       = 5.5f;
    p.parameterValues["ampMid"]        = 6.0f;
    p.parameterValues["ampTreble"]     = 5.5f;
    p.parameterValues["voidSize"]      = 0.4f;
    p.parameterValues["voidDecay"]     = 0.35f;
    p.parameterValues["voidMix"]       = reverb;
    p.parameterValues["voidBypass"]    = 0.0f;
    return p;
}

ChainPreset PresetManager::createTexasBluesPreset()      { return makeBluesPreset("Texas Blues","SRV-style Texas blues with thick tone",{"blues","texas","srv","lead"},0.45f,5.0f,0.2f); }
ChainPreset PresetManager::createChicagoBluesPreset()    { return makeBluesPreset("Chicago Blues","Urban electric blues",{"blues","chicago","electric"},0.3f,4.5f,0.15f); }
ChainPreset PresetManager::createDeltaBluesPreset()      { return makeBluesPreset("Delta Blues","Raw delta blues slide tone",{"blues","delta","slide"},0.25f,4.0f,0.25f); }
ChainPreset PresetManager::createBluesLeadPreset()       { return makeBluesPreset("Blues Lead","Classic blues lead with sustain",{"blues","lead","solo"},0.5f,5.5f,0.2f); }
ChainPreset PresetManager::createSmoothLeadPreset()      { return makeBluesPreset("Smooth Lead","Smooth, singing lead tone",{"blues","lead","smooth"},0.4f,5.0f,0.22f); }
ChainPreset PresetManager::createBBKingTonePreset()      { return makeBluesPreset("BB King Tone","Sweet BB King-inspired tone",{"blues","bbking","sweet"},0.35f,4.5f,0.18f); }
ChainPreset PresetManager::createSRVTonePreset()         { return makeBluesPreset("SRV Tone","Stevie Ray Vaughan inspired tone",{"blues","srv","texas"},0.5f,5.5f,0.2f); }
ChainPreset PresetManager::createBluesBreakerPreset()    { return makeBluesPreset("Blues Breaker","Classic British blues breaker tone",{"blues","british","classic"},0.4f,5.0f,0.18f); }
ChainPreset PresetManager::createOverdrivenBluesPreset() { return makeBluesPreset("Overdriven Blues","Pushed blues with heavy breakup",{"blues","overdriven","heavy"},0.65f,6.0f,0.2f); }
ChainPreset PresetManager::createSoulfulLeadPreset()     { return makeBluesPreset("Soulful Lead","Warm, soulful lead tone",{"blues","soul","warm","lead"},0.38f,4.8f,0.25f); }

// ─── AMBIENT ──────────────────────────────────────────────────────────────────

static ChainPreset makeAmbientPreset(const String& name, const String& desc,
    const StringArray& tags, float reverbSize, float reverbMix, float delayMix = 0.0f)
{
    ChainPreset p;
    p.name = name; p.author = "Factory"; p.description = desc;
    p.category = "Ambient"; p.tags = tags;
    p.dateCreated = p.dateModified = Time::getCurrentTime();
    p.modules.add(mkMod("Chorus"));
    p.modules.add(mkMod("Delay"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["pulseRate"]   = 1.2f;
    p.parameterValues["pulseDepth"]  = 0.5f;
    p.parameterValues["pulseMix"]    = 0.4f;
    p.parameterValues["pulseBypass"] = 0.0f;
    p.parameterValues["ampChannel"]  = 0.0f;
    p.parameterValues["ampGain"]     = 3.0f;
    p.parameterValues["delayTime"]   = 600.0f;
    p.parameterValues["delayFeedback"]= 0.4f;
    p.parameterValues["delayMix"]    = delayMix > 0.0f ? delayMix : 0.3f;
    p.parameterValues["voidSize"]    = reverbSize;
    p.parameterValues["voidDecay"]   = reverbSize * 0.9f;
    p.parameterValues["voidMix"]     = reverbMix;
    p.parameterValues["voidBypass"]  = 0.0f;
    return p;
}

ChainPreset PresetManager::createAmbientPadPreset()    { return makeAmbientPreset("Ambient Pad","Lush ambient pad for textures",{"ambient","pad","lush"},0.85f,0.55f,0.35f); }
ChainPreset PresetManager::createShimmerVerbPreset()   { return makeAmbientPreset("Shimmer Verb","Ethereal shimmer reverb with octave",{"ambient","shimmer","ethereal"},0.9f,0.6f,0.2f); }
ChainPreset PresetManager::createDreamscapePreset()    { return makeAmbientPreset("Dreamscape","Dreamy atmospheric soundscape",{"ambient","dream","atmospheric"},0.88f,0.58f,0.4f); }
ChainPreset PresetManager::createSpaceDelayPreset()    { return makeAmbientPreset("Space Delay","Deep space echo with reverb wash",{"ambient","space","delay"},0.8f,0.45f,0.45f); }
ChainPreset PresetManager::createReverbSwellPreset()   { return makeAmbientPreset("Reverb Swell","Swelling reverb for cinematic moments",{"ambient","swell","cinematic"},0.92f,0.65f,0.15f); }
ChainPreset PresetManager::createPostRockPreset()      { return makeAmbientPreset("Post Rock","Expansive post-rock build tone",{"ambient","post-rock","build"},0.85f,0.5f,0.35f); }
ChainPreset PresetManager::createSoundscapePreset()    { return makeAmbientPreset("Soundscape","Abstract textural soundscape",{"ambient","texture","abstract"},0.9f,0.6f,0.4f); }
ChainPreset PresetManager::createEtherealPreset()      { return makeAmbientPreset("Ethereal","Airy, ethereal clean tone",{"ambient","ethereal","airy"},0.88f,0.55f,0.3f); }
ChainPreset PresetManager::createCinematicPreset()     { return makeAmbientPreset("Cinematic","Epic cinematic guitar texture",{"ambient","cinematic","epic"},0.9f,0.6f,0.35f); }
ChainPreset PresetManager::createExperimentalPreset()  { return makeAmbientPreset("Experimental","Experimental effects chain",{"ambient","experimental","weird"},0.85f,0.5f,0.5f); }

// ─── LEAD TONES ───────────────────────────────────────────────────────────────

static ChainPreset makeLeadPreset(const String& name, const String& desc,
    const StringArray& tags, float drive, float gain, float delayTime, float delayMix, float reverbMix)
{
    ChainPreset p;
    p.name = name; p.author = "Factory"; p.description = desc;
    p.category = "Lead"; p.tags = tags;
    p.dateCreated = p.dateModified = Time::getCurrentTime();
    p.modules.add(mkMod("NoiseGate"));
    p.modules.add(mkMod("TubeOverdrive"));
    p.modules.add(mkMod("Delay"));
    p.modules.add(mkMod("Reverb"));
    p.parameterValues["deepHeatDrive"] = drive;
    p.parameterValues["deepHeatTone"]  = 0.55f;
    p.parameterValues["deepHeatBypass"]= 0.0f;
    p.parameterValues["ampChannel"]    = 2.0f;
    p.parameterValues["ampGain"]       = gain;
    p.parameterValues["ampBass"]       = 5.0f;
    p.parameterValues["ampMid"]        = 5.5f;
    p.parameterValues["ampTreble"]     = 6.0f;
    p.parameterValues["delayTime"]     = delayTime;
    p.parameterValues["delayFeedback"] = 0.3f;
    p.parameterValues["delayMix"]      = delayMix;
    p.parameterValues["voidSize"]      = 0.45f;
    p.parameterValues["voidDecay"]     = 0.4f;
    p.parameterValues["voidMix"]       = reverbMix;
    p.parameterValues["voidBypass"]    = 0.0f;
    return p;
}

ChainPreset PresetManager::createSingingLeadPreset()   { return makeLeadPreset("Singing Lead","Smooth singing lead tone",{"lead","singing","smooth"},0.5f,7.5f,420.0f,0.2f,0.2f); }
ChainPreset PresetManager::createScreamingLeadPreset() { return makeLeadPreset("Screaming Lead","High-energy screaming lead",{"lead","screaming","high"},0.7f,8.5f,380.0f,0.18f,0.15f); }
ChainPreset PresetManager::createSustainLeadPreset()   { return makeLeadPreset("Sustain Lead","Infinite sustain lead",{"lead","sustain","long"},0.6f,8.0f,400.0f,0.22f,0.22f); }
ChainPreset PresetManager::createVintageLeadPreset()   { return makeLeadPreset("Vintage Lead","Classic vintage lead tone",{"lead","vintage","classic"},0.45f,7.0f,450.0f,0.2f,0.25f); }
ChainPreset PresetManager::createModernLeadPreset()    { return makeLeadPreset("Modern Lead","Tight modern lead for shredding",{"lead","modern","shred"},0.55f,8.0f,370.0f,0.15f,0.15f); }
ChainPreset PresetManager::createSoloBoostPreset()     { return makeLeadPreset("Solo Boost","Level boost for lead solos",{"lead","boost","solo"},0.4f,7.0f,400.0f,0.18f,0.18f); }
ChainPreset PresetManager::createWarmLeadPreset()      { return makeLeadPreset("Warm Lead","Warm neck pickup lead",{"lead","warm","neck"},0.45f,7.5f,430.0f,0.2f,0.22f); }
ChainPreset PresetManager::createBrightLeadPreset()    { return makeLeadPreset("Bright Lead","Bright bridge pickup lead",{"lead","bright","bridge"},0.5f,8.0f,390.0f,0.18f,0.16f); }
ChainPreset PresetManager::createDelayLeadPreset()     { return makeLeadPreset("Delay Lead","U2 style delay lead",{"lead","delay","u2","edge"},0.4f,7.0f,500.0f,0.35f,0.2f); }
ChainPreset PresetManager::createLegendaryLeadPreset() { return makeLeadPreset("Legendary Lead","Iconic legendary lead tone",{"lead","legendary","iconic"},0.55f,8.0f,420.0f,0.22f,0.22f); }

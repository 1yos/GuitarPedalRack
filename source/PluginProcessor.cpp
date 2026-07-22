#include "PluginProcessor.h"
#include "UI/ModernPluginEditor.h"
#include <chrono>

//==============================================================================
// APVTS Parameter Layout Creation
AudioProcessorValueTreeState::ParameterLayout GuitarPedalRackProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;
    
    // ===================================================================
    // GLOBAL PARAMETERS
    // ===================================================================
    params.push_back(std::make_unique<AudioParameterFloat>(
        "globalInputGain", "Input Gain", -20.0f, 20.0f, 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "globalOutputGain", "Output Gain", -20.0f, 20.0f, 0.0f));
    params.push_back(std::make_unique<AudioParameterBool>(
        "globalBypass", "Global Bypass", false));
    
    // ===================================================================
    // NOISE GATE
    // ===================================================================
    params.push_back(std::make_unique<AudioParameterFloat>(
        "gateThreshold", "Gate Threshold",
        NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -40.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "gateAttack", "Gate Attack",
        NormalisableRange<float>(0.1f, 100.0f, 0.1f), 1.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "gateRelease", "Gate Release",
        NormalisableRange<float>(10.0f, 1000.0f, 1.0f), 100.0f));
    params.push_back(std::make_unique<AudioParameterBool>(
        "gateBypass", "Gate Bypass", false));
    
    // ===================================================================
    // DEEP HEAT PEDAL (Tube Overdrive)
    // ===================================================================
    params.push_back(std::make_unique<AudioParameterFloat>(
        "deepHeatDrive", "Deep Heat Drive",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "deepHeatTone", "Deep Heat Tone",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "deepHeatLevel", "Deep Heat Level",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));
    params.push_back(std::make_unique<AudioParameterBool>(
        "deepHeatBypass", "Deep Heat Bypass", false));
    
    // ===================================================================
    // DISTORTION
    // ===================================================================
    params.push_back(std::make_unique<AudioParameterFloat>(
        "distDrive", "Distortion Drive",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "distTone", "Distortion Tone",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "distLevel", "Distortion Level",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));
    params.push_back(std::make_unique<AudioParameterBool>(
        "distBypass", "Distortion Bypass", false));
    
    // ===================================================================
    // COMPRESSOR
    // ===================================================================
    params.push_back(std::make_unique<AudioParameterFloat>(
        "compThreshold", "Compressor Threshold",
        NormalisableRange<float>(-40.0f, 0.0f, 0.1f), -20.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "compRatio", "Compressor Ratio",
        NormalisableRange<float>(1.0f, 20.0f, 0.1f), 4.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "compAttack", "Compressor Attack",
        NormalisableRange<float>(0.1f, 100.0f, 0.1f), 10.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "compRelease", "Compressor Release",
        NormalisableRange<float>(10.0f, 1000.0f, 1.0f), 100.0f));
    params.push_back(std::make_unique<AudioParameterBool>(
        "compBypass", "Compressor Bypass", false));
    
    // ===================================================================
    // VOID PEDAL (Reverb/Ambient)
    // ===================================================================
    params.push_back(std::make_unique<AudioParameterFloat>(
        "voidSize", "Void Size",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.6f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "voidDecay", "Void Decay",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.6f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "voidMix", "Void Mix",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.4f));
    params.push_back(std::make_unique<AudioParameterBool>(
        "voidBypass", "Void Bypass", false));
    
    // ===================================================================
    // PULSE PEDAL (Chorus/Modulation)
    // ===================================================================
    params.push_back(std::make_unique<AudioParameterFloat>(
        "pulseRate", "Pulse Rate",
        NormalisableRange<float>(0.1f, 10.0f, 0.1f), 1.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "pulseDepth", "Pulse Depth",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "pulseMix", "Pulse Mix",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<AudioParameterBool>(
        "pulseBypass", "Pulse Bypass", false));
    
    // ===================================================================
    // DELAY
    // ===================================================================
    params.push_back(std::make_unique<AudioParameterFloat>(
        "delayTime", "Delay Time",
        NormalisableRange<float>(10.0f, 2000.0f, 1.0f), 500.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "delayFeedback", "Delay Feedback",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "delayMix", "Delay Mix",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));
    params.push_back(std::make_unique<AudioParameterChoice>(
        "delayMode", "Delay Mode",
        StringArray{"Analog", "Digital", "Tape"}, 0));
    params.push_back(std::make_unique<AudioParameterBool>(
        "delayBypass", "Delay Bypass", false));
    
    // ===================================================================
    // ALPHA AMP
    // ===================================================================
    params.push_back(std::make_unique<AudioParameterFloat>(
        "ampGain", "Amp Gain",
        NormalisableRange<float>(0.0f, 10.0f, 0.1f), 5.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "ampBass", "Amp Bass",
        NormalisableRange<float>(0.0f, 10.0f, 0.1f), 5.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "ampMid", "Amp Mid",
        NormalisableRange<float>(0.0f, 10.0f, 0.1f), 5.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "ampTreble", "Amp Treble",
        NormalisableRange<float>(0.0f, 10.0f, 0.1f), 5.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "ampPresence", "Amp Presence",
        NormalisableRange<float>(0.0f, 10.0f, 0.1f), 5.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "ampMaster", "Amp Master",
        NormalisableRange<float>(0.0f, 10.0f, 0.1f), 7.0f));
    params.push_back(std::make_unique<AudioParameterChoice>(
        "ampChannel", "Amp Channel",
        StringArray{"Clean", "Crunch", "Lead"}, 1));
    params.push_back(std::make_unique<AudioParameterBool>(
        "ampBypass", "Amp Bypass", false));
    
    // ===================================================================
    // CABINET & MICROPHONE
    // ===================================================================
    params.push_back(std::make_unique<AudioParameterFloat>(
        "cabMicX", "Cabinet Mic X Position",
        NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "cabMicY", "Cabinet Mic Y Position",
        NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "cabDistance", "Cabinet Distance",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "cabAxis", "Cabinet Axis Angle",
        NormalisableRange<float>(0.0f, 90.0f, 1.0f), 14.0f));
    params.push_back(std::make_unique<AudioParameterChoice>(
        "cabType", "Cabinet Type",
        StringArray{"4x12 V30", "4x12 Greenback", "2x12 Vintage", "1x12 Classic"}, 0));
    params.push_back(std::make_unique<AudioParameterBool>(
        "cabBypass", "Cabinet Bypass", false));
    
    // ===================================================================
    // PARAMETRIC EQ
    // ===================================================================
    params.push_back(std::make_unique<AudioParameterFloat>(
        "eqLowGain", "EQ Low Gain",
        NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "eqLowMidGain", "EQ Low-Mid Gain",
        NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "eqHighMidGain", "EQ High-Mid Gain",
        NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));
    params.push_back(std::make_unique<AudioParameterFloat>(
        "eqHighGain", "EQ High Gain",
        NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));
    params.push_back(std::make_unique<AudioParameterBool>(
        "eqBypass", "EQ Bypass", false));
    
    // ===================================================================
    // MORPH CONTROL (Macro System)
    // ===================================================================
    params.push_back(std::make_unique<AudioParameterFloat>(
        "morphPosition", "Morph Position",
        NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    
    return { params.begin(), params.end() };
}

//==============================================================================
GuitarPedalRackProcessor::GuitarPedalRackProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
#else
     :
#endif
       apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    buildDefaultChain();
    
    // Initialize parameter smoothing
    smoothedInputGain.reset(44100.0);
    smoothedOutputGain.reset(44100.0);
}

GuitarPedalRackProcessor::~GuitarPedalRackProcessor()
{
}

//==============================================================================
const String GuitarPedalRackProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GuitarPedalRackProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GuitarPedalRackProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GuitarPedalRackProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GuitarPedalRackProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GuitarPedalRackProcessor::getNumPrograms()
{
    return presetManager.getAllPresets().size();
}

int GuitarPedalRackProcessor::getCurrentProgram()
{
    auto& presets = presetManager.getAllPresets();
    for (int i = 0; i < presets.size(); ++i)
    {
        if (presets[i].name == currentPresetName)
            return i;
    }
    return 0;
}

void GuitarPedalRackProcessor::setCurrentProgram(int index)
{
    if (index >= 0 && index < presetManager.getAllPresets().size())
    {
        const auto& preset = presetManager.getAllPresets()[index];
        loadPreset(preset.name);
    }
}

const String GuitarPedalRackProcessor::getProgramName(int index)
{
    if (index >= 0 && index < presetManager.getAllPresets().size())
        return presetManager.getAllPresets()[index].name;
    
    return {};
}

void GuitarPedalRackProcessor::changeProgramName(int index, const String& newName)
{
    // Not implemented - presets are immutable
}

//==============================================================================
void GuitarPedalRackProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    smartSignalChain.prepare(sampleRate, samplesPerBlock);
    
    // Prepare parameter smoothing
    smoothedInputGain.reset(sampleRate, 0.05);  // 50ms ramp
    smoothedOutputGain.reset(sampleRate, 0.05);
    
    smoothedInputGain.setCurrentAndTargetValue(dbToLinear(inputGainDb));
    smoothedOutputGain.setCurrentAndTargetValue(dbToLinear(outputGainDb));
    
    DBG("SmartSignalChain prepared with " + String(smartSignalChain.getNumEffects()) + " effects");
}

void GuitarPedalRackProcessor::releaseResources()
{
    smartSignalChain.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GuitarPedalRackProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GuitarPedalRackProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    ScopedNoDenormals noDenormals;
    
    // Safety check: ensure buffer is valid
    if (buffer.getNumSamples() == 0 || buffer.getNumChannels() == 0)
        return;
    
    // Clear any extra channels
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
    
    // Update parameters from APVTS
    updateParametersFromAPVTS();
    
    // Bypass all processing if requested
    auto* globalBypassParam = apvts.getRawParameterValue("globalBypass");
    if (globalBypassParam && *globalBypassParam > 0.5f)
    {
        inputLevel.store(0.0f);
        outputLevel.store(0.0f);
        return;
    }
    
    // Track input peak level
    float inMax = 0.0f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        inMax = jmax(inMax, buffer.getMagnitude(ch, 0, buffer.getNumSamples()));
    inputLevel.store(inputLevel.load() * 0.8f + inMax * 0.2f);
    
    // Apply smoothed input gain
    auto* inputGainParam = apvts.getRawParameterValue("globalInputGain");
    if (inputGainParam)
    {
        float targetGain = dbToLinear(*inputGainParam);
        // Safety: clamp extreme values
        targetGain = jlimit(0.0f, 10.0f, targetGain);
        smoothedInputGain.setTargetValue(targetGain);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float gainValue = smoothedInputGain.getNextValue();
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                float inputSample = buffer.getSample(ch, sample);
                // Safety: check for invalid values (NaN, inf)
                if (std::isnan(inputSample) || std::isinf(inputSample))
                    inputSample = 0.0f;
                buffer.setSample(ch, sample, inputSample * gainValue);
            }
        }
    }
    
    // Process through smart signal chain
    smartSignalChain.process(buffer);
    
    // Apply smoothed output gain
    auto* outputGainParam = apvts.getRawParameterValue("globalOutputGain");
    if (outputGainParam)
    {
        float targetGain = dbToLinear(*outputGainParam);
        // Safety: clamp extreme values
        targetGain = jlimit(0.0f, 10.0f, targetGain);
        smoothedOutputGain.setTargetValue(targetGain);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float gainValue = smoothedOutputGain.getNextValue();
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
            {
                float outputSample = buffer.getSample(ch, sample);
                // Safety: check for invalid values and clip
                if (std::isnan(outputSample) || std::isinf(outputSample))
                    outputSample = 0.0f;
                outputSample = jlimit(-1.0f, 1.0f, outputSample * gainValue);
                buffer.setSample(ch, sample, outputSample);
            }
        }
    }
    
    // Track output peak level
    float outMax = 0.0f;
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        outMax = jmax(outMax, buffer.getMagnitude(ch, 0, buffer.getNumSamples()));
    outputLevel.store(outputLevel.load() * 0.8f + outMax * 0.2f);
    
    // Compute CPU usage
    auto endTime = std::chrono::high_resolution_clock::now();
    auto elapsedMicros = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    double blockDurationSeconds = (double)buffer.getNumSamples() / (getSampleRate() > 0.0 ? getSampleRate() : 44100.0);
    double blockDurationMicros = blockDurationSeconds * 1000000.0;
    float usagePercent = 0.0f;
    if (blockDurationMicros > 0.0)
        usagePercent = (float)((elapsedMicros / blockDurationMicros) * 100.0);
    
    // Smooth it with an exponential moving average (EMA)
    // Combine with SmartChain CPU tracking
    float smartChainCPU = smartSignalChain.getCPUUsage() * 100.0f;
    dspCpuUsage.store(dspCpuUsage.load() * 0.95f + (usagePercent + smartChainCPU) * 0.025f);
    
    // Smooth it with an exponential moving average (EMA)
    dspCpuUsage.store(dspCpuUsage.load() * 0.95f + usagePercent * 0.05f);
}

//==============================================================================
bool GuitarPedalRackProcessor::hasEditor() const
{
    return true;
}

AudioProcessorEditor* GuitarPedalRackProcessor::createEditor()
{
    // Use modern UI (Phase 3)
    return new ModernPluginEditor(*this);
    
    // Old UI available as: return new GuitarPedalRackEditor(*this);
}

//==============================================================================
void GuitarPedalRackProcessor::getStateInformation(MemoryBlock& destData)
{
    // Save APVTS parameter state
    auto state = apvts.copyState();
    state.setProperty("currentPresetName", currentPresetName, nullptr);
    
    // ── Persist the current signal chain order ──────────────────────────────
    // Stored as "EffectType0,EffectType1,...:bypassed0,bypassed1,..."
    juce::StringArray types, bypassed;
    for (int i = 0; i < smartSignalChain.getNumEffects(); ++i)
    {
        auto* fx = smartSignalChain.getEffect(i);
        if (fx)
        {
            types.add(fx->getModuleType());
            bypassed.add(fx->isBypassed() ? "1" : "0");
        }
    }
    state.setProperty("chainTypes",    types.joinIntoString(","),    nullptr);
    state.setProperty("chainBypassed", bypassed.joinIntoString(","), nullptr);
    
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GuitarPedalRackProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
    {
        auto state = juce::ValueTree::fromXml(*xmlState);
        apvts.replaceState(state);
        
        currentPresetName = state.getProperty("currentPresetName", "Default").toString();
        
        // ── Restore signal chain order ──────────────────────────────────────
        juce::String typesStr    = state.getProperty("chainTypes",    "").toString();
        juce::String bypassedStr = state.getProperty("chainBypassed", "").toString();
        
        if (typesStr.isNotEmpty())
        {
            auto typeList    = juce::StringArray::fromTokens(typesStr,    ",", "");
            auto bypassList  = juce::StringArray::fromTokens(bypassedStr, ",", "");
            
            smartSignalChain.clearAllEffects();
            
            for (int i = 0; i < typeList.size(); ++i)
            {
                addEffectToChain(typeList[i]);
                if (i < bypassList.size())
                    setEffectBypassed(i, bypassList[i] == "1");
            }
            
            connectParametersToSignalChain();
        }
    }
}

//==============================================================================
void GuitarPedalRackProcessor::buildDefaultChain()
{
    smartSignalChain.clearAllEffects();
    
    // Build complete signal chain:
    // Gate → Overdrive → Chorus → Amp → Reverb → Cabinet
    smartSignalChain.addEffect(std::make_unique<NoiseGate>());
    smartSignalChain.addEffect(std::make_unique<TubeOverdrive>());  // Deep Heat
    smartSignalChain.addEffect(std::make_unique<Chorus>());         // Pulse
    smartSignalChain.addEffect(std::make_unique<AmpSimulator>());   // Alpha Amp
    smartSignalChain.addEffect(std::make_unique<ReverbEffect>());   // Void
    smartSignalChain.addEffect(std::make_unique<CabinetIR>());
    
    currentPresetName = "Default Chain";
    
    // Connect parameters to modules
    connectParametersToSignalChain();
    
    DBG("Default chain built with " + String(smartSignalChain.getNumEffects()) + " effects");
}

void GuitarPedalRackProcessor::connectParametersToSignalChain()
{
    // Connect parameters to modules by searching their types rather than hardcoding indices,
    // which allows the signal chain order to be rearranged dynamically!
    for (int i = 0; i < smartSignalChain.getNumEffects(); ++i)
    {
        auto* module = smartSignalChain.getEffect(i);
        if (module == nullptr) continue;
        
        if (module->getModuleType() == "NoiseGate")
        {
            // Gate parameters (if any)
        }
        else if (module->getModuleType() == "TubeOverdrive") // Deep Heat
        {
            module->setParameterPointer("drive", apvts.getRawParameterValue("deepHeatDrive"));
            module->setParameterPointer("tone", apvts.getRawParameterValue("deepHeatTone"));
            module->setParameterPointer("level", apvts.getRawParameterValue("deepHeatLevel"));
            module->setParameterPointer("bypass", apvts.getRawParameterValue("deepHeatBypass"));
        }
        else if (module->getModuleType() == "Chorus") // Pulse
        {
            module->setParameterPointer("rate", apvts.getRawParameterValue("pulseRate"));
            module->setParameterPointer("depth", apvts.getRawParameterValue("pulseDepth"));
            module->setParameterPointer("mix", apvts.getRawParameterValue("pulseMix"));
            module->setParameterPointer("bypass", apvts.getRawParameterValue("pulseBypass"));
        }
        else if (module->getModuleType() == "AmpSimulator") // Alpha Amp
        {
            module->setParameterPointer("gain", apvts.getRawParameterValue("ampGain"));
            module->setParameterPointer("bass", apvts.getRawParameterValue("ampBass"));
            module->setParameterPointer("mid", apvts.getRawParameterValue("ampMid"));
            module->setParameterPointer("treble", apvts.getRawParameterValue("ampTreble"));
            module->setParameterPointer("presence", apvts.getRawParameterValue("ampPresence"));
            module->setParameterPointer("master", apvts.getRawParameterValue("ampMaster"));
            module->setParameterPointer("channel", apvts.getRawParameterValue("ampChannel"));
            module->setParameterPointer("bypass", apvts.getRawParameterValue("ampBypass"));
        }
        else if (module->getModuleType() == "ReverbEffect") // Void
        {
            module->setParameterPointer("size", apvts.getRawParameterValue("voidSize"));
            module->setParameterPointer("decay", apvts.getRawParameterValue("voidDecay"));
            module->setParameterPointer("mix", apvts.getRawParameterValue("voidMix"));
            module->setParameterPointer("bypass", apvts.getRawParameterValue("voidBypass"));
        }
        else if (module->getModuleType() == "CabinetIR")
        {
            module->setParameterPointer("micX", apvts.getRawParameterValue("cabMicX"));
            module->setParameterPointer("micY", apvts.getRawParameterValue("cabMicY"));
            module->setParameterPointer("distance", apvts.getRawParameterValue("cabDistance"));
            module->setParameterPointer("axis", apvts.getRawParameterValue("cabAxis"));
            module->setParameterPointer("type", apvts.getRawParameterValue("cabType"));
            module->setParameterPointer("bypass", apvts.getRawParameterValue("cabBypass"));
        }
    }
    
    DBG("Parameters connected to " + String(smartSignalChain.getNumEffects()) + " effects");
}

void GuitarPedalRackProcessor::rebuildChain()
{
    smartSignalChain.clearAllEffects();
    buildDefaultChain();
}

bool GuitarPedalRackProcessor::loadPreset(const String& presetName)
{
    ChainPreset preset;
    if (presetManager.loadPreset(presetName, preset))
    {
        currentPresetName = presetName;
        
        // Restore all parameters in APVTS
        for (const auto& pair : preset.parameterValues)
        {
            auto* param = apvts.getParameter(pair.first);
            if (param != nullptr)
            {
                float rawValue = pair.second;
                float normalized = param->getNormalisableRange().convertTo0to1(rawValue);
                param->setValueNotifyingHost(normalized);
            }
        }
        return true;
    }
    return false;
}

bool GuitarPedalRackProcessor::saveCurrentAsPreset(const String& presetName)
{
    ChainPreset preset;
    preset.name = presetName;
    preset.description = "User preset";
    preset.author = "User";
    preset.dateCreated = Time::getCurrentTime();
    preset.dateModified = preset.dateCreated;
    
    // Save current parameter values
    for (auto* param : getParameters())
    {
        if (auto* rangedParam = dynamic_cast<RangedAudioParameter*>(param))
        {
            String paramId = rangedParam->paramID;
            float value = apvts.getRawParameterValue(paramId)->load();
            preset.parameterValues[paramId] = value;
        }
    }
    
    currentPresetName = presetName;
    return presetManager.savePreset(preset);
}

void GuitarPedalRackProcessor::setInputGain(float gainDb)
{
    inputGainDb = jlimit(-20.0f, 20.0f, gainDb);
    updateGainValues();
}

void GuitarPedalRackProcessor::setOutputGain(float gainDb)
{
    outputGainDb = jlimit(-20.0f, 20.0f, gainDb);
    updateGainValues();
}

void GuitarPedalRackProcessor::setBypassAll(bool shouldBypass)
{
    bypassAll = shouldBypass;
}

float GuitarPedalRackProcessor::dbToLinear(float db)
{
    return std::pow(10.0f, db / 20.0f);
}

void GuitarPedalRackProcessor::updateGainValues()
{
    inputGainLinear = dbToLinear(inputGainDb);
    outputGainLinear = dbToLinear(outputGainDb);
}

void GuitarPedalRackProcessor::updateParametersFromAPVTS()
{
    // This function can be used to update DSP modules from APVTS
    // For now, modules will read parameters directly via APVTS pointers
    // In a full implementation, we would update each module here
}

//==============================================================================
// This creates new instances of the plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarPedalRackProcessor();
}

//==============================================================================
// UI Integration Methods
//==============================================================================

void GuitarPedalRackProcessor::addEffectToChain(const juce::String& effectId)
{
    auto& library = EffectLibrary::getInstance();
    auto effect = library.createEffect(effectId);
    
    if (effect)
    {
        smartSignalChain.addEffect(std::move(effect));
        DBG("Added effect: " + effectId + " (Total: " + juce::String(smartSignalChain.getNumEffects()) + ")");
    }
    else
    {
        DBG("Failed to create effect: " + effectId);
    }
}

void GuitarPedalRackProcessor::removeEffectFromChain(int index)
{
    int sizeBefore = smartSignalChain.getNumEffects();
    
    if (index >= 0 && index < sizeBefore)
    {
        DBG("PROCESSOR: Removing effect at index " + juce::String(index) + 
            " (chain size before: " + juce::String(sizeBefore) + ")");
        
        // Thread-safe removal: suspend processing while modifying chain
        suspendProcessing(true);
        smartSignalChain.removeEffect(index);
        suspendProcessing(false);
        
        int sizeAfter = smartSignalChain.getNumEffects();
        DBG("PROCESSOR: Effect removed. Chain size after: " + juce::String(sizeAfter));
        
        if (sizeBefore == sizeAfter)
        {
            DBG("WARNING: Effect was not actually removed from chain!");
        }
    }
    else
    {
        DBG("ERROR: Invalid index " + juce::String(index) + 
            " for chain of size " + juce::String(sizeBefore));
    }
}

void GuitarPedalRackProcessor::moveEffectInChain(int fromIndex, int toIndex)
{
    if (fromIndex >= 0 && fromIndex < smartSignalChain.getNumEffects() &&
        toIndex >= 0 && toIndex <= smartSignalChain.getNumEffects() &&
        fromIndex != toIndex)
    {
        smartSignalChain.moveEffect(fromIndex, toIndex);
        DBG("Moved effect from " + juce::String(fromIndex) + " to " + juce::String(toIndex));
    }
}

void GuitarPedalRackProcessor::setEffectBypassed(int index, bool shouldBypass)
{
    if (index >= 0 && index < smartSignalChain.getNumEffects())
    {
        auto* effect = smartSignalChain.getEffect(index);
        if (effect)
        {
            effect->setBypass(shouldBypass);
            DBG("Set effect " + juce::String(index) + " bypass: " + (shouldBypass ? "ON" : "OFF"));
        }
    }
}

int GuitarPedalRackProcessor::getEffectChainSize() const
{
    return smartSignalChain.getNumEffects();
}

juce::String GuitarPedalRackProcessor::getEffectNameAtIndex(int index) const
{
    auto* effect = const_cast<SmartSignalChain&>(smartSignalChain).getEffect(index);
    if (effect)
        return effect->getModuleType();
    return juce::String();
}

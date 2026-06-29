#include "PluginProcessor.h"
#include "PluginEditor.h"
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
    signalChain.prepare(sampleRate, samplesPerBlock);
    
    // Prepare parameter smoothing
    smoothedInputGain.reset(sampleRate, 0.05);  // 50ms ramp
    smoothedOutputGain.reset(sampleRate, 0.05);
    
    smoothedInputGain.setCurrentAndTargetValue(dbToLinear(inputGainDb));
    smoothedOutputGain.setCurrentAndTargetValue(dbToLinear(outputGainDb));
}

void GuitarPedalRackProcessor::releaseResources()
{
    signalChain.reset();
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
        return;
    
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
    
    // Process through signal chain
    signalChain.process(buffer);
    
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
    
    // Compute CPU usage
    auto endTime = std::chrono::high_resolution_clock::now();
    auto elapsedMicros = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    double blockDurationSeconds = (double)buffer.getNumSamples() / (getSampleRate() > 0.0 ? getSampleRate() : 44100.0);
    double blockDurationMicros = blockDurationSeconds * 1000000.0;
    float usagePercent = 0.0f;
    if (blockDurationMicros > 0.0)
        usagePercent = (float)((elapsedMicros / blockDurationMicros) * 100.0);
    
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
    return new GuitarPedalRackEditor(*this);
}

//==============================================================================
void GuitarPedalRackProcessor::getStateInformation(MemoryBlock& destData)
{
    // Use APVTS to save all parameter state
    auto state = apvts.copyState();
    
    // Add custom properties
    state.setProperty("currentPresetName", currentPresetName, nullptr);
    
    // Serialize to XML
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GuitarPedalRackProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore from APVTS
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            auto state = ValueTree::fromXml(*xmlState);
            apvts.replaceState(state);
            
            // Restore custom properties
            currentPresetName = state.getProperty("currentPresetName", "Default").toString();
        }
    }
}

//==============================================================================
void GuitarPedalRackProcessor::buildDefaultChain()
{
    signalChain.clearChain();
    
    // Build complete signal chain:
    // Gate → Overdrive → Chorus → Amp → Reverb → Cabinet
    signalChain.addModule(std::make_unique<NoiseGate>());
    signalChain.addModule(std::make_unique<TubeOverdrive>());  // Deep Heat
    signalChain.addModule(std::make_unique<Chorus>());         // Pulse
    signalChain.addModule(std::make_unique<AmpSimulator>());   // Alpha Amp
    signalChain.addModule(std::make_unique<ReverbEffect>());   // Void
    signalChain.addModule(std::make_unique<CabinetIR>());
    
    currentPresetName = "Default Chain";
    
    // Connect parameters to modules
    connectParametersToSignalChain();
}

void GuitarPedalRackProcessor::connectParametersToSignalChain()
{
    // Connect parameters to modules by searching their types rather than hardcoding indices,
    // which allows the signal chain order to be rearranged dynamically!
    for (int i = 0; i < signalChain.getNumModules(); ++i)
    {
        auto* module = signalChain.getModule(i);
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
}

void GuitarPedalRackProcessor::rebuildChain()
{
    signalChain.clearChain();
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

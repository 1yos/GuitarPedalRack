#pragma once

#include "JuceHeader.h"
#include "DSP/SignalChain.h"
#include "State/PresetManager.h"

// Include all effect modules
#include "DSP/Dynamics/NoiseGate.h"
#include "DSP/Dynamics/Compressor.h"
#include "DSP/Drive/TubeOverdrive.h"
#include "DSP/Drive/Distortion.h"
#include "DSP/Amp/AmpSimulator.h"
#include "DSP/Cabinet/CabinetIR.h"
#include "DSP/Modulation/Chorus.h"
#include "DSP/TimeFX/Delay.h"
#include "DSP/TimeFX/Reverb.h"
#include "DSP/Utility/EQ.h"

using namespace juce;

//==============================================================================
/**
 * Main plugin processor for Guitar Pedal Rack
 */
class GuitarPedalRackProcessor : public AudioProcessor
{
public:
    GuitarPedalRackProcessor();
    ~GuitarPedalRackProcessor() override;
    
    //==============================================================================
    // APVTS (Public for direct UI attachment)
    AudioProcessorValueTreeState apvts;

    //==============================================================================
    // AudioProcessor overrides
    
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
   #endif

    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const String getProgramName(int index) override;
    void changeProgramName(int index, const String& newName) override;

    void getStateInformation(MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Signal Chain Access
    
    SignalChain& getSignalChain() { return signalChain; }
    const SignalChain& getSignalChain() const { return signalChain; }
    
    /** Builds a default signal chain */
    void buildDefaultChain();
    
    /** Clears the chain and rebuilds from scratch */
    void rebuildChain();
    
    /** Connects APVTS parameters to signal chain modules */
    void connectParametersToSignalChain();

    //==============================================================================
    // Preset Management
    
    PresetManager& getPresetManager() { return presetManager; }
    
    /** Loads a preset by name */
    bool loadPreset(const String& presetName);
    
    /** Saves current configuration as a preset */
    bool saveCurrentAsPreset(const String& presetName);
    
    /** Returns the current preset name */
    String getCurrentPresetName() const { return currentPresetName; }

    //==============================================================================
    // Parameters (Global)
    
    void setInputGain(float gainDb);
    float getInputGain() const { return inputGainDb; }
    
    void setOutputGain(float gainDb);
    float getOutputGain() const { return outputGainDb; }
    
    void setBypassAll(bool shouldBypass);
    bool isBypassedAll() const { return bypassAll; }

    float getDspCpuUsage() const { return dspCpuUsage.load(); }
    float getCPUUsage() const { return dspCpuUsage.load(); } // Alias for editor

private:
    //==============================================================================
    // APVTS Parameter Layout
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    //==============================================================================
    // Core components
    SignalChain signalChain;
    PresetManager presetManager;
    
    // Global parameters
    float inputGainDb = 0.0f;
    float outputGainDb = 0.0f;
    float inputGainLinear = 1.0f;
    float outputGainLinear = 1.0f;
    bool bypassAll = false;
    
    String currentPresetName = "Default";
    std::atomic<float> dspCpuUsage { 0.0f };
    
    // Parameter smoothing
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothedInputGain;
    SmoothedValue<float, ValueSmoothingTypes::Linear> smoothedOutputGain;
    
    // Helper functions
    float dbToLinear(float db);
    void updateGainValues();
    void updateParametersFromAPVTS();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GuitarPedalRackProcessor)
};

#pragma once

#include "../AudioModule.h"
#include <juce_dsp/juce_dsp.h>

//==============================================================================
/**
 * Cabinet IR Loader - Impulse Response based cabinet simulation.
 * 
 * Features:
 * - Zero-latency convolution
 * - Stereo and mono IR support
 * - Built-in IR library
 * - User IR loading
 * - Mic position simulation
 */
class CabinetIR : public AudioModule
{
public:
    CabinetIR();
    ~CabinetIR() override = default;

    //==============================================================================
    // IR Management
    
    /** Loads an IR from a file */
    bool loadIRFromFile(const File& file);
    
    /** Loads a built-in IR by name */
    bool loadBuiltInIR(const String& irName);
    
    /** Returns the list of available built-in IRs */
    StringArray getBuiltInIRList() const;
    
    /** Returns the currently loaded IR name */
    String getCurrentIRName() const { return currentIRName; }

    //==============================================================================
    // Parameters
    
    /** Sets the mic mix (0.0 = on-axis, 1.0 = off-axis) */
    void setMicPosition(float position);
    float getMicPosition() const { return micPosition; }
    
    /** Sets the room mix (0.0 = close mic, 1.0 = room) */
    void setRoomMix(float mix);
    float getRoomMix() const { return roomMix; }
    
    /** Sets the output level (-20dB to +20dB) */
    void setOutputLevel(float levelDb);
    float getOutputLevel() const { return outputLevelDb; }

    //==============================================================================
    // AudioModule overrides
    
    void prepare(double sampleRate, int maximumExpectedSamplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "CabinetIR"; }
    String getDisplayName() const override { return "Cabinet"; }
    
    void saveState(MemoryBlock& destData) override;
    void loadState(const void* data, int sizeInBytes) override;

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    //==============================================================================
    // Convolution engines
    juce::dsp::Convolution convolution;
    
    // IR data
    AudioBuffer<float> currentIR;
    String currentIRName = "Default 4x12";
    
    // Parameters
    float micPosition = 0.5f;
    float roomMix = 0.0f;
    float outputLevelDb = 0.0f;
    float outputLevel = 1.0f;
    
    // Physical mic placement DSP simulation variables
    float lpValL = 0.0f;
    float lpValR = 0.0f;
    
    std::vector<float> delayBufferL;
    std::vector<float> delayBufferR;
    int writeIndexL = 0;
    int writeIndexR = 0;
    
    //==============================================================================
    // Helper functions
    void loadDefaultIR();
    AudioBuffer<float> generateDefaultIR();  // Simple fallback IR
    float dbToLinear(float db);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CabinetIR)
};

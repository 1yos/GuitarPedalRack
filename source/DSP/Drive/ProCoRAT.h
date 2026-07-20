#pragma once

#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>

using namespace juce;

//==============================================================================
/**
 * ProCo RAT - Classic Distortion Pedal
 * 
 * Aggressive hard-clipping distortion with filter control.
 * Based on the ProCo RAT circuit.
 * 
 * Features:
 * - Hard op-amp clipping
 * - Pre-distortion low-pass filter
 * - Post-distortion tone control (variable low-pass)
 * - Aggressive, cutting distortion character
 * - SIMD-optimized processing
 * 
 * Parameters:
 * - Distortion: Amount of gain/clipping (0.0 - 1.0)
 * - Filter: Tone control - brightness (0.0 = dark, 1.0 = bright)
 * - Volume: Output level (0.0 - 1.0)
 */
class ProCoRAT : public AudioModule
{
public:
    ProCoRAT();
    ~ProCoRAT() override;
    
    //==============================================================================
    // AudioModule interface
    
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    
    String getModuleType() const override { return "proCoRAT"; }
    
    //==============================================================================
    // Parameters
    
    /** Set distortion amount (0.0 - 1.0) */
    void setDistortion(float distortion);
    float getDistortion() const { return distortionParam.load(); }
    
    /** Set filter/tone control (0.0 - 1.0) */
    void setFilter(float filter);
    float getFilter() const { return filterParam.load(); }
    
    /** Set output volume (0.0 - 1.0) */
    void setVolume(float volume);
    float getVolume() const { return volumeParam.load(); }

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    //==============================================================================
    // DSP State
    
    // Parameters (atomic for thread-safety)
    std::atomic<float> distortionParam{0.5f};
    std::atomic<float> filterParam{0.5f};
    std::atomic<float> volumeParam{0.7f};
    
    // Filter states (per channel)
    struct ChannelState
    {
        // Input high-pass (DC blocking)
        float hpIn_z1 = 0.0f;
        
        // Pre-distortion low-pass filter
        float preLp_z1 = 0.0f;
        
        // Post-distortion tone control
        float tone_z1 = 0.0f;
        
        // Output high-pass (DC removal)
        float hpOut_z1 = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    // Filter coefficients
    float hpCoeff = 0.995f;  // High-pass cutoff
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProCoRAT)
};

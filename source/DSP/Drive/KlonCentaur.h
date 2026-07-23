#pragma once

#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>

using namespace juce;

//==============================================================================
/**
 * Klon Centaur - Legendary Transparent Overdrive
 * 
 * Transparent overdrive that preserves pick dynamics and cleans up with guitar volume.
 * Based on the Klon Centaur circuit.
 * 
 * Features:
 * - Clean/dirty signal blending
 * - Germanium diode soft clipping
 * - Treble boost circuit
 * - Transparent, amp-like response
 * - Touch-sensitive dynamics
 * - SIMD-optimized processing
 * 
 * Parameters:
 * - Gain: Amount of overdrive (0.0 - 1.0)
 * - Treble: High-frequency boost (0.0 - 1.0)
 * - Output: Output level (0.0 - 1.0)
 */
class KlonCentaur : public AudioModule
{
public:
    KlonCentaur();
    ~KlonCentaur() override;
    
    //==============================================================================
    // AudioModule interface
    
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    
    String getModuleType() const override { return "klonCentaur"; }
    
    //==============================================================================
    // Parameters
    
    /** Set gain amount (0.0 - 1.0) */
    void setGain(float gain);
    float getGain() const { return gainParam.load(); }
    
    /** Set treble boost (0.0 - 1.0) */
    void setTreble(float treble);
    float getTreble() const { return trebleParam.load(); }
    
    /** Set output level (0.0 - 1.0) */
    void setOutput(float output);
    float getOutput() const { return outputParam.load(); }

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    //==============================================================================
    // DSP State
    
    // Parameters (atomic for thread-safety)
    std::atomic<float> gainParam{0.5f};
    std::atomic<float> trebleParam{0.5f};
    std::atomic<float> outputParam{0.7f};
    
    // Filter states (per channel)
    struct ChannelState
    {
        // Input high-pass (DC blocking)
        float hpIn_z1 = 0.0f;
        
        // Treble boost filter
        float treble_z1 = 0.0f;
        
        // Output high-pass (DC removal)
        float hpOut_z1 = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    // Filter coefficients
    float hpCoeff = 0.995f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KlonCentaur)
};

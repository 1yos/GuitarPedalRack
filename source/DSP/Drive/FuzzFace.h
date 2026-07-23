#pragma once

#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>

using namespace juce;

//==============================================================================
/**
 * Fuzz Face - Classic Germanium Fuzz Pedal
 * 
 * Simple two-transistor fuzz with vintage character.
 * Based on the Dallas Arbiter Fuzz Face circuit (Germanium version).
 * 
 * Features:
 * - Two-transistor gain stages
 * - Temperature-sensitive germanium character
 * - Smooth, warm fuzz tone
 * - Guitar volume cleanup
 * - Simplicity (only 2 controls)
 * - SIMD-optimized processing
 * 
 * Parameters:
 * - Fuzz: Amount of fuzz/gain (0.0 - 1.0)
 * - Volume: Output level (0.0 - 1.0)
 */
class FuzzFace : public AudioModule
{
public:
    FuzzFace();
    ~FuzzFace() override;
    
    //==============================================================================
    // AudioModule interface
    
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    
    String getModuleType() const override { return "fuzzFace"; }
    
    //==============================================================================
    // Parameters
    
    /** Set fuzz amount (0.0 - 1.0) */
    void setFuzz(float fuzz);
    float getFuzz() const { return fuzzParam.load(); }
    
    /** Set output volume (0.0 - 1.0) */
    void setVolume(float volume);
    float getVolume() const { return volumeParam.load(); }

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    //==============================================================================
    // DSP State
    
    // Parameters (atomic for thread-safety)
    std::atomic<float> fuzzParam{0.5f};
    std::atomic<float> volumeParam{0.7f};
    
    // Filter states (per channel)
    struct ChannelState
    {
        // Input high-pass (AC coupling)
        float hpIn_z1 = 0.0f;
        
        // Q1 transistor low-pass
        float q1Lp_z1 = 0.0f;
        
        // Q2 transistor low-pass
        float q2Lp_z1 = 0.0f;
        
        // Output high-pass (DC removal)
        float hpOut_z1 = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    // Filter coefficients
    float hpCoeff = 0.995f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FuzzFace)
};

#pragma once

#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>

using namespace juce;

//==============================================================================
/**
 * Electro-Harmonix Big Muff Pi - Classic Fuzz Pedal
 * 
 * Thick, saturated fuzz with sustain and scooped mids.
 * Based on the Big Muff Pi circuit.
 * 
 * Features:
 * - Four-stage transistor gain
 * - Smooth fuzz clipping
 * - Big Muff tone stack (scooped mids)
 * - Long sustain
 * - SIMD-optimized processing
 * 
 * Parameters:
 * - Sustain: Amount of fuzz/gain (0.0 - 1.0)
 * - Tone: Mid scoop balance (0.0 = bass, 1.0 = treble)
 * - Volume: Output level (0.0 - 1.0)
 */
class BigMuffPi : public AudioModule
{
public:
    BigMuffPi();
    ~BigMuffPi() override;
    
    //==============================================================================
    // AudioModule interface
    
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    
    String getModuleType() const override { return "bigMuffPi"; }
    
    //==============================================================================
    // Parameters
    
    /** Set sustain/gain amount (0.0 - 1.0) */
    void setSustain(float sustain);
    float getSustain() const { return sustainParam.load(); }
    
    /** Set tone control (0.0 - 1.0) */
    void setTone(float tone);
    float getTone() const { return toneParam.load(); }
    
    /** Set output volume (0.0 - 1.0) */
    void setVolume(float volume);
    float getVolume() const { return volumeParam.load(); }

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    //==============================================================================
    // DSP State
    
    // Parameters (atomic for thread-safety)
    std::atomic<float> sustainParam{0.5f};
    std::atomic<float> toneParam{0.5f};
    std::atomic<float> volumeParam{0.7f};
    
    // Filter states (per channel)
    struct ChannelState
    {
        // Input high-pass (DC blocking)
        float hpIn_z1 = 0.0f;
        
        // Stage 1 filter
        float stage1_z1 = 0.0f;
        
        // Stage 2 filter
        float stage2_z1 = 0.0f;
        
        // Tone stack filters (parallel bass/treble paths)
        float bassPath_z1 = 0.0f;
        float treblePath_z1 = 0.0f;
        
        // Output high-pass (DC removal)
        float hpOut_z1 = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    // Filter coefficients
    float hpCoeff = 0.995f;  // High-pass cutoff
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BigMuffPi)
};

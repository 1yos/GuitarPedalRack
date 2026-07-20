#pragma once

#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>

using namespace juce;

//==============================================================================
/**
 * Boss DS-1 Distortion - Classic Distortion Pedal
 * 
 * Hard-edged distortion with asymmetric clipping and tone control.
 * Based on the Boss DS-1 circuit.
 * 
 * Features:
 * - Asymmetric hard clipping
 * - Active tone control (shelving EQ)
 * - Aggressive, cutting distortion
 * - High-gain capability
 * - SIMD-optimized processing
 * 
 * Parameters:
 * - Dist: Amount of distortion/gain (0.0 - 1.0)
 * - Tone: Frequency balance (0.0 = dark, 1.0 = bright)
 * - Level: Output volume (0.0 - 1.0)
 */
class BossDS1 : public AudioModule
{
public:
    BossDS1();
    ~BossDS1() override;
    
    //==============================================================================
    // AudioModule interface
    
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    
    String getModuleType() const override { return "bossDS1"; }
    
    //==============================================================================
    // Parameters
    
    /** Set distortion amount (0.0 - 1.0) */
    void setDist(float dist);
    float getDist() const { return distParam.load(); }
    
    /** Set tone control (0.0 - 1.0) */
    void setTone(float tone);
    float getTone() const { return toneParam.load(); }
    
    /** Set output level (0.0 - 1.0) */
    void setLevel(float level);
    float getLevel() const { return levelParam.load(); }

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    //==============================================================================
    // DSP State
    
    // Parameters (atomic for thread-safety)
    std::atomic<float> distParam{0.5f};
    std::atomic<float> toneParam{0.5f};
    std::atomic<float> levelParam{0.7f};
    
    // Filter states (per channel)
    struct ChannelState
    {
        // Input high-pass (DC blocking)
        float hpIn_z1 = 0.0f;
        
        // Tone control shelving filter
        float tone_z1 = 0.0f;
        float tone_z2 = 0.0f;
        
        // Output high-pass (DC removal)
        float hpOut_z1 = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    // Filter coefficients
    float hpCoeff = 0.995f;  // High-pass cutoff
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BossDS1)
};

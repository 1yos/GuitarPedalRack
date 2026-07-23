#pragma once

#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>

using namespace juce;

//==============================================================================
/**
 * Boss SD-1 Super Overdrive - Classic Overdrive Pedal
 * 
 * TS-style overdrive with harder clipping and more gain.
 * Based on the Boss SD-1 circuit.
 * 
 * Features:
 * - Asymmetric clipping (harder than TS808)
 * - Mid-range emphasis
 * - Higher gain range than Tube Screamer
 * - Active tone control
 * - SIMD-optimized processing
 * 
 * Parameters:
 * - Drive: Amount of overdrive (0.0 - 1.0)
 * - Tone: Frequency balance (0.0 - 1.0)
 * - Level: Output volume (0.0 - 1.0)
 */
class BossSD1 : public AudioModule
{
public:
    BossSD1();
    ~BossSD1() override;
    
    //==============================================================================
    // AudioModule interface
    
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    
    String getModuleType() const override { return "bossSD1"; }
    
    //==============================================================================
    // Parameters
    
    /** Set drive amount (0.0 - 1.0) */
    void setDrive(float drive);
    float getDrive() const { return driveParam.load(); }
    
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
    std::atomic<float> driveParam{0.5f};
    std::atomic<float> toneParam{0.5f};
    std::atomic<float> levelParam{0.7f};
    
    // Filter states (per channel)
    struct ChannelState
    {
        // Input high-pass (DC blocking)
        float hpIn_z1 = 0.0f;
        
        // Mid boost
        float mid_z1 = 0.0f;
        
        // Tone control
        float tone_z1 = 0.0f;
        
        // Output high-pass (DC removal)
        float hpOut_z1 = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    // Filter coefficients
    float hpCoeff = 0.995f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BossSD1)
};

#pragma once

#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>

using namespace juce;

//==============================================================================
/**
 * Tube Screamer 808 - Classic Overdrive Pedal
 * 
 * Iconic mid-range focused overdrive with soft asymmetric clipping.
 * Based on the Ibanez TS808 circuit.
 * 
 * Features:
 * - Asymmetric soft clipping (op-amp + diodes)
 * - Mid-range frequency boost (~720Hz)
 * - Smooth, singing overdrive character
 * - SIMD-optimized processing
 * 
 * Parameters:
 * - Drive: Amount of overdrive/distortion (0.0 - 1.0)
 * - Tone: Frequency balance (0.0 = dark, 1.0 = bright)
 * - Level: Output volume (0.0 - 1.0)
 */
class TubeScreamer808 : public AudioModule
{
public:
    TubeScreamer808();
    ~TubeScreamer808() override;
    
    //==============================================================================
    // AudioModule interface
    
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    
    String getModuleType() const override { return "tubeScreamer808"; }
    
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
        // High-pass input filter (removes DC)
        float hpIn_z1 = 0.0f;
        
        // Mid-boost filter state
        float midBoost_z1 = 0.0f;
        float midBoost_z2 = 0.0f;
        
        // Tone control filter
        float tone_z1 = 0.0f;
        
        // Output high-pass (DC removal)
        float hpOut_z1 = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    // Filter coefficients
    float hpCoeff = 0.995f;        // High-pass cutoff
    float midFreq = 720.0f;        // Mid-boost frequency
    float midGain = 6.0f;          // Mid-boost gain (dB)
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TubeScreamer808)
};

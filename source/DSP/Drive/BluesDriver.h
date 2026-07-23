#pragma once

#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>

using namespace juce;

//==============================================================================
/**
 * Boss BD-2 Blues Driver - Classic Blues Overdrive
 * 
 * Warm, tube-like overdrive designed for blues and classic rock.
 * Based on the Boss BD-2 circuit.
 * 
 * Features:
 * - Asymmetric soft clipping
 * - Warm, tube-like character
 * - Rich harmonic content
 * - Responsive to pick dynamics
 * - Active tone control
 * - SIMD-optimized processing
 * 
 * Parameters:
 * - Gain: Amount of overdrive (0.0 - 1.0)
 * - Tone: Frequency balance (0.0 - 1.0)
 * - Level: Output volume (0.0 - 1.0)
 */
class BluesDriver : public AudioModule
{
public:
    BluesDriver();
    ~BluesDriver() override;
    
    //==============================================================================
    // AudioModule interface
    
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    
    String getModuleType() const override { return "bluesDriver"; }
    
    //==============================================================================
    // Parameters
    
    /** Set gain amount (0.0 - 1.0) */
    void setGain(float gain);
    float getGain() const { return gainParam.load(); }
    
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
    std::atomic<float> gainParam{0.5f};
    std::atomic<float> toneParam{0.5f};
    std::atomic<float> levelParam{0.7f};
    
    // Filter states (per channel)
    struct ChannelState
    {
        // Input high-pass (DC blocking)
        float hpIn_z1 = 0.0f;
        
        // Pre-emphasis filter
        float preEmph_z1 = 0.0f;
        
        // Tone control
        float tone_z1 = 0.0f;
        
        // Output high-pass (DC removal)
        float hpOut_z1 = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    // Filter coefficients
    float hpCoeff = 0.995f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BluesDriver)
};

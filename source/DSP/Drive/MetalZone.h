#pragma once

#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>

using namespace juce;

//==============================================================================
/**
 * Boss MT-2 Metal Zone - High-Gain Metal Distortion
 * 
 * Extreme gain distortion with parametric EQ.
 * 
 * Parameters:
 * - Distortion: Amount of gain (0.0 - 1.0)
 * - Treble/Bass: Simple 2-band EQ (0.0 - 1.0 each)
 * - Level: Output volume (0.0 - 1.0)
 */
class MetalZone : public AudioModule
{
public:
    MetalZone();
    ~MetalZone() override;
    
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "metalZone"; }
    
    void setDistortion(float dist);
    void setTreble(float treble);
    void setBass(float bass);
    void setLevel(float level);

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    std::atomic<float> distParam{0.7f};
    std::atomic<float> trebleParam{0.5f};
    std::atomic<float> bassParam{0.5f};
    std::atomic<float> levelParam{0.7f};
    
    struct ChannelState
    {
        float hpIn_z1 = 0.0f;
        float bass_z1 = 0.0f;
        float treble_z1 = 0.0f;
        float hpOut_z1 = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    float hpCoeff = 0.995f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MetalZone)
};

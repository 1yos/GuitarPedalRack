#pragma once

#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>

using namespace juce;

//==============================================================================
/**
 * Ibanez Tube Screamer TS9 - Classic Overdrive
 * 
 * Similar to TS808 but with slightly different clipping characteristics.
 * Harder clipping, slightly less mid-boost.
 * 
 * Parameters:
 * - Drive: Amount of overdrive (0.0 - 1.0)
 * - Tone: Frequency balance (0.0 - 1.0)
 * - Level: Output volume (0.0 - 1.0)
 */
class TubeScreamerTS9 : public AudioModule
{
public:
    TubeScreamerTS9();
    ~TubeScreamerTS9() override;
    
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "tubeScreamerTS9"; }
    
    void setDrive(float drive);
    float getDrive() const { return driveParam.load(); }
    void setTone(float tone);
    float getTone() const { return toneParam.load(); }
    void setLevel(float level);
    float getLevel() const { return levelParam.load(); }

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    std::atomic<float> driveParam{0.5f};
    std::atomic<float> toneParam{0.5f};
    std::atomic<float> levelParam{0.7f};
    
    struct ChannelState
    {
        float hpIn_z1 = 0.0f;
        float mid_z1 = 0.0f;
        float tone_z1 = 0.0f;
        float hpOut_z1 = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    float hpCoeff = 0.995f;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TubeScreamerTS9)
};

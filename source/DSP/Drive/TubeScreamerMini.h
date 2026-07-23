#pragma once
#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>
using namespace juce;

class TubeScreamerMini : public AudioModule
{
public:
    TubeScreamerMini();
    ~TubeScreamerMini() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "tubeScreamerMini"; }
    void setOverdrive(float od);
    void setTone(float tone);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> overdriveParam{0.5f};
    std::atomic<float> toneParam{0.5f};
    struct ChannelState { float hpIn_z1 = 0.0f; float mid_z1 = 0.0f; float tone_z1 = 0.0f; float hpOut_z1 = 0.0f; };
    std::vector<ChannelState> channelState;
    float hpCoeff = 0.995f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TubeScreamerMini)
};

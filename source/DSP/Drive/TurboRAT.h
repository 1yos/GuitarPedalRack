#pragma once
#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>
using namespace juce;

class TurboRAT : public AudioModule
{
public:
    TurboRAT();
    ~TurboRAT() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "turboRAT"; }
    void setDistortion(float dist);
    void setTone(float tone);
    void setVolume(float volume);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> distortionParam{0.6f};
    std::atomic<float> toneParam{0.6f};
    std::atomic<float> volumeParam{0.7f};
    struct ChannelState { float hp_z1 = 0.0f; float tone_z1 = 0.0f; float hpOut_z1 = 0.0f; };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TurboRAT)
};

#pragma once
#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>
using namespace juce;

class FuzzFaceSilicon : public AudioModule
{
public:
    FuzzFaceSilicon();
    ~FuzzFaceSilicon() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "fuzzFaceSilicon"; }
    void setFuzz(float fuzz);
    void setVolume(float volume);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> fuzzParam{0.5f};
    std::atomic<float> volumeParam{0.7f};
    struct ChannelState { float hp_z1 = 0.0f; float q1_z1 = 0.0f; float q2_z1 = 0.0f; float hpOut_z1 = 0.0f; };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FuzzFaceSilicon)
};

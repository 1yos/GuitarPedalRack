#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class OpticalCompressor : public AudioModule
{
public:
    OpticalCompressor();
    ~OpticalCompressor() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "opticalCompressor"; }
    void setThreshold(float threshold);
    void setRatio(float ratio);
    void setAttack(float attack);
    void setRelease(float release);
    void setMakeup(float makeup);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> thresholdParam{0.6f}, ratioParam{0.4f}, attackParam{0.3f}, releaseParam{0.6f}, makeupParam{0.3f};
    struct ChannelState {
        float envelope = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpticalCompressor)
};

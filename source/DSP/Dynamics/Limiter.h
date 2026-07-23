#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class Limiter : public AudioModule
{
public:
    Limiter();
    ~Limiter() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "limiter"; }
    void setThreshold(float threshold);
    void setRelease(float release);
    void setMakeup(float makeup);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> thresholdParam{0.8f}, releaseParam{0.5f}, makeupParam{0.0f};
    struct ChannelState {
        float envelope = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Limiter)
};

#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class NotchFilter : public AudioModule
{
public:
    NotchFilter();
    ~NotchFilter() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "notchFilter"; }
    void setFrequency(float frequency);
    void setBandwidth(float bandwidth);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> frequencyParam{0.5f}, bandwidthParam{0.3f};
    struct ChannelState {
        float z1 = 0.0f;
        float z2 = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NotchFilter)
};

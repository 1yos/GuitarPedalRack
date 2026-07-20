#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class Expander : public AudioModule
{
public:
    Expander();
    ~Expander() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "expander"; }
    void setThreshold(float threshold);
    void setRatio(float ratio);
    void setAttack(float attack);
    void setRelease(float release);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> thresholdParam{0.3f}, ratioParam{0.5f}, attackParam{0.3f}, releaseParam{0.5f};
    struct ChannelState {
        float envelope = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Expander)
};

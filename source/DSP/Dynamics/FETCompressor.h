#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class FETCompressor : public AudioModule
{
public:
    FETCompressor();
    ~FETCompressor() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "fetCompressor"; }
    void setThreshold(float threshold);
    void setRatio(float ratio);
    void setAttack(float attack);
    void setRelease(float release);
    void setMakeup(float makeup);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> thresholdParam{0.65f}, ratioParam{0.3f}, attackParam{0.1f}, releaseParam{0.3f}, makeupParam{0.4f};
    struct ChannelState {
        float envelope = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FETCompressor)
};

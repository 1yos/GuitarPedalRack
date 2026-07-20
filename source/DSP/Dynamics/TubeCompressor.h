#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class TubeCompressor : public AudioModule
{
public:
    TubeCompressor();
    ~TubeCompressor() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "tubeCompressor"; }
    void setThreshold(float threshold);
    void setRatio(float ratio);
    void setAttack(float attack);
    void setRelease(float release);
    void setMakeup(float makeup);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> thresholdParam{0.7f}, ratioParam{0.3f}, attackParam{0.5f}, releaseParam{0.6f}, makeupParam{0.2f};
    struct ChannelState {
        float envelope = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TubeCompressor)
};

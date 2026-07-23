#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class MultibandCompressor : public AudioModule
{
public:
    MultibandCompressor();
    ~MultibandCompressor() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "multibandCompressor"; }
    void setLowThreshold(float threshold);
    void setMidThreshold(float threshold);
    void setHighThreshold(float threshold);
    void setRatio(float ratio);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> lowThresholdParam{0.6f}, midThresholdParam{0.6f}, highThresholdParam{0.6f}, ratioParam{0.5f};
    static constexpr int numBands = 3;
    struct BandState {
        float lpZ1 = 0.0f, lpZ2 = 0.0f;
        float hpZ1 = 0.0f, hpZ2 = 0.0f;
        float envelope = 0.0f;
    };
    struct ChannelState {
        BandState bands[numBands];
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultibandCompressor)
};

#pragma once
#include "../AudioModule.h"
#include <atomic>
#include <vector>
using namespace juce;

class BossCE2 : public AudioModule
{
public:
    BossCE2();
    ~BossCE2() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "bossCE2"; }
    void setRate(float rate);
    void setDepth(float depth);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> rateParam{0.35f};
    std::atomic<float> depthParam{0.7f};
    struct ChannelState { std::vector<float> delayBuf; int writePos = 0; float lfoPhase = 0.0f; };
    std::vector<ChannelState> channelState;
    static constexpr int maxDelay = 2205;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BossCE2)
};

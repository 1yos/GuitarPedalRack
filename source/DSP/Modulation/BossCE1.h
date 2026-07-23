#pragma once
#include "../AudioModule.h"
#include <atomic>
#include <vector>
using namespace juce;

class BossCE1 : public AudioModule
{
public:
    BossCE1();
    ~BossCE1() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "bossCE1"; }
    void setRate(float rate);
    void setDepth(float depth);
    void setMode(float mode);  // 0=chorus, 1=vibrato
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> rateParam{0.3f};
    std::atomic<float> depthParam{0.65f};
    std::atomic<float> modeParam{0.0f};
    struct ChannelState { std::vector<float> delayBuf; int writePos = 0; float lfoPhase = 0.0f; };
    std::vector<ChannelState> channelState;
    static constexpr int maxDelay = 4410;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BossCE1)
};

#pragma once
#include "../AudioModule.h"
#include <atomic>
#include <vector>
using namespace juce;

class FlangerBasic : public AudioModule
{
public:
    FlangerBasic();
    ~FlangerBasic() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "flangerBasic"; }
    void setRate(float rate);
    void setDepth(float depth);
    void setFeedback(float feedback);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> rateParam{0.3f}, depthParam{0.6f}, feedbackParam{0.5f}, mixParam{0.5f};
    struct ChannelState { std::vector<float> delayBuf; int writePos = 0; float lfoPhase = 0.0f; };
    std::vector<ChannelState> channelState;
    static constexpr int maxDelay = 2205;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlangerBasic)
};

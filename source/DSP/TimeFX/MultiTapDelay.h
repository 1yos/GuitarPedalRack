#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class MultiTapDelay : public AudioModule
{
public:
    MultiTapDelay();
    ~MultiTapDelay() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "multiTapDelay"; }
    void setTime(float time);
    void setFeedback(float feedback);
    void setTaps(float taps);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> timeParam{0.3f}, feedbackParam{0.4f}, tapsParam{0.5f}, mixParam{0.5f};
    static constexpr int maxDelay = 96000;
    struct ChannelState { 
        std::vector<float> delayBuf; 
        int writePos = 0;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiTapDelay)
};

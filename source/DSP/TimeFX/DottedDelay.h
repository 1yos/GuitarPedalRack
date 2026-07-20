#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class DottedDelay : public AudioModule
{
public:
    DottedDelay();
    ~DottedDelay() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "dottedDelay"; }
    void setTime(float time);
    void setFeedback(float feedback);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> timeParam{0.33f}, feedbackParam{0.5f}, mixParam{0.5f};
    static constexpr int maxDelay = 96000;
    struct ChannelState { 
        std::vector<float> delayBuf; 
        int writePos = 0;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DottedDelay)
};

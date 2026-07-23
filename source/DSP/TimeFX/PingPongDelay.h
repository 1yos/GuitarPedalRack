#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class PingPongDelay : public AudioModule
{
public:
    PingPongDelay();
    ~PingPongDelay() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "pingPongDelay"; }
    void setTime(float time);
    void setFeedback(float feedback);
    void setStereoWidth(float width);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> timeParam{0.5f}, feedbackParam{0.5f}, widthParam{1.0f}, mixParam{0.5f};
    static constexpr int maxDelay = 96000;
    struct ChannelState { 
        std::vector<float> delayBuf; 
        int writePos = 0;
        float lpfZ1 = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PingPongDelay)
};

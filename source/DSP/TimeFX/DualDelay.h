#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class DualDelay : public AudioModule
{
public:
    DualDelay();
    ~DualDelay() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "dualDelay"; }
    void setTimeL(float timeL);
    void setTimeR(float timeR);
    void setFeedback(float feedback);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> timeLParam{0.3f}, timeRParam{0.5f}, feedbackParam{0.5f}, mixParam{0.5f};
    static constexpr int maxDelay = 96000;
    struct ChannelState { 
        std::vector<float> delayBuf; 
        int writePos = 0;
        float lpfZ1 = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DualDelay)
};

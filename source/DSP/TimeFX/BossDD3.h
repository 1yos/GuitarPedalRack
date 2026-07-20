#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class BossDD3 : public AudioModule
{
public:
    BossDD3();
    ~BossDD3() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "bossDD3"; }
    void setTime(float time);
    void setFeedback(float feedback);
    void setLevel(float level);
    void setMode(float mode);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> timeParam{0.4f}, feedbackParam{0.5f}, levelParam{0.5f}, modeParam{0.0f};
    static constexpr int maxDelay = 96000;
    struct ChannelState { 
        std::vector<float> delayBuf; 
        int writePos = 0;
        float modPhase = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BossDD3)
};

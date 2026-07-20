#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class SpaceEcho : public AudioModule
{
public:
    SpaceEcho();
    ~SpaceEcho() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "spaceEcho"; }
    void setTime(float time);
    void setFeedback(float feedback);
    void setWow(float wow);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> timeParam{0.5f}, feedbackParam{0.65f}, wowParam{0.4f}, mixParam{0.5f};
    static constexpr int maxDelay = 96000;
    struct ChannelState { 
        std::vector<float> delayBuf; 
        int writePos = 0;
        float wowPhase = 0.0f;
        float lpfZ1 = 0.0f;
        float hpfZ1 = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpaceEcho)
};

#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class SlapbackDelay : public AudioModule
{
public:
    SlapbackDelay();
    ~SlapbackDelay() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "slapbackDelay"; }
    void setTime(float time);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> timeParam{0.3f}, mixParam{0.5f};
    static constexpr int maxDelay = 22050;
    struct ChannelState { 
        std::vector<float> delayBuf; 
        int writePos = 0;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SlapbackDelay)
};

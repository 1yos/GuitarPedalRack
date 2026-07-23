#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class AnalogDelay : public AudioModule
{
public:
    AnalogDelay();
    ~AnalogDelay() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "analogDelay"; }
    void setTime(float time);
    void setFeedback(float feedback);
    void setModulation(float modulation);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> timeParam{0.35f}, feedbackParam{0.6f}, modulationParam{0.3f}, mixParam{0.5f};
    static constexpr int maxDelay = 96000;
    struct ChannelState { 
        std::vector<float> delayBuf; 
        int writePos = 0; 
        float modPhase = 0.0f;
        float lpfZ1 = 0.0f;
        float hpfZ1 = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnalogDelay)
};

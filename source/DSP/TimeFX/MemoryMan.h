#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class MemoryMan : public AudioModule
{
public:
    MemoryMan();
    ~MemoryMan() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "memoryMan"; }
    void setTime(float time);
    void setFeedback(float feedback);
    void setModulation(float modulation);
    void setBlend(float blend);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> timeParam{0.4f}, feedbackParam{0.6f}, modulationParam{0.4f}, blendParam{0.5f};
    static constexpr int maxDelay = 96000;
    struct ChannelState { 
        std::vector<float> delayBuf; 
        int writePos = 0;
        float modPhase = 0.0f;
        float lpfZ1 = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MemoryMan)
};

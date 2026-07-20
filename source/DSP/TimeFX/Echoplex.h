#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class Echoplex : public AudioModule
{
public:
    Echoplex();
    ~Echoplex() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "echoplex"; }
    void setTime(float time);
    void setFeedback(float feedback);
    void setVolume(float volume);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> timeParam{0.45f}, feedbackParam{0.7f}, volumeParam{0.6f};
    static constexpr int maxDelay = 96000;
    struct ChannelState { 
        std::vector<float> delayBuf; 
        int writePos = 0;
        float lpfZ1 = 0.0f;
        float hpfZ1 = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Echoplex)
};

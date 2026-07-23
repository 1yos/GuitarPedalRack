#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class RhythmicDelay : public AudioModule
{
public:
    RhythmicDelay();
    ~RhythmicDelay() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "rhythmicDelay"; }
    void setTime(float time);
    void setPattern(float pattern);
    void setFeedback(float feedback);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> timeParam{0.4f}, patternParam{0.5f}, feedbackParam{0.4f}, mixParam{0.5f};
    static constexpr int maxDelay = 96000;
    struct ChannelState { 
        std::vector<float> delayBuf; 
        int writePos = 0;
        int stepCounter = 0;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RhythmicDelay)
};

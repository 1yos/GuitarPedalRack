#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class TapeDelay : public AudioModule
{
public:
    TapeDelay();
    ~TapeDelay() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "tapeDelay"; }
    void setTime(float time);
    void setFeedback(float feedback);
    void setWow(float wow);
    void setFlutter(float flutter);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> timeParam{0.4f}, feedbackParam{0.5f}, wowParam{0.3f}, flutterParam{0.2f}, mixParam{0.4f};
    static constexpr int maxDelay = 96000;
    struct ChannelState { 
        std::vector<float> delayBuf; 
        int writePos = 0; 
        float wowPhase = 0.0f; 
        float flutterPhase = 0.0f;
        float lpfZ1 = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TapeDelay)
};

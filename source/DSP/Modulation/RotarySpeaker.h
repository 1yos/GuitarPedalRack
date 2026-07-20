#pragma once
#include "../AudioModule.h"
#include <atomic>
#include <vector>
using namespace juce;

class RotarySpeaker : public AudioModule
{
public:
    RotarySpeaker();
    ~RotarySpeaker() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "rotarySpeaker"; }
    void setSpeed(float speed);
    void setSeparation(float separation);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> speedParam{0.5f}, separationParam{0.6f};
    struct ChannelState { 
        std::vector<float> delayBuf; 
        int writePos = 0; 
        float hornPhase = 0.0f;
        float bassPhase = 0.0f;
    };
    std::vector<ChannelState> channelState;
    static constexpr int maxDelay = 4410;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotarySpeaker)
};

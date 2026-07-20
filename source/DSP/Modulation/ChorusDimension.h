#pragma once
#include "../AudioModule.h"
#include <atomic>
#include <vector>
using namespace juce;

class ChorusDimension : public AudioModule
{
public:
    ChorusDimension();
    ~ChorusDimension() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "chorusDimension"; }
    void setIntensity(float intensity);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> intensityParam{0.6f};
    struct ChannelState { std::vector<float> delayBuf; int writePos = 0; float lfo1 = 0, lfo2 = 0; };
    std::vector<ChannelState> channelState;
    static constexpr int maxDelay = 2205;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusDimension)
};

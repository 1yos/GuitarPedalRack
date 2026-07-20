#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class Phaser90 : public AudioModule
{
public:
    Phaser90();
    ~Phaser90() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "phaser90"; }
    void setRate(float rate);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> rateParam{0.5f};
    struct APF { float z1 = 0.0f; };
    struct ChannelState { APF apf[4]; float lfoPhase = 0.0f; };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Phaser90)
};

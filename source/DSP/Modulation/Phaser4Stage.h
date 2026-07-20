#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class Phaser4Stage : public AudioModule
{
public:
    Phaser4Stage();
    ~Phaser4Stage() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "phaser4Stage"; }
    void setRate(float rate);
    void setDepth(float depth);
    void setFeedback(float feedback);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> rateParam{0.5f}, depthParam{0.6f}, feedbackParam{0.4f};
    struct APF { float z1 = 0.0f; };
    struct ChannelState { APF apf[4]; float lfoPhase = 0.0f; };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Phaser4Stage)
};

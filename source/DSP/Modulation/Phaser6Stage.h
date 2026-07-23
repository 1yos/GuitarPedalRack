#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class Phaser6Stage : public AudioModule
{
public:
    Phaser6Stage();
    ~Phaser6Stage() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "phaser6Stage"; }
    void setRate(float rate);
    void setDepth(float depth);
    void setFeedback(float feedback);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> rateParam{0.4f}, depthParam{0.7f}, feedbackParam{0.5f};
    struct APF { float z1 = 0.0f; };
    struct ChannelState { APF apf[6]; float lfoPhase = 0.0f; };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Phaser6Stage)
};

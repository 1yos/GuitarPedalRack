#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class TremoloOptical : public AudioModule
{
public:
    TremoloOptical();
    ~TremoloOptical() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "tremoloOptical"; }
    void setRate(float rate);
    void setDepth(float depth);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> rateParam{0.4f}, depthParam{0.7f};
    struct ChannelState { float lfoPhase = 0.0f; };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TremoloOptical)
};

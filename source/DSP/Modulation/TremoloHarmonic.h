#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class TremoloHarmonic : public AudioModule
{
public:
    TremoloHarmonic();
    ~TremoloHarmonic() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "tremoloHarmonic"; }
    void setRate(float rate);
    void setDepth(float depth);
    void setHarmonic(float harmonic);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> rateParam{0.5f}, depthParam{0.6f}, harmonicParam{0.3f};
    struct ChannelState { float lfoPhase = 0.0f; };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TremoloHarmonic)
};

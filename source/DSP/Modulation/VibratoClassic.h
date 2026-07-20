#pragma once
#include "../AudioModule.h"
#include <atomic>
#include <vector>
using namespace juce;

class VibratoClassic : public AudioModule
{
public:
    VibratoClassic();
    ~VibratoClassic() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "vibratoClassic"; }
    void setRate(float rate);
    void setDepth(float depth);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> rateParam{0.4f}, depthParam{0.5f};
    struct ChannelState { std::vector<float> delayBuf; int writePos = 0; float lfoPhase = 0.0f; };
    std::vector<ChannelState> channelState;
    static constexpr int maxDelay = 4410;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VibratoClassic)
};

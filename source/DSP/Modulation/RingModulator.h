#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class RingModulator : public AudioModule
{
public:
    RingModulator();
    ~RingModulator() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "ringModulator"; }
    void setFrequency(float freq);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> freqParam{0.3f}, mixParam{0.5f};
    struct ChannelState { float phase = 0.0f; };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RingModulator)
};

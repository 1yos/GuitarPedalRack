#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class LowPassFilter : public AudioModule
{
public:
    LowPassFilter();
    ~LowPassFilter() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "lowPassFilter"; }
    void setFrequency(float frequency);
    void setResonance(float resonance);
    void setSlope(float slope);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> frequencyParam{0.6f}, resonanceParam{0.3f}, slopeParam{0.5f};
    struct ChannelState {
        float z1 = 0.0f;
        float z2 = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LowPassFilter)
};

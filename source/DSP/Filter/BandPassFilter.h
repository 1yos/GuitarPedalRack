#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class BandPassFilter : public AudioModule
{
public:
    BandPassFilter();
    ~BandPassFilter() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "bandPassFilter"; }
    void setFrequency(float frequency);
    void setBandwidth(float bandwidth);
    void setGain(float gain);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> frequencyParam{0.5f}, bandwidthParam{0.5f}, gainParam{0.5f};
    struct ChannelState {
        float z1 = 0.0f;
        float z2 = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BandPassFilter)
};

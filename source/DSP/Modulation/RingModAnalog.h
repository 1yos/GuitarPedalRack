#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class RingModAnalog : public AudioModule
{
public:
    RingModAnalog();
    ~RingModAnalog() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "ringModAnalog"; }
    void setFrequency(float freq);
    void setMix(float mix);
    void setWave(float wave);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> freqParam{0.4f}, mixParam{0.6f}, waveParam{0.3f};
    struct ChannelState { float phase = 0.0f; };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RingModAnalog)
};

#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class AutoWah : public AudioModule
{
public:
    AutoWah();
    ~AutoWah() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "autoWah"; }
    void setSensitivity(float sensitivity);
    void setFrequency(float frequency);
    void setResonance(float resonance);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> sensitivityParam{0.6f}, frequencyParam{0.5f}, resonanceParam{0.7f}, mixParam{1.0f};
    struct ChannelState {
        float envelope = 0.0f;
        float z1 = 0.0f;
        float z2 = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutoWah)
};

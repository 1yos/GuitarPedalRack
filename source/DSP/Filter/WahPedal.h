#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class WahPedal : public AudioModule
{
public:
    WahPedal();
    ~WahPedal() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "wahPedal"; }
    void setPosition(float position);
    void setResonance(float resonance);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> positionParam{0.5f}, resonanceParam{0.7f};
    struct ChannelState {
        float z1 = 0.0f;
        float z2 = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WahPedal)
};

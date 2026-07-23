#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class EnvelopeFilter : public AudioModule
{
public:
    EnvelopeFilter();
    ~EnvelopeFilter() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "envelopeFilter"; }
    void setSensitivity(float sensitivity);
    void setAttack(float attack);
    void setDecay(float decay);
    void setResonance(float resonance);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> sensitivityParam{0.7f}, attackParam{0.3f}, decayParam{0.5f}, resonanceParam{0.6f};
    struct ChannelState {
        float envelope = 0.0f;
        float z1 = 0.0f;
        float z2 = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopeFilter)
};

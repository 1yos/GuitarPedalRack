#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class StateVariableFilter : public AudioModule
{
public:
    StateVariableFilter();
    ~StateVariableFilter() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "stateVariableFilter"; }
    void setFrequency(float frequency);
    void setResonance(float resonance);
    void setMode(float mode);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> frequencyParam{0.5f}, resonanceParam{0.5f}, modeParam{0.0f};
    struct ChannelState {
        float low = 0.0f;
        float band = 0.0f;
        float high = 0.0f;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StateVariableFilter)
};

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
    // Correct DF1 state: separate x (input) and y (output) history
    struct ChannelState {
        float x1 = 0.0f; // x[n-1]
        float x2 = 0.0f; // x[n-2]
        float y1 = 0.0f; // y[n-1]
        float y2 = 0.0f; // y[n-2]
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WahPedal)
};

#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class TalkBox : public AudioModule
{
public:
    TalkBox();
    ~TalkBox() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "talkBox"; }
    void setFormant(float formant);
    void setMix(float mix);
    void setQuality(float quality);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> formantParam{0.5f}, mixParam{0.8f}, qualityParam{0.6f};
    static constexpr int numFormants = 5;
    struct FormantFilter {
        float z1 = 0.0f;
        float z2 = 0.0f;
    };
    struct ChannelState {
        FormantFilter formants[numFormants];
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TalkBox)
};

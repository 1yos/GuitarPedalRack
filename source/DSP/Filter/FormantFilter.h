#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class FormantFilter : public AudioModule
{
public:
    FormantFilter();
    ~FormantFilter() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "formantFilter"; }
    void setVowel(float vowel);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> vowelParam{0.3f}, mixParam{1.0f};
    static constexpr int numFormants = 4;
    struct Filter {
        float z1 = 0.0f;
        float z2 = 0.0f;
    };
    struct ChannelState {
        Filter formants[numFormants];
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FormantFilter)
};

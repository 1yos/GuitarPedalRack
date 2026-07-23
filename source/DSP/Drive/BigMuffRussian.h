#pragma once
#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>
using namespace juce;

class BigMuffRussian : public AudioModule
{
public:
    BigMuffRussian();
    ~BigMuffRussian() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "bigMuffRussian"; }
    void setSustain(float sustain);
    void setTone(float tone);
    void setVolume(float volume);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> sustainParam{0.5f};
    std::atomic<float> toneParam{0.5f};
    std::atomic<float> volumeParam{0.7f};
    struct ChannelState { float hp_z1 = 0.0f; float s1_z1 = 0.0f; float s2_z1 = 0.0f; float bass_z1 = 0.0f; float treb_z1 = 0.0f; float hpOut_z1 = 0.0f; };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BigMuffRussian)
};

#pragma once
#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>
using namespace juce;

class ProCoRAT2 : public AudioModule
{
public:
    ProCoRAT2();
    ~ProCoRAT2() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "proCoRAT2"; }
    void setDistortion(float dist);
    void setFilter(float filter);
    void setVolume(float volume);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> distortionParam{0.5f};
    std::atomic<float> filterParam{0.5f};
    std::atomic<float> volumeParam{0.7f};
    struct ChannelState { float hpIn_z1 = 0.0f; float preLp_z1 = 0.0f; float tone_z1 = 0.0f; float hpOut_z1 = 0.0f; };
    std::vector<ChannelState> channelState;
    float hpCoeff = 0.995f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProCoRAT2)
};

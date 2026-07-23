#pragma once
#include "../AudioModule.h"
#include <atomic>
#include "../../JuceHeader.h"
using namespace juce;

class HallReverb : public AudioModule
{
public:
    HallReverb();
    ~HallReverb() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "hallReverb"; }
    void setSize(float size);
    void setDecay(float decay);
    void setDamping(float damping);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> sizeParam{0.85f}, decayParam{0.7f}, dampingParam{0.5f}, mixParam{0.4f};
    juce::Reverb reverbProcessor;
    juce::Reverb::Parameters reverbParams;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HallReverb)
};

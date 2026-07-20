#pragma once
#include "../AudioModule.h"
#include <atomic>
#include "../../JuceHeader.h"
using namespace juce;

class PlateReverb : public AudioModule
{
public:
    PlateReverb();
    ~PlateReverb() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "plateReverb"; }
    void setSize(float size);
    void setDamping(float damping);
    void setPreDelay(float preDelay);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> sizeParam{0.7f}, dampingParam{0.4f}, preDelayParam{0.1f}, mixParam{0.35f};
    juce::Reverb reverbProcessor;
    juce::Reverb::Parameters reverbParams;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlateReverb)
};

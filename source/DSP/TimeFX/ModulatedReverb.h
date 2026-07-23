#pragma once
#include "../AudioModule.h"
#include <atomic>
#include "../../JuceHeader.h"
using namespace juce;

class ModulatedReverb : public AudioModule
{
public:
    ModulatedReverb();
    ~ModulatedReverb() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "modulatedReverb"; }
    void setSize(float size);
    void setModulation(float modulation);
    void setRate(float rate);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> sizeParam{0.65f}, modulationParam{0.5f}, rateParam{0.3f}, mixParam{0.4f};
    juce::Reverb reverbProcessor;
    juce::Reverb::Parameters reverbParams;
    float lfoPhase = 0.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulatedReverb)
};

#pragma once
#include "../AudioModule.h"
#include <atomic>
#include "../../JuceHeader.h"
using namespace juce;

class ChamberReverb : public AudioModule
{
public:
    ChamberReverb();
    ~ChamberReverb() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "chamberReverb"; }
    void setSize(float size);
    void setDecay(float decay);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> sizeParam{0.6f}, decayParam{0.5f}, mixParam{0.35f};
    juce::Reverb reverbProcessor;
    juce::Reverb::Parameters reverbParams;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChamberReverb)
};

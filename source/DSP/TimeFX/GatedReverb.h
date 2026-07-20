#pragma once
#include "../AudioModule.h"
#include <atomic>
#include "../../JuceHeader.h"
using namespace juce;

class GatedReverb : public AudioModule
{
public:
    GatedReverb();
    ~GatedReverb() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "gatedReverb"; }
    void setSize(float size);
    void setGateTime(float gateTime);
    void setThreshold(float threshold);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> sizeParam{0.5f}, gateTimeParam{0.4f}, thresholdParam{0.3f}, mixParam{0.4f};
    juce::Reverb reverbProcessor;
    juce::Reverb::Parameters reverbParams;
    float envelopeLevel = 0.0f;
    int gateCounter = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GatedReverb)
};

#pragma once
#include "../AudioModule.h"
#include <atomic>
#include "../../JuceHeader.h"
using namespace juce;

class RoomReverb : public AudioModule
{
public:
    RoomReverb();
    ~RoomReverb() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "roomReverb"; }
    void setSize(float size);
    void setDamping(float damping);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> sizeParam{0.4f}, dampingParam{0.6f}, mixParam{0.3f};
    juce::Reverb reverbProcessor;
    juce::Reverb::Parameters reverbParams;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RoomReverb)
};

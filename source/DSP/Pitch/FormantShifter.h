#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class FormantShifter : public AudioModule
{
public:
    FormantShifter();
    ~FormantShifter() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "formantShifter"; }
    
    void setFormantShift(float shift);
    void setPitchShift(float pitch);
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    std::atomic<float> formantShiftParam{0.5f};
    std::atomic<float> pitchShiftParam{0.5f};
    
    static constexpr int delayBufferSize = 8192;
    
    struct ChannelState
    {
        AudioBuffer<float> delayBuffer;
        int writePos = 0;
        float readPos = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FormantShifter)
};

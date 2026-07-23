#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class PitchShifter : public AudioModule
{
public:
    PitchShifter();
    ~PitchShifter() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "pitchShifter"; }
    
    void setPitch(float pitch);      // -12 to +12 semitones
    void setFormant(float formant);  // Preserve formants
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    std::atomic<float> pitchParam{0.5f};    // 0 semitones
    std::atomic<float> formantParam{0.5f};  // Normal formants
    
    static constexpr int delayBufferSize = 8192;
    
    struct ChannelState
    {
        AudioBuffer<float> delayBuffer;
        int writePos = 0;
        float readPos = 0.0f;
        float phase = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchShifter)
};

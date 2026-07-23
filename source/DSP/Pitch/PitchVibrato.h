#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class PitchVibrato : public AudioModule
{
public:
    PitchVibrato();
    ~PitchVibrato() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "pitchVibrato"; }
    
    void setRate(float rate);
    void setDepth(float depth);
    void setWaveform(float waveform);
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    std::atomic<float> rateParam{0.3f};
    std::atomic<float> depthParam{0.3f};
    std::atomic<float> waveformParam{0.0f};
    
    static constexpr int delayBufferSize = 4096;
    
    struct ChannelState
    {
        AudioBuffer<float> delayBuffer;
        int writePos = 0;
        float lfoPhase = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchVibrato)
};

#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class DeEsser : public AudioModule
{
public:
    DeEsser();
    ~DeEsser() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "deEsser"; }
    
    void setThreshold(float threshold);
    void setFrequency(float frequency);
    void setRatio(float ratio);
    void setAttack(float attack);
    void setRelease(float release);
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    std::atomic<float> thresholdParam{0.7f};
    std::atomic<float> frequencyParam{0.6f};  // 5-10kHz range
    std::atomic<float> ratioParam{0.5f};
    std::atomic<float> attackParam{0.05f};    // Fast attack for sibilance
    std::atomic<float> releaseParam{0.3f};
    
    struct ChannelState
    {
        // Bandpass filter state for sibilance detection
        float bpZ1 = 0.0f;
        float bpZ2 = 0.0f;
        
        // Envelope follower for detected sibilance
        float envelope = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeEsser)
};

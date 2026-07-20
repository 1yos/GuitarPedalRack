#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class PitchCorrection : public AudioModule
{
public:
    PitchCorrection();
    ~PitchCorrection() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "pitchCorrection"; }
    
    void setKey(float key);
    void setRetune(float retune);
    void setAmount(float amount);
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    std::atomic<float> keyParam{0.0f};
    std::atomic<float> retuneParam{0.5f};
    std::atomic<float> amountParam{0.7f};
    
    static constexpr int delayBufferSize = 4096;
    
    struct ChannelState
    {
        AudioBuffer<float> delayBuffer;
        int writePos = 0;
        float readPos = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchCorrection)
};

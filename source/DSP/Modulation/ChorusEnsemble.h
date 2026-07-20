#pragma once
#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>
#include <vector>
using namespace juce;

class ChorusEnsemble : public AudioModule
{
public:
    ChorusEnsemble();
    ~ChorusEnsemble() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "chorusEnsemble"; }
    void setRate(float rate);
    void setDepth(float depth);
    void setVoices(float voices);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> rateParam{0.4f};
    std::atomic<float> depthParam{0.6f};
    std::atomic<float> voicesParam{0.7f};
    std::atomic<float> mixParam{0.5f};
    
    struct ChannelState
    {
        std::vector<float> delayBuffer;
        int writePos = 0;
        float lfo1Phase = 0.0f;
        float lfo2Phase = 0.333f;
        float lfo3Phase = 0.667f;
    };
    std::vector<ChannelState> channelState;
    static constexpr int maxDelay = 4410;  // 100ms at 44.1kHz
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChorusEnsemble)
};

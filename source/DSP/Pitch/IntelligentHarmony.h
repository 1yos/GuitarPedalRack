#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class IntelligentHarmony : public AudioModule
{
public:
    IntelligentHarmony();
    ~IntelligentHarmony() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "intelligentHarmony"; }
    
    void setKey(float key);
    void setScale(float scale);
    void setVoices(float voices);
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    std::atomic<float> keyParam{0.0f};
    std::atomic<float> scaleParam{0.0f};
    std::atomic<float> voicesParam{0.5f};
    
    static constexpr int delayBufferSize = 8192;
    static constexpr int maxVoices = 3;
    
    struct Voice
    {
        float readPos = 0.0f;
        int interval = 0;
    };
    
    struct ChannelState
    {
        AudioBuffer<float> delayBuffer;
        int writePos = 0;
        Voice voices[maxVoices];
    };
    
    std::vector<ChannelState> channelState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IntelligentHarmony)
};

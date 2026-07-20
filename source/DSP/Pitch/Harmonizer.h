#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class Harmonizer : public AudioModule
{
public:
    Harmonizer();
    ~Harmonizer() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "harmonizer"; }
    
    void setInterval(float interval);  // Musical interval
    void setKey(float key);            // Key for intelligent harmony
    void setMix(float mix);            // Dry/wet mix
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    std::atomic<float> intervalParam{0.33f};  // Perfect 4th
    std::atomic<float> keyParam{0.5f};        // C major
    std::atomic<float> mixParam{0.5f};        // 50% mix
    
    static constexpr int delayBufferSize = 8192;
    
    struct ChannelState
    {
        AudioBuffer<float> delayBuffer;
        int writePos = 0;
        float readPos = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Harmonizer)
};

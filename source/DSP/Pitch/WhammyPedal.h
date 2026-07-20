#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class WhammyPedal : public AudioModule
{
public:
    WhammyPedal();
    ~WhammyPedal() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "whammyPedal"; }
    
    void setRange(float range);      // Pitch bend range
    void setExpression(float expr);  // Pedal position
    void setMode(float mode);        // Up/down/both
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    std::atomic<float> rangeParam{0.5f};      // 1 octave
    std::atomic<float> expressionParam{0.0f}; // Heel down
    std::atomic<float> modeParam{0.5f};       // Up mode
    
    static constexpr int delayBufferSize = 8192;
    
    struct ChannelState
    {
        AudioBuffer<float> delayBuffer;
        int writePos = 0;
        float readPos = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WhammyPedal)
};

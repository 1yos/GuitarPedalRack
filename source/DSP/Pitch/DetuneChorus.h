#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class DetuneChorus : public AudioModule
{
public:
    DetuneChorus();
    ~DetuneChorus() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "detuneChorus"; }
    
    void setDetune(float detune);  // Detune amount
    void setVoices(float voices);  // Number of voices
    void setSpread(float spread);  // Stereo spread
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    std::atomic<float> detuneParam{0.3f};
    std::atomic<float> voicesParam{0.5f};
    std::atomic<float> spreadParam{0.5f};
    
    static constexpr int numVoices = 4;
    static constexpr int delayBufferSize = 4096;
    
    struct Voice
    {
        float readPos = 0.0f;
        float detuneCents = 0.0f;
    };
    
    struct ChannelState
    {
        AudioBuffer<float> delayBuffer;
        int writePos = 0;
        Voice voices[numVoices];
    };
    
    std::vector<ChannelState> channelState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DetuneChorus)
};

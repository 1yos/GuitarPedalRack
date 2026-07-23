#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class ShelvingEQ : public AudioModule
{
public:
    ShelvingEQ();
    ~ShelvingEQ() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "shelvingEQ"; }
    
    void setLowShelfFreq(float freq);   // 20Hz - 500Hz
    void setLowShelfGain(float gain);   // -12dB to +12dB
    void setHighShelfFreq(float freq);  // 2kHz - 16kHz
    void setHighShelfGain(float gain);  // -12dB to +12dB
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    std::atomic<float> lowShelfFreqParam{0.3f};  // ~100Hz
    std::atomic<float> lowShelfGainParam{0.5f};  // 0dB
    std::atomic<float> highShelfFreqParam{0.5f}; // ~6kHz
    std::atomic<float> highShelfGainParam{0.5f}; // 0dB
    
    struct ShelfFilter
    {
        float z1 = 0.0f;
        float z2 = 0.0f;
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;
    };
    
    struct ChannelState
    {
        ShelfFilter lowShelf;
        ShelfFilter highShelf;
    };
    
    std::vector<ChannelState> channelState;
    
    void updateCoefficients();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShelvingEQ)
};

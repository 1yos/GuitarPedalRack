#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class HighPassEQ : public AudioModule
{
public:
    HighPassEQ();
    ~HighPassEQ() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "highPassEQ"; }
    
    void setFrequency(float freq);  // 20Hz - 2kHz
    void setResonance(float res);   // Q factor
    void setSlope(float slope);     // 12dB/oct or 24dB/oct
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    std::atomic<float> frequencyParam{0.1f};  // ~100Hz
    std::atomic<float> resonanceParam{0.3f};  // Q = 0.7
    std::atomic<float> slopeParam{0.0f};      // 12dB/oct
    
    struct HPFilter
    {
        float z1 = 0.0f;
        float z2 = 0.0f;
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;
    };
    
    struct ChannelState
    {
        HPFilter filter1; // First stage (always active)
        HPFilter filter2; // Second stage (24dB mode)
    };
    
    std::vector<ChannelState> channelState;
    
    void updateCoefficients();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HighPassEQ)
};

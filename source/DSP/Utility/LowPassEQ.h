#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class LowPassEQ : public AudioModule
{
public:
    LowPassEQ();
    ~LowPassEQ() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "lowPassEQ"; }
    
    void setFrequency(float freq);  // 200Hz - 20kHz
    void setResonance(float res);   // Q factor
    void setSlope(float slope);     // 12dB/oct or 24dB/oct
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    std::atomic<float> frequencyParam{0.5f};  // ~5kHz
    std::atomic<float> resonanceParam{0.3f};  // Q = 0.7
    std::atomic<float> slopeParam{0.0f};      // 12dB/oct
    
    struct LPFilter
    {
        float z1 = 0.0f;
        float z2 = 0.0f;
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;
    };
    
    struct ChannelState
    {
        LPFilter filter1; // First stage (always active)
        LPFilter filter2; // Second stage (24dB mode)
    };
    
    std::vector<ChannelState> channelState;
    
    void updateCoefficients();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LowPassEQ)
};

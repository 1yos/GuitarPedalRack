#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class ToneStackFender : public AudioModule
{
public:
    ToneStackFender();
    ~ToneStackFender() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "toneStackFender"; }
    
    void setBass(float bass);
    void setMid(float mid);
    void setTreble(float treble);
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    std::atomic<float> bassParam{0.5f};
    std::atomic<float> midParam{0.5f};
    std::atomic<float> trebleParam{0.5f};
    
    struct ChannelState
    {
        float z1 = 0.0f;
        float z2 = 0.0f;
        float z3 = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    // Fender tone stack coefficients
    float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f, b3 = 0.0f;
    float a0 = 1.0f, a1 = 0.0f, a2 = 0.0f, a3 = 0.0f;
    
    void updateCoefficients();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToneStackFender)
};

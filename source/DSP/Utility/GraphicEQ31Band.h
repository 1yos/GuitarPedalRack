#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class GraphicEQ31Band : public AudioModule
{
public:
    GraphicEQ31Band();
    ~GraphicEQ31Band() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "graphicEQ31Band"; }
    
    void setBandGain(int bandIndex, float gain); // bandIndex 0-30, gain 0.0-1.0
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    static constexpr int numBands = 31;
    
    // ISO standard 1/3 octave frequencies
    static constexpr float frequencies[numBands] = {
        20.0f, 25.0f, 31.5f, 40.0f, 50.0f, 63.0f, 80.0f, 100.0f, 125.0f, 160.0f,
        200.0f, 250.0f, 315.0f, 400.0f, 500.0f, 630.0f, 800.0f, 1000.0f, 1250.0f, 1600.0f,
        2000.0f, 2500.0f, 3150.0f, 4000.0f, 5000.0f, 6300.0f, 8000.0f, 10000.0f, 12500.0f, 16000.0f,
        20000.0f
    };
    
    std::atomic<float> bandGains[numBands];
    
    struct BandFilter
    {
        float z1 = 0.0f;
        float z2 = 0.0f;
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;
    };
    
    struct ChannelState
    {
        BandFilter filters[numBands];
    };
    
    std::vector<ChannelState> channelState;
    
    void updateFilterCoefficients();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphicEQ31Band)
};

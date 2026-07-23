#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class GraphicEQ10Band : public AudioModule
{
public:
    GraphicEQ10Band();
    ~GraphicEQ10Band() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "graphicEQ10Band"; }
    
    // Set individual band gains (-12dB to +12dB)
    void setBand31Hz(float gain);
    void setBand62Hz(float gain);
    void setBand125Hz(float gain);
    void setBand250Hz(float gain);
    void setBand500Hz(float gain);
    void setBand1kHz(float gain);
    void setBand2kHz(float gain);
    void setBand4kHz(float gain);
    void setBand8kHz(float gain);
    void setBand16kHz(float gain);
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    static constexpr int numBands = 10;
    
    // Center frequencies
    static constexpr float frequencies[numBands] = {
        31.5f, 63.0f, 125.0f, 250.0f, 500.0f,
        1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f
    };
    
    std::atomic<float> bandGains[numBands] = {
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f
    };
    
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
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GraphicEQ10Band)
};

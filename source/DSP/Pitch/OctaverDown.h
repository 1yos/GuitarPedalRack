#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class OctaverDown : public AudioModule
{
public:
    OctaverDown();
    ~OctaverDown() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "octaverDown"; }
    
    void setOctaveLevel(float level);
    void setDirect(float direct);
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    std::atomic<float> octaveLevelParam{0.7f};
    std::atomic<float> directParam{0.5f};
    
    struct ChannelState
    {
        float lastSample = 0.0f;
        bool flip = false;
    };
    
    std::vector<ChannelState> channelState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OctaverDown)
};

#pragma once
#include "../AudioModule.h"
#include <atomic>
using namespace juce;

class OctaverUp : public AudioModule
{
public:
    OctaverUp();
    ~OctaverUp() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "octaverUp"; }
    
    void setOctaveLevel(float level);
    void setDirect(float direct);
    
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
    
private:
    std::atomic<float> octaveLevelParam{0.7f};
    std::atomic<float> directParam{0.5f};
    
    static constexpr int delayBufferSize = 4096;
    
    struct ChannelState
    {
        AudioBuffer<float> delayBuffer;
        int writePos = 0;
        float readPos = 0.0f;
    };
    
    std::vector<ChannelState> channelState;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OctaverUp)
};

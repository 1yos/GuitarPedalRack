#pragma once
#include "../AudioModule.h"
#include <atomic>
#include <vector>
using namespace juce;

class ReverseReverb : public AudioModule
{
public:
    ReverseReverb();
    ~ReverseReverb() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "reverseReverb"; }
    void setSize(float size);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> sizeParam{0.6f}, mixParam{0.4f};
    static constexpr int reverseBufSize = 96000;
    struct ChannelState {
        std::vector<float> reverseBuf;
        int writePos = 0;
        int blockCounter = 0;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverseReverb)
};

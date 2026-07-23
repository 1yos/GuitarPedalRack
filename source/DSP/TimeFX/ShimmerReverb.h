#pragma once
#include "../AudioModule.h"
#include <atomic>
#include <vector>
#include "../../JuceHeader.h"
using namespace juce;

class ShimmerReverb : public AudioModule
{
public:
    ShimmerReverb();
    ~ShimmerReverb() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "shimmerReverb"; }
    void setSize(float size);
    void setShimmer(float shimmer);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> sizeParam{0.75f}, shimmerParam{0.5f}, mixParam{0.4f};
    juce::Reverb reverbProcessor;
    juce::Reverb::Parameters reverbParams;
    static constexpr int pitchBufSize = 4096;
    struct ChannelState {
        std::vector<float> pitchBuf;
        int writePos = 0;
    };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShimmerReverb)
};

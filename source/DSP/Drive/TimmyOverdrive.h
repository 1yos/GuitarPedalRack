#pragma once
#include "../AudioModule.h"
#include "../SIMDProcessor.h"
#include <atomic>
using namespace juce;

class TimmyOverdrive : public AudioModule
{
public:
    TimmyOverdrive();
    ~TimmyOverdrive() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "timmyOverdrive"; }
    void setGain(float gain);
    void setBass(float bass);
    void setTreble(float treble);
    void setVolume(float volume);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> gainParam{0.4f};
    std::atomic<float> bassParam{0.5f};
    std::atomic<float> trebleParam{0.5f};
    std::atomic<float> volumeParam{0.8f};
    struct ChannelState { float hp_z1 = 0.0f; float bass_z1 = 0.0f; float treb_z1 = 0.0f; float hpOut_z1 = 0.0f; };
    std::vector<ChannelState> channelState;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimmyOverdrive)
};

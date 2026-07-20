#pragma once
#include "../AudioModule.h"
#include <atomic>
#include <vector>
using namespace juce;

class SpringReverb : public AudioModule
{
public:
    SpringReverb();
    ~SpringReverb() override;
    void prepare(double sampleRate, int samplesPerBlock) override;
    void reset() override;
    String getModuleType() const override { return "springReverb"; }
    void setDecay(float decay);
    void setTone(float tone);
    void setMix(float mix);
protected:
    void processInternal(AudioBuffer<float>& buffer) override;
private:
    std::atomic<float> decayParam{0.6f}, toneParam{0.5f}, mixParam{0.4f};
    static constexpr int numSprings = 3;
    static constexpr int maxDelay = 22050;
    struct SpringLine {
        std::vector<float> delayBuf;
        int writePos = 0;
        float lpfZ1 = 0.0f;
        float apfZ1[2] = {0.0f, 0.0f};
    };
    std::vector<SpringLine> springs;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpringReverb)
};

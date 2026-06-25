#pragma once

#include "../AudioModule.h"
#include "../../JuceHeader.h"

using namespace juce;

/**
 * Multi-Mode Delay Effect
 * 
 * Features:
 * - Three modes: Digital, Analog, Tape
 * - Tempo sync option
 * - Feedback control
 * - High-pass and low-pass filtering
 * - Ping-pong stereo option
 */
class Delay : public AudioModule
{
public:
    enum Mode
    {
        Digital,    // Clean digital delay
        Analog,     // BBD-style with modulation
        Tape        // Tape delay with wow/flutter
    };
    
    Delay();
    
    String getModuleType() const override { return "Delay"; }
    String getDisplayName() const override { return "Delay"; }

    void prepare(double sampleRate, int maxBlockSize) override;
    void reset() override;

    // Parameters
    void setTime(float time);           // 0.0 to 1.0 (10ms to 2000ms)
    void setFeedback(float feedback);   // 0.0 to 1.0 (0% to 95%)
    void setMix(float mix);             // 0.0 to 1.0 (dry/wet)
    void setDamping(float damping);     // 0.0 to 1.0 (low-pass filter)
    void setMode(Mode mode);            // Digital, Analog, or Tape
    void setPingPong(bool enabled);     // Stereo ping-pong mode

    float getTime() const { return timeParam; }
    float getFeedback() const { return feedbackParam; }
    float getMix() const { return mixParam; }
    float getDamping() const { return dampingParam; }
    Mode getMode() const { return currentMode; }
    bool getPingPong() const { return pingPongEnabled; }

protected:
    void processInternal(AudioBuffer<float>& buffer) override;

private:
    // Parameters
    float timeParam = 0.3f;
    float feedbackParam = 0.5f;
    float mixParam = 0.3f;
    float dampingParam = 0.7f;
    Mode currentMode = Digital;
    bool pingPongEnabled = false;
    
    // DSP state
    static constexpr int MAX_DELAY_SAMPLES = 96000; // 2 seconds at 48kHz
    
    float delayBuffer[2][MAX_DELAY_SAMPLES];
    int delayWritePos[2];
    
    float delayTimeMs = 500.0f;
    float feedbackAmount = 0.5f;
    float wetLevel = 0.3f;
    float dryLevel = 0.7f;
    
    // Damping filter (low-pass)
    float dampingZ1[2] = {0.0f, 0.0f};
    float dampingCoeff = 0.7f;
    
    // Analog mode: modulation
    float modulationPhase = 0.0f;
    float modulationRate = 0.5f; // Hz
    
    // Tape mode: wow & flutter
    float wowPhase = 0.0f;
    float flutterPhase = 0.0f;
    
    double currentSampleRate = 44100.0;

    void updateParameters();
    float readDelay(int channel, float delaySamples);
    float applyModeCharacter(float sample, int channel);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Delay)
};

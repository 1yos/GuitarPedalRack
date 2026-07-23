#include "Expander.h"

Expander::Expander() : AudioModule("Expander") {}
Expander::~Expander() {}

void Expander::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
}

void Expander::reset()
{
    for (auto& ch : channelState) { ch.envelope = 0.0f; }
}

void Expander::setThreshold(float threshold) { thresholdParam.store(threshold); }
void Expander::setRatio(float ratio) { ratioParam.store(ratio); }
void Expander::setAttack(float attack) { attackParam.store(attack); }
void Expander::setRelease(float release) { releaseParam.store(release); }

void Expander::processInternal(AudioBuffer<float>& buffer)
{
    const float threshold = thresholdParam.load();
    const float ratio = ratioParam.load() * 9.0f + 1.0f;
    const float attackCoeff = 1.0f - std::exp(-1.0f / (attackParam.load() * 0.1f * currentSampleRate + 1.0f));
    const float releaseCoeff = 1.0f - std::exp(-1.0f / (releaseParam.load() * 0.5f * currentSampleRate + 1.0f));
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float input = data[i];
            float inputLevel = std::abs(input);
            
            if (inputLevel > state.envelope)
                state.envelope += (inputLevel - state.envelope) * attackCoeff;
            else
                state.envelope += (inputLevel - state.envelope) * releaseCoeff;
            
            float gain = 1.0f;
            if (state.envelope < threshold)
            {
                float diff = threshold - state.envelope;
                gain = std::pow(state.envelope / threshold, 1.0f / ratio - 1.0f);
                gain = juce::jlimit(0.0f, 1.0f, gain);
            }
            
            data[i] = input * gain;
        }
    }
}

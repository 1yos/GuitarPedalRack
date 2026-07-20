#include "Limiter.h"

Limiter::Limiter() : AudioModule("Limiter") {}
Limiter::~Limiter() {}

void Limiter::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
}

void Limiter::reset()
{
    for (auto& ch : channelState) { ch.envelope = 0.0f; }
}

void Limiter::setThreshold(float threshold) { thresholdParam.store(threshold); }
void Limiter::setRelease(float release) { releaseParam.store(release); }
void Limiter::setMakeup(float makeup) { makeupParam.store(makeup); }

void Limiter::processInternal(AudioBuffer<float>& buffer)
{
    const float threshold = thresholdParam.load();
    const float releaseCoeff = 1.0f - std::exp(-1.0f / (releaseParam.load() * 0.5f * currentSampleRate + 1.0f));
    const float makeup = std::pow(10.0f, makeupParam.load() * 2.0f / 20.0f);
    const float attackCoeff = 0.9999f;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float input = data[i] * makeup;
            float inputLevel = std::abs(input);
            
            if (inputLevel > state.envelope)
                state.envelope = inputLevel;
            else
                state.envelope += (inputLevel - state.envelope) * releaseCoeff;
            
            float gain = 1.0f;
            if (state.envelope > threshold)
                gain = threshold / state.envelope;
            
            data[i] = input * gain;
        }
    }
}

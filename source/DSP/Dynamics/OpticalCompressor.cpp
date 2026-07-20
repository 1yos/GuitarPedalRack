#include "OpticalCompressor.h"

OpticalCompressor::OpticalCompressor() : AudioModule("OpticalCompressor") {}
OpticalCompressor::~OpticalCompressor() {}

void OpticalCompressor::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
}

void OpticalCompressor::reset()
{
    for (auto& ch : channelState) { ch.envelope = 0.0f; }
}

void OpticalCompressor::setThreshold(float threshold) { thresholdParam.store(threshold); }
void OpticalCompressor::setRatio(float ratio) { ratioParam.store(ratio); }
void OpticalCompressor::setAttack(float attack) { attackParam.store(attack); }
void OpticalCompressor::setRelease(float release) { releaseParam.store(release); }
void OpticalCompressor::setMakeup(float makeup) { makeupParam.store(makeup); }

void OpticalCompressor::processInternal(AudioBuffer<float>& buffer)
{
    const float threshold = thresholdParam.load();
    const float ratio = ratioParam.load() * 7.0f + 1.5f;
    const float attackCoeff = 1.0f - std::exp(-1.0f / (attackParam.load() * 0.05f * currentSampleRate + 1.0f));
    const float releaseCoeff = 1.0f - std::exp(-1.0f / (releaseParam.load() * 1.0f * currentSampleRate + 1.0f));
    const float makeup = std::pow(10.0f, makeupParam.load() * 2.0f / 20.0f);
    
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
            if (state.envelope > threshold)
            {
                float excess = state.envelope - threshold;
                float compressedExcess = excess / ratio;
                float targetLevel = threshold + compressedExcess;
                gain = targetLevel / state.envelope;
            }
            
            data[i] = input * gain * makeup;
        }
    }
}

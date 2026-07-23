#include "TubeCompressor.h"

TubeCompressor::TubeCompressor() : AudioModule("TubeCompressor") {}
TubeCompressor::~TubeCompressor() {}

void TubeCompressor::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
}

void TubeCompressor::reset()
{
    for (auto& ch : channelState) { ch.envelope = 0.0f; }
}

void TubeCompressor::setThreshold(float threshold) { thresholdParam.store(threshold); }
void TubeCompressor::setRatio(float ratio) { ratioParam.store(ratio); }
void TubeCompressor::setAttack(float attack) { attackParam.store(attack); }
void TubeCompressor::setRelease(float release) { releaseParam.store(release); }
void TubeCompressor::setMakeup(float makeup) { makeupParam.store(makeup); }

void TubeCompressor::processInternal(AudioBuffer<float>& buffer)
{
    const float threshold = thresholdParam.load();
    const float ratio = ratioParam.load() * 8.0f + 2.0f;
    const float attackCoeff = 1.0f - std::exp(-1.0f / (attackParam.load() * 0.3f * currentSampleRate + 1.0f));
    const float releaseCoeff = 1.0f - std::exp(-1.0f / (releaseParam.load() * 1.5f * currentSampleRate + 1.0f));
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
                float softKnee = std::tanh(gain * 2.0f) * 0.5f + 0.5f;
                gain = gain * 0.7f + softKnee * 0.3f;
            }
            
            float output = input * gain * makeup;
            output = std::tanh(output * 1.2f) * 0.9f;
            data[i] = output;
        }
    }
}

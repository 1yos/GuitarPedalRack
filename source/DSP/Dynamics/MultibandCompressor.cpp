#include "MultibandCompressor.h"

MultibandCompressor::MultibandCompressor() : AudioModule("MultibandCompressor") {}
MultibandCompressor::~MultibandCompressor() {}

void MultibandCompressor::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
}

void MultibandCompressor::reset()
{
    for (auto& ch : channelState)
    {
        for (auto& band : ch.bands)
        {
            band.lpZ1 = 0.0f; band.lpZ2 = 0.0f;
            band.hpZ1 = 0.0f; band.hpZ2 = 0.0f;
            band.envelope = 0.0f;
        }
    }
}

void MultibandCompressor::setLowThreshold(float threshold) { lowThresholdParam.store(threshold); }
void MultibandCompressor::setMidThreshold(float threshold) { midThresholdParam.store(threshold); }
void MultibandCompressor::setHighThreshold(float threshold) { highThresholdParam.store(threshold); }
void MultibandCompressor::setRatio(float ratio) { ratioParam.store(ratio); }

void MultibandCompressor::processInternal(AudioBuffer<float>& buffer)
{
    const float lowThresh = lowThresholdParam.load();
    const float midThresh = midThresholdParam.load();
    const float highThresh = highThresholdParam.load();
    const float ratio = ratioParam.load() * 18.0f + 2.0f;
    const float attackCoeff = 0.05f;
    const float releaseCoeff = 0.001f;
    const float lowCutoff = 250.0f;
    const float highCutoff = 2500.0f;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float input = data[i];
            float output = 0.0f;
            float thresholds[3] = {lowThresh, midThresh, highThresh};
            
            for (int b = 0; b < numBands; ++b)
            {
                float bandSig = input;
                if (b == 0) // Low
                {
                    float w0 = juce::MathConstants<float>::twoPi * lowCutoff / (float)currentSampleRate;
                    float alpha = std::sin(w0) / 1.414f;
                    float b0 = (1.0f - std::cos(w0)) / 2.0f;
                    float a0 = 1.0f + alpha;
                    float a1 = -2.0f * std::cos(w0);
                    bandSig = (b0/a0) * input + (b0/a0)*2.0f * state.bands[b].lpZ1 + (b0/a0) * state.bands[b].lpZ2
                            - (a1/a0) * state.bands[b].lpZ1 - ((1.0f-alpha)/a0) * state.bands[b].lpZ2;
                    state.bands[b].lpZ2 = state.bands[b].lpZ1;
                    state.bands[b].lpZ1 = input;
                }
                
                float level = std::abs(bandSig);
                if (level > state.bands[b].envelope)
                    state.bands[b].envelope += (level - state.bands[b].envelope) * attackCoeff;
                else
                    state.bands[b].envelope += (level - state.bands[b].envelope) * releaseCoeff;
                
                float gain = 1.0f;
                if (state.bands[b].envelope > thresholds[b])
                {
                    float excess = state.bands[b].envelope - thresholds[b];
                    gain = (thresholds[b] + excess / ratio) / state.bands[b].envelope;
                }
                output += bandSig * gain;
            }
            data[i] = output / numBands;
        }
    }
}

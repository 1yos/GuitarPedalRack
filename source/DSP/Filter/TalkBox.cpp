#include "TalkBox.h"

TalkBox::TalkBox() : AudioModule("TalkBox") {}
TalkBox::~TalkBox() {}

void TalkBox::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
}

void TalkBox::reset()
{
    for (auto& ch : channelState)
    {
        for (auto& formant : ch.formants)
        {
            formant.z1 = 0.0f;
            formant.z2 = 0.0f;
        }
    }
}

void TalkBox::setFormant(float formant) { formantParam.store(formant); }
void TalkBox::setMix(float mix) { mixParam.store(mix); }
void TalkBox::setQuality(float quality) { qualityParam.store(quality); }

void TalkBox::processInternal(AudioBuffer<float>& buffer)
{
    const float formant = formantParam.load();
    const float mix = mixParam.load();
    const float Q = qualityParam.load() * 8.0f + 2.0f;
    const float vowelFreqs[numFormants] = {
        400.0f + formant * 400.0f,
        800.0f + formant * 800.0f,
        1600.0f + formant * 1200.0f,
        2400.0f + formant * 1000.0f,
        3200.0f + formant * 800.0f
    };
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float dry = data[i];
            float wet = 0.0f;
            
            for (int f = 0; f < numFormants; ++f)
            {
                float freq = vowelFreqs[f];
                float w0 = juce::MathConstants<float>::twoPi * freq / (float)currentSampleRate;
                float alpha = std::sin(w0) / (2.0f * Q);
                float b0 = alpha;
                float b1 = 0.0f;
                float b2 = -alpha;
                float a0 = 1.0f + alpha;
                float a1 = -2.0f * std::cos(w0);
                float a2 = 1.0f - alpha;
                
                float out = (b0/a0) * dry + (b1/a0) * state.formants[f].z1 + (b2/a0) * state.formants[f].z2
                          - (a1/a0) * state.formants[f].z1 - (a2/a0) * state.formants[f].z2;
                state.formants[f].z2 = state.formants[f].z1;
                state.formants[f].z1 = dry;
                wet += out * (1.0f / numFormants);
            }
            
            data[i] = dry * (1.0f - mix) + wet * mix;
        }
    }
}

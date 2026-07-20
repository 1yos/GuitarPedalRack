#include "FormantFilter.h"

FormantFilter::FormantFilter() : AudioModule("FormantFilter") {}
FormantFilter::~FormantFilter() {}

void FormantFilter::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
}

void FormantFilter::reset()
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

void FormantFilter::setVowel(float vowel) { vowelParam.store(vowel); }
void FormantFilter::setMix(float mix) { mixParam.store(mix); }

void FormantFilter::processInternal(AudioBuffer<float>& buffer)
{
    const float vowel = vowelParam.load();
    const float mix = mixParam.load();
    float formantFreqs[numFormants];
    float formantGains[numFormants];
    float formantQs[numFormants];
    
    if (vowel < 0.25f)
    {
        float t = vowel / 0.25f;
        formantFreqs[0] = 730.0f + t * (140.0f);
        formantFreqs[1] = 1090.0f + t * (690.0f);
        formantFreqs[2] = 2440.0f + t * (480.0f);
        formantFreqs[3] = 3200.0f;
    }
    else if (vowel < 0.5f)
    {
        float t = (vowel - 0.25f) / 0.25f;
        formantFreqs[0] = 270.0f + t * (230.0f);
        formantFreqs[1] = 2290.0f + t * (710.0f);
        formantFreqs[2] = 3010.0f + t * (440.0f);
        formantFreqs[3] = 3500.0f;
    }
    else
    {
        float t = (vowel - 0.5f) / 0.5f;
        formantFreqs[0] = 640.0f + t * (90.0f);
        formantFreqs[1] = 1190.0f + t * (910.0f);
        formantFreqs[2] = 2390.0f + t * (60.0f);
        formantFreqs[3] = 3400.0f;
    }
    
    for (int f = 0; f < numFormants; ++f)
    {
        formantGains[f] = 1.0f / numFormants;
        formantQs[f] = 6.0f + f * 2.0f;
    }
    
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
                float freq = formantFreqs[f];
                float Q = formantQs[f];
                float w0 = juce::MathConstants<float>::twoPi * freq / (float)currentSampleRate;
                float alpha = std::sin(w0) / (2.0f * Q);
                float b0 = alpha * formantGains[f];
                float b1 = 0.0f;
                float b2 = -alpha * formantGains[f];
                float a0 = 1.0f + alpha;
                float a1 = -2.0f * std::cos(w0);
                float a2 = 1.0f - alpha;
                
                float out = (b0/a0) * dry + (b1/a0) * state.formants[f].z1 + (b2/a0) * state.formants[f].z2
                          - (a1/a0) * state.formants[f].z1 - (a2/a0) * state.formants[f].z2;
                state.formants[f].z2 = state.formants[f].z1;
                state.formants[f].z1 = dry;
                wet += out;
            }
            
            data[i] = dry * (1.0f - mix) + wet * mix;
        }
    }
}

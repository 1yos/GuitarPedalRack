#include "AutoWah.h"

AutoWah::AutoWah() : AudioModule("AutoWah") {}
AutoWah::~AutoWah() {}

void AutoWah::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
}

void AutoWah::reset()
{
    for (auto& ch : channelState) { ch.envelope = 0.0f; ch.z1 = 0.0f; ch.z2 = 0.0f; }
}

void AutoWah::setSensitivity(float sensitivity) { sensitivityParam.store(sensitivity); }
void AutoWah::setFrequency(float frequency) { frequencyParam.store(frequency); }
void AutoWah::setResonance(float resonance) { resonanceParam.store(resonance); }
void AutoWah::setMix(float mix) { mixParam.store(mix); }

void AutoWah::processInternal(AudioBuffer<float>& buffer)
{
    const float sensitivity = sensitivityParam.load();
    const float baseFreq = frequencyParam.load() * 1800.0f + 200.0f;
    const float Q = resonanceParam.load() * 8.0f + 1.0f;
    const float mix = mixParam.load();
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float inputLevel = std::abs(data[i]);
            state.envelope += (inputLevel - state.envelope) * 0.01f;
            float modFreq = baseFreq + state.envelope * sensitivity * 2000.0f;
            modFreq = juce::jlimit(200.0f, 4000.0f, modFreq);
            float w0 = juce::MathConstants<float>::twoPi * modFreq / (float)currentSampleRate;
            float alpha = std::sin(w0) / (2.0f * Q);
            float b0 = alpha;
            float b1 = 0.0f;
            float b2 = -alpha;
            float a0 = 1.0f + alpha;
            float a1 = -2.0f * std::cos(w0);
            float a2 = 1.0f - alpha;
            float dry = data[i];
            float filtered = (b0/a0) * dry + (b1/a0) * state.z1 + (b2/a0) * state.z2
                           - (a1/a0) * state.z1 - (a2/a0) * state.z2;
            state.z2 = state.z1;
            state.z1 = dry;
            data[i] = dry * (1.0f - mix) + filtered * mix;
        }
    }
}

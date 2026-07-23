#include "VibratoVintage.h"

VibratoVintage::VibratoVintage() : AudioModule("VibratoVintage") {}
VibratoVintage::~VibratoVintage() {}

void VibratoVintage::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState)
    {
        ch.delayBuf.resize(maxDelay, 0.0f);
        ch.writePos = 0;
        ch.lfoPhase = 0.0f;
    }
}

void VibratoVintage::reset()
{
    for (auto& ch : channelState)
    {
        std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f);
        ch.writePos = 0;
    }
}

void VibratoVintage::setRate(float rate) { rateParam.store(rate); }
void VibratoVintage::setDepth(float depth) { depthParam.store(depth); }
void VibratoVintage::setWave(float wave) { waveParam.store(wave); }

void VibratoVintage::processInternal(AudioBuffer<float>& buffer)
{
    const float rate = rateParam.load() * 7.0f + 1.5f;
    const float depth = depthParam.load() * 10.0f;
    const float wave = waveParam.load();
    const float phaseInc = rate / (float)currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            state.delayBuf[state.writePos] = data[i];
            float sine = std::sin(state.lfoPhase * juce::MathConstants<float>::twoPi);
            float tri = 2.0f * std::abs(2.0f * (state.lfoPhase - 0.5f)) - 1.0f;
            float lfo = sine * (1.0f - wave) + tri * wave;
            float delayTime = 6.0f + depth * (lfo + 1.0f) * 0.5f;
            float readPos = state.writePos - delayTime;
            if (readPos < 0) readPos += maxDelay;
            int idx = (int)readPos;
            float frac = readPos - idx;
            data[i] = state.delayBuf[idx] * (1.0f - frac) + state.delayBuf[(idx + 1) % maxDelay] * frac;
            state.writePos = (state.writePos + 1) % maxDelay;
            state.lfoPhase += phaseInc;
            if (state.lfoPhase >= 1.0f) state.lfoPhase -= 1.0f;
        }
    }
}

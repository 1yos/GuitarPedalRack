#include "TapeDelay.h"

TapeDelay::TapeDelay() : AudioModule("TapeDelay") {}
TapeDelay::~TapeDelay() {}

void TapeDelay::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.delayBuf.resize(maxDelay, 0.0f); ch.writePos = 0; }
}

void TapeDelay::reset()
{
    for (auto& ch : channelState) { std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f); ch.writePos = 0; }
}

void TapeDelay::setTime(float time) { timeParam.store(time); }
void TapeDelay::setFeedback(float feedback) { feedbackParam.store(feedback); }
void TapeDelay::setWow(float wow) { wowParam.store(wow); }
void TapeDelay::setFlutter(float flutter) { flutterParam.store(flutter); }
void TapeDelay::setMix(float mix) { mixParam.store(mix); }

void TapeDelay::processInternal(AudioBuffer<float>& buffer)
{
    const float time = timeParam.load() * 1900.0f + 100.0f;
    const float feedback = feedbackParam.load() * 0.85f;
    const float wow = wowParam.load();
    const float flutter = flutterParam.load();
    const float mix = mixParam.load();
    const float delaySamples = (time / 1000.0f) * currentSampleRate;
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float wowLfo = std::sin(state.wowPhase * juce::MathConstants<float>::twoPi);
            float flutterLfo = std::sin(state.flutterPhase * juce::MathConstants<float>::twoPi * 20.0f);
            float modDelay = delaySamples * (1.0f + wow * 0.01f * wowLfo + flutter * 0.005f * flutterLfo);
            float readPos = state.writePos - modDelay;
            if (readPos < 0) readPos += maxDelay;
            int idx = (int)readPos;
            float frac = readPos - idx;
            float delayed = state.delayBuf[idx] * (1.0f - frac) + state.delayBuf[(idx + 1) % maxDelay] * frac;
            state.lpfZ1 = state.lpfZ1 + 0.3f * (delayed - state.lpfZ1);
            state.delayBuf[state.writePos] = data[i] + state.lpfZ1 * feedback;
            data[i] = data[i] * (1.0f - mix) + state.lpfZ1 * mix;
            state.writePos = (state.writePos + 1) % maxDelay;
            state.wowPhase += 0.5f / currentSampleRate;
            state.flutterPhase += 6.0f / currentSampleRate;
            if (state.wowPhase >= 1.0f) state.wowPhase -= 1.0f;
            if (state.flutterPhase >= 1.0f) state.flutterPhase -= 1.0f;
        }
    }
}

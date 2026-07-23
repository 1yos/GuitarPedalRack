#include "PingPongDelay.h"

PingPongDelay::PingPongDelay() : AudioModule("PingPongDelay") {}
PingPongDelay::~PingPongDelay() {}

void PingPongDelay::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState) { ch.delayBuf.resize(maxDelay, 0.0f); ch.writePos = 0; }
}

void PingPongDelay::reset()
{
    for (auto& ch : channelState) { std::fill(ch.delayBuf.begin(), ch.delayBuf.end(), 0.0f); ch.writePos = 0; }
}

void PingPongDelay::setTime(float time) { timeParam.store(time); }
void PingPongDelay::setFeedback(float feedback) { feedbackParam.store(feedback); }
void PingPongDelay::setStereoWidth(float width) { widthParam.store(width); }
void PingPongDelay::setMix(float mix) { mixParam.store(mix); }

void PingPongDelay::processInternal(AudioBuffer<float>& buffer)
{
    const float time = timeParam.load() * 1400.0f + 100.0f;
    const float feedback = feedbackParam.load() * 0.85f;
    const float width = widthParam.load();
    const float mix = mixParam.load();
    const float delaySamples = (time / 1000.0f) * (float)currentSampleRate;
    const int delayInt = juce::jlimit(1, maxDelay - 1, (int)delaySamples);
    
    if (buffer.getNumChannels() < 2)
    {
        auto* data = buffer.getWritePointer(0);
        auto& state = channelState[0];
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            int readPos = (state.writePos - delayInt + maxDelay) % maxDelay;
            float delayed = state.delayBuf[readPos];
            state.lpfZ1 = state.lpfZ1 + 0.5f * (delayed - state.lpfZ1);
            state.delayBuf[state.writePos] = data[i] + state.lpfZ1 * feedback;
            data[i] = data[i] * (1.0f - mix) + state.lpfZ1 * mix;
            state.writePos = (state.writePos + 1) % maxDelay;
        }
    }
    else
    {
        auto* dataL = buffer.getWritePointer(0);
        auto* dataR = buffer.getWritePointer(1);
        auto& stateL = channelState[0];
        auto& stateR = channelState[1];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            // FIXED: each channel uses its own write position for reading
            int readPosL = (stateL.writePos - delayInt + maxDelay) % maxDelay;
            int readPosR = (stateR.writePos - delayInt + maxDelay) % maxDelay;
            
            float delayedL = stateL.delayBuf[readPosL];
            float delayedR = stateR.delayBuf[readPosR];
            
            stateL.lpfZ1 = stateL.lpfZ1 + 0.5f * (delayedL - stateL.lpfZ1);
            stateR.lpfZ1 = stateR.lpfZ1 + 0.5f * (delayedR - stateR.lpfZ1);
            
            // Ping-pong cross-feed: L feeds into R delay, R feeds into L delay
            stateL.delayBuf[stateL.writePos] = dataL[i] + stateR.lpfZ1 * feedback * width;
            stateR.delayBuf[stateR.writePos] = dataR[i] + stateL.lpfZ1 * feedback * width;
            
            dataL[i] = dataL[i] * (1.0f - mix) + stateL.lpfZ1 * mix;
            dataR[i] = dataR[i] * (1.0f - mix) + stateR.lpfZ1 * mix;
            
            stateL.writePos = (stateL.writePos + 1) % maxDelay;
            stateR.writePos = (stateR.writePos + 1) % maxDelay;
        }
    }
}

#include "ReverseReverb.h"

ReverseReverb::ReverseReverb() : AudioModule("ReverseReverb") {}
ReverseReverb::~ReverseReverb() {}

void ReverseReverb::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    channelState.resize(2);
    for (auto& ch : channelState)
    {
        ch.reverseBuf.resize(reverseBufSize, 0.0f);
        ch.writePos = 0;
    }
}

void ReverseReverb::reset()
{
    for (auto& ch : channelState)
    {
        std::fill(ch.reverseBuf.begin(), ch.reverseBuf.end(), 0.0f);
        ch.writePos = 0;
    }
}

void ReverseReverb::setSize(float size) { sizeParam.store(size); }
void ReverseReverb::setMix(float mix) { mixParam.store(mix); }

void ReverseReverb::processInternal(AudioBuffer<float>& buffer)
{
    const float size = sizeParam.load();
    const float mix = mixParam.load();
    const int bufSize = (int)(size * 44100.0f + 2205.0f);
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            state.reverseBuf[state.writePos] = data[i];
            int readPos = state.writePos + bufSize;
            if (readPos >= reverseBufSize) readPos -= reverseBufSize;
            float reversed = state.reverseBuf[readPos] * 0.7f;
            data[i] = data[i] * (1.0f - mix) + reversed * mix;
            state.writePos = (state.writePos + 1) % reverseBufSize;
        }
    }
}

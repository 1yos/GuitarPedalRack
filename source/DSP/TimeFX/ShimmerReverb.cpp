#include "ShimmerReverb.h"

ShimmerReverb::ShimmerReverb() : AudioModule("ShimmerReverb") {}
ShimmerReverb::~ShimmerReverb() {}

void ShimmerReverb::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    reverbProcessor.setSampleRate(sampleRate);
    channelState.resize(2);
    for (auto& ch : channelState)
    {
        ch.pitchBuf.resize(pitchBufSize, 0.0f);
        ch.writePos = 0;
    }
}

void ShimmerReverb::reset()
{
    reverbProcessor.reset();
    for (auto& ch : channelState)
    {
        std::fill(ch.pitchBuf.begin(), ch.pitchBuf.end(), 0.0f);
        ch.writePos = 0;
    }
}

void ShimmerReverb::setSize(float size) { sizeParam.store(size); }
void ShimmerReverb::setShimmer(float shimmer) { shimmerParam.store(shimmer); }
void ShimmerReverb::setMix(float mix) { mixParam.store(mix); }

void ShimmerReverb::processInternal(AudioBuffer<float>& buffer)
{
    const float shimmer = shimmerParam.load();
    
    AudioBuffer<float> workBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    workBuffer.makeCopyOf(buffer);
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = workBuffer.getWritePointer(ch);
        auto& state = channelState[ch];
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            state.pitchBuf[state.writePos] = data[i];
            int readPos = state.writePos - (pitchBufSize / 2);
            if (readPos < 0) readPos += pitchBufSize;
            float octaveUp = state.pitchBuf[readPos];
            data[i] = data[i] + octaveUp * shimmer * 0.5f;
            state.writePos = (state.writePos + 1) % pitchBufSize;
        }
    }
    
    reverbParams.roomSize = sizeParam.load();
    reverbParams.damping = 0.3f;
    reverbParams.width = 1.0f;
    reverbParams.wetLevel = mixParam.load();
    reverbParams.dryLevel = 1.0f - mixParam.load();
    reverbParams.freezeMode = 0.0f;
    
    reverbProcessor.setParameters(reverbParams);
    
    if (workBuffer.getNumChannels() == 1)
    {
        AudioBuffer<float> stereoBuffer(2, buffer.getNumSamples());
        stereoBuffer.copyFrom(0, 0, workBuffer, 0, 0, buffer.getNumSamples());
        stereoBuffer.copyFrom(1, 0, workBuffer, 0, 0, buffer.getNumSamples());
        reverbProcessor.processStereo(stereoBuffer.getWritePointer(0), stereoBuffer.getWritePointer(1), buffer.getNumSamples());
        buffer.copyFrom(0, 0, stereoBuffer, 0, 0, buffer.getNumSamples());
    }
    else
    {
        reverbProcessor.processStereo(workBuffer.getWritePointer(0), workBuffer.getWritePointer(1), buffer.getNumSamples());
        buffer.makeCopyOf(workBuffer);
    }
}

#include "HallReverb.h"

HallReverb::HallReverb() : AudioModule("HallReverb") {}
HallReverb::~HallReverb() {}

void HallReverb::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    reverbProcessor.setSampleRate(sampleRate);
}

void HallReverb::reset()
{
    reverbProcessor.reset();
}

void HallReverb::setSize(float size) { sizeParam.store(size); }
void HallReverb::setDecay(float decay) { decayParam.store(decay); }
void HallReverb::setDamping(float damping) { dampingParam.store(damping); }
void HallReverb::setMix(float mix) { mixParam.store(mix); }

void HallReverb::processInternal(AudioBuffer<float>& buffer)
{
    reverbParams.roomSize = sizeParam.load();
    reverbParams.damping = dampingParam.load() * 0.7f;
    reverbParams.width = 1.0f;
    reverbParams.wetLevel = mixParam.load();
    reverbParams.dryLevel = 1.0f - mixParam.load();
    reverbParams.freezeMode = 0.0f;
    
    reverbProcessor.setParameters(reverbParams);
    
    if (buffer.getNumChannels() == 1)
    {
        AudioBuffer<float> stereoBuffer(2, buffer.getNumSamples());
        stereoBuffer.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
        stereoBuffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());
        reverbProcessor.processStereo(stereoBuffer.getWritePointer(0), stereoBuffer.getWritePointer(1), buffer.getNumSamples());
        buffer.copyFrom(0, 0, stereoBuffer, 0, 0, buffer.getNumSamples());
    }
    else
    {
        reverbProcessor.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());
    }
}

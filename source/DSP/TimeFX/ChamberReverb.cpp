#include "ChamberReverb.h"

ChamberReverb::ChamberReverb() : AudioModule("ChamberReverb") {}
ChamberReverb::~ChamberReverb() {}

void ChamberReverb::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    reverbProcessor.setSampleRate(sampleRate);
}

void ChamberReverb::reset()
{
    reverbProcessor.reset();
}

void ChamberReverb::setSize(float size) { sizeParam.store(size); }
void ChamberReverb::setDecay(float decay) { decayParam.store(decay); }
void ChamberReverb::setMix(float mix) { mixParam.store(mix); }

void ChamberReverb::processInternal(AudioBuffer<float>& buffer)
{
    reverbParams.roomSize = sizeParam.load() * 0.7f + 0.2f;
    reverbParams.damping = 0.6f;
    reverbParams.width = 0.9f;
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

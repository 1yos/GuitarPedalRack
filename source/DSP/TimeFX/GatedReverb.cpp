#include "GatedReverb.h"

GatedReverb::GatedReverb() : AudioModule("GatedReverb") {}
GatedReverb::~GatedReverb() {}

void GatedReverb::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    reverbProcessor.setSampleRate(sampleRate);
}

void GatedReverb::reset()
{
    reverbProcessor.reset();
    envelopeLevel = 0.0f;
    gateCounter = 0;
}

void GatedReverb::setSize(float size) { sizeParam.store(size); }
void GatedReverb::setGateTime(float gateTime) { gateTimeParam.store(gateTime); }
void GatedReverb::setThreshold(float threshold) { thresholdParam.store(threshold); }
void GatedReverb::setMix(float mix) { mixParam.store(mix); }

void GatedReverb::processInternal(AudioBuffer<float>& buffer)
{
    const float gateTime = gateTimeParam.load() * 0.8f + 0.1f;
    const float threshold = thresholdParam.load();
    const int gateSamples = (int)(gateTime * currentSampleRate);
    
    reverbParams.roomSize = sizeParam.load();
    reverbParams.damping = 0.5f;
    reverbParams.width = 1.0f;
    reverbParams.wetLevel = 1.0f;
    reverbParams.dryLevel = 0.0f;
    reverbParams.freezeMode = 0.0f;
    
    reverbProcessor.setParameters(reverbParams);
    
    AudioBuffer<float> reverbBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    reverbBuffer.makeCopyOf(buffer);
    
    if (buffer.getNumChannels() == 1)
    {
        AudioBuffer<float> stereoBuffer(2, buffer.getNumSamples());
        stereoBuffer.copyFrom(0, 0, reverbBuffer, 0, 0, buffer.getNumSamples());
        stereoBuffer.copyFrom(1, 0, reverbBuffer, 0, 0, buffer.getNumSamples());
        reverbProcessor.processStereo(stereoBuffer.getWritePointer(0), stereoBuffer.getWritePointer(1), buffer.getNumSamples());
        reverbBuffer.copyFrom(0, 0, stereoBuffer, 0, 0, buffer.getNumSamples());
    }
    else
    {
        reverbProcessor.processStereo(reverbBuffer.getWritePointer(0), reverbBuffer.getWritePointer(1), buffer.getNumSamples());
    }
    
    const float mix = mixParam.load();
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* dry = buffer.getWritePointer(ch);
        auto* wet = reverbBuffer.getReadPointer(ch);
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float inputLevel = std::abs(dry[i]);
            if (inputLevel > threshold)
            {
                gateCounter = gateSamples;
                envelopeLevel = 1.0f;
            }
            else if (gateCounter > 0)
            {
                gateCounter--;
                envelopeLevel = (float)gateCounter / gateSamples;
            }
            else
            {
                envelopeLevel = 0.0f;
            }
            
            dry[i] = dry[i] * (1.0f - mix) + wet[i] * envelopeLevel * mix;
        }
    }
}

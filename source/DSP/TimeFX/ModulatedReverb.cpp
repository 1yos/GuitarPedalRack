#include "ModulatedReverb.h"

ModulatedReverb::ModulatedReverb() : AudioModule("ModulatedReverb") {}
ModulatedReverb::~ModulatedReverb() {}

void ModulatedReverb::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    reverbProcessor.setSampleRate(sampleRate);
}

void ModulatedReverb::reset()
{
    reverbProcessor.reset();
    lfoPhase = 0.0f;
}

void ModulatedReverb::setSize(float size) { sizeParam.store(size); }
void ModulatedReverb::setModulation(float modulation) { modulationParam.store(modulation); }
void ModulatedReverb::setRate(float rate) { rateParam.store(rate); }
void ModulatedReverb::setMix(float mix) { mixParam.store(mix); }

void ModulatedReverb::processInternal(AudioBuffer<float>& buffer)
{
    const float rate = rateParam.load() * 2.0f + 0.1f;
    const float modulation = modulationParam.load();
    const float phaseInc = rate / (float)currentSampleRate;
    
    float lfo = std::sin(lfoPhase * juce::MathConstants<float>::twoPi);
    float modSize = sizeParam.load() * (1.0f + modulation * 0.15f * lfo);
    modSize = juce::jlimit(0.0f, 1.0f, modSize);
    
    reverbParams.roomSize = modSize;
    reverbParams.damping = 0.5f;
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
    
    lfoPhase += phaseInc * buffer.getNumSamples();
    if (lfoPhase >= 1.0f) lfoPhase -= 1.0f;
}

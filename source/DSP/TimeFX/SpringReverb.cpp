#include "SpringReverb.h"

SpringReverb::SpringReverb() : AudioModule("SpringReverb") {}
SpringReverb::~SpringReverb() {}

void SpringReverb::prepare(double sampleRate, int samplesPerBlock)
{
    AudioModule::prepare(sampleRate, samplesPerBlock);
    springs.resize(numSprings);
    for (auto& spring : springs)
    {
        spring.delayBuf.resize(maxDelay, 0.0f);
        spring.writePos = 0;
    }
}

void SpringReverb::reset()
{
    for (auto& spring : springs)
    {
        std::fill(spring.delayBuf.begin(), spring.delayBuf.end(), 0.0f);
        spring.writePos = 0;
        spring.lpfZ1 = 0.0f;
        for (auto& z : spring.apfZ1) z = 0.0f;
    }
}

void SpringReverb::setDecay(float decay) { decayParam.store(decay); }
void SpringReverb::setTone(float tone) { toneParam.store(tone); }
void SpringReverb::setMix(float mix) { mixParam.store(mix); }

void SpringReverb::processInternal(AudioBuffer<float>& buffer)
{
    const float decay = decayParam.load() * 0.85f;
    const float tone = toneParam.load();
    const float mix = mixParam.load();
    const float lpfCoeff = 0.2f + tone * 0.6f;
    const int delays[3] = {347, 419, 571};
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float wet = 0.0f;
            for (int s = 0; s < numSprings; ++s)
            {
                auto& spring = springs[s];
                float readPos = spring.writePos - delays[s];
                if (readPos < 0) readPos += maxDelay;
                int idx = (int)readPos;
                float delayed = spring.delayBuf[idx];
                float apf = delayed;
                for (int a = 0; a < 2; ++a)
                {
                    float coeff = -0.6f + a * 0.2f;
                    float y = coeff * apf + spring.apfZ1[a];
                    spring.apfZ1[a] = apf - coeff * y;
                    apf = y;
                }
                spring.lpfZ1 = spring.lpfZ1 + lpfCoeff * (apf - spring.lpfZ1);
                spring.delayBuf[spring.writePos] = data[i] + spring.lpfZ1 * decay;
                wet += spring.lpfZ1;
                spring.writePos = (spring.writePos + 1) % maxDelay;
            }
            wet /= numSprings;
            data[i] = data[i] * (1.0f - mix) + wet * mix;
        }
    }
}

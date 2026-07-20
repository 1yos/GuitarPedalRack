#include "ChorusEnsemble.h"

ChorusEnsemble::ChorusEnsemble() : AudioModule("Chorus Ensemble") {}
ChorusEnsemble::~ChorusEnsemble() {}

void ChorusEnsemble::prepare(double r, int b)
{
    AudioModule::prepare(r, b);
    channelState.resize(2);
    for (auto& ch : channelState)
    {
        ch.delayBuffer.resize(maxDelay, 0.0f);
        ch.writePos = 0;
        ch.lfo1Phase = 0.0f;
        ch.lfo2Phase = 0.333f;
        ch.lfo3Phase = 0.667f;
    }
}

void ChorusEnsemble::processInternal(AudioBuffer<float>& buffer)
{
    float rate = rateParam.load() * 2.0f + 0.2f;  // 0.2Hz to 2.2Hz
    float depth = depthParam.load() * 8.0f + 2.0f;  // 2-10ms modulation
    float voices = voicesParam.load();
    float mix = mixParam.load();
    
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* data = buffer.getWritePointer(ch);
        auto& state = channelState[ch < channelState.size() ? ch : 0];
        float lfoInc = rate / static_cast<float>(currentSampleRate);
        
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            float input = data[i];
            state.delayBuffer[state.writePos] = input;
            
            // Multi-voice chorus with 3 LFOs
            float lfo1 = std::sin(state.lfo1Phase * MathConstants<float>::twoPi);
            float lfo2 = std::sin(state.lfo2Phase * MathConstants<float>::twoPi);
            float lfo3 = std::sin(state.lfo3Phase * MathConstants<float>::twoPi);
            
            // Calculate delay times (in samples)
            float baseDelay = 500.0f;  // 11ms base at 44.1kHz
            float delay1 = baseDelay + lfo1 * depth * static_cast<float>(currentSampleRate) / 1000.0f;
            float delay2 = baseDelay + lfo2 * depth * static_cast<float>(currentSampleRate) / 1000.0f * 0.9f;
            float delay3 = baseDelay + lfo3 * depth * static_cast<float>(currentSampleRate) / 1000.0f * 0.8f;
            
            // Read from delay with linear interpolation
            auto readDelay = [&](float delayInSamples) -> float {
                float readPos = state.writePos - delayInSamples;
                while (readPos < 0) readPos += maxDelay;
                int pos1 = static_cast<int>(readPos) % maxDelay;
                int pos2 = (pos1 + 1) % maxDelay;
                float frac = readPos - std::floor(readPos);
                return state.delayBuffer[pos1] * (1.0f - frac) + state.delayBuffer[pos2] * frac;
            };
            
            float voice1 = readDelay(delay1);
            float voice2 = readDelay(delay2);
            float voice3 = readDelay(delay3);
            
            // Mix voices based on voices parameter
            float chorus = voice1;
            if (voices > 0.33f) chorus = (chorus + voice2) * 0.5f;
            if (voices > 0.67f) chorus = (chorus + voice3) * 0.666f;
            
            // Mix with dry signal
            data[i] = input * (1.0f - mix) + chorus * mix;
            
            // Update LFO phases
            state.lfo1Phase += lfoInc;
            state.lfo2Phase += lfoInc * 1.03f;  // Slightly detuned
            state.lfo3Phase += lfoInc * 0.97f;
            if (state.lfo1Phase >= 1.0f) state.lfo1Phase -= 1.0f;
            if (state.lfo2Phase >= 1.0f) state.lfo2Phase -= 1.0f;
            if (state.lfo3Phase >= 1.0f) state.lfo3Phase -= 1.0f;
            
            state.writePos = (state.writePos + 1) % maxDelay;
        }
    }
}

void ChorusEnsemble::reset()
{
    for (auto& ch : channelState)
    {
        std::fill(ch.delayBuffer.begin(), ch.delayBuffer.end(), 0.0f);
        ch.writePos = 0;
    }
}

void ChorusEnsemble::setRate(float r) { rateParam.store(jlimit(0.0f, 1.0f, r)); }
void ChorusEnsemble::setDepth(float d) { depthParam.store(jlimit(0.0f, 1.0f, d)); }
void ChorusEnsemble::setVoices(float v) { voicesParam.store(jlimit(0.0f, 1.0f, v)); }
void ChorusEnsemble::setMix(float m) { mixParam.store(jlimit(0.0f, 1.0f, m)); }

#pragma once

#include "../JuceHeader.h"
#include <vector>
#include <functional>

using namespace juce;

//==============================================================================
/**
 * LFO (Low Frequency Oscillator) for parameter modulation
 */
class LFO
{
public:
    enum class Waveform
    {
        Sine,
        Triangle,
        Square,
        Sawtooth,
        Random
    };
    
    LFO() = default;
    
    void prepare(double sampleRate)
    {
        this->sampleRate = sampleRate;
        phase = 0.0f;
        lastRandomValue = 0.0f;
    }
    
    void setFrequency(float hz)
    {
        frequency = jlimit(0.01f, 50.0f, hz);
    }
    
    void setWaveform(Waveform wave)
    {
        waveform = wave;
    }
    
    void setDepth(float depth)
    {
        this->depth = jlimit(0.0f, 1.0f, depth);
    }
    
    float getNextSample()
    {
        float value = 0.0f;
        
        switch (waveform)
        {
            case Waveform::Sine:
                value = std::sin(phase * 2.0f * MathConstants<float>::pi);
                break;
                
            case Waveform::Triangle:
                value = 2.0f * std::abs(2.0f * (phase - 0.5f)) - 1.0f;
                break;
                
            case Waveform::Square:
                value = phase < 0.5f ? 1.0f : -1.0f;
                break;
                
            case Waveform::Sawtooth:
                value = 2.0f * phase - 1.0f;
                break;
                
            case Waveform::Random:
            {
                // Sample and hold random values
                if (phase < phaseIncrement)
                {
                    Random random;
                    lastRandomValue = random.nextFloat() * 2.0f - 1.0f;
                }
                value = lastRandomValue;
                break;
            }
        }
        
        // Advance phase
        phaseIncrement = frequency / (float)sampleRate;
        phase += phaseIncrement;
        if (phase >= 1.0f)
            phase -= 1.0f;
        
        return value * depth;
    }
    
    void reset()
    {
        phase = 0.0f;
        lastRandomValue = 0.0f;
    }

private:
    double sampleRate = 44100.0;
    float frequency = 1.0f;
    float depth = 1.0f;
    float phase = 0.0f;
    float phaseIncrement = 0.0f;
    float lastRandomValue = 0.0f;
    Waveform waveform = Waveform::Sine;
};

//==============================================================================
/**
 * Envelope follower for dynamic parameter modulation
 */
class EnvelopeFollower
{
public:
    EnvelopeFollower() = default;
    
    void prepare(double sampleRate)
    {
        this->sampleRate = sampleRate;
        envelope = 0.0f;
    }
    
    void setAttackTime(float ms)
    {
        attackCoeff = std::exp(-1.0f / (ms * 0.001f * (float)sampleRate));
    }
    
    void setReleaseTime(float ms)
    {
        releaseCoeff = std::exp(-1.0f / (ms * 0.001f * (float)sampleRate));
    }
    
    float process(float inputSample)
    {
        float rectified = std::abs(inputSample);
        
        if (rectified > envelope)
            envelope = attackCoeff * envelope + (1.0f - attackCoeff) * rectified;
        else
            envelope = releaseCoeff * envelope + (1.0f - releaseCoeff) * rectified;
        
        return envelope;
    }
    
    float getCurrentEnvelope() const { return envelope; }
    
    void reset()
    {
        envelope = 0.0f;
    }

private:
    double sampleRate = 44100.0;
    float envelope = 0.0f;
    float attackCoeff = 0.99f;
    float releaseCoeff = 0.999f;
};

//==============================================================================
/**
 * Parameter automation and modulation system
 */
class AutomationEngine
{
public:
    struct ModulationTarget
    {
        std::function<void(float)> setter;  // Parameter setter function
        std::function<float()> getter;      // Parameter getter function
        float baseValue = 0.0f;             // Unmodulated value
        float minValue = 0.0f;
        float maxValue = 1.0f;
        float modulationAmount = 0.0f;      // -1.0 to 1.0
    };
    
    AutomationEngine() = default;
    
    void prepare(double sampleRate, int maxBlockSize)
    {
        this->sampleRate = sampleRate;
        this->maxBlockSize = maxBlockSize;
        
        lfo.prepare(sampleRate);
        envFollower.prepare(sampleRate);
    }
    
    //==============================================================================
    // LFO Control
    
    void setLfoFrequency(float hz)
    {
        lfo.setFrequency(hz);
    }
    
    void setLfoWaveform(LFO::Waveform wave)
    {
        lfo.setWaveform(wave);
    }
    
    void setLfoDepth(float depth)
    {
        lfo.setDepth(depth);
    }
    
    //==============================================================================
    // Envelope Follower Control
    
    void setEnvelopeAttack(float ms)
    {
        envFollower.setAttackTime(ms);
    }
    
    void setEnvelopeRelease(float ms)
    {
        envFollower.setReleaseTime(ms);
    }
    
    //==============================================================================
    // Modulation Targets
    
    int addTarget(std::function<void(float)> setter,
                  std::function<float()> getter,
                  float minValue, float maxValue)
    {
        ModulationTarget target;
        target.setter = setter;
        target.getter = getter;
        target.minValue = minValue;
        target.maxValue = maxValue;
        targets.push_back(target);
        return static_cast<int>(targets.size()) - 1;
    }
    
    void setModulationAmount(int targetIndex, float amount)
    {
        if (targetIndex >= 0 && targetIndex < (int)targets.size())
        {
            targets[targetIndex].modulationAmount = jlimit(-1.0f, 1.0f, amount);
        }
    }
    
    void removeAllTargets()
    {
        targets.clear();
    }
    
    //==============================================================================
    // Processing
    
    void processBlock(const AudioBuffer<float>& audioBuffer)
    {
        if (targets.empty())
            return;
        
        const int numSamples = audioBuffer.getNumSamples();
        
        // Process envelope follower
        float envelopeValue = 0.0f;
        if (audioBuffer.getNumChannels() > 0)
        {
            const float* channelData = audioBuffer.getReadPointer(0);
            for (int i = 0; i < numSamples; ++i)
            {
                envelopeValue = envFollower.process(channelData[i]);
            }
        }
        
        // Get LFO value (once per block for efficiency)
        float lfoValue = lfo.getNextSample();
        
        // Update all modulation targets
        for (auto& target : targets)
        {
            if (target.setter && target.getter)
            {
                // Get current base value
                target.baseValue = target.getter();
                
                // Calculate modulated value
                float modulation = lfoValue * target.modulationAmount;
                float range = target.maxValue - target.minValue;
                float modulatedValue = target.baseValue + (modulation * range * 0.5f);
                
                // Clamp and apply
                modulatedValue = jlimit(target.minValue, target.maxValue, modulatedValue);
                target.setter(modulatedValue);
            }
        }
    }
    
    void reset()
    {
        lfo.reset();
        envFollower.reset();
    }

private:
    double sampleRate = 44100.0;
    int maxBlockSize = 512;
    
    LFO lfo;
    EnvelopeFollower envFollower;
    std::vector<ModulationTarget> targets;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AutomationEngine)
};

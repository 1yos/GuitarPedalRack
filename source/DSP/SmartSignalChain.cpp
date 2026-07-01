#include "SmartSignalChain.h"
#include <chrono>
#include <algorithm>

//==============================================================================
SmartSignalChain::SmartSignalChain()
{
    // Initialize thread pool for parallel processing (4 threads)
    threadPool = std::make_unique<ThreadPool>(4);
}

SmartSignalChain::~SmartSignalChain()
{
    clearAllEffects();
}

//==============================================================================
// Effect Management

void SmartSignalChain::addEffect(std::unique_ptr<AudioModule> effect, int position)
{
    if (!effect)
        return;
    
    // Prepare the effect if we're already initialized
    if (currentSampleRate > 0)
        effect->prepare(currentSampleRate, currentBlockSize);
    
    auto slot = std::make_unique<EffectSlot>(std::move(effect));
    
    if (position < 0 || position >= static_cast<int>(effects.size()))
    {
        effects.push_back(std::move(slot));
    }
    else
    {
        effects.insert(effects.begin() + position, std::move(slot));
    }
}

void SmartSignalChain::removeEffect(int position)
{
    if (position >= 0 && position < static_cast<int>(effects.size()))
    {
        effects.erase(effects.begin() + position);
    }
}

void SmartSignalChain::moveEffect(int fromPosition, int toPosition)
{
    if (fromPosition < 0 || fromPosition >= static_cast<int>(effects.size()))
        return;
    
    if (toPosition < 0 || toPosition >= static_cast<int>(effects.size()))
        return;
    
    if (fromPosition == toPosition)
        return;
    
    auto effect = std::move(effects[fromPosition]);
    effects.erase(effects.begin() + fromPosition);
    
    if (toPosition > fromPosition)
        toPosition--;
    
    effects.insert(effects.begin() + toPosition, std::move(effect));
}

void SmartSignalChain::clearAllEffects()
{
    effects.clear();
    currentCPUUsage.store(0.0f);
    numActiveEffects.store(0);
}

AudioModule* SmartSignalChain::getEffect(int position)
{
    if (position >= 0 && position < static_cast<int>(effects.size()))
        return effects[position]->effect.get();
    
    return nullptr;
}

//==============================================================================
// Processing

void SmartSignalChain::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    
    // Prepare all effects
    for (auto& slot : effects)
    {
        if (slot->effect)
            slot->effect->prepare(sampleRate, samplesPerBlock);
    }
}

void SmartSignalChain::process(AudioBuffer<float>& buffer)
{
    if (effects.empty())
        return;
    
    // Start CPU timing
    auto processingStart = std::chrono::high_resolution_clock::now();
    
    int activeCount = 0;
    float totalCPU = 0.0f;
    
    // Process each effect
    for (auto& slot : effects)
    {
        if (!slot->effect)
            continue;
        
        // Check if effect should be bypassed
        if (slot->isBypassed.load())
            continue;
        
        // Smart optimization: check if input is silent
        if (autoOptimizationEnabled && isSilent(buffer))
        {
            slot->silentFrameCount++;
            
            // After 10 silent frames, mark as inactive
            if (slot->silentFrameCount > 10)
            {
                slot->isActive.store(false);
                continue;
            }
        }
        else
        {
            slot->silentFrameCount = 0;
            slot->isActive.store(true);
        }
        
        // Process the effect with CPU monitoring
        processSingleEffect(*slot, buffer);
        
        activeCount++;
        totalCPU += slot->cpuUsage.load();
    }
    
    // Update global stats
    numActiveEffects.store(activeCount);
    currentCPUUsage.store(totalCPU);
    
    // Optimize chain if needed
    if (autoOptimizationEnabled && currentCPUUsage.load() > maxCPUUsage)
    {
        optimizeChain();
    }
}

void SmartSignalChain::reset()
{
    for (auto& slot : effects)
    {
        if (slot->effect)
            slot->effect->reset();
        
        slot->silentFrameCount = 0;
        slot->lastRMS = 0.0f;
        slot->isActive.store(true);
    }
}

//==============================================================================
// Smart Processing Helpers

bool SmartSignalChain::isSilent(const AudioBuffer<float>& buffer) const
{
    float rms = 0.0f;
    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();
    
    for (int ch = 0; ch < numChannels; ++ch)
    {
        const float* data = buffer.getReadPointer(ch);
        float channelRMS = 0.0f;
        
        for (int i = 0; i < numSamples; ++i)
        {
            channelRMS += data[i] * data[i];
        }
        
        rms += channelRMS / static_cast<float>(numSamples);
    }
    
    rms = std::sqrt(rms / static_cast<float>(numChannels));
    
    // Convert to dB
    float dB = 20.0f * std::log10(rms + 1e-10f);
    
    return dB < silenceThresholdDb;
}

void SmartSignalChain::processSingleEffect(EffectSlot& slot, AudioBuffer<float>& buffer)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Process the effect
    slot.effect->process(buffer);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    
    // Calculate CPU usage for this effect
    double blockDuration = (buffer.getNumSamples() / currentSampleRate) * 1000000.0; // microseconds
    float cpuPercent = static_cast<float>(duration / blockDuration);
    
    // Smooth CPU reading
    float currentCPU = slot.cpuUsage.load();
    slot.cpuUsage.store(currentCPU * 0.9f + cpuPercent * 0.1f);
}

void SmartSignalChain::optimizeChain()
{
    // If CPU is too high, temporarily bypass lowest-priority effects
    // Priority: Effects with lowest CPU usage are bypassed first
    
    std::vector<int> effectsByPriority;
    for (int i = 0; i < static_cast<int>(effects.size()); ++i)
    {
        effectsByPriority.push_back(i);
    }
    
    // Sort by CPU usage (ascending)
    std::sort(effectsByPriority.begin(), effectsByPriority.end(),
              [this](int a, int b) {
                  return effects[a]->cpuUsage.load() < effects[b]->cpuUsage.load();
              });
    
    // Bypass effects until CPU is under limit
    float targetCPU = maxCPUUsage * 0.9f; // Target 90% of max
    for (int idx : effectsByPriority)
    {
        if (currentCPUUsage.load() < targetCPU)
            break;
        
        if (!effects[idx]->isBypassed.load())
        {
            effects[idx]->isBypassed.store(true);
            currentCPUUsage.store(currentCPUUsage.load() - effects[idx]->cpuUsage.load());
        }
    }
}

//==============================================================================
// CPU Monitoring

float SmartSignalChain::getEffectCPUUsage(int position) const
{
    if (position >= 0 && position < static_cast<int>(effects.size()))
        return effects[position]->cpuUsage.load();
    
    return 0.0f;
}

//==============================================================================
// Serialization

ValueTree SmartSignalChain::saveState() const
{
    ValueTree state("SmartSignalChain");
    
    for (int i = 0; i < static_cast<int>(effects.size()); ++i)
    {
        if (effects[i]->effect)
        {
            ValueTree effectState("Effect");
            effectState.setProperty("type", effects[i]->effect->getModuleType(), nullptr);
            effectState.setProperty("bypassed", effects[i]->isBypassed.load(), nullptr);
            
            // Save effect-specific state
            // effects[i]->effect->saveState(effectState);
            
            state.appendChild(effectState, nullptr);
        }
    }
    
    return state;
}

void SmartSignalChain::loadState(const ValueTree& state)
{
    clearAllEffects();
    
    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        ValueTree effectState = state.getChild(i);
        
        String effectType = effectState.getProperty("type").toString();
        bool bypassed = effectState.getProperty("bypassed");
        
        // Create effect based on type
        // auto effect = EffectFactory::createEffect(effectType);
        // if (effect)
        // {
        //     effect->loadState(effectState);
        //     addEffect(std::move(effect));
        //     effects.back()->isBypassed.store(bypassed);
        // }
    }
}

//==============================================================================
// Buffer Pool Implementation

AudioBuffer<float>* SmartSignalChain::BufferPool::acquire(int numChannels, int numSamples)
{
    const SpinLock::ScopedLockType sl(lock);
    
    // Check if we have a suitable buffer available
    for (int i = available.size(); --i >= 0;)
    {
        if (available[i]->getNumChannels() >= numChannels &&
            available[i]->getNumSamples() >= numSamples)
        {
            auto* buffer = available[i];
            available.erase(available.begin() + i);
            return buffer;
        }
    }
    
    // Create new buffer
    auto buffer = std::make_unique<AudioBuffer<float>>(numChannels, numSamples);
    auto* ptr = buffer.get();
    buffers.push_back(std::move(buffer));
    return ptr;
}

void SmartSignalChain::BufferPool::release(AudioBuffer<float>* buffer)
{
    if (buffer)
    {
        const SpinLock::ScopedLockType sl(lock);
        available.push_back(buffer);
    }
}

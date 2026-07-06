#include "SmartSignalChain.h"
#include <chrono>
#include <algorithm>

//==============================================================================
SmartSignalChain::SmartSignalChain()
{
    // Initialize thread pool for parallel processing
    // Use N-1 cores (leave one for UI/system)
    int systemCores = SystemStats::getNumCpus();
    int idealThreads = jmax(1, systemCores - 1);
    
    threadPool = std::make_unique<ThreadPool>(idealThreads);
    numThreads = idealThreads;
    
    DBG("SmartSignalChain initialized with " + String(idealThreads) + " threads");
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

// Process method is now at the end of the file with parallel processing support

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

//==============================================================================
// Multi-Threading Configuration

void SmartSignalChain::setNumThreads(int newNumThreads)
{
    if (newNumThreads <= 0)
    {
        // Auto-detect
        int systemCores = SystemStats::getNumCpus();
        numThreads = jmax(1, systemCores - 1);
    }
    else
    {
        numThreads = jlimit(1, 16, newNumThreads);
    }
    
    // Recreate thread pool with new size
    threadPool = std::make_unique<ThreadPool>(numThreads);
    
    DBG("Thread pool resized to " + String(numThreads) + " threads");
}

float SmartSignalChain::getThreadingEfficiency() const
{
    return threadingEfficiency.load();
}

//==============================================================================
// Effect Grouping for Parallel Processing

std::vector<SmartSignalChain::EffectGroup> SmartSignalChain::createEffectGroups()
{
    std::vector<EffectGroup> groups;
    
    if (effects.empty())
        return groups;
    
    // Group effects by estimated CPU cost
    // Goal: Balance load across available threads
    
    int numGroups = jmin(numThreads, static_cast<int>(effects.size()));
    groups.resize(numGroups);
    
    // Simple round-robin distribution
    // TODO: Implement smarter grouping based on actual CPU measurements
    for (int i = 0; i < static_cast<int>(effects.size()); ++i)
    {
        int groupIndex = i % numGroups;
        groups[groupIndex].effectIndices.push_back(i);
        groups[groupIndex].estimatedCPU += effects[i]->cpuUsage.load();
    }
    
    return groups;
}

bool SmartSignalChain::shouldUseParallelProcessing() const
{
    // Don't use parallel processing if:
    // 1. Not enabled
    // 2. Too few effects
    // 3. CPU usage too low (overhead not worth it)
    
    if (!parallelProcessingEnabled)
        return false;
    
    if (static_cast<int>(effects.size()) < minEffectsForParallel)
        return false;
    
    if (adaptiveThreading && currentCPUUsage.load() < parallelThreshold)
        return false;
    
    return true;
}

//==============================================================================
// Serial Processing (original method)

void SmartSignalChain::processSerial(AudioBuffer<float>& buffer)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    
    int activeCount = 0;
    float totalCPU = 0.0f;
    
    // Process each effect sequentially
    for (auto& slot : effects)
    {
        if (!slot->effect)
            continue;
        
        if (slot->isBypassed.load())
            continue;
        
        // Smart optimization: check if input is silent
        if (autoOptimizationEnabled && isSilent(buffer))
        {
            slot->silentFrameCount++;
            
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
    
    auto endTime = std::chrono::high_resolution_clock::now();
    serialProcessingTime.store(
        std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()
    );
}

//==============================================================================
// Parallel Processing

void SmartSignalChain::processParallelGroups(AudioBuffer<float>& buffer, 
                                              const std::vector<EffectGroup>& groups)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::atomic<int> activeCount{0};
    std::atomic<int> totalCPUInt{0};  // Use int for atomic, convert to float later
    
    // Create jobs for each group
    std::vector<std::function<void()>> jobs;
    
    for (const auto& group : groups)
    {
        jobs.push_back([&, group]() {
            int localActive = 0;
            float localCPU = 0.0f;
            
            // Process all effects in this group
            for (int idx : group.effectIndices)
            {
                if (idx < 0 || idx >= static_cast<int>(effects.size()))
                    continue;
                
                auto& slot = effects[idx];
                
                if (!slot->effect || slot->isBypassed.load())
                    continue;
                
                // Check silence
                if (autoOptimizationEnabled && isSilent(buffer))
                {
                    slot->silentFrameCount++;
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
                
                // Process effect
                processSingleEffect(*slot, buffer);
                
                localActive++;
                localCPU += slot->cpuUsage.load();
            }
            
            // Update atomic counters
            activeCount += localActive;
            totalCPUInt += static_cast<int>(localCPU * 100.0f);  // Store as int percentage
        });
    }
    
    // Execute jobs in parallel
    for (auto& job : jobs)
    {
        threadPool->addJob(std::move(job));
    }
    
    // Wait for all jobs to complete
    while (threadPool->getNumJobs() > 0)
    {
        Thread::sleep(0);  // Yield to allow threads to process
    }
    
    // Update global stats
    numActiveEffects.store(activeCount.load());
    currentCPUUsage.store(static_cast<float>(totalCPUInt.load()) / 100.0f);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    int64_t parallelTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    parallelProcessingTime.store(parallelTime);
    
    // Calculate efficiency
    int64_t serialTime = serialProcessingTime.load();
    if (serialTime > 0 && parallelTime > 0)
    {
        float efficiency = static_cast<float>(serialTime) / static_cast<float>(parallelTime);
        threadingEfficiency.store(efficiency);
    }
}

//==============================================================================
// Modified process() to use parallel processing when beneficial

void SmartSignalChain::process(AudioBuffer<float>& buffer)
{
    if (effects.empty())
        return;
    
    // Decide: serial or parallel?
    if (shouldUseParallelProcessing())
    {
        // Create effect groups
        auto groups = createEffectGroups();
        
        // Process in parallel
        processParallelGroups(buffer, groups);
    }
    else
    {
        // Process serially
        processSerial(buffer);
    }
    
    // Optimize chain if needed
    if (autoOptimizationEnabled && currentCPUUsage.load() > maxCPUUsage)
    {
        optimizeChain();
    }
}

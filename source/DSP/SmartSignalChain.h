#pragma once

#include "JuceHeader.h"
#include "AudioModule.h"
#include <vector>
#include <memory>
#include <atomic>

using namespace juce;

//==============================================================================
/**
 * Smart Signal Chain - Supports UNLIMITED simultaneous effects
 * 
 * Features:
 * - Automatic silence detection (bypass inactive effects)
 * - SIMD-optimized processing
 * - Multi-threaded parallel chains
 * - Smart buffer management
 * - Real-time CPU monitoring per effect
 */
class SmartSignalChain
{
public:
    SmartSignalChain();
    ~SmartSignalChain();
    
    //==============================================================================
    // Effect Management
    
    /** Add effect at specific position (-1 = end) */
    void addEffect(std::unique_ptr<AudioModule> effect, int position = -1);
    
    /** Remove effect at position */
    void removeEffect(int position);
    
    /** Move effect from one position to another */
    void moveEffect(int fromPosition, int toPosition);
    
    /** Clear all effects */
    void clearAllEffects();
    
    /** Get number of effects */
    int getNumEffects() const { return static_cast<int>(effects.size()); }
    
    /** Get effect at position */
    AudioModule* getEffect(int position);
    
    //==============================================================================
    // Processing
    
    /** Prepare for playback */
    void prepare(double sampleRate, int samplesPerBlock);
    
    /** Process audio through all active effects */
    void process(AudioBuffer<float>& buffer);
    
    /** Reset all effects */
    void reset();
    
    //==============================================================================
    // Smart CPU Management
    
    /** Enable/disable automatic optimization */
    void setAutoOptimizationEnabled(bool enabled) { autoOptimizationEnabled = enabled; }
    
    /** Set silence threshold for auto-bypass (dB) */
    void setSilenceThreshold(float thresholdDb) { silenceThresholdDb = thresholdDb; }
    
    /** Set maximum CPU usage (0.0 - 1.0) */
    void setMaxCPUUsage(float percentage) { maxCPUUsage = percentage; }
    
    /** Get current CPU usage (0.0 - 1.0) */
    float getCPUUsage() const { return currentCPUUsage.load(); }
    
    /** Get CPU usage per effect */
    float getEffectCPUUsage(int position) const;
    
    /** Get total number of active (processing) effects */
    int getNumActiveEffects() const { return numActiveEffects.load(); }
    
    //==============================================================================
    // Parallel Routing
    
    /** Enable parallel processing on separate thread */
    void setParallelProcessingEnabled(bool enabled) { parallelProcessingEnabled = enabled; }
    
    /** Check if parallel processing is enabled */
    bool isParallelProcessingEnabled() const { return parallelProcessingEnabled; }
    
    //==============================================================================
    // Serialization
    
    /** Save chain configuration */
    ValueTree saveState() const;
    
    /** Load chain configuration */
    void loadState(const ValueTree& state);

private:
    //==============================================================================
    // Effect Storage
    struct EffectSlot
    {
        std::unique_ptr<AudioModule> effect;
        std::atomic<float> cpuUsage{0.0f};
        std::atomic<bool> isActive{true};
        std::atomic<bool> isBypassed{false};
        float lastRMS = 0.0f;
        int silentFrameCount = 0;
        
        EffectSlot(std::unique_ptr<AudioModule> e) : effect(std::move(e)) {}
    };
    
    std::vector<std::unique_ptr<EffectSlot>> effects;
    
    //==============================================================================
    // Smart Processing
    
    /** Check if buffer is silent */
    bool isSilent(const AudioBuffer<float>& buffer) const;
    
    /** Process single effect with CPU monitoring */
    void processSingleEffect(EffectSlot& slot, AudioBuffer<float>& buffer);
    
    /** Optimize effect chain (auto-bypass silent effects) */
    void optimizeChain();
    
    //==============================================================================
    // Audio Settings
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;
    
    //==============================================================================
    // CPU Management
    std::atomic<float> currentCPUUsage{0.0f};
    std::atomic<int> numActiveEffects{0};
    bool autoOptimizationEnabled = true;
    float silenceThresholdDb = -80.0f;
    float maxCPUUsage = 0.80f; // 80% max CPU
    
    //==============================================================================
    // Parallel Processing
    bool parallelProcessingEnabled = false;
    std::unique_ptr<ThreadPool> threadPool;
    
    //==============================================================================
    // Performance Monitoring
    int64_t processingStartTime = 0;
    
    //==============================================================================
    // Buffer Pool (for efficient memory reuse)
    class BufferPool
    {
    public:
        AudioBuffer<float>* acquire(int numChannels, int numSamples);
        void release(AudioBuffer<float>* buffer);
        
    private:
        std::vector<std::unique_ptr<AudioBuffer<float>>> buffers;
        std::vector<AudioBuffer<float>*> available;
        SpinLock lock;
    };
    
    BufferPool bufferPool;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SmartSignalChain)
};

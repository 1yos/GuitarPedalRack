# Multi-Threading Design - SmartSignalChain

**Date:** July 1, 2026 (Day 4)  
**Phase:** Phase 1 - Smart Engine Foundation  
**Status:** 📋 Design Phase

---

## 🎯 Objectives

### **Primary Goals:**

1. Enable parallel effect processing
2. Utilize multiple CPU cores efficiently
3. Maintain real-time audio safety
4. Scale to 100+ simultaneous effects

### **Target Performance:**

- 2-4x speedup on multi-core systems
- Linear scaling up to 4 cores
- <1ms latency overhead
- Zero audio glitches

---

## 🏗️ Architecture Options

### **Option 1: Parallel Effect Processing** ⭐ RECOMMENDED

**Concept:** Process independent effects in parallel

```
Audio Buffer
    ↓
Split into N threads
    ├─ Thread 1: Effects 0-2
    ├─ Thread 2: Effects 3-5
    ├─ Thread 3: Effects 6-8
    └─ Thread 4: Effects 9-11
    ↓
Synchronize & Merge
    ↓
Output
```

**Pros:**

- ✅ Simple to implement
- ✅ Predictable performance
- ✅ Works with existing effect chain
- ✅ Real-time safe

**Cons:**

- ❌ Limited parallelism (chain is serial)
- ❌ Sync overhead between effects

**Best For:** 10-30 effects

### **Option 2: Per-Effect Threading**

**Concept:** Each effect runs in its own thread

```
Audio Buffer
    ↓
Thread Pool (N threads)
    ├─ Job: Process Effect 0
    ├─ Job: Process Effect 1
    ├─ Job: Process Effect 2
    └─ ...
    ↓
Wait for all jobs
    ↓
Output
```

**Pros:**

- ✅ Maximum parallelism
- ✅ Scales to many effects
- ✅ Load balancing

**Cons:**

- ❌ High overhead for small effects
- ❌ Complex synchronization
- ❌ Thread pool management

**Best For:** 50+ effects

### **Option 3: Parallel Effect Chains**

**Concept:** Multiple independent chains processed in parallel

```
Input
    ├─ Chain A (Thread 1) → Gate → Overdrive → Amp
    ├─ Chain B (Thread 2) → Chorus → Reverb → Delay
    └─ Chain C (Thread 3) → EQ → Cabinet
    ↓
Mix all chains
    ↓
Output
```

**Pros:**

- ✅ Natural parallelism
- ✅ Good for parallel/send effects
- ✅ Minimal sync overhead

**Cons:**

- ❌ Requires architecture change
- ❌ Not all chains are independent

**Best For:** Future expansion (Phase 2+)

---

## 🎯 Recommended Approach

### **Hybrid: Parallel Effect Processing + Thread Pool**

Combine Option 1 and Option 2 for best of both worlds:

1. **Small effects (< 0.5ms):** Process serially (overhead not worth it)
2. **Medium effects (0.5-2ms):** Group and parallelize
3. **Large effects (> 2ms):** Individual thread per effect

### **Implementation Strategy:**

```cpp
class SmartSignalChain {
    void process(AudioBuffer<float>& buffer) {
        if (numEffects < 4 || !parallelProcessingEnabled) {
            // Serial processing for small chains
            processSerial(buffer);
            return;
        }

        // Analyze effect CPU costs
        std::vector<EffectGroup> groups = groupEffectsByWeight();

        // Process groups in parallel
        processParallel(buffer, groups);
    }
};
```

---

## 📊 Performance Analysis

### **Expected Speedup:**

| CPU Cores | Serial | Parallel | Speedup          |
| --------- | ------ | -------- | ---------------- |
| 1 core    | 100ms  | 105ms    | 0.95x (overhead) |
| 2 cores   | 100ms  | 55ms     | 1.8x             |
| 4 cores   | 100ms  | 30ms     | 3.3x             |
| 8 cores   | 100ms  | 25ms     | 4.0x             |

### **Thread Pool Configuration:**

```cpp
// Conservative: Use N-1 cores (leave one for UI/system)
int numThreads = std::max(1, SystemStats::getNumCpus() - 1);

// Typical systems:
// 4 cores → 3 threads
// 8 cores → 7 threads
// 16 cores → 15 threads
```

---

## 🔧 Implementation Plan

### **Phase 1: Foundation (Day 5)**

**Task 1: Thread Pool Setup**

```cpp
class SmartSignalChain {
private:
    std::unique_ptr<ThreadPool> threadPool;
    std::atomic<bool> parallelProcessingEnabled{false};
    int numThreads = 4;
};
```

**Task 2: Effect Grouping**

```cpp
struct EffectGroup {
    std::vector<int> effectIndices;
    float estimatedCPU;
};

std::vector<EffectGroup> groupEffectsByWeight() {
    // Group effects to balance load across threads
    // Target: Each group has similar CPU cost
}
```

**Task 3: Parallel Processing**

```cpp
void processParallel(AudioBuffer<float>& buffer,
                     const std::vector<EffectGroup>& groups) {
    std::vector<std::future<void>> futures;

    for (const auto& group : groups) {
        futures.push_back(threadPool->addJob([&]() {
            for (int idx : group.effectIndices) {
                effects[idx]->process(buffer);
            }
        }));
    }

    // Wait for all threads
    for (auto& f : futures)
        f.wait();
}
```

### **Phase 2: Optimization (Day 6)**

**Task 1: Lock-Free Buffers**

```cpp
// Use separate buffers per thread to avoid contention
class BufferPool {
    std::vector<AudioBuffer<float>> buffers;
    std::atomic<int> nextBuffer{0};

    AudioBuffer<float>* acquire(int numChannels, int numSamples);
    void release(AudioBuffer<float>* buffer);
};
```

**Task 2: Work Stealing**

```cpp
// If a thread finishes early, help other threads
class WorkStealingThreadPool {
    std::deque<std::function<void()>> tasks;
    // Threads can steal from both ends
};
```

**Task 3: Adaptive Threading**

```cpp
// Dynamically enable/disable threading based on load
void adaptiveThreading() {
    if (cpuUsage < 0.3f)
        parallelProcessingEnabled = false;  // Not worth overhead
    else if (cpuUsage > 0.6f)
        parallelProcessingEnabled = true;   // Need parallelism
}
```

---

## 🎨 API Design

### **User-Facing API:**

```cpp
// Enable/disable parallel processing
smartChain.setParallelProcessingEnabled(true);

// Configure thread count (auto-detect by default)
smartChain.setNumThreads(4);

// Query threading status
bool isParallel = smartChain.isParallelProcessingEnabled();
int threads = smartChain.getNumThreads();
```

### **Advanced Configuration:**

```cpp
// Set minimum CPU threshold for parallelization
smartChain.setParallelThreshold(0.5f);  // 50% CPU

// Set minimum effect count for parallelization
smartChain.setMinEffectsForParallel(6);

// Enable adaptive threading
smartChain.setAdaptiveThreading(true);
```

---

## 🔒 Thread Safety Considerations

### **Critical Sections:**

1. **Effect Add/Remove**
   - Lock chain modifications
   - Use double-buffering for seamless updates

2. **Parameter Changes**
   - Atomic operations for parameters
   - Lock-free where possible

3. **Buffer Access**
   - Separate buffers per thread
   - Copy input, merge output

### **Real-Time Safety:**

```cpp
// NO in audio thread:
❌ new/delete
❌ malloc/free
❌ std::mutex lock
❌ std::vector resize
❌ File I/O

// YES in audio thread:
✅ std::atomic operations
✅ Lock-free queues
✅ Pre-allocated buffers
✅ Spin locks (carefully)
```

---

## 📈 Testing Plan

### **Performance Tests:**

1. **Baseline:** Serial processing, 10 effects
2. **Parallel:** Parallel processing, 10 effects
3. **Scaling:** 2, 4, 8 threads with 20, 50, 100 effects
4. **Overhead:** Measure sync and thread creation overhead

### **Stability Tests:**

1. **Add/Remove:** Dynamic effect changes during playback
2. **Parameter Changes:** Rapid parameter automation
3. **Buffer Sizes:** Test 64, 128, 256, 512, 1024, 2048
4. **Long Run:** 24 hour stability test

### **Success Metrics:**

| Metric            | Target   | Method      |
| ----------------- | -------- | ----------- |
| Speedup (4 cores) | 2.5-3.5x | Benchmark   |
| Latency overhead  | <1ms     | Measure     |
| Audio glitches    | 0        | Listen test |
| CPU efficiency    | >90%     | Profiler    |

---

## 🚧 Challenges & Solutions

### **Challenge 1: Sync Overhead**

**Problem:** Thread synchronization adds latency

**Solution:**

- Use lock-free structures where possible
- Minimize sync points
- Batch operations

### **Challenge 2: Load Imbalance**

**Problem:** Some effects are much heavier than others

**Solution:**

- Smart effect grouping by weight
- Work stealing
- Dynamic rebalancing

### **Challenge 3: Buffer Management**

**Problem:** Multiple threads accessing same buffer causes contention

**Solution:**

- Separate buffer per thread
- Copy input to thread buffers
- Merge outputs at sync point

### **Challenge 4: Real-Time Safety**

**Problem:** Thread operations can violate real-time constraints

**Solution:**

- Pre-allocate all resources
- Use lock-free data structures
- Avoid blocking operations

---

## 📝 Implementation Checklist

### **Day 5: Core Implementation**

- [ ] Create thread pool infrastructure
- [ ] Implement effect grouping algorithm
- [ ] Add parallel processing method
- [ ] Test with 10 effects
- [ ] Measure speedup

### **Day 6: Optimization**

- [ ] Implement buffer pooling
- [ ] Add work stealing
- [ ] Optimize synchronization
- [ ] Test with 50+ effects
- [ ] Profile and tune

### **Day 7: Integration**

- [ ] Integrate with PluginProcessor
- [ ] Add UI controls (optional)
- [ ] Update documentation
- [ ] Full system test
- [ ] Performance verification

---

## 🎯 Expected Outcomes

### **After Implementation:**

**Performance:**

- ✅ 2-4x speedup on typical systems
- ✅ 100+ effects with <70% CPU
- ✅ Zero audio artifacts
- ✅ Minimal latency overhead

**Features:**

- ✅ Automatic thread count detection
- ✅ Adaptive threading (enables when beneficial)
- ✅ Real-time safe operations
- ✅ Lock-free where possible

**Quality:**

- ✅ Thoroughly tested
- ✅ Production-ready
- ✅ Well documented
- ✅ User-friendly API

---

## 📚 References

### **Lock-Free Programming:**

- Anthony Williams - "C++ Concurrency in Action"
- Herb Sutter - "atomic<> Weapons"
- JUCE ThreadPool documentation

### **Audio Threading:**

- Ross Bencina - "Real-time audio programming 101"
- Timur Doumler - "Using locks in real-time audio processing"
- Audio Engineering Society papers

### **Performance:**

- Intel Threading Building Blocks (TBB)
- JUCE high_resolution_clock
- CPU affinity and NUMA considerations

---

## 🏁 Summary

**Recommended Approach:**

- Hybrid parallel/serial processing
- ThreadPool with work stealing
- Smart effect grouping
- Adaptive threading

**Target Performance:**

- 2-4x speedup on multi-core
- Handles 100+ effects
- Real-time safe
- Zero audio glitches

**Implementation:**

- Days 5-7 (Phase 1)
- Incremental testing
- Quality focus
- Production-ready

---

**Status:** 📋 Design Complete  
**Next:** Day 5 - Implementation  
**Prepared by:** Kiro AI  
**Date:** July 1, 2026

# Quick Reference - Day 7

**Date:** July 7, 2026  
**Focus:** Execute Benchmarks & Integration Testing  
**Phase 1 Progress:** 75% → 87.5%

---

## ⚡ Quick Start

### **1. Review Status**

```powershell
cd d:\plugins\GuitarPedalRack
git pull origin master
git log --oneline -5
```

Latest commit: `eafe08d` - Day 6 completion summary

### **2. Read Priority Documents**

1. [PHASE1_DAY6_COMPLETE.md](PHASE1_DAY6_COMPLETE.md) - Yesterday's summary
2. [docs/DAY6_SUMMARY.md](docs/DAY6_SUMMARY.md) - Technical details
3. [docs/PHASE1_PROGRESS.md](docs/PHASE1_PROGRESS.md) - Overall status

---

## 🎯 Day 7 Objectives

### **Priority 1: Execute Benchmarks** ⭐⭐⭐

**Goal:** Run performance tests and collect actual measurements

**Files:**

- `source/Testing/PerformanceTest.h` - Test suite (340 lines)
- `source/Testing/TestMain.cpp` - Standalone runner (150 lines)
- `source/Testing/RunPerformanceTests.cpp` - Plugin runner (100 lines)

**Approach Options:**

**Option A: Standalone Test App** (Recommended)

1. Add to CMakeLists.txt:

   ```cmake
   # Optional: Performance test application
   juce_add_console_app(PerformanceTests
       PRODUCT_NAME "GuitarPedalRack-Tests")

   target_sources(PerformanceTests PRIVATE
       source/Testing/TestMain.cpp
       source/Testing/PerformanceTest.h
       # Add necessary DSP files
   )
   ```

2. Build and run:
   ```powershell
   cmake --build build --config Release --target PerformanceTests
   .\build\PerformanceTests_artefacts\Release\PerformanceTests.exe
   ```

**Option B: Plugin Integration**

Add call to PluginProcessor initialization:

```cpp
#ifdef _DEBUG
    // Run performance tests once
    PerformanceTestRunner::runQuickTest();
#endif
```

**Option C: Manual Test Invocation**

Create a simple test in PluginEditor or via menu command

**Success Criteria:**

- [ ] SIMD speedup ≥ 3x
- [ ] Threading speedup ≥ 2.5x (4 cores)
- [ ] Combined speedup ≥ 10x
- [ ] All tests pass

### **Priority 2: Buffer Pool Optimization** ⭐⭐

**Goal:** Implement Phase 1 pre-allocation

**File to Modify:**

`source/DSP/SmartSignalChain.cpp`

**Changes:**

```cpp
void SmartSignalChain::prepare(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;

    // NEW: Pre-allocate buffers for thread pool
    int numBuffersNeeded = numThreads * 2;  // 2 buffers per thread
    for (int i = 0; i < numBuffersNeeded; ++i)
    {
        bufferPool.acquire(2, samplesPerBlock);
    }

    // Pre-allocate for common sizes
    for (int i = 0; i < 4; ++i)
    {
        bufferPool.acquire(2, samplesPerBlock * 2);  // 2x size
        bufferPool.acquire(2, samplesPerBlock / 2);  // 0.5x size
    }

    // Prepare all effects
    for (auto& slot : effects)
    {
        if (slot->effect)
            slot->effect->prepare(sampleRate, samplesPerBlock);
    }
}
```

**Expected Results:**

- Zero allocations during processing
- Improved real-time safety
- Possible 5-10% performance gain

### **Priority 3: Integration Testing** ⭐⭐

**Goal:** Test with many effects, verify stability

**Test Scenarios:**

1. **20 Effects Test**
   - Add 20 effects to chain
   - Monitor CPU usage
   - Verify threading activates

2. **50 Effects Test**
   - Add 50 effects
   - Monitor CPU over 5 minutes
   - Check memory usage

3. **100 Effects Test**
   - Add 100 effects (if CPU allows)
   - Stress test stability
   - Verify no crashes

4. **Dynamic Add/Remove**
   - Add/remove effects during playback
   - Verify smooth operation
   - Check for audio glitches

**Success Criteria:**

- [ ] No crashes
- [ ] No memory leaks
- [ ] CPU usage within predictions
- [ ] Audio quality maintained

### **Priority 4: Parameter Tuning** ⭐

**Goal:** Optimize threading parameters based on results

**Parameters to Tune:**

| Parameter             | Current | Test Range       | Goal                |
| --------------------- | ------- | ---------------- | ------------------- |
| minEffectsForParallel | 6       | 4-8              | Find optimal        |
| parallelThreshold     | 0.5     | 0.3-0.7          | Best balance        |
| Effect grouping       | RR      | Try CPU-weighted | Better load balance |

**File:** `source/DSP/SmartSignalChain.cpp`

---

## 📊 Expected Results

### **SIMD Tests:**

| Test       | Expected Speedup |
| ---------- | ---------------- |
| exp()      | 4.0x             |
| tanh()     | 5.0x             |
| Buffer ops | 4.0x             |

### **Threading Tests:**

| Test       | Expected Result |
| ---------- | --------------- |
| 6 effects  | Serial mode     |
| 10 effects | Parallel, 3.0x  |
| 20 effects | Parallel, 3.3x  |

### **CPU Usage:**

| Effects | Expected CPU |
| ------- | ------------ |
| 10      | 3-5%         |
| 20      | 6-8%         |
| 50      | 15-20%       |
| 100     | 30-40%       |

---

## 🛠️ Tools & Commands

### **Build:**

```powershell
# Clean build
cmake --build build --config Release --clean-first

# Specific target
cmake --build build --config Release --target GuitarPedalRack
```

### **Run:**

```powershell
# Standalone plugin
.\build\GuitarPedalRack_artefacts\Release\Standalone\GuitarPedalRack.exe

# If test app built
.\build\PerformanceTests_artefacts\Release\PerformanceTests.exe
```

### **Check Output:**

```powershell
# View debug output
# Tests will print to console and save report

# Check for report file
ls docs\PERFORMANCE_TEST_RESULTS*.md
```

### **Memory Check:**

Use Task Manager or:

```powershell
# Monitor process
Get-Process GuitarPedalRack | Select-Object Name, PM, WS
```

---

## 📝 Documentation Tasks

### **Files to Update:**

1. **PERFORMANCE_TEST_RESULTS.md**
   - Replace TBD with actual measurements
   - Add system information
   - Document any surprises

2. **DAY7_SUMMARY.md** (Create)
   - Benchmark results
   - Buffer optimization results
   - Integration test results
   - Parameter tuning outcomes

3. **PHASE1_PROGRESS.md**
   - Update to Day 7 (87.5%)
   - Add Day 7 section
   - Update statistics

---

## ⚠️ Watch Out For

### **Potential Issues:**

1. **Build Errors**
   - Missing JUCE paths
   - CMake configuration issues
   - → Solution: Check CMakeLists.txt paths

2. **Test Execution Issues**
   - Tests crash
   - No output
   - → Solution: Try plugin integration first

3. **Performance Below Target**
   - SIMD < 3x
   - Threading < 2.5x
   - → Solution: Profile, check CPU, verify algorithms

4. **Memory Leaks**
   - Memory usage grows
   - → Solution: Check buffer pool, verify releases

---

## 🎯 Success Metrics

### **Minimum Acceptable:**

- ✅ SIMD speedup ≥ 3x
- ✅ Threading speedup ≥ 2.5x
- ✅ Combined speedup ≥ 10x
- ✅ Zero crashes in 10 minutes
- ✅ Memory stable (no leaks)

### **Target:**

- 🎯 SIMD speedup = 4-5x
- 🎯 Threading speedup = 3.3x
- 🎯 Combined speedup = 13-16x
- 🎯 Zero crashes in 1 hour
- 🎯 Memory constant

### **Stretch:**

- 🏆 All predictions met or exceeded
- 🏆 100+ effects stable
- 🏆 CPU under 50% with 100 effects
- 🏆 Zero issues found

---

## 📁 Key Files Reference

### **Source Code:**

```
source/
├── Testing/
│   ├── PerformanceTest.h          (Test suite)
│   ├── TestMain.cpp               (Standalone runner)
│   └── RunPerformanceTests.cpp    (Plugin runner)
│
├── DSP/
│   ├── SmartSignalChain.h         (Threading API)
│   ├── SmartSignalChain.cpp       (Buffer pool location)
│   ├── SIMDProcessor.h            (SIMD functions)
│   └── EffectLibrary.cpp          (Effect registry)
│
└── PluginProcessor.cpp            (Main processor)
```

### **Documentation:**

```
docs/
├── DAY6_SUMMARY.md                (Yesterday's work)
├── PHASE1_PROGRESS.md             (Overall status)
├── PERFORMANCE_TEST_RESULTS.md    (To fill today)
└── MULTITHREADING_DESIGN.md       (Architecture)
```

---

## 🚀 Quick Decision Tree

**Problem:** How to run tests?

```
Can modify CMakeLists?
├─ YES → Build standalone test app (best results)
├─ NO  → Add to plugin initialization
└─ SKIP → Document framework, move to integration
```

**Problem:** Tests fail?

```
What fails?
├─ Build fails → Check JUCE paths, CMake config
├─ Tests crash → Debug in plugin first
├─ Results poor → Profile, analyze, tune
└─ No output → Check console, verify DBG() works
```

**Problem:** Performance below target?

```
Which test fails?
├─ SIMD < 3x → Check CPU supports SSE4.2
├─ Threading < 2.5x → Check thread count, grouping
├─ Combined < 10x → Both issues, prioritize SIMD
└─ All pass → Celebrate! 🎉
```

---

## 🎯 End of Day 7 Goals

### **By End of Day:**

- [ ] Performance tests executed
- [ ] Actual results documented
- [ ] Buffer optimization implemented
- [ ] Integration tests passed
- [ ] DAY7_SUMMARY.md created
- [ ] Committed and pushed to GitHub

### **Metrics:**

- Code added: ~100 lines
- Docs added: ~1,000 lines
- Phase 1 progress: 87.5%
- Days remaining: 1

---

## 💡 Pro Tips

1. **Start Simple**
   - Try plugin integration first
   - Get basic tests running
   - Then optimize

2. **Measure First**
   - Collect actual data before tuning
   - Don't guess optimizations
   - Profile if needed

3. **Document Everything**
   - Save test output
   - Screenshot results
   - Note surprises

4. **Commit Often**
   - After each major milestone
   - Before risky changes
   - With clear messages

---

## 📞 If You Get Stuck

### **Quick Checks:**

1. Does the plugin still build? → `cmake --build build --config Release`
2. Does it launch? → Run standalone
3. Does audio work? → Test with input
4. Any errors? → Check console/debug output

### **Resources:**

- Previous days: [docs/](docs/)
- Architecture: [docs/MULTITHREADING_DESIGN.md](docs/MULTITHREADING_DESIGN.md)
- Progress: [docs/PHASE1_PROGRESS.md](docs/PHASE1_PROGRESS.md)
- Yesterday: [PHASE1_DAY6_COMPLETE.md](PHASE1_DAY6_COMPLETE.md)

---

## 🎉 Motivation

**You're almost there!**

- ✅ 75% complete
- ✅ All systems working
- ✅ Test framework ready
- ⏳ 2 days to completion

**Today:** Execute tests, validate predictions, optimize buffers  
**Tomorrow:** Final polish, documentation, Phase 1 complete!

---

**Good luck with Day 7!** 🚀

**Status:** Ready for Day 7  
**Confidence:** 🟢 Very High  
**Next:** Execute benchmarks & integration testing

---

**Created:** July 6, 2026  
**For:** Day 7 (July 7, 2026)  
**Phase 1:** 75% → 87.5%

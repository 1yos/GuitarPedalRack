#pragma once

#include "../JuceHeader.h"
#include "../DSP/SmartSignalChain.h"
#include "../DSP/EffectLibrary.h"
#include "../DSP/SIMDProcessor.h"
#include <chrono>
#include <vector>

using namespace juce;

//==============================================================================
/**
 * Performance Testing Suite
 * 
 * Tests:
 * - SIMD speedup verification
 * - SmartSignalChain CPU optimization
 * - Silence detection efficiency
 * - Effect scaling (10, 50, 100 effects)
 */
class PerformanceTest
{
public:
    //==============================================================================
    struct TestResult
    {
        String testName;
        double duration;        // milliseconds
        double cpuUsage;        // percentage
        int numEffects;
        int activeEffects;
        bool passed;
        String notes;
    };
    
    //==============================================================================
    // SIMD Performance Tests
    
    /** Test SIMD vs standard exp() */
    static TestResult testSIMDExp()
    {
        TestResult result;
        result.testName = "SIMD exp() Performance";
        
        const int iterations = 1000000;
        std::vector<float> testValues(iterations);
        
        // Generate test data
        Random rng;
        for (int i = 0; i < iterations; ++i)
            testValues[i] = rng.nextFloat() * 10.0f - 5.0f;
        
        // Test standard exp
        auto startStd = std::chrono::high_resolution_clock::now();
        float sumStd = 0.0f;
        for (int i = 0; i < iterations; ++i)
            sumStd += std::exp(testValues[i]);
        auto endStd = std::chrono::high_resolution_clock::now();
        auto durationStd = std::chrono::duration<double, std::milli>(endStd - startStd).count();
        
        // Test SIMD exp
        auto startSIMD = std::chrono::high_resolution_clock::now();
        float sumSIMD = 0.0f;
        for (int i = 0; i < iterations; ++i)
            sumSIMD += SIMDProcessor::fastExp(testValues[i]);
        auto endSIMD = std::chrono::high_resolution_clock::now();
        auto durationSIMD = std::chrono::duration<double, std::milli>(endSIMD - startSIMD).count();
        
        double speedup = durationStd / durationSIMD;
        result.duration = durationSIMD;
        result.passed = speedup >= 3.0; // Should be at least 3x faster
        result.notes = String::formatted("Speedup: %.2fx (Std: %.2fms, SIMD: %.2fms)", 
                                        speedup, durationStd, durationSIMD);
        
        return result;
    }
    
    /** Test SIMD vs standard tanh() */
    static TestResult testSIMDTanh()
    {
        TestResult result;
        result.testName = "SIMD tanh() Performance";
        
        const int iterations = 1000000;
        std::vector<float> testValues(iterations);
        
        Random rng;
        for (int i = 0; i < iterations; ++i)
            testValues[i] = rng.nextFloat() * 6.0f - 3.0f;
        
        // Test standard tanh
        auto startStd = std::chrono::high_resolution_clock::now();
        float sumStd = 0.0f;
        for (int i = 0; i < iterations; ++i)
            sumStd += std::tanh(testValues[i]);
        auto endStd = std::chrono::high_resolution_clock::now();
        auto durationStd = std::chrono::duration<double, std::milli>(endStd - startStd).count();
        
        // Test SIMD tanh
        auto startSIMD = std::chrono::high_resolution_clock::now();
        float sumSIMD = 0.0f;
        for (int i = 0; i < iterations; ++i)
            sumSIMD += SIMDProcessor::fastTanh(testValues[i]);
        auto endSIMD = std::chrono::high_resolution_clock::now();
        auto durationSIMD = std::chrono::duration<double, std::milli>(endSIMD - startSIMD).count();
        
        double speedup = durationStd / durationSIMD;
        result.duration = durationSIMD;
        result.passed = speedup >= 3.0;
        result.notes = String::formatted("Speedup: %.2fx (Std: %.2fms, SIMD: %.2fms)", 
                                        speedup, durationStd, durationSIMD);
        
        return result;
    }
    
    /** Test SIMD buffer operations */
    static TestResult testSIMDBufferOps()
    {
        TestResult result;
        result.testName = "SIMD Buffer Operations";
        
        const int bufferSize = 512;
        const int iterations = 10000;
        
        AudioBuffer<float> buffer(2, bufferSize);
        
        // Fill with test data
        Random rng;
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < bufferSize; ++i)
                buffer.setSample(ch, i, rng.nextFloat() * 2.0f - 1.0f);
        
        // Test SIMD gain
        auto start = std::chrono::high_resolution_clock::now();
        for (int iter = 0; iter < iterations; ++iter)
        {
            for (int ch = 0; ch < 2; ++ch)
                SIMDProcessor::applyGain(buffer.getWritePointer(ch), bufferSize, 0.5f);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();
        
        result.duration = duration;
        result.passed = duration < 100.0; // Should be fast
        result.notes = String::formatted("%.2f ms for %d iterations", duration, iterations);
        
        return result;
    }
    
    //==============================================================================
    // SmartSignalChain Tests
    
    /** Test basic chain performance (6 effects) */
    static TestResult testBasicChain()
    {
        TestResult result;
        result.testName = "Basic Chain (6 effects)";
        result.numEffects = 6;
        
        // Create chain with default effects
        SmartSignalChain chain;
        auto& lib = EffectLibrary::getInstance();
        
        chain.addEffect(lib.createEffect("noiseGate"));
        chain.addEffect(lib.createEffect("tubeOverdrive"));
        chain.addEffect(lib.createEffect("chorus"));
        chain.addEffect(lib.createEffect("ampSimulator"));
        chain.addEffect(lib.createEffect("reverb"));
        chain.addEffect(lib.createEffect("cabinetIR"));
        
        chain.prepare(44100.0, 512);
        
        // Process test audio
        AudioBuffer<float> buffer(2, 512);
        Random rng;
        
        // Fill with noise
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < 512; ++i)
                buffer.setSample(ch, i, rng.nextFloat() * 0.5f - 0.25f);
        
        // Measure processing time
        const int iterations = 1000;
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i)
            chain.process(buffer);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();
        
        result.duration = duration;
        result.activeEffects = chain.getNumActiveEffects();
        result.cpuUsage = chain.getCPUUsage() * 100.0;
        result.passed = duration < 500.0; // Should process 1000 blocks in <500ms
        result.notes = String::formatted("%.2f ms for %d iterations, %.1f%% CPU, %d/%d active",
                                        duration, iterations, result.cpuUsage, 
                                        result.activeEffects, result.numEffects);
        
        return result;
    }
    
    /** Test silence detection and auto-bypass */
    static TestResult testSilenceOptimization()
    {
        TestResult result;
        result.testName = "Silence Detection & Auto-Bypass";
        result.numEffects = 6;
        
        SmartSignalChain chain;
        auto& lib = EffectLibrary::getInstance();
        
        chain.addEffect(lib.createEffect("noiseGate"));
        chain.addEffect(lib.createEffect("tubeOverdrive"));
        chain.addEffect(lib.createEffect("chorus"));
        chain.addEffect(lib.createEffect("ampSimulator"));
        chain.addEffect(lib.createEffect("reverb"));
        chain.addEffect(lib.createEffect("cabinetIR"));
        
        chain.prepare(44100.0, 512);
        
        AudioBuffer<float> buffer(2, 512);
        
        // First, process with signal
        Random rng;
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < 512; ++i)
                buffer.setSample(ch, i, rng.nextFloat() * 0.5f - 0.25f);
        
        for (int i = 0; i < 50; ++i)
            chain.process(buffer);
        
        int activeWithSignal = chain.getNumActiveEffects();
        
        // Now process silence
        buffer.clear();
        
        for (int i = 0; i < 50; ++i)
            chain.process(buffer);
        
        int activeWithSilence = chain.getNumActiveEffects();
        
        result.activeEffects = activeWithSilence;
        result.passed = activeWithSilence < activeWithSignal; // Should auto-bypass some effects
        result.notes = String::formatted("Active: %d with signal → %d with silence (bypassed %d)",
                                        activeWithSignal, activeWithSilence, 
                                        activeWithSignal - activeWithSilence);
        
        return result;
    }
    
    /** Test with 10 effects */
    static TestResult testScaling10Effects()
    {
        TestResult result;
        result.testName = "Scaling Test (10 effects)";
        result.numEffects = 10;
        
        SmartSignalChain chain;
        auto& lib = EffectLibrary::getInstance();
        
        // Add 10 effects
        chain.addEffect(lib.createEffect("noiseGate"));
        chain.addEffect(lib.createEffect("compressor"));
        chain.addEffect(lib.createEffect("tubeOverdrive"));
        chain.addEffect(lib.createEffect("distortion"));
        chain.addEffect(lib.createEffect("chorus"));
        chain.addEffect(lib.createEffect("delay"));
        chain.addEffect(lib.createEffect("ampSimulator"));
        chain.addEffect(lib.createEffect("reverb"));
        chain.addEffect(lib.createEffect("parametricEQ"));
        chain.addEffect(lib.createEffect("cabinetIR"));
        
        chain.prepare(44100.0, 512);
        
        AudioBuffer<float> buffer(2, 512);
        Random rng;
        
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < 512; ++i)
                buffer.setSample(ch, i, rng.nextFloat() * 0.5f - 0.25f);
        
        const int iterations = 1000;
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i)
            chain.process(buffer);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();
        
        result.duration = duration;
        result.activeEffects = chain.getNumActiveEffects();
        result.cpuUsage = chain.getCPUUsage() * 100.0;
        result.passed = duration < 1000.0; // Should handle 10 effects reasonably
        result.notes = String::formatted("%.2f ms for %d iterations, %.1f%% CPU, %d/%d active",
                                        duration, iterations, result.cpuUsage,
                                        result.activeEffects, result.numEffects);
        
        return result;
    }
    
    //==============================================================================
    // Test Suite Runner
    
    static std::vector<TestResult> runAllTests()
    {
        std::vector<TestResult> results;
        
        DBG("=== Running Performance Test Suite ===");
        
        // SIMD tests
        DBG("\n--- SIMD Performance Tests ---");
        results.push_back(testSIMDExp());
        results.push_back(testSIMDTanh());
        results.push_back(testSIMDBufferOps());
        
        // SmartSignalChain tests
        DBG("\n--- SmartSignalChain Tests ---");
        results.push_back(testBasicChain());
        results.push_back(testSilenceOptimization());
        results.push_back(testScaling10Effects());
        
        // Print results
        DBG("\n=== Test Results ===");
        int passed = 0;
        for (const auto& result : results)
        {
            String status = result.passed ? "[PASS]" : "[FAIL]";
            DBG(status + " " + result.testName);
            DBG("    " + result.notes);
            if (result.passed) passed++;
        }
        
        DBG(String::formatted("\n=== Summary: %d/%d tests passed ===", 
                             passed, (int)results.size()));
        
        return results;
    }
    
    /** Format results as markdown table */
    static String resultsToMarkdown(const std::vector<TestResult>& results)
    {
        String markdown;
        markdown << "# Performance Test Results\n\n";
        markdown << "**Date:** " << Time::getCurrentTime().toString(true, true) << "\n";
        markdown << "**CPU:** " << SystemStats::getCpuModel() << "\n";
        markdown << "**Cores:** " << SystemStats::getNumCpus() << "\n\n";
        
        markdown << "## Test Summary\n\n";
        markdown << "| Test | Duration (ms) | CPU % | Effects | Active | Status | Notes |\n";
        markdown << "|------|--------------|-------|---------|--------|--------|-------|\n";
        
        for (const auto& result : results)
        {
            markdown << "| " << result.testName << " | ";
            markdown << String::formatted("%.2f", result.duration) << " | ";
            markdown << String::formatted("%.1f", result.cpuUsage) << " | ";
            markdown << result.numEffects << " | ";
            markdown << result.activeEffects << " | ";
            markdown << (result.passed ? "✅ PASS" : "❌ FAIL") << " | ";
            markdown << result.notes << " |\n";
        }
        
        return markdown;
    }
};

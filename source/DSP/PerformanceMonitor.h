#pragma once

#include "../JuceHeader.h"
#include <chrono>
#include <vector>
#include <string>

using namespace juce;

//==============================================================================
/**
 * Performance monitoring and profiling for DSP modules
 * 
 * Tracks:
 * - CPU usage per module
 * - Total processing time
 * - Average/Peak/Current load
 * - Real-time performance metrics
 */
class PerformanceMonitor
{
public:
    struct ModuleStats
    {
        String moduleName;
        double averageTimeMs = 0.0;
        double peakTimeMs = 0.0;
        double currentTimeMs = 0.0;
        double cpuPercentage = 0.0;
        int sampleCount = 0;
    };
    
    PerformanceMonitor() = default;
    ~PerformanceMonitor() = default;
    
    //==============================================================================
    // Module Registration
    
    /** Registers a module for monitoring */
    int registerModule(const String& moduleName)
    {
        ModuleStats stats;
        stats.moduleName = moduleName;
        moduleStats.push_back(stats);
        return static_cast<int>(moduleStats.size()) - 1;
    }
    
    /** Unregisters all modules */
    void clear()
    {
        moduleStats.clear();
    }
    
    //==============================================================================
    // Timing Measurement
    
    /** Starts timing for a module */
    void startTiming(int moduleIndex)
    {
        if (moduleIndex >= 0 && moduleIndex < (int)moduleStats.size())
        {
            startTimes[moduleIndex] = std::chrono::high_resolution_clock::now();
        }
    }
    
    /** Stops timing for a module and records the duration */
    void stopTiming(int moduleIndex, int numSamples, double sampleRate)
    {
        if (moduleIndex < 0 || moduleIndex >= (int)moduleStats.size())
            return;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            endTime - startTimes[moduleIndex]
        );
        
        double timeMs = duration.count() / 1000.0;
        auto& stats = moduleStats[moduleIndex];
        
        // Update current time
        stats.currentTimeMs = timeMs;
        
        // Update peak time
        if (timeMs > stats.peakTimeMs)
            stats.peakTimeMs = timeMs;
        
        // Update running average (exponential moving average)
        const double alpha = 0.05;  // Smoothing factor
        if (stats.sampleCount == 0)
            stats.averageTimeMs = timeMs;
        else
            stats.averageTimeMs = alpha * timeMs + (1.0 - alpha) * stats.averageTimeMs;
        
        ++stats.sampleCount;
        
        // Calculate CPU percentage
        double blockTimeMs = (numSamples / sampleRate) * 1000.0;
        stats.cpuPercentage = (timeMs / blockTimeMs) * 100.0;
    }
    
    //==============================================================================
    // Statistics Access
    
    /** Gets stats for a specific module */
    const ModuleStats* getModuleStats(int moduleIndex) const
    {
        if (moduleIndex >= 0 && moduleIndex < (int)moduleStats.size())
            return &moduleStats[moduleIndex];
        return nullptr;
    }
    
    /** Gets total CPU usage across all modules */
    double getTotalCpuUsage() const
    {
        double total = 0.0;
        for (const auto& stats : moduleStats)
            total += stats.cpuPercentage;
        return total;
    }
    
    /** Gets average CPU usage */
    double getAverageCpuUsage() const
    {
        if (moduleStats.empty())
            return 0.0;
        return getTotalCpuUsage() / moduleStats.size();
    }
    
    /** Gets total processing time for all modules */
    double getTotalProcessingTime() const
    {
        double total = 0.0;
        for (const auto& stats : moduleStats)
            total += stats.currentTimeMs;
        return total;
    }
    
    /** Resets all peak values */
    void resetPeaks()
    {
        for (auto& stats : moduleStats)
            stats.peakTimeMs = 0.0;
    }
    
    /** Resets all statistics */
    void resetAll()
    {
        for (auto& stats : moduleStats)
        {
            stats.averageTimeMs = 0.0;
            stats.peakTimeMs = 0.0;
            stats.currentTimeMs = 0.0;
            stats.cpuPercentage = 0.0;
            stats.sampleCount = 0;
        }
    }
    
    //==============================================================================
    // Reporting
    
    /** Generates a performance report string */
    String generateReport() const
    {
        String report;
        report << "=== Performance Report ===\n\n";
        report << "Total CPU Usage: " << String(getTotalCpuUsage(), 2) << "%\n";
        report << "Total Processing Time: " << String(getTotalProcessingTime(), 3) << " ms\n\n";
        
        report << "Per-Module Stats:\n";
        report << String::repeatedString("-", 70) << "\n";
        report << String("Module").paddedRight(' ', 25)
               << String("Current").paddedRight(' ', 12)
               << String("Average").paddedRight(' ', 12)
               << String("Peak").paddedRight(' ', 12)
               << String("CPU%").paddedRight(' ', 9) << "\n";
        report << String::repeatedString("-", 70) << "\n";
        
        for (const auto& stats : moduleStats)
        {
            report << stats.moduleName.paddedRight(' ', 25)
                   << String(stats.currentTimeMs, 3).paddedRight(' ', 12)
                   << String(stats.averageTimeMs, 3).paddedRight(' ', 12)
                   << String(stats.peakTimeMs, 3).paddedRight(' ', 12)
                   << String(stats.cpuPercentage, 2).paddedRight(' ', 9) << "\n";
        }
        
        return report;
    }
    
    /** Logs the performance report to console */
    void logReport() const
    {
        DBG(generateReport());
    }

private:
    std::vector<ModuleStats> moduleStats;
    std::unordered_map<int, std::chrono::high_resolution_clock::time_point> startTimes;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PerformanceMonitor)
};

//==============================================================================
/**
 * RAII-style scoped timer for easy performance measurement
 * 
 * Usage:
 *   {
 *       ScopedPerformanceTimer timer(monitor, moduleIndex, numSamples, sampleRate);
 *       // ... process audio ...
 *   }  // Automatically stops timing when scope exits
 */
class ScopedPerformanceTimer
{
public:
    ScopedPerformanceTimer(PerformanceMonitor& monitor, int moduleIndex,
                          int numSamples, double sampleRate)
        : monitor(monitor)
        , moduleIndex(moduleIndex)
        , numSamples(numSamples)
        , sampleRate(sampleRate)
    {
        monitor.startTiming(moduleIndex);
    }
    
    ~ScopedPerformanceTimer()
    {
        monitor.stopTiming(moduleIndex, numSamples, sampleRate);
    }

private:
    PerformanceMonitor& monitor;
    int moduleIndex;
    int numSamples;
    double sampleRate;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopedPerformanceTimer)
};

#include "PerformanceTest.h"
#include "../JuceHeader.h"

//==============================================================================
/**
 * Performance test runner - can be called from plugin initialization
 * or via menu command for benchmarking
 * 
 * This version writes results to the debug console and saves a report file
 */

namespace PerformanceTestRunner
{
    /**
     * Run all performance tests and save results to docs folder
     * Returns true if all tests passed
     */
    static bool runAndSaveResults()
    {
        DBG("========================================");
        DBG("  Guitar Pedal Rack - Performance Tests");
        DBG("========================================");
        DBG("");
        
        // System information
        DBG("System Information:");
        DBG("  CPU: " + SystemStats::getCpuModel());
        DBG("  Cores: " + String(SystemStats::getNumCpus()));
        DBG("  RAM: " + String(SystemStats::getMemorySizeInMegabytes()) + " MB");
        DBG("  OS: " + SystemStats::getOperatingSystemName());
        DBG("");
        
        // SIMD information
        DBG("SIMD Support:");
        DBG("  " + SIMDProcessor::getSIMDInfo());
        DBG("");
        
        // Run all tests
        auto results = PerformanceTest::runAllTests();
        
        DBG("");
        DBG("========================================");
        DBG("  Results Summary");
        DBG("========================================");
        DBG("");
        
        int passed = 0;
        int failed = 0;
        
        for (const auto& result : results)
        {
            if (result.passed)
                passed++;
            else
                failed++;
        }
        
        DBG("Total Tests: " + String(results.size()));
        DBG("Passed: " + String(passed));
        DBG("Failed: " + String(failed));
        DBG("");
        
        if (failed == 0)
        {
            DBG("✅ ALL TESTS PASSED!");
        }
        else
        {
            DBG("❌ SOME TESTS FAILED");
        }
        
        DBG("");
        
        // Generate markdown report
        String markdownReport = PerformanceTest::resultsToMarkdown(results);
        
        // Try to save to docs folder
        File docsFolder = File::getSpecialLocation(File::currentExecutableFile)
                              .getParentDirectory()
                              .getChildFile("docs");
        
        if (!docsFolder.exists())
        {
            // Try relative path from working directory
            docsFolder = File::getCurrentWorkingDirectory().getChildFile("docs");
        }
        
        File reportFile = docsFolder.getChildFile("PERFORMANCE_TEST_RESULTS.md");
        
        if (reportFile.replaceWithText(markdownReport))
        {
            DBG("📄 Report saved to: " + reportFile.getFullPathName());
        }
        else
        {
            DBG("⚠️  Could not save report file to: " + reportFile.getFullPathName());
            DBG("");
            DBG("Markdown Report:");
            DBG("================");
            DBG(markdownReport);
        }
        
        DBG("");
        
        return (failed == 0);
    }
    
    /**
     * Quick test - runs a subset of tests for faster validation
     */
    static bool runQuickTest()
    {
        DBG("Running quick performance test...");
        
        auto results = std::vector<PerformanceTest::TestResult>();
        
        // Run just essential tests
        results.push_back(PerformanceTest::testSIMDExp());
        results.push_back(PerformanceTest::testBasicChain());
        
        bool allPassed = true;
        for (const auto& result : results)
        {
            DBG((result.passed ? "[PASS] " : "[FAIL] ") + result.testName);
            if (!result.passed)
                allPassed = false;
        }
        
        return allPassed;
    }
}

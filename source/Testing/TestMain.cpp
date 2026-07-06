//==============================================================================
/**
 * Simple test main - Minimal JUCE application to run performance tests
 * 
 * Compile and run this to benchmark the Guitar Pedal Rack performance
 */

#include "../JuceHeader.h"
#include "PerformanceTest.h"
#include <iostream>

//==============================================================================
class PerformanceTestApplication : public JUCEApplication
{
public:
    PerformanceTestApplication() {}

    const String getApplicationName() override { return "GuitarPedalRackTests"; }
    const String getApplicationVersion() override { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const String& commandLine) override
    {
        std::cout << "\n";
        std::cout << "========================================\n";
        std::cout << "  Guitar Pedal Rack - Performance Tests\n";
        std::cout << "========================================\n\n";
        
        // System information
        std::cout << "System Information:\n";
        std::cout << "  CPU: " << SystemStats::getCpuModel().toStdString() << "\n";
        std::cout << "  Cores: " << SystemStats::getNumCpus() << "\n";
        std::cout << "  RAM: " << SystemStats::getMemorySizeInMegabytes() << " MB\n";
        std::cout << "  OS: " << SystemStats::getOperatingSystemName().toStdString() << "\n";
        std::cout << "\n";
        
        // SIMD information
        std::cout << "SIMD Support:\n";
        std::cout << "  " << SIMDProcessor::getSIMDInfo().toStdString() << "\n";
        std::cout << "\n";
        
        // Run all tests
        std::cout << "Running performance tests...\n\n";
        auto results = PerformanceTest::runAllTests();
        
        std::cout << "\n";
        std::cout << "========================================\n";
        std::cout << "  Results Summary\n";
        std::cout << "========================================\n\n";
        
        int passed = 0;
        int failed = 0;
        
        for (const auto& result : results)
        {
            if (result.passed)
                passed++;
            else
                failed++;
            
            std::cout << (result.passed ? "[PASS] " : "[FAIL] ") 
                      << result.testName.toStdString() << "\n";
            std::cout << "  " << result.notes.toStdString() << "\n";
        }
        
        std::cout << "\n";
        std::cout << "Total Tests: " << results.size() << "\n";
        std::cout << "Passed: " << passed << "\n";
        std::cout << "Failed: " << failed << "\n";
        std::cout << "\n";
        
        if (failed == 0)
        {
            std::cout << "✅ ALL TESTS PASSED!\n";
        }
        else
        {
            std::cout << "❌ SOME TESTS FAILED\n";
        }
        
        std::cout << "\n";
        
        // Generate markdown report
        String markdownReport = PerformanceTest::resultsToMarkdown(results);
        
        // Save to docs folder
        File reportFile = File::getCurrentWorkingDirectory()
                             .getParentDirectory()
                             .getParentDirectory()
                             .getChildFile("docs/PERFORMANCE_TEST_RESULTS_ACTUAL.md");
        
        if (reportFile.replaceWithText(markdownReport))
        {
            std::cout << "📄 Report saved to: " << reportFile.getFullPathName().toStdString() << "\n";
        }
        else
        {
            // Try alternative path
            reportFile = File::getCurrentWorkingDirectory().getChildFile("PERFORMANCE_TEST_RESULTS_ACTUAL.md");
            if (reportFile.replaceWithText(markdownReport))
            {
                std::cout << "📄 Report saved to: " << reportFile.getFullPathName().toStdString() << "\n";
            }
            else
            {
                std::cout << "⚠️  Could not save report file\n";
            }
        }
        
        std::cout << "\n";
        std::cout << "Press Enter to exit...\n";
        std::cin.get();
        
        quit();
    }

    void shutdown() override
    {
        // Clean shutdown
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const String& commandLine) override
    {
    }
};

//==============================================================================
START_JUCE_APPLICATION(PerformanceTestApplication)

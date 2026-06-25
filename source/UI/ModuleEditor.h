#pragma once

#include "../JuceHeader.h"

using namespace juce;

//==============================================================================
/**
 * Editor for individual module parameters
 */
class ModuleEditor : public Component
{
public:
    ModuleEditor();
    ~ModuleEditor() override = default;

    void paint(Graphics& g) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModuleEditor)
};

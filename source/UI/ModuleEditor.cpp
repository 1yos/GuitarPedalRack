#include "ModuleEditor.h"

ModuleEditor::ModuleEditor()
{
    setSize(400, 300);
}

void ModuleEditor::paint(Graphics& g)
{
    g.fillAll(Colours::black);
    
    g.setColour(Colours::white);
    g.setFont(16.0f);
    g.drawText("Module Editor", getLocalBounds(), Justification::centred);
}

void ModuleEditor::resized()
{
    // TODO: Layout parameter controls
}

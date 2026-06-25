#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Modern Look and Feel class for styling
class ModernLookAndFeel : public LookAndFeel_V4
{
public:
    ModernLookAndFeel()
    {
        setColour(ResizableWindow::backgroundColourId, Colour(0xff0d0d0d));
        setColour(Slider::thumbColourId, Colour(0xff00d4ff));
        setColour(Slider::trackColourId, Colour(0xff2a2a2a));
        setColour(Slider::backgroundColourId, Colour(0xff1a1a1a));
        setColour(TextButton::buttonColourId, Colour(0xff2a2a2a));
        setColour(TextButton::textColourOffId, Colours::white);
        setColour(ComboBox::backgroundColourId, Colour(0xff2a2a2a));
        setColour(ComboBox::outlineColourId, Colour(0xff00d4ff));
    }
    
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height,
                         float sliderPosProportional, float rotaryStartAngle,
                         float rotaryEndAngle, Slider& slider) override
    {
        auto bounds = Rectangle<float>(x, y, width, height).reduced(10);
        auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY();
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        
        // Background circle
        g.setColour(Colour(0xff2a2a2a));
        g.fillEllipse(rx, ry, rw, rw);
        
        // Track arc
        Path backgroundArc;
        backgroundArc.addCentredArc(centreX, centreY, radius, radius,
                                   0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(Colour(0xff1a1a1a));
        g.strokePath(backgroundArc, PathStrokeType(3.0f));
        
        // Value arc (cyan glow)
        Path valueArc;
        valueArc.addCentredArc(centreX, centreY, radius, radius,
                              0.0f, rotaryStartAngle, angle, true);
        g.setColour(Colour(0xff00d4ff));
        g.strokePath(valueArc, PathStrokeType(3.0f));
        
        // Pointer
        Path pointer;
        auto pointerLength = radius * 0.6f;
        auto pointerThickness = 3.0f;
        pointer.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        g.setColour(Colour(0xff00d4ff));
        g.fillPath(pointer, AffineTransform::rotation(angle).translated(centreX, centreY));
        
        // Center dot
        g.setColour(Colour(0xff0d0d0d));
        g.fillEllipse(centreX - 4, centreY - 4, 8, 8);
    }
};

//==============================================================================
GuitarPedalRackEditor::GuitarPedalRackEditor(GuitarPedalRackProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(1200, 750);
    setResizable(true, true);
    setResizeLimits(1000, 650, 1920, 1080);
    
    // Set modern look
    setLookAndFeel(&modernLookAndFeel);
    
    // Top bar - Title
    addAndMakeVisible(titleLabel);
    titleLabel.setText("GUITAR PEDAL RACK", dontSendNotification);
    titleLabel.setFont(Font(22.0f, Font::bold));
    titleLabel.setJustificationType(Justification::centredLeft);
    titleLabel.setColour(Label::textColourId, Colour(0xff00d4ff));
    
    // Top bar - Save preset button
    addAndMakeVisible(savePresetButton);
    savePresetButton.setButtonText("Save Preset");
    savePresetButton.setColour(TextButton::buttonColourId, Colour(0xff00d4ff));
    savePresetButton.setColour(TextButton::textColourOffId, Colour(0xff0d0d0d));
    
    // Sidebar (Left panel)
    addAndMakeVisible(sidebarPanel);
    
    // Browser title in sidebar
    addAndMakeVisible(browserLabel);
    browserLabel.setText("Browser", dontSendNotification);
    browserLabel.setFont(Font(20.0f, Font::bold));
    browserLabel.setJustificationType(Justification::centredLeft);
    browserLabel.setColour(Label::textColourId, Colours::white);
    
    addAndMakeVisible(advancedControlsLabel);
    advancedControlsLabel.setText("ADVANCED CONTROLS", dontSendNotification);
    advancedControlsLabel.setFont(Font(10.0f));
    advancedControlsLabel.setJustificationType(Justification::centredLeft);
    advancedControlsLabel.setColour(Label::textColourId, Colour(0xff666666));
    
    // Category buttons
    addAndMakeVisible(presetsButton);
    presetsButton.setButtonText("📁 Presets");
    presetsButton.setColour(TextButton::buttonColourId, Colour(0xffffaa00));
    presetsButton.setColour(TextButton::textColourOffId, Colour(0xff0d0d0d));
    
    addAndMakeVisible(rigGenButton);
    rigGenButton.setButtonText("⚡ Rig Gen");
    rigGenButton.setColour(TextButton::buttonColourId, Colour(0xff2a2a2a));
    
    addAndMakeVisible(libraryButton);
    libraryButton.setButtonText("📚 Library");  
    libraryButton.setColour(TextButton::buttonColourId, Colour(0xff2a2a2a));
    
    addAndMakeVisible(historyButton);
    historyButton.setButtonText("🕐 History");
    historyButton.setColour(TextButton::buttonColourId, Colour(0xff2a2a2a));
    
    // Generate button at bottom of sidebar
    addAndMakeVisible(generateButton);
    generateButton.setButtonText("Generate New Rig");
    generateButton.setColour(TextButton::buttonColourId, Colour(0xffffaa00));
    generateButton.setColour(TextButton::textColourOffId, Colour(0xff0d0d0d));
    
    // Main content area - Pedalboard
    addAndMakeVisible(pedalboardView);
    
    // Bottom status bar
    addAndMakeVisible(cpuLabel);
    cpuLabel.setText("CPU LOAD: 5%", dontSendNotification);
    cpuLabel.setFont(Font(11.0f));
    cpuLabel.setColour(Label::textColourId, Colour(0xff666666));
    
    addAndMakeVisible(liveModeLabel);
    liveModeLabel.setText("⚡ LIVE MODE", dontSendNotification);
    liveModeLabel.setFont(Font(11.0f, Font::bold));
    liveModeLabel.setColour(Label::textColourId, Colour(0xff00ff88));
}

GuitarPedalRackEditor::~GuitarPedalRackEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void GuitarPedalRackEditor::paint(Graphics& g)
{
    // Main background - dark gradient
    ColourGradient gradient(Colour(0xff0d0d0d), 0, 0,
                           Colour(0xff1a1a1a), 0, (float)getHeight(),
                           false);
    g.setGradientFill(gradient);
    g.fillAll();
    
    // Top bar background
    g.setColour(Colour(0xff0a0a0a));
    g.fillRect(0, 0, getWidth(), 60);
    
    // Top bar accent line
    g.setColour(Colour(0xff00d4ff));
    g.fillRect(0, 59, getWidth(), 1);
    
    // Sidebar background
    g.setColour(Colour(0xff0a0a0a));
    g.fillRect(0, 60, 220, getHeight() - 60);
    
    // Sidebar accent line
    g.setColour(Colour(0xff1a1a1a));
    g.fillRect(219, 60, 1, getHeight() - 60);
    
    // Bottom status bar
    g.setColour(Colour(0xff0a0a0a));
    g.fillRect(0, getHeight() - 30, getWidth(), 30);
}

void GuitarPedalRackEditor::resized()
{
    auto area = getLocalBounds();
    
    // Top bar (60px)
    auto topBar = area.removeFromTop(60);
    titleLabel.setBounds(topBar.removeFromLeft(300).reduced(20, 15));
    savePresetButton.setBounds(topBar.removeFromRight(150).reduced(10, 15));
    
    // Bottom status bar (30px)
    auto bottomBar = area.removeFromBottom(30);
    cpuLabel.setBounds(bottomBar.removeFromLeft(200).reduced(10, 5));
    liveModeLabel.setBounds(bottomBar.removeFromRight(150).reduced(10, 5));
    
    // Sidebar (220px width)
    auto sidebar = area.removeFromLeft(220);
    
    browserLabel.setBounds(sidebar.removeFromTop(50).reduced(15, 10));
    advancedControlsLabel.setBounds(sidebar.removeFromTop(20).reduced(15, 0));
    
    sidebar.removeFromTop(10);
    presetsButton.setBounds(sidebar.removeFromTop(45).reduced(15, 3));
    rigGenButton.setBounds(sidebar.removeFromTop(45).reduced(15, 3));
    libraryButton.setBounds(sidebar.removeFromTop(45).reduced(15, 3));
    historyButton.setBounds(sidebar.removeFromTop(45).reduced(15, 3));
    
    // Generate button at bottom of sidebar
    auto sidebarBottom = sidebar.removeFromBottom(60);
    generateButton.setBounds(sidebarBottom.reduced(15, 10));
    
    // Main content area - Pedalboard
    pedalboardView.setBounds(area.reduced(20));
}

ModernLookAndFeel GuitarPedalRackEditor::modernLookAndFeel;

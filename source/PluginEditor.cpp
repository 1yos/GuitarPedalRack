#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "UI/CabinetEditor.h"

//==============================================================================
// SIDEBAR BROWSER COMPONENTS
//==============================================================================

class PresetItemComponent : public Component
{
public:
    PresetItemComponent(const ChainPreset& preset, std::function<void()> onClickCallback)
        : presetName(preset.name), categoryName(preset.category), onClick(onClickCallback)
    {
        if (categoryName.contains("Rock"))
            categoryColor = Colour(0xffff6b35);
        else if (categoryName.contains("Metal"))
            categoryColor = Colour(0xffdd2e44);
        else if (categoryName.contains("Blues"))
            categoryColor = Colour(0xff4fc3f7);
        else if (categoryName.contains("Clean") || categoryName.contains("Jazz"))
            categoryColor = Colour(0xff66bb6a);
        else if (categoryName.contains("Ambient"))
            categoryColor = Colour(0xffab47bc);
        else
            categoryColor = Colour(0xff8a8a8a);
    }
    
    void paint(Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        bool isSelected = (editorPresetName == presetName);
        
        g.setColour(isHovered ? Colour(0xff2A2A2A) : (isSelected ? Colour(0xff1A1A1A) : Colour(0xff121212)));
        g.fillRoundedRectangle(bounds, 4.0f);
        
        // Category color stripe
        g.setColour(categoryColor);
        g.fillRoundedRectangle(bounds.getX() + 2, bounds.getY() + 2, 4.0f, bounds.getHeight() - 4, 2.0f);
        
        // Text
        g.setColour(isSelected ? Colour(0xff00E5FF) : Colours::white);
        g.setFont(Font(12.0f, Font::bold));
        g.drawText(presetName, bounds.getX() + 12, bounds.getY(), bounds.getWidth() - 75, bounds.getHeight(), Justification::centredLeft);
        
        // Category tag
        g.setColour(categoryColor.withAlpha(0.8f));
        g.setFont(Font(9.0f, Font::bold));
        g.drawText(categoryName.toUpperCase(), bounds.getRight() - 65, bounds.getY(), 60, bounds.getHeight(), Justification::centredRight);
        
        if (isHovered)
        {
            g.setColour(Colour(0xff2A2A2A).brighter(0.5f));
            g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
        }
        else if (isSelected)
        {
            g.setColour(Colour(0xff00E5FF).withAlpha(0.4f));
            g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
        }
    }
    
    void mouseEnter(const MouseEvent&) override { isHovered = true; repaint(); }
    void mouseExit(const MouseEvent&) override { isHovered = false; repaint(); }
    void mouseDown(const MouseEvent&) override { if (onClick) onClick(); }
    
    void setEditorPresetName(const String& name) { editorPresetName = name; repaint(); }
    
private:
    String presetName;
    String categoryName;
    Colour categoryColor;
    String editorPresetName;
    bool isHovered = false;
    std::function<void()> onClick;
};

class PresetBrowser : public Component, private TextEditor::Listener
{
public:
    PresetBrowser(GuitarPedalRackEditor& editor)
        : editorRef(editor)
    {
        addAndMakeVisible(searchBar);
        searchBar.setTextToShowWhenEmpty("Search presets...", Colour(0xff8A8A8A));
        searchBar.setColour(TextEditor::backgroundColourId, Colour(0xff161616));
        searchBar.setColour(TextEditor::outlineColourId, Colour(0xff2A2A2A));
        searchBar.setColour(TextEditor::focusedOutlineColourId, Colour(0xff00E5FF));
        searchBar.addListener(this);
        
        addAndMakeVisible(viewport);
        viewport.setScrollBarThickness(6);
        
        viewportContent = std::make_unique<Component>();
        viewport.setViewedComponent(viewportContent.get(), false);
        
        rebuildList();
    }
    
    void resized() override
    {
        auto area = getLocalBounds();
        searchBar.setBounds(area.removeFromTop(35).reduced(5, 4));
        viewport.setBounds(area);
        layoutItems();
    }
    
    void rebuildList()
    {
        items.clear();
        viewportContent->deleteAllChildren();
        
        String query = searchBar.getText();
        auto& presets = editorRef.audioProcessor.getPresetManager().getAllPresets();
        
        for (const auto& preset : presets)
        {
            if (query.isNotEmpty() && !preset.name.containsIgnoreCase(query) && !preset.description.containsIgnoreCase(query))
                continue;
                
            auto* item = new PresetItemComponent(preset, [this, name = preset.name]() {
                editorRef.audioProcessor.loadPreset(name);
                editorRef.updatePresetSelection();
            });
            item->setEditorPresetName(editorRef.audioProcessor.getCurrentPresetName());
            items.add(item);
            viewportContent->addAndMakeVisible(item);
        }
        
        layoutItems();
    }
    
    void updateSelection()
    {
        for (auto* item : items)
            item->setEditorPresetName(editorRef.audioProcessor.getCurrentPresetName());
    }
    
private:
    void textEditorTextChanged(TextEditor&) override { rebuildList(); }
    
    void layoutItems()
    {
        int y = 0;
        int itemH = 28;
        int spacing = 3;
        int width = viewport.getWidth() - 10;
        
        for (auto* item : items)
        {
            item->setBounds(2, y, width, itemH);
            y += itemH + spacing;
        }
        
        viewportContent->setSize(viewport.getWidth(), y);
    }
    
    GuitarPedalRackEditor& editorRef;
    TextEditor searchBar;
    Viewport viewport;
    std::unique_ptr<Component> viewportContent;
    OwnedArray<PresetItemComponent> items;
};

class RigGeneratorPanel : public Component, private FileDragAndDropTarget
{
public:
    RigGeneratorPanel(GuitarPedalRackEditor& editor)
        : editorRef(editor)
    {
        // Style label & combo
        addAndMakeVisible(styleLabel);
        styleLabel.setText("STYLE", dontSendNotification);
        styleLabel.setFont(Font(11.0f, Font::bold));
        styleLabel.setColour(Label::textColourId, Colour(0xff8A8A8A));
        
        addAndMakeVisible(styleCombo);
        styleCombo.addItem("Modern Metal", 1);
        styleCombo.addItem("Ambient", 2);
        styleCombo.addItem("Blues", 3);
        styleCombo.addItem("Clean", 4);
        styleCombo.addItem("Rock", 5);
        styleCombo.setSelectedId(1, dontSendNotification);
        styleCombo.setColour(ComboBox::backgroundColourId, Colour(0xff161616));
        
        // Pickup label & combo
        addAndMakeVisible(pickupLabel);
        pickupLabel.setText("PICKUP TYPE", dontSendNotification);
        pickupLabel.setFont(Font(11.0f, Font::bold));
        pickupLabel.setColour(Label::textColourId, Colour(0xff8A8A8A));
        
        addAndMakeVisible(pickupCombo);
        pickupCombo.addItem("Single Coil", 1);
        pickupCombo.addItem("Humbucker", 2);
        pickupCombo.setSelectedId(2, dontSendNotification);
        pickupCombo.setColour(ComboBox::backgroundColourId, Colour(0xff161616));
        
        // Intensity label & slider
        addAndMakeVisible(intensityLabel);
        intensityLabel.setText("TONE INTENSITY", dontSendNotification);
        intensityLabel.setFont(Font(11.0f, Font::bold));
        intensityLabel.setColour(Label::textColourId, Colour(0xff8A8A8A));
        
        addAndMakeVisible(intensitySlider);
        intensitySlider.setSliderStyle(Slider::LinearHorizontal);
        intensitySlider.setRange(0.0, 1.0, 0.01);
        intensitySlider.setValue(0.7);
        intensitySlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
        
        // Drag-and-drop file upload area
        addAndMakeVisible(uploadArea);
        uploadArea.setText("DRAG & DROP AUDIO FILE\n(WAV / AIFF)\nTO MATCH TONE", dontSendNotification);
        uploadArea.setFont(Font(11.0f, Font::bold));
        uploadArea.setJustificationType(Justification::centred);
        uploadArea.setColour(Label::textColourId, Colour(0xff8A8A8A));
        uploadArea.setColour(Label::backgroundColourId, Colour(0xff161616));
        
        // Browse button
        addAndMakeVisible(browseButton);
        browseButton.setButtonText("Browse Audio File...");
        browseButton.onClick = [this]() { browseForAudioFile(); };
        
        // Generate button
        addAndMakeVisible(generateButton);
        generateButton.setButtonText("GENERATE RIG");
        generateButton.setColour(TextButton::buttonColourId, Colour(0xffF5B400));
        generateButton.setColour(TextButton::textColourOffId, Colour(0xff000000));
        generateButton.onClick = [this]() { generateRigFromInputs(); };
    }
    
    void paint(Graphics& g) override
    {
        auto uploadBounds = uploadArea.getBounds().toFloat();
        g.setColour(isDraggingOver ? Colour(0xff00E5FF) : Colour(0xff2A2A2A));
        g.drawRoundedRectangle(uploadBounds, 6.0f, 2.0f);
        
        if (!isDraggingOver)
        {
            Path borderPath;
            borderPath.addRoundedRectangle(uploadBounds.reduced(1.0f), 6.0f);
            Path dashedBorder;
            float dashes[] = { 4.0f, 4.0f };
            PathStrokeType(1.5f).createDashedStroke(dashedBorder, borderPath, dashes, 2);
            g.fillPath(dashedBorder);
        }
    }
    
    void resized() override
    {
        auto area = getLocalBounds().reduced(10);
        
        styleLabel.setBounds(area.removeFromTop(20));
        styleCombo.setBounds(area.removeFromTop(28));
        area.removeFromTop(8);
        
        pickupLabel.setBounds(area.removeFromTop(20));
        pickupCombo.setBounds(area.removeFromTop(28));
        area.removeFromTop(8);
        
        intensityLabel.setBounds(area.removeFromTop(20));
        intensitySlider.setBounds(area.removeFromTop(20));
        area.removeFromTop(12);
        
        int buttonH = 28;
        auto buttonArea = area.removeFromBottom(buttonH + 5 + buttonH);
        
        browseButton.setBounds(buttonArea.removeFromTop(buttonH));
        buttonArea.removeFromTop(5);
        generateButton.setBounds(buttonArea);
        
        area.removeFromBottom(8);
        uploadArea.setBounds(area);
    }
    
    bool isInterestedInFileDrag(const StringArray& files) override
    {
        for (const auto& file : files)
        {
            if (file.endsWithIgnoreCase(".wav") || file.endsWithIgnoreCase(".aif") || file.endsWithIgnoreCase(".aiff") || file.endsWithIgnoreCase(".mp3"))
                return true;
        }
        return false;
    }
    
    void fileDragEnter(const StringArray&, int, int) override { isDraggingOver = true; repaint(); }
    void fileDragExit(const StringArray&) override { isDraggingOver = false; repaint(); }
    
    void filesDropped(const StringArray& files, int, int) override
    {
        isDraggingOver = false;
        repaint();
        if (files.size() > 0)
            processAudioFile(File(files[0]));
    }
    
private:
    void browseForAudioFile()
    {
        fileChooser = std::make_unique<FileChooser>("Select Reference Audio File",
                                                    File::getSpecialLocation(File::userHomeDirectory),
                                                    "*.wav;*.aif;*.aiff;*.mp3");
        auto flags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;
        fileChooser->launchAsync(flags, [this](const FileChooser& chooser) {
            auto result = chooser.getResult();
            if (result.existsAsFile())
                processAudioFile(result);
        });
    }
    
    void processAudioFile(const File& file)
    {
        AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));
        
        if (reader != nullptr)
        {
            int numSamples = jmin(100000, (int)reader->lengthInSamples);
            AudioBuffer<float> buffer(1, numSamples);
            reader->read(&buffer, 0, numSamples, 0, true, false);
            
            float sumSquares = 0.0f;
            float maxVal = 0.0f;
            auto* channelData = buffer.getReadPointer(0);
            
            int zeroCrossings = 0;
            for (int i = 0; i < numSamples; ++i)
            {
                float val = channelData[i];
                sumSquares += val * val;
                maxVal = jmax(maxVal, std::abs(val));
                
                if (i > 0 && ((channelData[i] >= 0 && channelData[i-1] < 0) || (channelData[i] < 0 && channelData[i-1] >= 0)))
                    zeroCrossings++;
            }
            
            float rms = std::sqrt(sumSquares / numSamples);
            float crestFactor = rms > 0.001f ? (maxVal / rms) : 1.0f;
            float zcr = (float)zeroCrossings / numSamples;
            
            String detectedStyle = "Rock";
            float detectedIntensity = 0.5f;
            
            if (crestFactor < 2.8f && zcr > 0.12f)
            {
                detectedStyle = "Modern Metal";
                detectedIntensity = 0.85f;
            }
            else if (crestFactor > 4.2f)
            {
                detectedStyle = "Ambient";
                detectedIntensity = 0.4f;
            }
            else if (crestFactor > 3.2f && zcr < 0.08f)
            {
                detectedStyle = "Clean";
                detectedIntensity = 0.2f;
            }
            else if (crestFactor < 3.3f)
            {
                detectedStyle = "Rock";
                detectedIntensity = 0.7f;
            }
            else
            {
                detectedStyle = "Blues";
                detectedIntensity = 0.5f;
            }
            
            for (int i = 1; i <= styleCombo.getNumItems(); ++i)
            {
                if (styleCombo.getItemText(i) == detectedStyle)
                {
                    styleCombo.setSelectedId(i, dontSendNotification);
                    break;
                }
            }
            
            intensitySlider.setValue(detectedIntensity, sendNotification);
            
            AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon,
                                             "Tone Match Completed",
                                             "Analyzed: " + file.getFileName() + "\nDetected Style: " + detectedStyle + "\nGenerating matching rig...",
                                             "OK",
                                             nullptr,
                                             ModalCallbackFunction::create([this](int) {
                                                 generateRigFromInputs();
                                             }));
        }
        else
        {
            AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, "Error", "Could not read audio file.", "OK");
        }
    }
    
    void generateRigFromInputs()
    {
        String style = styleCombo.getItemText(styleCombo.getSelectedItemIndex());
        String pickup = pickupCombo.getItemText(pickupCombo.getSelectedItemIndex());
        float intensity = (float)intensitySlider.getValue();
        
        auto& apvts = editorRef.audioProcessor.getAPVTS();
        
        if (style == "Modern Metal")
        {
            apvts.getParameter("ampChannel")->setValueNotifyingHost(1.0f); // Lead
            apvts.getParameter("ampGain")->setValueNotifyingHost(0.8f);
            apvts.getParameter("ampBass")->setValueNotifyingHost(0.6f);
            apvts.getParameter("ampMid")->setValueNotifyingHost(0.35f);    // Scooped mids!
            apvts.getParameter("ampTreble")->setValueNotifyingHost(0.7f);
            apvts.getParameter("ampPresence")->setValueNotifyingHost(0.6f);
            
            apvts.getParameter("deepHeatBypass")->setValueNotifyingHost(0.0f); // Active
            apvts.getParameter("deepHeatDrive")->setValueNotifyingHost(0.2f);
            apvts.getParameter("deepHeatTone")->setValueNotifyingHost(0.65f);
            apvts.getParameter("deepHeatLevel")->setValueNotifyingHost(0.85f);
            
            apvts.getParameter("voidBypass")->setValueNotifyingHost(1.0f); // Bypassed
            apvts.getParameter("pulseBypass")->setValueNotifyingHost(1.0f); // Bypassed
            apvts.getParameter("cabBypass")->setValueNotifyingHost(0.0f); // Active
            apvts.getParameter("cabType")->setValueNotifyingHost(0.0f); // 4x12 V30
        }
        else if (style == "Ambient")
        {
            apvts.getParameter("ampChannel")->setValueNotifyingHost(0.0f); // Clean
            apvts.getParameter("ampGain")->setValueNotifyingHost(0.35f);
            apvts.getParameter("ampBass")->setValueNotifyingHost(0.5f);
            apvts.getParameter("ampMid")->setValueNotifyingHost(0.6f);
            apvts.getParameter("ampTreble")->setValueNotifyingHost(0.55f);
            
            apvts.getParameter("deepHeatBypass")->setValueNotifyingHost(1.0f); // Bypassed
            
            apvts.getParameter("voidBypass")->setValueNotifyingHost(0.0f); // Active
            apvts.getParameter("voidSize")->setValueNotifyingHost(0.85f);
            apvts.getParameter("voidDecay")->setValueNotifyingHost(0.8f);
            apvts.getParameter("voidMix")->setValueNotifyingHost(0.6f);
            
            apvts.getParameter("pulseBypass")->setValueNotifyingHost(0.0f); // Active
            apvts.getParameter("pulseRate")->setValueNotifyingHost(1.2f / 10.0f);
            apvts.getParameter("pulseDepth")->setValueNotifyingHost(0.65f);
            apvts.getParameter("pulseMix")->setValueNotifyingHost(0.45f);
            
            apvts.getParameter("cabBypass")->setValueNotifyingHost(0.0f); // Active
            apvts.getParameter("cabType")->setValueNotifyingHost(2.0f / 3.0f); // 2x12 Vintage
        }
        else if (style == "Blues")
        {
            apvts.getParameter("ampChannel")->setValueNotifyingHost(0.0f); // Clean
            apvts.getParameter("ampGain")->setValueNotifyingHost(0.45f);
            apvts.getParameter("ampBass")->setValueNotifyingHost(0.5f);
            apvts.getParameter("ampMid")->setValueNotifyingHost(0.5f);
            apvts.getParameter("ampTreble")->setValueNotifyingHost(0.6f);
            
            apvts.getParameter("deepHeatBypass")->setValueNotifyingHost(0.0f); // Active
            apvts.getParameter("deepHeatDrive")->setValueNotifyingHost(0.38f);
            apvts.getParameter("deepHeatTone")->setValueNotifyingHost(0.5f);
            apvts.getParameter("deepHeatLevel")->setValueNotifyingHost(0.6f);
            
            apvts.getParameter("voidBypass")->setValueNotifyingHost(0.0f); // Active
            apvts.getParameter("voidSize")->setValueNotifyingHost(0.4f);
            apvts.getParameter("voidDecay")->setValueNotifyingHost(0.3f);
            apvts.getParameter("voidMix")->setValueNotifyingHost(0.25f);
            
            apvts.getParameter("pulseBypass")->setValueNotifyingHost(1.0f); // Bypassed
            apvts.getParameter("cabBypass")->setValueNotifyingHost(0.0f); // Active
            apvts.getParameter("cabType")->setValueNotifyingHost(3.0f / 3.0f); // 1x12 Classic
        }
        else if (style == "Clean")
        {
            apvts.getParameter("ampChannel")->setValueNotifyingHost(0.0f); // Clean
            apvts.getParameter("ampGain")->setValueNotifyingHost(0.3f);
            apvts.getParameter("ampBass")->setValueNotifyingHost(0.45f);
            apvts.getParameter("ampMid")->setValueNotifyingHost(0.55f);
            apvts.getParameter("ampTreble")->setValueNotifyingHost(0.5f);
            
            apvts.getParameter("deepHeatBypass")->setValueNotifyingHost(1.0f); // Bypassed
            
            apvts.getParameter("voidBypass")->setValueNotifyingHost(0.0f); // Active
            apvts.getParameter("voidSize")->setValueNotifyingHost(0.3f);
            apvts.getParameter("voidDecay")->setValueNotifyingHost(0.2f);
            apvts.getParameter("voidMix")->setValueNotifyingHost(0.15f);
            
            apvts.getParameter("pulseBypass")->setValueNotifyingHost(1.0f); // Bypassed
            apvts.getParameter("cabBypass")->setValueNotifyingHost(0.0f); // Active
            apvts.getParameter("cabType")->setValueNotifyingHost(2.0f / 3.0f); // 2x12 Vintage
        }
        else // Rock
        {
            apvts.getParameter("ampChannel")->setValueNotifyingHost(0.5f); // Crunch
            apvts.getParameter("ampGain")->setValueNotifyingHost(0.58f);
            apvts.getParameter("ampBass")->setValueNotifyingHost(0.5f);
            apvts.getParameter("ampMid")->setValueNotifyingHost(0.55f);
            apvts.getParameter("ampTreble")->setValueNotifyingHost(0.6f);
            
            apvts.getParameter("deepHeatBypass")->setValueNotifyingHost(0.0f); // Active
            apvts.getParameter("deepHeatDrive")->setValueNotifyingHost(0.45f);
            apvts.getParameter("deepHeatTone")->setValueNotifyingHost(0.5f);
            apvts.getParameter("deepHeatLevel")->setValueNotifyingHost(0.7f);
            
            apvts.getParameter("voidBypass")->setValueNotifyingHost(0.0f); // Active
            apvts.getParameter("voidSize")->setValueNotifyingHost(0.45f);
            apvts.getParameter("voidDecay")->setValueNotifyingHost(0.35f);
            apvts.getParameter("voidMix")->setValueNotifyingHost(0.2f);
            
            apvts.getParameter("pulseBypass")->setValueNotifyingHost(1.0f); // Bypassed
            apvts.getParameter("cabBypass")->setValueNotifyingHost(0.0f); // Active
            apvts.getParameter("cabType")->setValueNotifyingHost(1.0f / 3.0f); // 4x12 Greenback
        }
        
        if (pickup == "Single Coil")
        {
            float currentDrive = apvts.getParameter("deepHeatDrive")->getValue();
            apvts.getParameter("deepHeatDrive")->setValueNotifyingHost(jmin(1.0f, currentDrive + 0.05f));
            float currentAmpGain = apvts.getParameter("ampGain")->getValue();
            apvts.getParameter("ampGain")->setValueNotifyingHost(jmin(1.0f, currentAmpGain + 0.05f));
        }
        
        float currentGain = apvts.getParameter("ampGain")->getValue();
        apvts.getParameter("ampGain")->setValueNotifyingHost(jlimit(0.1f, 1.0f, currentGain * intensity / 0.7f));
        
        editorRef.repaint();
    }
    
    GuitarPedalRackEditor& editorRef;
    Label styleLabel, pickupLabel, intensityLabel;
    ComboBox styleCombo, pickupCombo;
    Slider intensitySlider;
    Label uploadArea;
    TextButton browseButton, generateButton;
    std::unique_ptr<FileChooser> fileChooser;
    bool isDraggingOver = false;
};

class LibraryBrowser : public Component
{
public:
    LibraryBrowser(GuitarPedalRackEditor& editor)
        : editorRef(editor)
    {
        addAndMakeVisible(viewport);
        viewport.setScrollBarThickness(6);
        
        viewportContent = std::make_unique<Component>();
        viewport.setViewedComponent(viewportContent.get(), false);
        
        rebuildLibrary();
    }
    
    void resized() override
    {
        viewport.setBounds(getLocalBounds());
        layoutLibrary();
    }
    
    void rebuildLibrary()
    {
        viewportContent->deleteAllChildren();
        categoryHeaders.clear();
        presetItems.clear();
        
        auto& presetManager = editorRef.audioProcessor.getPresetManager();
        auto categories = presetManager.getAllCategories();
        auto& presets = presetManager.getAllPresets();
        
        for (const auto& category : categories)
        {
            Array<ChainPreset> catPresets;
            for (const auto& p : presets)
            {
                if (p.category == category)
                    catPresets.add(p);
            }
            
            if (catPresets.isEmpty())
                continue;
                
            auto* header = new Label("", category.toUpperCase());
            header->setFont(Font(11.0f, Font::bold));
            header->setColour(Label::textColourId, Colour(0xffF5B400));
            viewportContent->addAndMakeVisible(header);
            categoryHeaders.add(header);
            
            for (const auto& p : catPresets)
            {
                auto* item = new PresetItemComponent(p, [this, name = p.name]() {
                    editorRef.audioProcessor.loadPreset(name);
                    editorRef.updatePresetSelection();
                });
                item->setEditorPresetName(editorRef.audioProcessor.getCurrentPresetName());
                viewportContent->addAndMakeVisible(item);
                presetItems.add(item);
                item->getProperties().set("category", category);
            }
        }
        
        layoutLibrary();
    }
    
    void updateSelection()
    {
        for (auto* item : presetItems)
            item->setEditorPresetName(editorRef.audioProcessor.getCurrentPresetName());
    }
    
private:
    void layoutLibrary()
    {
        auto& presetManager = editorRef.audioProcessor.getPresetManager();
        auto categories = presetManager.getAllCategories();
        
        int y = 5;
        int itemH = 28;
        int headerH = 22;
        int spacing = 3;
        int width = viewport.getWidth() - 10;
        
        for (const auto& category : categories)
        {
            Label* currentHeader = nullptr;
            for (auto* h : categoryHeaders)
            {
                if (h->getText() == category.toUpperCase())
                {
                    currentHeader = h;
                    break;
                }
            }
            
            if (currentHeader == nullptr)
                continue;
                
            currentHeader->setBounds(5, y, width - 10, headerH);
            y += headerH + 2;
            
            for (auto* item : presetItems)
            {
                if (item->getProperties().getWithDefault("category", "").toString() == category)
                {
                    item->setBounds(2, y, width, itemH);
                    y += itemH + spacing;
                }
            }
            y += 6;
        }
        
        viewportContent->setSize(viewport.getWidth(), y);
    }
    
    GuitarPedalRackEditor& editorRef;
    Viewport viewport;
    std::unique_ptr<Component> viewportContent;
    OwnedArray<Label> categoryHeaders;
    OwnedArray<PresetItemComponent> presetItems;
};

class HistoryBrowser : public Component
{
public:
    HistoryBrowser(GuitarPedalRackEditor& editor)
        : editorRef(editor)
    {
        addAndMakeVisible(title);
        title.setText("RECENT PRESETS", dontSendNotification);
        title.setFont(Font(11.0f, Font::bold));
        title.setColour(Label::textColourId, Colour(0xff8A8A8A));
        
        addAndMakeVisible(viewport);
        viewport.setScrollBarThickness(6);
        
        viewportContent = std::make_unique<Component>();
        viewport.setViewedComponent(viewportContent.get(), false);
        
        updateHistory();
    }
    
    void resized() override
    {
        auto area = getLocalBounds();
        title.setBounds(area.removeFromTop(25).reduced(10, 2));
        viewport.setBounds(area);
        layoutHistory();
    }
    
    void addPresetToHistory(const String& name)
    {
        historyList.removeString(name);
        historyList.insert(0, name);
        while (historyList.size() > 15)
            historyList.remove(historyList.size() - 1);
            
        updateHistory();
    }
    
    void updateHistory()
    {
        viewportContent->deleteAllChildren();
        items.clear();
        
        auto& presetManager = editorRef.audioProcessor.getPresetManager();
        
        for (const auto& name : historyList)
        {
            auto& presets = presetManager.getAllPresets();
            auto preset = presets.begin();
            for (; preset != presets.end(); ++preset)
            {
                if (preset->name == name)
                    break;
            }
            
            if (preset == presets.end())
                continue;
                
            auto* item = new PresetItemComponent(*preset, [this, name]() {
                editorRef.audioProcessor.loadPreset(name);
                editorRef.updatePresetSelection();
            });
            item->setEditorPresetName(editorRef.audioProcessor.getCurrentPresetName());
            items.add(item);
            viewportContent->addAndMakeVisible(item);
        }
        
        layoutHistory();
    }
    
    void updateSelection()
    {
        for (auto* item : items)
            item->setEditorPresetName(editorRef.audioProcessor.getCurrentPresetName());
    }
    
private:
    void layoutHistory()
    {
        int y = 0;
        int itemH = 28;
        int spacing = 3;
        int width = viewport.getWidth() - 10;
        
        for (auto* item : items)
        {
            item->setBounds(2, y, width, itemH);
            y += itemH + spacing;
        }
        
        viewportContent->setSize(viewport.getWidth(), y);
    }
    
    GuitarPedalRackEditor& editorRef;
    Label title;
    Viewport viewport;
    std::unique_ptr<Component> viewportContent;
    StringArray historyList;
    OwnedArray<PresetItemComponent> items;
};

//==============================================================================
// PREMIUM LOOK AND FEEL
//==============================================================================

PremiumLookAndFeel::PremiumLookAndFeel()
{
    // Dark matte background
    setColour(ResizableWindow::backgroundColourId, Colour(0xff0a0a0a));
    
    // Neon accents
    setColour(Slider::thumbColourId, Colour(0xff00d4ff));          // Cyan
    setColour(Slider::trackColourId, Colour(0xff1a1a1a));
    setColour(Slider::rotarySliderFillColourId, Colour(0xff00d4ff));
    
    // Buttons
    setColour(TextButton::buttonColourId, Colour(0xff1a1a1a));
    setColour(TextButton::textColourOffId, Colour(0xffcccccc));
    setColour(ComboBox::backgroundColourId, Colour(0xff1a1a1a));
}

void PremiumLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height,
                                          float sliderPosProportional, float rotaryStartAngle,
                                          float rotaryEndAngle, Slider& slider)
{
    auto bounds = Rectangle<float>(x, y, width, height).reduced(10);
    auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    
    // Outer ring (matte black with subtle highlight)
    g.setColour(Colour(0xff2a2a2a));
    g.fillEllipse(rx, ry, rw, rw);
    
    // Inner circle (darker)
    g.setColour(Colour(0xff0d0d0d));
    g.fillEllipse(rx + 4, ry + 4, rw - 8, rw - 8);
    
    // Value arc with neon glow
    Path valueArc;
    valueArc.addCentredArc(centreX, centreY, radius - 4, radius - 4,
                          0.0f, rotaryStartAngle, angle, true);
    
    // Glow effect
    g.setColour(Colour(0xff00d4ff).withAlpha(0.3f));
    g.strokePath(valueArc, PathStrokeType(6.0f));
    
    g.setColour(Colour(0xff00d4ff));
    g.strokePath(valueArc, PathStrokeType(3.0f));
    
    // Pointer (subtle)
    Path pointer;
    auto pointerLength = radius * 0.5f;
    auto pointerThickness = 2.5f;
    pointer.addRectangle(-pointerThickness * 0.5f, -radius + 8, pointerThickness, pointerLength);
    g.setColour(Colour(0xffdddddd));
    g.fillPath(pointer, AffineTransform::rotation(angle).translated(centreX, centreY));
    
    // Center dot
    g.setColour(Colour(0xff0a0a0a));
    g.fillEllipse(centreX - 3, centreY - 3, 6, 6);
}

void PremiumLookAndFeel::drawButtonBackground(Graphics& g, Button& button,
                                              const Colour& backgroundColour,
                                              bool shouldDrawButtonAsHighlighted,
                                              bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
    
    auto baseColour = backgroundColour;
    if (shouldDrawButtonAsDown)
        baseColour = baseColour.brighter(0.2f);
    else if (shouldDrawButtonAsHighlighted)
        baseColour = baseColour.brighter(0.1f);
    
    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, 4.0f);
    
    // Subtle border
    g.setColour(baseColour.brighter(0.3f));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
}

//==============================================================================
// PREMIUM PEDAL COMPONENT
//==============================================================================

PremiumPedal::PremiumPedal(const String& name, const Colour& categoryColor)
    : pedalName(name), accentColor(categoryColor)
{
    setSize(140, 200);
}

void PremiumPedal::paint(Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Card background with category color
    g.setColour(accentColor);
    g.fillRoundedRectangle(bounds, 12.0f);
    
    // Slight darkening gradient for depth
    ColourGradient darkGrad(accentColor, bounds.getCentreX(), bounds.getY(),
                           accentColor.darker(0.3f), bounds.getCentreX(), bounds.getBottom(),
                           false);
    g.setGradientFill(darkGrad);
    g.fillRoundedRectangle(bounds, 12.0f);
    
    // Pedal name at top
    g.setColour(Colours::white.withAlpha(0.95f));
    g.setFont(Font(16.0f, Font::bold));
    g.drawText(pedalName, bounds.getX() + 10, bounds.getY() + 15,
              bounds.getWidth() - 20, 25, Justification::centred);
    
    // Subtitle/description
    g.setColour(Colours::white.withAlpha(0.6f));
    g.setFont(Font(10.0f));
    g.drawText("OVERDRIVE / BOOSTER", bounds.getX() + 10, bounds.getY() + 38,
              bounds.getWidth() - 20, 15, Justification::centred);
    
    // Draw knobs in middle area
    for (const auto& knob : knobs)
    {
        // Knob body (dark grey)
        g.setColour(Colour(0xff3a3a3a));
        g.fillEllipse(knob.bounds.toFloat());
        
        // Knob indicator line
        float angle = knob.value * MathConstants<float>::pi * 1.5f - MathConstants<float>::pi * 0.75f;
        float centerX = knob.bounds.getCentreX();
        float centerY = knob.bounds.getCentreY();
        float indicatorLength = knob.bounds.getWidth() * 0.35f;
        
        g.setColour(Colours::white);
        g.drawLine(centerX, centerY,
                  centerX + std::cos(angle) * indicatorLength,
                  centerY + std::sin(angle) * indicatorLength,
                  2.5f);
        
        // Knob label below
        g.setColour(Colours::white.withAlpha(0.8f));
        g.setFont(Font(9.0f, Font::bold));
        g.drawText(knob.label, knob.bounds.getX() - 15, knob.bounds.getBottom() + 3,
                  knob.bounds.getWidth() + 30, 12, Justification::centred);
    }
    
    // Stomp switch at bottom - large white rounded rectangle
    auto switchBounds = Rectangle<float>(bounds.getCentreX() - 50, bounds.getBottom() - 45, 100, 30);
    
    g.setColour(Colours::white);
    g.fillRoundedRectangle(switchBounds, 6.0f);
    
    // Active indicator (LED style dot above switch)
    if (active)
    {
        g.setColour(Colour(0xff00ff00).brighter());  // Bright green
        g.fillEllipse(bounds.getCentreX() - 4, bounds.getBottom() - 60, 8, 8);
        
        // Glow
        g.setColour(Colour(0xff00ff00).withAlpha(0.3f));
        g.fillEllipse(bounds.getCentreX() - 8, bounds.getBottom() - 64, 16, 16);
    }
}

void PremiumPedal::resized()
{
    // Layout knobs in the middle area in a grid
    auto area = getLocalBounds();
    int knobSize = 45;  // Larger knobs
    int knobY = 70;  // Below title
    
    if (knobs.size() == 1)
    {
        // Single knob - centered
        int x = (getWidth() - knobSize) / 2;
        knobs[0].bounds = Rectangle<int>(x, knobY, knobSize, knobSize);
    }
    else if (knobs.size() == 2)
    {
        // Two knobs - side by side
        int spacing = 20;
        int totalWidth = knobSize * 2 + spacing;
        int startX = (getWidth() - totalWidth) / 2;
        knobs[0].bounds = Rectangle<int>(startX, knobY, knobSize, knobSize);
        knobs[1].bounds = Rectangle<int>(startX + knobSize + spacing, knobY, knobSize, knobSize);
    }
    else if (knobs.size() >= 3)
    {
        // Three or more knobs - 2x2 grid or row
        int spacing = 15;
        int totalWidth = knobSize * 2 + spacing;
        int startX = (getWidth() - totalWidth) / 2;
        
        for (size_t i = 0; i < knobs.size() && i < 4; ++i)
        {
            int col = i % 2;
            int row = i / 2;
            int x = startX + col * (knobSize + spacing);
            int y = knobY + row * (knobSize + 25);
            knobs[i].bounds = Rectangle<int>(x, y, knobSize, knobSize);
        }
    }
}

void PremiumPedal::mouseDown(const MouseEvent& e)
{
    // Check if clicked on stomp switch
    auto switchArea = Rectangle<float>(getWidth() / 2.0f - 15, getHeight() - 45, 30, 30);
    if (switchArea.contains(e.position))
    {
        setActive(!active);
        repaint();
    }
}

void PremiumPedal::setActive(bool shouldBeActive)
{
    active = shouldBeActive;
    ledOn = active;
    repaint();
}

void PremiumPedal::addKnob(const String& label, float value)
{
    Knob knob;
    knob.label = label;
    knob.value = jlimit(0.0f, 1.0f, value);
    knobs.push_back(knob);
    resized();
}

//==============================================================================
// AMP HEAD COMPONENT
//==============================================================================

AmpHead::AmpHead()
{
    setSize(400, 250);
    
    // Configure sliders (minimalist knobs)
    auto configureSlider = [this](Slider& slider, Label& label, const String& text)
    {
        addAndMakeVisible(slider);
        slider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
        
        addAndMakeVisible(label);
        label.setText(text, dontSendNotification);
        label.setJustificationType(Justification::centred);
        label.setFont(Font(10.0f, Font::bold));
        label.setColour(Label::textColourId, Colours::white.withAlpha(0.8f));
    };
    
    configureSlider(gainSlider, gainLabel, "GAIN");
    configureSlider(bassSlider, bassLabel, "BASS");
    configureSlider(midSlider, midLabel, "MID");
    configureSlider(trebleSlider, trebleLabel, "TREBLE");
    configureSlider(presenceSlider, presenceLabel, "PRESENCE");
    
    addAndMakeVisible(ampModelLabel);
    ampModelLabel.setText("CRUNCH AMP", dontSendNotification);
    ampModelLabel.setFont(Font(16.0f, Font::bold));
    ampModelLabel.setJustificationType(Justification::centred);
    ampModelLabel.setColour(Label::textColourId, Colour(0xffffaa00));
}

void AmpHead::paint(Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Shadow (more prominent)
    g.setColour(Colours::black.withAlpha(0.8f));
    g.fillRoundedRectangle(bounds.translated(5, 5), 10.0f);
    
    // Main amp body (gradient for depth)
    ColourGradient gradient(Colour(0xff3a3a3a), bounds.getCentreX(), bounds.getY(),
                           Colour(0xff1a1a1a), bounds.getCentreX(), bounds.getBottom(),
                           false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, 10.0f);
    
    // Top panel accent (amber glow - more prominent)
    g.setColour(Colour(0xffffaa00).withAlpha(0.4f));
    g.fillRoundedRectangle(bounds.getX() + 10, bounds.getY() + 10,
                          bounds.getWidth() - 20, 50, 6.0f);
    
    // Border highlight (brighter)
    g.setColour(Colour(0xff4a4a4a));
    g.drawRoundedRectangle(bounds, 10.0f, 2.5f);
    
    // Speaker grill texture at bottom (more visible)
    g.setColour(Colour(0xff0a0a0a));
    for (int i = 0; i < 10; ++i)
    {
        float y = bounds.getBottom() - 90 + i * 9;
        g.fillRect(bounds.getX() + 25.0f, y, bounds.getWidth() - 50.0f, 3.5f);
    }
}

void AmpHead::resized()
{
    auto area = getLocalBounds();
    
    // Model label at top
    ampModelLabel.setBounds(area.removeFromTop(60).reduced(20, 15));
    
    // Knobs in a row
    area.removeFromTop(10);
    int knobSize = 60;
    int knobY = area.getY();
    int totalKnobWidth = knobSize * 5;
    int spacing = (area.getWidth() - totalKnobWidth) / 6;
    
    auto placeKnob = [&](Slider& slider, Label& label)
    {
        int x = area.getX() + spacing;
        slider.setBounds(x, knobY, knobSize, knobSize);
        label.setBounds(x - 10, knobY + knobSize + 2, knobSize + 20, 15);
        spacing += knobSize + (area.getWidth() - totalKnobWidth) / 6;
    };
    
    placeKnob(gainSlider, gainLabel);
    placeKnob(bassSlider, bassLabel);
    placeKnob(midSlider, midLabel);
    placeKnob(trebleSlider, trebleLabel);
    placeKnob(presenceSlider, presenceLabel);
}

//==============================================================================
// PRESET CARD COMPONENT
//==============================================================================

PresetCard::PresetCard(const String& name, const String& category)
    : presetName(name), categoryName(category)
{
    setSize(180, 110);  // Taller
    
    // Assign category color
    if (category.contains("Rock"))
        categoryColor = Colour(0xffff6b35);
    else if (category.contains("Metal"))
        categoryColor = Colour(0xffdd2e44);
    else if (category.contains("Blues"))
        categoryColor = Colour(0xff4fc3f7);
    else if (category.contains("Clean") || category.contains("Jazz"))
        categoryColor = Colour(0xff66bb6a);
    else if (category.contains("Ambient"))
        categoryColor = Colour(0xffab47bc);
    else
        categoryColor = Colour(0xff78909c);
}

void PresetCard::paint(Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(3);
    
    // Card background (darker when not hovered, lighter when hovered)
    g.setColour(isHovered ? Colour(0xff2a2a2a) : Colour(0xff1a1a1a));
    g.fillRoundedRectangle(bounds, 8.0f);
    
    // Category color stripe (thicker, more visible)
    g.setColour(categoryColor);
    g.fillRoundedRectangle(bounds.getX(), bounds.getY(), 6, bounds.getHeight(), 3.0f);
    
    // Hover glow (more prominent)
    if (isHovered)
    {
        g.setColour(categoryColor.withAlpha(0.4f));
        g.drawRoundedRectangle(bounds, 8.0f, 2.5f);
    }
    
    // Preset name (bigger, more prominent)
    g.setColour(Colours::white.withAlpha(isHovered ? 1.0f : 0.95f));
    g.setFont(Font(16.0f, Font::bold));  // Bigger font
    g.drawText(presetName, bounds.getX() + 18, bounds.getY() + 20,
              bounds.getWidth() - 25, 50, Justification::topLeft);
    
    // Category label (more visible)
    g.setColour(categoryColor.withAlpha(0.8f));
    g.setFont(Font(11.0f, Font::bold));
    g.drawText(categoryName, bounds.getX() + 18, bounds.getBottom() - 30,
              bounds.getWidth() - 25, 20, Justification::bottomLeft);
}

void PresetCard::mouseEnter(const MouseEvent&)
{
    isHovered = true;
    repaint();
    // TODO: Trigger hover preview sound
}

void PresetCard::mouseExit(const MouseEvent&)
{
    isHovered = false;
    repaint();
}

void PresetCard::mouseDown(const MouseEvent&)
{
    // TODO: Load preset
    DBG("Preset selected: " + presetName);
}


//==============================================================================
// RIG GENERATOR OVERLAY
//==============================================================================

RigGeneratorOverlay::RigGeneratorOverlay()
{
    setSize(700, 500);
    
    addAndMakeVisible(titleLabel);
    titleLabel.setText("Instant Rig Generator", dontSendNotification);
    titleLabel.setFont(Font(32.0f, Font::bold));
    titleLabel.setJustificationType(Justification::centred);
    titleLabel.setColour(Label::textColourId, Colour(0xff00d4ff));
    
    // Style slider
    addAndMakeVisible(styleSlider);
    styleSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    styleSlider.setRange(0, 10, 1);
    styleSlider.setValue(5);
    
    addAndMakeVisible(styleLabel);
    styleLabel.setText("STYLE", dontSendNotification);
    styleLabel.setFont(Font(14.0f, Font::bold));
    styleLabel.setJustificationType(Justification::centred);
    
    // Energy slider
    addAndMakeVisible(energySlider);
    energySlider.setSliderStyle(Slider::LinearHorizontal);
    energySlider.setRange(0, 100, 1);
    energySlider.setValue(50);
    
    addAndMakeVisible(energyLabel);
    energyLabel.setText("ENERGY", dontSendNotification);
    energyLabel.setFont(Font(14.0f, Font::bold));
    
    // Intensity slider
    addAndMakeVisible(intensitySlider);
    intensitySlider.setSliderStyle(Slider::LinearHorizontal);
    intensitySlider.setRange(0, 100, 1);
    intensitySlider.setValue(60);
    
    addAndMakeVisible(intensityLabel);
    intensityLabel.setText("INTENSITY", dontSendNotification);
    intensityLabel.setFont(Font(14.0f, Font::bold));
    
    // Buttons
    addAndMakeVisible(generateButton);
    generateButton.setButtonText("GENERATE RIG");
    generateButton.setColour(TextButton::buttonColourId, Colour(0xff00d4ff));
    generateButton.setColour(TextButton::textColourOffId, Colour(0xff0a0a0a));
    
    addAndMakeVisible(cancelButton);
    cancelButton.setButtonText("CANCEL");
    cancelButton.setColour(TextButton::buttonColourId, Colour(0xff444444));
    
    // Close button (X in top-right corner)
    addAndMakeVisible(closeButton);
    closeButton.setButtonText("✕");
    closeButton.setColour(TextButton::buttonColourId, Colours::transparentBlack);
    closeButton.setColour(TextButton::textColourOffId, Colour(0xff888888));
    
    // Connect cancel button to hide overlay
    cancelButton.onClick = [this]() {
        setVisible(false);
    };
    
    // Connect close button to hide overlay
    closeButton.onClick = [this]() {
        setVisible(false);
    };
    
    // Connect generate button (placeholder for now)
    generateButton.onClick = [this]() {
        DBG("Rig Generator: Generate clicked");
        // TODO: Implement rig generation logic
    };
    
    setVisible(false);
}

void RigGeneratorOverlay::paint(Graphics& g)
{
    // Semi-transparent dark background
    g.fillAll(Colour(0xcc000000));
    
    // Center panel
    auto bounds = getLocalBounds().toFloat().reduced(50);
    
    // Panel shadow
    g.setColour(Colours::black.withAlpha(0.8f));
    g.fillRoundedRectangle(bounds.translated(4, 4), 12.0f);
    
    // Panel background
    ColourGradient gradient(Colour(0xff1a1a1a), bounds.getCentreX(), bounds.getY(),
                           Colour(0xff0d0d0d), bounds.getCentreX(), bounds.getBottom(),
                           false);
    g.setGradientFill(gradient);
    g.fillRoundedRectangle(bounds, 12.0f);
    
    // Border with neon glow
    g.setColour(Colour(0xff00d4ff).withAlpha(0.5f));
    g.drawRoundedRectangle(bounds, 12.0f, 2.0f);
}

void RigGeneratorOverlay::resized()
{
    auto area = getLocalBounds().reduced(80);
    
    // Close button in top-right corner of panel
    auto closeArea = area.removeFromTop(40);
    closeButton.setBounds(closeArea.removeFromRight(40));
    
    titleLabel.setBounds(area.removeFromTop(80));
    area.removeFromTop(30);
    
    // Style wheel (big centered knob)
    int wheelSize = 150;
    auto wheelArea = area.removeFromTop(wheelSize + 30);
    styleSlider.setBounds(wheelArea.getCentreX() - wheelSize/2, wheelArea.getY(), wheelSize, wheelSize);
    styleLabel.setBounds(wheelArea.getX(), wheelArea.getBottom() - 25, wheelArea.getWidth(), 25);
    
    area.removeFromTop(40);
    
    // Energy slider
    auto energyArea = area.removeFromTop(60);
    energyLabel.setBounds(energyArea.removeFromTop(25));
    energySlider.setBounds(energyArea.reduced(20, 5));
    
    area.removeFromTop(20);
    
    // Intensity slider
    auto intensityArea = area.removeFromTop(60);
    intensityLabel.setBounds(intensityArea.removeFromTop(25));
    intensitySlider.setBounds(intensityArea.reduced(20, 5));
    
    // Buttons at bottom
    auto buttonArea = area.removeFromBottom(60).reduced(50, 10);
    generateButton.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2 - 10));
    buttonArea.removeFromLeft(20);
    cancelButton.setBounds(buttonArea);
}

void RigGeneratorOverlay::mouseDown(const MouseEvent& e)
{
    // Allow clicking outside the panel to close
    auto bounds = getLocalBounds().reduced(50).toFloat();
    if (!bounds.contains(e.position))
    {
        setVisible(false);
    }
}

void RigGeneratorOverlay::setVisible(bool shouldBeVisible)
{
    Component::setVisible(shouldBeVisible);
    if (shouldBeVisible)
    {
        // TODO: Add animation (fade in + scale)
    }
}

//==============================================================================
// MACRO CONTROL PANEL
//==============================================================================

MacroControlPanel::MacroControlPanel()
{
    setSize(200, 600);
    
    // Morph knob (BIG, dominant)
    addAndMakeVisible(morphKnob);
    morphKnob.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    morphKnob.setRange(0, 100, 1);
    morphKnob.setValue(25);  // Crunch position
    morphKnob.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    
    addAndMakeVisible(morphLabel);
    morphLabel.setText("MORPH", dontSendNotification);
    morphLabel.setFont(Font(16.0f, Font::bold));
    morphLabel.setJustificationType(Justification::centred);
    morphLabel.setColour(Label::textColourId, Colour(0xff00d4ff));
    
    // Macro sliders (vertical)
    auto configureMacro = [this](Slider& slider, Label& label, const String& text)
    {
        addAndMakeVisible(slider);
        slider.setSliderStyle(Slider::LinearVertical);
        slider.setRange(0, 100, 1);
        slider.setValue(50);
        slider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
        
        addAndMakeVisible(label);
        label.setText(text, dontSendNotification);
        label.setFont(Font(11.0f, Font::bold));
        label.setJustificationType(Justification::centred);
        label.setColour(Label::textColourId, Colours::white.withAlpha(0.8f));
    };
    
    configureMacro(brightnessSlider, brightnessLabel, "TONE");
    configureMacro(spaceSlider, spaceLabel, "SPACE");
    configureMacro(tightnessSlider, tightnessLabel, "TIGHT");
}

void MacroControlPanel::paint(Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // Panel background (subtle)
    g.setColour(Colour(0xff0d0d0d));
    g.fillRoundedRectangle(bounds.reduced(5), 8.0f);
    
    // Border
    g.setColour(Colour(0xff1a1a1a));
    g.drawRoundedRectangle(bounds.reduced(5), 8.0f, 1.0f);
    
    // Section divider
    g.setColour(Colour(0xff1a1a1a));
    g.fillRect(bounds.getX() + 20, 250.0f, bounds.getWidth() - 40, 1.0f);
}

void MacroControlPanel::resized()
{
    auto area = getLocalBounds().reduced(15);
    
    // Morph knob at top (BIG)
    area.removeFromTop(20);
    int morphSize = 120;
    auto morphArea = area.removeFromTop(morphSize + 40);
    morphKnob.setBounds(morphArea.getCentreX() - morphSize/2, morphArea.getY(),
                       morphSize, morphSize);
    morphLabel.setBounds(morphArea.getX(), morphArea.getBottom() - 30,
                        morphArea.getWidth(), 30);
    
    area.removeFromTop(40);
    
    // Macro sliders in a row
    int sliderWidth = 40;
    int sliderHeight = 180;
    int spacing = (area.getWidth() - sliderWidth * 3) / 4;
    
    auto placeMacroSlider = [&](Slider& slider, Label& label)
    {
        int x = area.getX() + spacing;
        slider.setBounds(x, area.getY() + 30, sliderWidth, sliderHeight);
        label.setBounds(x - 10, area.getY(), sliderWidth + 20, 25);
        spacing += sliderWidth + (area.getWidth() - sliderWidth * 3) / 4;
    };
    
    placeMacroSlider(brightnessSlider, brightnessLabel);
    placeMacroSlider(spaceSlider, spaceLabel);
    placeMacroSlider(tightnessSlider, tightnessLabel);
}

//==============================================================================
// MAIN EDITOR
//==============================================================================

PremiumLookAndFeel GuitarPedalRackEditor::premiumLookAndFeel;

GuitarPedalRackEditor::GuitarPedalRackEditor(GuitarPedalRackProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Professional plugin window - scaled to 1200x600
    setSize(1200, 600);
    setResizable(false, false);  // Fixed size like Neural DSP
    
    setLookAndFeel(&premiumLookAndFeel);
    
    //==========================================================================
    // TOP TOOLBAR - 72px
    //==========================================================================
    
    // Brand logo
    addAndMakeVisible(brandLabel);
    brandLabel.setText("UNIVERSAL GUITAR PEDAL RACK", dontSendNotification);
    brandLabel.setFont(Font(18.0f, Font::bold));
    brandLabel.setColour(Label::textColourId, Colour(0xff00E5FF));  // Cyan from spec
    
    // View Switcher Buttons (Center of Top Toolbar)
    addAndMakeVisible(pedalboardViewButton);
    pedalboardViewButton.setButtonText("STOMPBOXES");
    pedalboardViewButton.onClick = [this]() { selectMainView(MainView::Pedalboard); };
    
    addAndMakeVisible(ampViewButton);
    ampViewButton.setButtonText("ALPHA AMP");
    ampViewButton.onClick = [this]() { selectMainView(MainView::Amp); };
    
    addAndMakeVisible(cabinetViewButton);
    cabinetViewButton.setButtonText("CABINET SIM");
    cabinetViewButton.onClick = [this]() { selectMainView(MainView::Cabinet); };
    
    // Menu buttons - File, Edit, Settings
    addAndMakeVisible(fileButton);
    fileButton.setButtonText("File");
    fileButton.onClick = [this]() {
        DBG("File button clicked");
        PopupMenu menu;
        menu.addItem(1, "New Preset");
        menu.addItem(2, "Load Preset...");
        menu.addItem(3, "Save Preset");
        menu.addItem(4, "Save Preset As...");
        menu.addSeparator();
        menu.addItem(5, "Import Preset...");
        menu.addItem(6, "Export Preset...");
        menu.showMenuAsync(PopupMenu::Options().withTargetComponent(&fileButton),
                          [](int result) {
                              DBG("File menu item selected: " + String(result));
                          });
    };
    
    addAndMakeVisible(editButton);
    editButton.setButtonText("Edit");
    editButton.onClick = [this]() {
        DBG("Edit button clicked");
        PopupMenu menu;
        menu.addItem(1, "Undo");
        menu.addItem(2, "Redo");
        menu.addSeparator();
        menu.addItem(3, "Copy Settings");
        menu.addItem(4, "Paste Settings");
        menu.addSeparator();
        menu.addItem(5, "Reset All");
        menu.showMenuAsync(PopupMenu::Options().withTargetComponent(&editButton),
                          [](int result) {
                              DBG("Edit menu item selected: " + String(result));
                          });
    };
    
    addAndMakeVisible(settingsButton);
    settingsButton.setButtonText("Settings");
    settingsButton.onClick = [this]() {
        DBG("Settings button clicked");
        PopupMenu menu;
        menu.addItem(1, "Audio Settings...");
        menu.addItem(2, "Plugin Preferences...");
        menu.addSeparator();
        menu.addItem(3, "About");
        menu.showMenuAsync(PopupMenu::Options().withTargetComponent(&settingsButton),
                          [](int result) {
                              DBG("Settings menu item selected: " + String(result));
                          });
    };
    
    // Save Preset button - bright cyan
    addAndMakeVisible(saveButton);
    saveButton.setButtonText("Save Preset");
    saveButton.setColour(TextButton::buttonColourId, Colour(0xff00E5FF));
    saveButton.setColour(TextButton::textColourOffId, Colour(0xff000000));
    saveButton.onClick = [this]() {
        DBG("Save Preset clicked");
        AlertWindow::showMessageBoxAsync(
            AlertWindow::InfoIcon,
            "Save Preset",
            "Preset save functionality will be implemented soon.",
            "OK"
        );
    };
    
    //==========================================================================
    // LEFT SIDEBAR - Browser - 300px
    //==========================================================================
    
    addAndMakeVisible(soundBrowserPanel);
    
    addAndMakeVisible(browserTitleLabel);
    browserTitleLabel.setText("Browser", dontSendNotification);
    browserTitleLabel.setFont(Font(20.0f, Font::bold));
    browserTitleLabel.setColour(Label::textColourId, Colour(0xffF2F2F2));
    
    Label* advancedLabel = new Label();
    addAndMakeVisible(advancedLabel);
    advancedLabel->setText("ADVANCED CONTROLS", dontSendNotification);
    advancedLabel->setFont(Font(9.0f));
    advancedLabel->setColour(Label::textColourId, Colour(0xff8A8A8A));
    
    // Navigation buttons with yellow active state
    addAndMakeVisible(presetsButton);
    presetsButton.setButtonText("📦 Presets");
    presetsButton.onClick = [this]() { selectSidebarTab(0); };
    
    addAndMakeVisible(rigGenButton);
    rigGenButton.setButtonText("⚡ Rig Gen");
    rigGenButton.onClick = [this]() { selectSidebarTab(1); };
    
    addAndMakeVisible(libraryButton);
    libraryButton.setButtonText("☰ Library");
    libraryButton.onClick = [this]() { selectSidebarTab(2); };
    
    addAndMakeVisible(historyButton);
    historyButton.setButtonText("🕐 History");
    historyButton.onClick = [this]() { selectSidebarTab(3); };
    
    // Generate New Rig button at bottom
    addAndMakeVisible(generateRigButton);
    generateRigButton.setButtonText("✨ Generate New Rig");
    generateRigButton.setColour(TextButton::buttonColourId, Colour(0xffF5B400));
    generateRigButton.setColour(TextButton::textColourOffId, Colour(0xff000000));
    generateRigButton.onClick = [this]() { selectSidebarTab(1); };
    
    // Instantiate all sidebar browser panels
    presetBrowser = std::make_unique<PresetBrowser>(*this);
    addAndMakeVisible(presetBrowser.get());
    
    rigGenPanel = std::make_unique<RigGeneratorPanel>(*this);
    addChildComponent(rigGenPanel.get());
    
    libraryBrowser = std::make_unique<LibraryBrowser>(*this);
    addChildComponent(libraryBrowser.get());
    
    historyBrowser = std::make_unique<HistoryBrowser>(*this);
    addChildComponent(historyBrowser.get());
    
    //==========================================================================
    // MAIN WORKSPACE - Pedalboard with Grid Background
    //==========================================================================
    
    addAndMakeVisible(pedalboardStage);
    createDefaultPedalboard();
    
    // Instantiate cabinet simulator and add as child of pedalboardStage
    cabinetEditor = std::make_unique<CabinetEditor>(audioProcessor.getAPVTS());
    cabinetEditor->setVisible(false);
    pedalboardStage.addAndMakeVisible(cabinetEditor.get());
    
    // Set initial view and tab selection
    selectMainView(MainView::Pedalboard);
    selectSidebarTab(0);
    
    //==========================================================================
    // BOTTOM STATUS BAR - 56px
    //==========================================================================
    
    addAndMakeVisible(undoButton);
    undoButton.setButtonText("↶ Undo");
    undoButton.onClick = [this]() {
        DBG("Undo clicked");
    };
    
    addAndMakeVisible(redoButton);
    redoButton.setButtonText("↷ Redo");
    redoButton.onClick = [this]() {
        DBG("Redo clicked");
    };
    
    addAndMakeVisible(statusLabel);
    statusLabel.setText("STATUS: READY", dontSendNotification);
    statusLabel.setFont(Font(11.0f));
    statusLabel.setColour(Label::textColourId, Colour(0xff8A8A8A));
    
    addAndMakeVisible(cpuLabel);
    cpuLabel.setText("CPU LOAD: 0%", dontSendNotification);
    cpuLabel.setFont(Font(11.0f));
    cpuLabel.setColour(Label::textColourId, Colour(0xff8A8A8A));
    
    addAndMakeVisible(liveModeButton);
    liveModeButton.setButtonText("LIVE MODE");
    liveModeButton.setColour(TextButton::buttonColourId, Colour(0xff00FF88).withAlpha(0.3f));
    liveModeButton.setColour(TextButton::textColourOffId, Colour(0xff00FF88));
    liveModeButton.onClick = [this]() {
        DBG("Live Mode toggled");
    };
    
    //==========================================================================
    // RIG GENERATOR OVERLAY
    //==========================================================================
    
    rigGenerator = std::make_unique<RigGeneratorOverlay>();
    addChildComponent(rigGenerator.get());
    
    // Start timer for 60fps updates
    startTimer(16);  // ~60fps
}

GuitarPedalRackEditor::~GuitarPedalRackEditor()
{
    setLookAndFeel(nullptr);
}


void GuitarPedalRackEditor::paint(Graphics& g)
{
    // Main background - #070707 from spec
    g.fillAll(Colour(0xff070707));
    
    //==========================================================================
    // TOP TOOLBAR - #101010
    //==========================================================================
    g.setColour(Colour(0xff101010));
    g.fillRect(0, 0, getWidth(), 48);
    
    // Bottom border - #2A2A2A
    g.setColour(Colour(0xff2A2A2A));
    g.fillRect(0, 47, getWidth(), 1);
    
    //==========================================================================
    // LEFT SIDEBAR - #101010
    //==========================================================================
    g.setColour(Colour(0xff101010));
    g.fillRect(0, 48, 200, getHeight() - 88);
    
    // Right border - #2A2A2A
    g.setColour(Colour(0xff2A2A2A));
    g.fillRect(199, 48, 1, getHeight() - 88);
    
    //==========================================================================
    // MAIN WORKSPACE - Grid Background (Professional Pedalboard)
    //==========================================================================
    auto workspaceArea = Rectangle<int>(200, 48, getWidth() - 200, getHeight() - 88);
    
    // Dark background - #070707
    g.setColour(Colour(0xff070707));
    g.fillRect(workspaceArea);
    
    // Draw professional grid (like Helix/Neural DSP)
    g.setColour(Colour(0xff0f0f0f));
    int gridSize = 40;
    
    // Vertical lines
    for (int x = workspaceArea.getX(); x < workspaceArea.getRight(); x += gridSize)
    {
        g.drawVerticalLine(x, workspaceArea.getY(), workspaceArea.getBottom());
    }
    
    // Horizontal lines
    for (int y = workspaceArea.getY(); y < workspaceArea.getBottom(); y += gridSize)
    {
        g.drawHorizontalLine(y, workspaceArea.getX(), workspaceArea.getRight());
    }
    
    //==========================================================================
    // BOTTOM STATUS BAR - #101010
    //==========================================================================
    g.setColour(Colour(0xff101010));
    g.fillRect(0, getHeight() - 56, getWidth(), 56);
    
    // Top border - #2A2A2A
    g.setColour(Colour(0xff2A2A2A));
    g.fillRect(0, getHeight() - 57, getWidth(), 1);
}

void GuitarPedalRackEditor::resized()
{
    auto area = getLocalBounds();  // 1200x600
    
    //==========================================================================
    // TOP TOOLBAR - 72px
    //==========================================================================
    auto topBar = area.removeFromTop(48);
    
    // Left: Brand logo
    brandLabel.setBounds(topBar.removeFromLeft(200).reduced(10, 6));
    
    // Right: Save Preset button
    saveButton.setBounds(topBar.removeFromRight(95).reduced(6, 10));
    
    // Right: Menu buttons
    settingsButton.setBounds(topBar.removeFromRight(58).reduced(3, 12));
    editButton.setBounds(topBar.removeFromRight(48).reduced(3, 12));
    fileButton.setBounds(topBar.removeFromRight(48).reduced(3, 12));
    
    // Center: View switcher buttons
    int viewSwitcherW = 240;
    int viewSwitcherH = 26;
    auto viewSwitcherArea = Rectangle<int>(topBar.getCentreX() - viewSwitcherW / 2,
                                           topBar.getCentreY() - viewSwitcherH / 2,
                                           viewSwitcherW,
                                           viewSwitcherH);
    
    int btnW = viewSwitcherW / 3;
    pedalboardViewButton.setBounds(viewSwitcherArea.removeFromLeft(btnW));
    ampViewButton.setBounds(viewSwitcherArea.removeFromLeft(btnW));
    cabinetViewButton.setBounds(viewSwitcherArea);
    
    //==========================================================================
    // BOTTOM STATUS BAR - 56px
    //==========================================================================
    auto bottomBar = area.removeFromBottom(40);
    
    // Left: Undo, Redo
    undoButton.setBounds(bottomBar.removeFromLeft(55).reduced(6, 6));
    redoButton.setBounds(bottomBar.removeFromLeft(55).reduced(6, 6));
    
    bottomBar.removeFromLeft(14);  // Spacing
    
    // Center: Status
    statusLabel.setBounds(bottomBar.removeFromLeft(170).reduced(8, 10));
    
    bottomBar.removeFromLeft(10);  // Spacing
    
    // CPU Label
    cpuLabel.setBounds(bottomBar.removeFromLeft(80).reduced(8, 10));
    
    // Use remaining space for spacing
    bottomBar.removeFromLeft(jmax(0, bottomBar.getWidth() - 95));
    
    // Right: Live Mode button
    liveModeButton.setBounds(bottomBar.removeFromRight(jmin(95, bottomBar.getWidth())).reduced(8, 6));
    
    //==========================================================================
    // LEFT SIDEBAR - 300px
    //==========================================================================
    auto leftSidebar = area.removeFromLeft(200);
    
    // Browser title
    browserTitleLabel.setBounds(leftSidebar.removeFromTop(34).reduced(10, 6));
    
    // Navigation buttons - stacked
    int tabH = 28;
    presetsButton.setBounds(leftSidebar.removeFromTop(tabH).reduced(10, 2));
    rigGenButton.setBounds(leftSidebar.removeFromTop(tabH).reduced(10, 2));
    libraryButton.setBounds(leftSidebar.removeFromTop(tabH).reduced(10, 2));
    historyButton.setBounds(leftSidebar.removeFromTop(tabH).reduced(10, 2));
    
    // Generate New Rig at bottom
    generateRigButton.setBounds(leftSidebar.removeFromBottom(38).reduced(8, 5));
    
    // Active browser panel fills the rest of the sidebar
    auto browserArea = leftSidebar.reduced(7, 4);
    
    if (presetBrowser != nullptr) presetBrowser->setBounds(browserArea);
    if (rigGenPanel != nullptr) rigGenPanel->setBounds(browserArea);
    if (libraryBrowser != nullptr) libraryBrowser->setBounds(browserArea);
    if (historyBrowser != nullptr) historyBrowser->setBounds(browserArea);
    
    //==========================================================================
    // MAIN WORKSPACE - Pedalboard/Amp/Cabinet view container
    //==========================================================================
    pedalboardStage.setBounds(area);
    
    // Coordinates relative to pedalboardStage
    auto stageArea = pedalboardStage.getLocalBounds();
    
    if (currentMainView == MainView::Pedalboard)
    {
        if (!pedals.isEmpty())
        {
            int pedalWidth = 110;
            int pedalHeight = 165;
            int pedalSpacing = 26;
            int startX = 40;
            int pedalY = (stageArea.getHeight() - pedalHeight) / 2;
            
            for (int i = 0; i < pedals.size(); ++i)
            {
                if (pedals[i] != draggedPedal)
                {
                    int x = startX + i * (pedalWidth + pedalSpacing);
                    pedals[i]->setBounds(x, pedalY, pedalWidth, pedalHeight);
                }
            }
            
            // Amp module after pedals
            if (ampHead != nullptr)
            {
                int ampX = startX + pedals.size() * (pedalWidth + pedalSpacing);
                ampHead->setBounds(ampX, pedalY - 10, 230, 185);
            }
        }
    }
    else if (currentMainView == MainView::Amp)
    {
        // Standalone dedicated Amp view - centered
        if (ampHead != nullptr)
        {
            int ampW = 440;
            int ampH = 260;
            int ampX = (stageArea.getWidth() - ampW) / 2;
            int ampY = (stageArea.getHeight() - ampH) / 2;
            ampHead->setBounds(ampX, ampY, ampW, ampH);
        }
    }
    else if (currentMainView == MainView::Cabinet)
    {
        // Cabinet view
        if (cabinetEditor != nullptr)
        {
            cabinetEditor->setBounds(stageArea.reduced(20));
        }
    }
    
    //==========================================================================
    // RIG GENERATOR OVERLAY (full screen)
    //==========================================================================
    if (rigGenerator != nullptr)
        rigGenerator->setBounds(getLocalBounds());
}

void GuitarPedalRackEditor::timerCallback()
{
    // Thread-safe UI update (called on message thread, safe to update UI)
    try
    {
        // Update CPU display (read from atomic values)
        updateCpuDisplay();
        
        // Update status (simple UI text update)
        statusLabel.setText("STATUS: READY", dontSendNotification);
    }
    catch (...)
    {
        // Safety: catch any exceptions to prevent crashes
        DBG("Exception in timerCallback");
    }
}

//==============================================================================
// HELPER METHODS
//==============================================================================

void GuitarPedalRackEditor::updateCpuDisplay()
{
    cpuUsage = audioProcessor.getDspCpuUsage();
    
    cpuLabel.setText("CPU LOAD: " + String(cpuUsage, 1) + "%", dontSendNotification);
    
    // Change color based on load
    if (cpuUsage > 80.0f)
        cpuLabel.setColour(Label::textColourId, Colour(0xffff4444));
    else if (cpuUsage > 50.0f)
        cpuLabel.setColour(Label::textColourId, Colour(0xffffaa00));
    else
        cpuLabel.setColour(Label::textColourId, Colour(0xff888888));
}

void GuitarPedalRackEditor::createDefaultPedalboard()
{
    pedalboardStage.deleteAllChildren();
    pedals.clear();
    
    // Create functional stompbox pedals with APVTS connections
    
    // DEEP HEAT - Burnt Gold (Overdrive)
    auto* deepHeat = new FunctionalPedal(
        "DEEP HEAT",
        "Overdrive",
        Colour(0xffCC8844),
        audioProcessor.getAPVTS(),
        "deepHeatBypass",
        "deepHeatDrive",
        "deepHeatTone",
        "deepHeatLevel"
    );
    deepHeat->setKnobLabels("DRIVE", "TONE", "LEVEL");
    pedals.add(deepHeat);
    pedalboardStage.addAndMakeVisible(deepHeat);
    
    // VOID - Deep Purple (Reverb/Ambient)
    auto* voidPedal = new FunctionalPedal(
        "VOID",
        "Reverb",
        Colour(0xff8F2FFF),
        audioProcessor.getAPVTS(),
        "voidBypass",
        "voidSize",
        "voidDecay",
        "voidMix"
    );
    voidPedal->setKnobLabels("SIZE", "DECAY", "MIX");
    pedals.add(voidPedal);
    pedalboardStage.addAndMakeVisible(voidPedal);
    
    // PULSE - Dark Teal (Chorus/Modulation)
    auto* pulse = new FunctionalPedal(
        "PULSE",
        "Chorus",
        Colour(0xff228877),
        audioProcessor.getAPVTS(),
        "pulseBypass",
        "pulseRate",
        "pulseDepth",
        "pulseMix"
    );
    pulse->setKnobLabels("RATE", "DEPTH", "MIX");
    pedals.add(pulse);
    pedalboardStage.addAndMakeVisible(pulse);
    
    // ALPHA Amp - Functional amplifier head
    ampHead = std::make_unique<FunctionalAmpHead>(audioProcessor.getAPVTS());
    ampHead->setVisible(true);  // Visible by default
    pedalboardStage.addAndMakeVisible(ampHead.get());

    // Setup drag-and-drop callbacks for all pedals
    auto setupDragCallbacks = [this](FunctionalPedal* pedal) {
        pedal->onDragStartCallback = [this](FunctionalPedal* p) {
            draggedPedal = p;
            p->toFront(true);
        };
        
        pedal->onDragCallback = [this](FunctionalPedal* p, const MouseEvent& e) {
            auto mouseInParent = e.getEventRelativeTo(&pedalboardStage).position;
            int newX = mouseInParent.getX() - p->getWidth() / 2;
            newX = jlimit(0, pedalboardStage.getWidth() - p->getWidth(), newX);
            p->setTopLeftPosition(newX, p->getY());
            
            // Dynamic swap reordering
            int draggedIndex = pedals.indexOf(p);
            if (draggedIndex >= 0)
            {
                int pCenter = p->getX() + p->getWidth() / 2;
                for (int i = 0; i < pedals.size(); ++i)
                {
                    if (i == draggedIndex) continue;
                    auto* other = pedals[i];
                    int otherCenter = other->getX() + other->getWidth() / 2;
                    
                    if ((i < draggedIndex && pCenter < otherCenter) ||
                        (i > draggedIndex && pCenter > otherCenter))
                    {
                        pedals.swap(draggedIndex, i);
                        draggedIndex = i;
                        
                        // Layout other pedals around it
                        layoutPedalsExcept(p);
                        break;
                    }
                }
            }
        };
        
        pedal->onDragEndCallback = [this](FunctionalPedal* p) {
            draggedPedal = nullptr;
            resized();
            updateDspRouting();
        };
    };
    
    setupDragCallbacks(deepHeat);
    setupDragCallbacks(voidPedal);
    setupDragCallbacks(pulse);
}

void GuitarPedalRackEditor::showRigGenerator()
{
    if (rigGenerator != nullptr)
    {
        rigGenerator->setVisible(true);
        rigGenerator->toFront(true);
        rigGenerator->grabKeyboardFocus();
    }
}

void GuitarPedalRackEditor::hideRigGenerator()
{
    if (rigGenerator != nullptr)
    {
        rigGenerator->setVisible(false);
    }
}

void GuitarPedalRackEditor::updateSignalPath()
{
    signalPath.clear();
    
    if (pedals.size() < 2 && ampHead == nullptr)
        return;
    
    // Get the pedalboard stage offset
    auto stageX = pedalboardStage.getX();
    auto stageY = pedalboardStage.getY();
    
    // Draw curved signal flow between pedals
    if (pedals.size() >= 2)
    {
        for (int i = 0; i < pedals.size() - 1; ++i)
        {
            auto* pedal1 = pedals[i];
            auto* pedal2 = pedals[i + 1];
            
            float x1 = stageX + pedal1->getX() + pedal1->getWidth();
            float y1 = stageY + pedal1->getY() + pedal1->getHeight() / 2;
            float x2 = stageX + pedal2->getX();
            float y2 = stageY + pedal2->getY() + pedal2->getHeight() / 2;
            
            if (i == 0)
                signalPath.startNewSubPath(x1, y1);
            
            // Curved line
            float controlX = (x1 + x2) / 2;
            signalPath.cubicTo(controlX, y1, controlX, y2, x2, y2);
        }
    }
    
    // Connect last pedal to amp if amp exists
    if (ampHead != nullptr && !pedals.isEmpty())
    {
        auto* lastPedal = pedals.getLast();
        float x1 = stageX + lastPedal->getX() + lastPedal->getWidth() / 2;
        float y1 = stageY + lastPedal->getY() + lastPedal->getHeight();
        float x2 = stageX + ampHead->getX() + ampHead->getWidth() / 2;
        float y2 = stageY + ampHead->getY();
        
        if (pedals.size() == 1)
            signalPath.startNewSubPath(x1, y1);
        
        float controlY = (y1 + y2) / 2;
        signalPath.cubicTo(x1, controlY, x2, controlY, x2, y2);
    }
}

Colour GuitarPedalRackEditor::getCategoryColor(const String& category)
{
    if (category.contains("Rock"))
        return Colour(0xffff6b35);
    else if (category.contains("Metal"))
        return Colour(0xffdd2e44);
    else if (category.contains("Blues"))
        return Colour(0xff4fc3f7);
    else if (category.contains("Clean") || category.contains("Jazz"))
        return Colour(0xff66bb6a);
    else if (category.contains("Ambient"))
        return Colour(0xffab47bc);
    else
        return Colour(0xff78909c);
}

void GuitarPedalRackEditor::selectMainView(MainView view)
{
    currentMainView = view;
    
    // Update top bar view buttons selection styles
    pedalboardViewButton.setColour(TextButton::buttonColourId, view == MainView::Pedalboard ? Colour(0xff00E5FF) : Colour(0xff161616));
    pedalboardViewButton.setColour(TextButton::textColourOffId, view == MainView::Pedalboard ? Colour(0xff000000) : Colour(0xff8A8A8A));
    
    ampViewButton.setColour(TextButton::buttonColourId, view == MainView::Amp ? Colour(0xff00E5FF) : Colour(0xff161616));
    ampViewButton.setColour(TextButton::textColourOffId, view == MainView::Amp ? Colour(0xff000000) : Colour(0xff8A8A8A));
    
    cabinetViewButton.setColour(TextButton::buttonColourId, view == MainView::Cabinet ? Colour(0xff00E5FF) : Colour(0xff161616));
    cabinetViewButton.setColour(TextButton::textColourOffId, view == MainView::Cabinet ? Colour(0xff000000) : Colour(0xff8A8A8A));
    
    // Set visibility of workspace components
    bool isPedalboard = (view == MainView::Pedalboard);
    bool isAmp = (view == MainView::Amp);
    bool isCabinet = (view == MainView::Cabinet);
    
    for (auto* pedal : pedals)
        pedal->setVisible(isPedalboard);
        
    if (ampHead != nullptr)
        ampHead->setVisible(isAmp || isPedalboard);
        
    if (cabinetEditor != nullptr)
        cabinetEditor->setVisible(isCabinet);
        
    resized();
    repaint();
}

void GuitarPedalRackEditor::selectSidebarTab(int tabIndex)
{
    presetsButton.setColour(TextButton::buttonColourId, tabIndex == 0 ? Colour(0xffF5B400) : Colour(0xff161616));
    presetsButton.setColour(TextButton::textColourOffId, tabIndex == 0 ? Colour(0xff000000) : Colour(0xff8A8A8A));
    
    rigGenButton.setColour(TextButton::buttonColourId, tabIndex == 1 ? Colour(0xffF5B400) : Colour(0xff161616));
    rigGenButton.setColour(TextButton::textColourOffId, tabIndex == 1 ? Colour(0xff000000) : Colour(0xff8A8A8A));
    
    libraryButton.setColour(TextButton::buttonColourId, tabIndex == 2 ? Colour(0xffF5B400) : Colour(0xff161616));
    libraryButton.setColour(TextButton::textColourOffId, tabIndex == 2 ? Colour(0xff000000) : Colour(0xff8A8A8A));
    
    historyButton.setColour(TextButton::buttonColourId, tabIndex == 3 ? Colour(0xffF5B400) : Colour(0xff161616));
    historyButton.setColour(TextButton::textColourOffId, tabIndex == 3 ? Colour(0xff000000) : Colour(0xff8A8A8A));
    
    if (presetBrowser != nullptr) presetBrowser->setVisible(tabIndex == 0);
    if (rigGenPanel != nullptr) rigGenPanel->setVisible(tabIndex == 1);
    if (libraryBrowser != nullptr) libraryBrowser->setVisible(tabIndex == 2);
    if (historyBrowser != nullptr) historyBrowser->setVisible(tabIndex == 3);
    
    repaint();
}

void GuitarPedalRackEditor::layoutPedalsExcept(FunctionalPedal* exceptedPedal)
{
    auto stageArea = pedalboardStage.getLocalBounds();
    int pedalWidth = 160;
    int pedalHeight = 240;
    int pedalSpacing = 40;
    int startX = 60;
    int pedalY = (stageArea.getHeight() - pedalHeight) / 2;
    
    for (int i = 0; i < pedals.size(); ++i)
    {
        auto* pedal = pedals[i];
        if (pedal == exceptedPedal) continue;
        
        int x = startX + i * (pedalWidth + pedalSpacing);
        pedal->setBounds(x, pedalY, pedalWidth, pedalHeight);
    }
    
    // Amp module after pedals
    if (ampHead != nullptr)
    {
        int ampX = startX + pedals.size() * (pedalWidth + pedalSpacing);
        ampHead->setBounds(ampX, pedalY - 15, 340, 270);
    }
}

void GuitarPedalRackEditor::updateDspRouting()
{
    auto& signalChain = audioProcessor.getSignalChain();
    
    signalChain.clearChain();
    
    // 1. Gate is always first
    signalChain.addModule(std::make_unique<NoiseGate>());
    
    // 2. Add the user-reordered pedals
    for (auto* pedal : pedals)
    {
        String type = pedal->getEffectType();
        if (type == "Overdrive")
            signalChain.addModule(std::make_unique<TubeOverdrive>());
        else if (type == "Chorus")
            signalChain.addModule(std::make_unique<Chorus>());
        else if (type == "Reverb")
            signalChain.addModule(std::make_unique<ReverbEffect>());
    }
    
    // 3. Amp Simulator
    signalChain.addModule(std::make_unique<AmpSimulator>());
    
    // 4. Cabinet Simulator is always last
    signalChain.addModule(std::make_unique<CabinetIR>());
    
    // Reconnect the parameters
    audioProcessor.connectParametersToSignalChain();
    
    repaint();
}

void GuitarPedalRackEditor::updatePresetSelection()
{
    currentPresetName = audioProcessor.getCurrentPresetName();
    
    if (presetBrowser != nullptr)
        presetBrowser->updateSelection();
    if (libraryBrowser != nullptr)
        libraryBrowser->updateSelection();
    if (historyBrowser != nullptr)
        historyBrowser->updateSelection();
        
    if (historyBrowser != nullptr && currentPresetName.isNotEmpty())
        historyBrowser->addPresetToHistory(currentPresetName);
        
    repaint();
}

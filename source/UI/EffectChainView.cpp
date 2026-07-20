#include "EffectChainView.h"

EffectChainView::EffectChainView()
    : scrollBar(true) // true = vertical
{
    addAndMakeVisible(scrollBar);
    scrollBar.setAutoHide(false);
    scrollBar.addListener(this);
}

EffectChainView::~EffectChainView()
{
    scrollBar.removeListener(this);
}

void EffectChainView::paint(juce::Graphics& g)
{
    // Dark background with subtle texture
    g.fillAll(juce::Colour(0xff1a1a1a));
    
    // Grid pattern
    g.setColour(juce::Colour(0xff2a2a2a));
    for (int y = 0; y < getHeight(); y += 20)
    {
        g.drawLine(0, y - scrollOffset, getWidth(), y - scrollOffset, 1.0f);
    }
    
    // Draw insertion indicator if dragging
    if (insertionIndex >= 0)
    {
        float y = insertionIndex * (effectHeight + effectSpacing) - scrollOffset;
        
        g.setColour(juce::Colours::cyan.withAlpha(0.6f));
        g.fillRect(10.0f, y - 2.0f, getWidth() - scrollBar.getWidth() - 20.0f, 4.0f);
        
        // Arrow indicators
        juce::Path arrow;
        arrow.addTriangle(5, y, 15, y - 8, 15, y + 8);
        g.fillPath(arrow);
        
        arrow.clear();
        arrow.addTriangle(getWidth() - scrollBar.getWidth() - 5, y,
                         getWidth() - scrollBar.getWidth() - 15, y - 8,
                         getWidth() - scrollBar.getWidth() - 15, y + 8);
        g.fillPath(arrow);
    }
    
    // Empty state message
    if (effects.isEmpty())
    {
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.setFont(juce::Font(18.0f));
        g.drawText("No effects in chain\nClick '+' to add effects",
                   getLocalBounds(),
                   juce::Justification::centred);
    }
}

void EffectChainView::resized()
{
    auto bounds = getLocalBounds();
    
    // Position scrollbar
    scrollBar.setBounds(bounds.removeFromRight(20));
    
    updateScrollBar();
    updateEffectPositions();
}

void EffectChainView::addEffect(const juce::String& effectName, const juce::String& category)
{
    auto* slot = new EffectSlot(effectName, category);
    
    slot->onSelected = [this](EffectSlot* s) { handleEffectSelected(s); };
    slot->onBypassToggled = [this](EffectSlot* s) { handleEffectBypassToggled(s); };
    slot->onRemoveClicked = [this](EffectSlot* s) { handleEffectRemoved(s); };
    slot->onDragStarted = [this](EffectSlot* s, juce::Point<int> p) { handleDragStarted(s, p); };
    slot->onDragMoved = [this](EffectSlot* s, juce::Point<int> p) { handleDragMoved(s, p); };
    slot->onDragEnded = [this](EffectSlot* s) { handleDragEnded(s); };
    
    addAndMakeVisible(slot);
    effects.add(slot);
    
    updateScrollBar();
    updateEffectPositions();
    repaint();
}

void EffectChainView::removeEffect(int index)
{
    if (index >= 0 && index < effects.size())
    {
        effects.remove(index);
        
        if (selectedEffectIndex == index)
            selectedEffectIndex = -1;
        else if (selectedEffectIndex > index)
            selectedEffectIndex--;
        
        updateScrollBar();
        updateEffectPositions();
        repaint();
    }
}

void EffectChainView::removeEffect(EffectSlot* slot)
{
    int index = effects.indexOf(slot);
    if (index >= 0)
        removeEffect(index);
}

void EffectChainView::moveEffect(int fromIndex, int toIndex)
{
    if (fromIndex >= 0 && fromIndex < effects.size() &&
        toIndex >= 0 && toIndex <= effects.size() &&
        fromIndex != toIndex)
    {
        auto* slot = effects.removeAndReturn(fromIndex);
        
        // Adjust toIndex if necessary
        if (toIndex > fromIndex)
            toIndex--;
        
        effects.insert(toIndex, slot);
        
        // Update selected index
        if (selectedEffectIndex == fromIndex)
            selectedEffectIndex = toIndex;
        else if (selectedEffectIndex > fromIndex && selectedEffectIndex <= toIndex)
            selectedEffectIndex--;
        else if (selectedEffectIndex < fromIndex && selectedEffectIndex >= toIndex)
            selectedEffectIndex++;
        
        updateEffectPositions();
        repaint();
    }
}

void EffectChainView::clearAllEffects()
{
    effects.clear();
    selectedEffectIndex = -1;
    updateScrollBar();
    repaint();
}

EffectSlot* EffectChainView::getEffectSlot(int index) const
{
    if (index >= 0 && index < effects.size())
        return effects[index];
    return nullptr;
}

void EffectChainView::setSelectedEffect(int index)
{
    if (selectedEffectIndex != index)
    {
        // Deselect previous
        if (selectedEffectIndex >= 0 && selectedEffectIndex < effects.size())
            effects[selectedEffectIndex]->setSelected(false);
        
        selectedEffectIndex = index;
        
        // Select new
        if (selectedEffectIndex >= 0 && selectedEffectIndex < effects.size())
            effects[selectedEffectIndex]->setSelected(true);
        
        repaint();
    }
}

void EffectChainView::scrollBarMoved(juce::ScrollBar* bar, double newRangeStart)
{
    juce::ignoreUnused(bar);
    scrollOffset = (float)newRangeStart;
    updateEffectPositions();
    repaint();
}

void EffectChainView::updateScrollBar()
{
    float totalHeight = effects.size() * (effectHeight + effectSpacing);
    float visibleHeight = (float)getHeight();
    
    scrollBar.setRangeLimits(juce::Range<double>(0.0, juce::jmax(0.0, (double)(totalHeight - visibleHeight))));
    scrollBar.setCurrentRange(scrollOffset, visibleHeight, juce::dontSendNotification);
    scrollBar.setVisible(totalHeight > visibleHeight);
}

void EffectChainView::updateEffectPositions()
{
    int scrollBarWidth = scrollBar.isVisible() ? scrollBar.getWidth() : 0;
    int availableWidth = getWidth() - scrollBarWidth - 20;
    
    for (int i = 0; i < effects.size(); ++i)
    {
        auto* slot = effects[i];
        
        float y = i * (effectHeight + effectSpacing) - scrollOffset + 10;
        
        slot->setBounds(10, (int)y, availableWidth, (int)effectHeight);
        slot->setVisible(y + effectHeight > 0 && y < getHeight());
    }
}

void EffectChainView::handleEffectSelected(EffectSlot* slot)
{
    int index = effects.indexOf(slot);
    setSelectedEffect(index);
    
    if (onEffectSelected && index >= 0)
        onEffectSelected(index);
}

void EffectChainView::handleEffectBypassToggled(EffectSlot* slot)
{
    int index = effects.indexOf(slot);
    
    if (onEffectBypassToggled && index >= 0)
        onEffectBypassToggled(index);
}

void EffectChainView::handleEffectRemoved(EffectSlot* slot)
{
    int index = effects.indexOf(slot);
    
    if (onEffectRemoved && index >= 0)
    {
        onEffectRemoved(index);
        removeEffect(index);
    }
}

void EffectChainView::handleDragStarted(EffectSlot* slot, juce::Point<int>)
{
    draggedEffectIndex = effects.indexOf(slot);
}

void EffectChainView::handleDragMoved(EffectSlot* slot, juce::Point<int> position)
{
    if (draggedEffectIndex >= 0)
    {
        // Calculate insertion position
        auto globalPos = slot->getLocalPoint(this, position);
        insertionIndex = calculateInsertionIndex(globalPos);
        repaint();
        
        // Auto-scroll if near edges
        if (scrollBar.isVisible())
        {
            if (globalPos.getY() < 50)
                scrollBar.setCurrentRange(scrollOffset - 10, scrollBar.getCurrentRangeSize());
            else if (globalPos.getY() > getHeight() - 50)
                scrollBar.setCurrentRange(scrollOffset + 10, scrollBar.getCurrentRangeSize());
        }
    }
}

void EffectChainView::handleDragEnded(EffectSlot*)
{
    if (draggedEffectIndex >= 0 && insertionIndex >= 0)
    {
        if (draggedEffectIndex != insertionIndex &&
            draggedEffectIndex != insertionIndex - 1)
        {
            moveEffect(draggedEffectIndex, insertionIndex);
            
            if (onEffectMoved)
                onEffectMoved(draggedEffectIndex, insertionIndex);
        }
    }
    
    draggedEffectIndex = -1;
    insertionIndex = -1;
    repaint();
}

int EffectChainView::getEffectIndexAt(juce::Point<int> position) const
{
    for (int i = 0; i < effects.size(); ++i)
    {
        if (effects[i]->getBounds().contains(position))
            return i;
    }
    return -1;
}

int EffectChainView::calculateInsertionIndex(juce::Point<int> position) const
{
    float yWithScroll = position.getY() + scrollOffset;
    int index = (int)((yWithScroll + effectSpacing / 2) / (effectHeight + effectSpacing));
    return juce::jlimit(0, effects.size(), index);
}

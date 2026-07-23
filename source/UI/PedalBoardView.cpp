#include "PedalBoardView.h"
#include "Materials.h"
#include "../PluginProcessor.h"

PedalBoardView::PedalBoardView()
    : scrollBar(false) // false = horizontal
{
    addAndMakeVisible(scrollBar);
    scrollBar.setAutoHide(false);
    scrollBar.addListener(this);
    
    startTimerHz(60); // Smooth 60fps animation
}

PedalBoardView::~PedalBoardView()
{
    scrollBar.removeListener(this);
}

void PedalBoardView::paint(juce::Graphics& g)
{
    drawPedalBoard(g);
    drawSignalFlowIndicators(g);
    drawCables(g);
    
    if (insertionIndex >= 0)
    {
        drawInsertionIndicator(g);
    }
    
    // Empty state
    if (pedals.isEmpty())
    {
        g.setFont(juce::Font(22.0f, juce::Font::bold));
        g.setColour(juce::Colours::white.withAlpha(0.35f));
        g.drawText("YOUR PROFESSIONAL PEDALBOARD", 
                   getLocalBounds().removeFromTop(getHeight() / 2),
                   juce::Justification::centredBottom);
        
        g.setFont(juce::Font(14.0f));
        g.setColour(juce::Colours::white.withAlpha(0.25f));
        g.drawText("Click '+ ADD EFFECT' to add your first boutique pedal",
                   getLocalBounds().removeFromBottom(getHeight() / 2).removeFromTop(40),
                   juce::Justification::centredTop);
    }
}

void PedalBoardView::resized()
{
    auto bounds = getLocalBounds();
    
    // Position scrollbar at bottom
    scrollBar.setBounds(bounds.removeFromBottom(18).reduced(10, 0));
    
    updateScrollBar();
    updatePedalPositions();
}

void PedalBoardView::timerCallback()
{
    animationTime += 1.0f / 60.0f; // Track time for animations
    
    // Smooth scroll animation
    if (animatingScroll)
    {
        float diff = targetScrollOffset - scrollOffset;
        if (std::abs(diff) < 0.5f)
        {
            scrollOffset = targetScrollOffset;
            animatingScroll = false;
        }
        else
        {
            scrollOffset += diff * 0.2f; // Ease out
        }
        
        updatePedalPositions();
        updateCables();
        repaint();
    }
    
    // Animate cables (signal flow)
    cableManager.animate(1.0f / 60.0f);
    
    // Repaint for cable animations
    if (!pedals.isEmpty())
    {
        repaint();
    }
}

void PedalBoardView::addPedal(const juce::String& effectName, const juce::String& category)
{
    auto* pedal = new PedalSlot(effectName, category);
    
    pedal->onSelected = [this](PedalSlot* s) { handlePedalSelected(s); };
    pedal->onBypassToggled = [this](PedalSlot* s) { handleBypassToggled(s); };
    pedal->onRemoveClicked = [this](PedalSlot* s) { handlePedalRemoved(s); };
    pedal->onDragStarted = [this](PedalSlot* s, juce::Point<int> p) { handleDragStarted(s, p); };
    pedal->onDragMoved = [this](PedalSlot* s, juce::Point<int> p) { handleDragMoved(s, p); };
    pedal->onDragEnded = [this](PedalSlot* s) { handleDragEnded(s); };
    
    addAndMakeVisible(pedal);
    pedals.add(pedal);
    
    updateScrollBar();
    updatePedalPositions(true); // Animate
    updateCables();
    repaint();
}

void PedalBoardView::removePedal(int index)
{
    if (index >= 0 && index < pedals.size())
    {
        pedals.remove(index);
        
        if (selectedPedalIndex == index)
            selectedPedalIndex = -1;
        else if (selectedPedalIndex > index)
            selectedPedalIndex--;
        
        updateScrollBar();
        updatePedalPositions(true);
        updateCables();
        repaint();
    }
}

void PedalBoardView::removePedal(PedalSlot* slot)
{
    int index = pedals.indexOf(slot);
    if (index >= 0)
        removePedal(index);
}

void PedalBoardView::movePedal(int fromIndex, int toIndex)
{
    if (fromIndex >= 0 && fromIndex < pedals.size() &&
        toIndex >= 0 && toIndex <= pedals.size() &&
        fromIndex != toIndex)
    {
        auto* pedal = pedals.removeAndReturn(fromIndex);
        
        if (toIndex > fromIndex)
            toIndex--;
        
        pedals.insert(toIndex, pedal);
        
        if (selectedPedalIndex == fromIndex)
            selectedPedalIndex = toIndex;
        else if (selectedPedalIndex > fromIndex && selectedPedalIndex <= toIndex)
            selectedPedalIndex--;
        else if (selectedPedalIndex < fromIndex && selectedPedalIndex >= toIndex)
            selectedPedalIndex++;
        
        updatePedalPositions(true);
        updateCables();
        repaint();
    }
}

void PedalBoardView::clearAllPedals()
{
    pedals.clear();
    selectedPedalIndex = -1;
    updateScrollBar();
    repaint();
}

PedalSlot* PedalBoardView::getPedalSlot(int index) const
{
    if (index >= 0 && index < pedals.size())
        return pedals[index];
    return nullptr;
}

void PedalBoardView::setSelectedPedal(int index)
{
    if (selectedPedalIndex != index)
    {
        if (selectedPedalIndex >= 0 && selectedPedalIndex < pedals.size())
            pedals[selectedPedalIndex]->setSelected(false);
        
        selectedPedalIndex = index;
        
        if (selectedPedalIndex >= 0 && selectedPedalIndex < pedals.size())
            pedals[selectedPedalIndex]->setSelected(true);
        
        repaint();
    }
}

void PedalBoardView::scrollBarMoved(juce::ScrollBar* bar, double newRangeStart)
{
    juce::ignoreUnused(bar);
    targetScrollOffset = (float)newRangeStart;
    animatingScroll = true;
}

void PedalBoardView::updateScrollBar()
{
    int topCount = juce::jmin(5, pedals.size());
    int bottomCount = juce::jmax(0, pedals.size() - 5);
    int maxRowSize = juce::jmax(topCount, bottomCount);
    
    float totalWidth = 100.0f + maxRowSize * (pedalWidth + pedalSpacing) + 100.0f;
    float visibleWidth = (float)getWidth();
    
    scrollBar.setRangeLimits(juce::Range<double>(0.0, juce::jmax(0.0, (double)(totalWidth - visibleWidth))));
    scrollBar.setCurrentRange(scrollOffset, visibleWidth, juce::dontSendNotification);
    scrollBar.setVisible(totalWidth > visibleWidth);
}

void PedalBoardView::updatePedalPositions(bool animate)
{
    float startX = 100.0f; // Space for input rail
    float topRowY = 30.0f;
    float bottomRowY = 265.0f;
    
    for (int i = 0; i < pedals.size(); ++i)
    {
        auto* pedal = pedals[i];
        
        float xPos = 0.0f;
        float yPos = 0.0f;
        
        if (i < 5)
        {
            xPos = startX + i * (pedalWidth + pedalSpacing) - scrollOffset;
            yPos = topRowY;
        }
        else
        {
            xPos = startX + (i - 5) * (pedalWidth + pedalSpacing) - scrollOffset;
            yPos = bottomRowY;
        }
        
        auto targetBounds = juce::Rectangle<int>((int)xPos, (int)yPos, (int)pedalWidth, (int)pedalHeight);
        
        if (animate && xPos > -pedalWidth && xPos < getWidth())
        {
            auto currentBounds = pedal->getBounds();
            if (currentBounds != targetBounds)
            {
                pedal->setBounds(targetBounds);
            }
        }
        else
        {
            pedal->setBounds(targetBounds);
        }
        
        pedal->setVisible(xPos + pedalWidth > 0 && xPos < getWidth());
    }
}

void PedalBoardView::drawPedalBoard(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // ─── OUTER FRAME ────────────────────────────────────────────────────────
    // Very dark gunmetal background
    g.setColour(juce::Colour(0xff0d0d0d));
    g.fillAll();
    
    // ─── WALNUT WOOD SHELF ──────────────────────────────────────────────────
    // Two rows of pedals sit on a warm walnut rack shelf
    float railY1 = 18.0f;   // top of shelf 1
    float railH  = 235.0f;  // shelf height
    float railY2 = railY1 + railH + 10.0f;  // shelf 2 starts here
    
    auto drawWoodShelf = [&](float sy, float sh)
    {
        auto shelf = juce::Rectangle<float>(65.0f, sy, bounds.getWidth() - 130.0f, sh);
        
        // Base wood color – warm dark walnut
        juce::ColourGradient wood(
            juce::Colour(0xff2c1a0a), shelf.getCentreX(), shelf.getY(),
            juce::Colour(0xff1a0e05), shelf.getCentreX(), shelf.getBottom(), false);
        g.setGradientFill(wood);
        g.fillRoundedRectangle(shelf, 4.0f);
        
        // Wood grain lines (horizontal)
        juce::Random rng(99887);
        for (int i = 0; i < 80; ++i)
        {
            float gy = shelf.getY() + rng.nextFloat() * shelf.getHeight();
            float gh = rng.nextFloat() * 1.4f + 0.3f;
            float ga = rng.nextFloat() * 0.10f + 0.02f;
            g.setColour(juce::Colour(0xff100804).withAlpha(ga));
            g.fillRect(shelf.getX(), gy, shelf.getWidth(), gh);
        }
        
        // Metal bar inset (the black slatted rail pedals clip onto)
        auto rail = shelf.reduced(0, 12).withY(shelf.getCentreY() - 28).withHeight(56);
        juce::ColourGradient metalGrad(
            juce::Colour(0xff1c1c1c), rail.getCentreX(), rail.getY(),
            juce::Colour(0xff0d0d0d), rail.getCentreX(), rail.getBottom(), false);
        g.setGradientFill(metalGrad);
        g.fillRoundedRectangle(rail, 3.0f);
        
        // Slat shadow lines
        for (int s = 0; s < 4; ++s)
        {
            float lx = rail.getX() + 40.0f + s * (rail.getWidth() / 4.0f);
            g.setColour(juce::Colours::black.withAlpha(0.5f));
            g.fillRect(lx, rail.getY(), 2.0f, rail.getHeight());
        }
        
        g.setColour(juce::Colours::white.withAlpha(0.08f));
        g.drawRoundedRectangle(rail, 3.0f, 1.0f);
        
        // Shelf highlight on top edge
        g.setColour(juce::Colours::white.withAlpha(0.10f));
        g.fillRoundedRectangle(shelf.removeFromTop(3), 2.0f);
        
        // Shelf border
        g.setColour(juce::Colours::black.withAlpha(0.6f));
        g.drawRoundedRectangle(juce::Rectangle<float>(65.0f, sy, bounds.getWidth() - 130.0f, sh), 4.0f, 1.5f);
    };
    
    drawWoodShelf(railY1, railH);
    drawWoodShelf(railY2, railH);
    
    // ─── SIDE RAILS (INPUT / OUTPUT) ─────────────────────────────────────
    float railW = 60.0f;
    float rh    = bounds.getHeight() - 35.0f;
    
    auto drawSideRail = [&](float rx, const juce::String& label, bool isInput)
    {
        auto rBounds = juce::Rectangle<float>(rx, 15.0f, railW, rh);
        
        // Rail body – brushed dark aluminum
        juce::ColourGradient rg(
            juce::Colour(0xff252525), rBounds.getCentreX(), rBounds.getY(),
            juce::Colour(0xff141414), rBounds.getCentreX(), rBounds.getBottom(), false);
        g.setGradientFill(rg);
        g.fillRoundedRectangle(rBounds, 5.0f);
        
        // Brushed texture
        Materials::drawBrushedAluminum(g, rBounds, juce::Colour(0xff252525), true, 0.12f);
        
        // Border
        g.setColour(juce::Colours::white.withAlpha(0.14f));
        g.drawRoundedRectangle(rBounds, 5.0f, 1.2f);
        g.setColour(juce::Colours::black.withAlpha(0.4f));
        g.drawRoundedRectangle(rBounds.reduced(1), 5.0f, 0.8f);
        
        // Label (vertical text)
        g.saveState();
        g.setFont(juce::Font(9.5f, juce::Font::bold));
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        float cx = rBounds.getCentreX(), ty = rBounds.getY() + 22;
        g.addTransform(juce::AffineTransform::translation(cx, ty));
        g.drawText(label, -40, -8, 80, 16, juce::Justification::centred);
        g.restoreState();
        
        // 1/4" Jack socket
        float jy = rBounds.getCentreY() - 5.0f;
        juce::Point<float> jp(rBounds.getCentreX(), jy);
        
        g.setColour(juce::Colours::black);
        g.fillEllipse(jp.x - 11, jp.y - 11, 22, 22);
        
        // Hex nut ring
        g.setColour(juce::Colour(0xff888888));
        juce::Path hex;
        for (int i = 0; i < 6; ++i)
        {
            float a = i * juce::MathConstants<float>::pi / 3.0f;
            float hx = jp.x + std::cos(a) * 9, hy = jp.y + std::sin(a) * 9;
            i == 0 ? hex.startNewSubPath(hx, hy) : hex.lineTo(hx, hy);
        }
        hex.closeSubPath();
        g.fillPath(hex);
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.strokePath(hex, juce::PathStrokeType(1.0f));
        
        // Inner barrel
        g.setColour(juce::Colour(0xffbbbbbb));
        g.fillEllipse(jp.x - 5, jp.y - 5, 10, 10);
        g.setColour(juce::Colour(0xff1a1a1a));
        g.fillEllipse(jp.x - 3, jp.y - 3, 6, 6);
        
        // Level meter (5 segments)
        float peakLvl = (processor != nullptr)
            ? (isInput ? processor->getInputLevel() : processor->getOutputLevel())
            : 0.0f;
        
        float meterY = rBounds.getBottom() - 120.0f;
        float segH = 9.0f, segW = 7.0f, segGap = 2.5f;
        float mx = rBounds.getCentreX() - segW * 0.5f;
        float thresholds[] = { 0.05f, 0.20f, 0.45f, 0.70f, 0.90f };
        
        for (int i = 0; i < 5; ++i)
        {
            auto seg = juce::Rectangle<float>(mx,
                meterY + (4 - i) * (segH + segGap), segW, segH);
            bool active = peakLvl >= thresholds[i];
            juce::Colour sc = active
                ? (i >= 4 ? juce::Colours::red : (i >= 3 ? juce::Colours::orange : juce::Colour(0xff39ff14)))
                : juce::Colour(0xff112211);
            g.setColour(sc);
            g.fillRoundedRectangle(seg, 1.5f);
            if (active)
            {
                g.setColour(sc.withAlpha(0.22f));
                g.fillRoundedRectangle(seg.expanded(1.5f), 2.0f);
            }
        }
        
        // Corner screws
        auto ds = [&](float sx, float sy){ Materials::drawStainlessSteelScrew(g, {sx, sy}, 8.0f); };
        ds(rBounds.getX() + 10, rBounds.getY() + 10);
        ds(rBounds.getRight() - 10, rBounds.getY() + 10);
        ds(rBounds.getX() + 10, rBounds.getBottom() - 10);
        ds(rBounds.getRight() - 10, rBounds.getBottom() - 10);
    };
    
    drawSideRail(5.0f,  "INPUT",  true);
    drawSideRail(bounds.getWidth() - 65.0f, "OUTPUT", false);
    
    // ─── VIGNETTE ──────────────────────────────────────────────────────────
    juce::ColourGradient vig(
        juce::Colours::transparentBlack, bounds.getCentreX(), bounds.getCentreY(),
        juce::Colours::black.withAlpha(0.22f), 0, 0, true);
    g.setGradientFill(vig);
    g.fillAll();
}

void PedalBoardView::drawCables(juce::Graphics& g)
{
    // Use professional cable manager to draw all cables
    cableManager.drawAll(g);
}

void PedalBoardView::updateCables()
{
    // Clear old cables
    cableManager.clearAll();
    
    if (pedals.isEmpty())
        return;
        
    auto inputRailJack = juce::Point<float>(47.5f, 210.0f);
    auto outputRailJack = juce::Point<float>(getWidth() - 47.5f, 210.0f);
    
    // Connection 1: Input Rail -> Pedal 0
    auto* firstPedal = pedals[0];
    auto firstInputPos = firstPedal->getInputJackPosition();
    cableManager.updateConnection(-1, 0, inputRailJack, firstInputPos, firstPedal->getCategoryColor());
    cableManager.setActive(-1, 0, !firstPedal->isBypassed());
    cableManager.setSignalLevel(-1, 0, firstPedal->isBypassed() ? 0.0f : 0.7f);
    
    // Create cables between adjacent pedals
    for (int i = 0; i < pedals.size() - 1; ++i)
    {
        auto* pedal1 = pedals[i];
        auto* pedal2 = pedals[i + 1];
        
        auto outputPos = pedal1->getOutputJackPosition();
        auto inputPos = pedal2->getInputJackPosition();
        
        auto cableColor = pedal1->getCategoryColor();
        cableManager.updateConnection(i, i + 1, outputPos, inputPos, cableColor);
        
        bool isActive = !pedal1->isBypassed();
        cableManager.setActive(i, i + 1, isActive);
        
        float signalLevel = isActive ? 0.7f : 0.0f;
        cableManager.setSignalLevel(i, i + 1, signalLevel);
    }
    
    // Connection Last: Last Pedal -> Output Rail
    int lastIdx = pedals.size() - 1;
    auto* lastPedal = pedals[lastIdx];
    auto lastOutputPos = lastPedal->getOutputJackPosition();
    cableManager.updateConnection(lastIdx, 999, lastOutputPos, outputRailJack, lastPedal->getCategoryColor());
    cableManager.setActive(lastIdx, 999, !lastPedal->isBypassed());
    cableManager.setSignalLevel(lastIdx, 999, lastPedal->isBypassed() ? 0.0f : 0.7f);
}

void PedalBoardView::drawSignalFlowIndicators(juce::Graphics& g)
{
    // Custom rails and cables handle signal flow indicators visually
}

void PedalBoardView::drawInsertionIndicator(juce::Graphics& g)
{
    if (insertionIndex < 0) return;

    float topRowY    = 30.0f;
    float bottomRowY = 265.0f;
    float startX     = 100.0f;

    float rowY;
    int   idxInRow;
    if (insertionIndex < 5)
    {
        rowY      = topRowY;
        idxInRow  = insertionIndex;
    }
    else
    {
        rowY      = bottomRowY;
        idxInRow  = insertionIndex - 5;
    }

    float xPos = startX + idxInRow * (pedalWidth + pedalSpacing) - scrollOffset;

    g.setColour(juce::Colour(0xff00E5FF).withAlpha(0.85f));
    g.fillRect(xPos - 3.0f, rowY, 6.0f, pedalHeight);

    juce::Path arrow;
    arrow.addTriangle(xPos, rowY - 5, xPos - 8, rowY + 10, xPos + 8, rowY + 10);
    g.fillPath(arrow);

    arrow.clear();
    arrow.addTriangle(xPos, rowY + pedalHeight + 5,
                      xPos - 8, rowY + pedalHeight - 10,
                      xPos + 8, rowY + pedalHeight - 10);
    g.fillPath(arrow);
}

void PedalBoardView::handlePedalSelected(PedalSlot* slot)
{
    int index = pedals.indexOf(slot);
    setSelectedPedal(index);
    
    if (onPedalSelected && index >= 0)
        onPedalSelected(index);
}

void PedalBoardView::handleBypassToggled(PedalSlot* slot)
{
    int index = pedals.indexOf(slot);
    
    if (onPedalBypassToggled && index >= 0)
        onPedalBypassToggled(index);
    
    updateCables(); // Update cable appearance based on bypass state
    repaint(); // Redraw cables
}

void PedalBoardView::handlePedalRemoved(PedalSlot* slot)
{
    int index = pedals.indexOf(slot);
    
    if (onPedalRemoved && index >= 0)
    {
        onPedalRemoved(index);
        removePedal(index);
    }
}

void PedalBoardView::handleDragStarted(PedalSlot* slot, juce::Point<int>)
{
    draggedPedalIndex = pedals.indexOf(slot);
}

void PedalBoardView::handleDragMoved(PedalSlot* slot, juce::Point<int> position)
{
    if (draggedPedalIndex >= 0)
    {
        auto globalPos = slot->getLocalPoint(this, position);
        insertionIndex = calculateInsertionIndex(globalPos);
        repaint();
        
        // Auto-scroll when near edges
        if (scrollBar.isVisible())
        {
            if (globalPos.getX() < 80)
            {
                targetScrollOffset = juce::jmax(0.0f, scrollOffset - 15.0f);
                animatingScroll = true;
            }
            else if (globalPos.getX() > getWidth() - 80)
            {
                targetScrollOffset = juce::jmin((float)scrollBar.getMaximumRangeLimit(), scrollOffset + 15.0f);
                animatingScroll = true;
            }
        }
    }
}

void PedalBoardView::handleDragEnded(PedalSlot*)
{
    if (draggedPedalIndex >= 0 && insertionIndex >= 0)
    {
        if (draggedPedalIndex != insertionIndex &&
            draggedPedalIndex != insertionIndex - 1)
        {
            movePedal(draggedPedalIndex, insertionIndex);
            
            if (onPedalMoved)
                onPedalMoved(draggedPedalIndex, insertionIndex);
        }
    }
    
    draggedPedalIndex = -1;
    insertionIndex = -1;
    repaint();
}

int PedalBoardView::getPedalIndexAt(juce::Point<int> position) const
{
    for (int i = 0; i < pedals.size(); ++i)
    {
        if (pedals[i]->getBounds().contains(position))
            return i;
    }
    return -1;
}

int PedalBoardView::calculateInsertionIndex(juce::Point<int> position) const
{
    // Determine which row based on Y position
    float topRowY    = 30.0f;
    float bottomRowY = 265.0f;
    float midY       = (topRowY + pedalHeight + bottomRowY) * 0.5f;

    int rowOffset = (position.getY() > midY && pedals.size() > 5) ? 5 : 0;

    float xWithScroll = (float)position.getX() + scrollOffset - 100.0f; // subtract startX
    int idxInRow = (int)((xWithScroll + pedalSpacing * 0.5f) / (pedalWidth + pedalSpacing));
    idxInRow = juce::jlimit(0, 5, idxInRow); // max 5 per row

    return juce::jlimit(0, pedals.size(), rowOffset + idxInRow);
}

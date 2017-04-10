#include "GAFPrecompiled.h"
#include "GAFBoxLayoutView.h"
#include "GAFObject.h"
#include "GAFTimeline.h"
#include "GAFSubobjectState.h"
#include "GAFAnimationFrame.h"
#include <GAFUtils.h>

NS_GAF_BEGIN
GAFBoxLayoutView::GAFBoxLayoutView()
    : m_direction(Direction::horizontal)
    , m_gap(0.f)
    , m_marginTop(0.f)
    , m_marginRight(0.f)
    , m_marginBottom(0.f)
    , m_marginLeft(0.f)
    , m_gapMode(MarginGapMode::percents)
    , m_marginMode(MarginGapMode::percents)
    , m_horizontalAlign(HorizontalAlign::center)
    , m_verticalAlign(VerticalAlign::center)
    , m_dynamicContentBounds(cocos2d::Rect::ZERO)
    , m_dynamicContentBoundsDirty(false)
    , m_layoutDirty(true)
{
}

GAFBoxLayoutView* GAFBoxLayoutView::create(GAFAssetConstPtr anAsset, GAFTimelineConstPtr timeline)
{
    GAFBoxLayoutView* ret = new GAFBoxLayoutView();

    if (ret && ret->init(anAsset, timeline))
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_RELEASE(ret);
    return nullptr;
}

bool GAFBoxLayoutView::init(GAFAssetConstPtr anAnimationData, GAFTimelineConstPtr timeline)
{
    bool ret = GAFLayoutView::init(anAnimationData, timeline);

    if (ret)
        m_container->setLocalZOrder(-1);

    return ret;
}

cocos2d::Rect GAFBoxLayoutView::getInternalBoundingBox() const
{
    auto result = GAFLayoutView::getInternalBoundingBox();
    result = result.unionWithRect(flashBoundsToCocos(getDynamicContentBounds()));

    return result;
}

void GAFBoxLayoutView::addChild(Node* child, int localZOrder, int tag)
{
    GAFLayoutView::addChild(child, localZOrder, tag);

    m_layoutDirty = m_dynamicContentBoundsDirty = true;
}

void GAFBoxLayoutView::addChild(Node* child, int localZOrder, const std::string& name)
{
    GAFLayoutView::addChild(child, localZOrder, name);

    m_layoutDirty = m_dynamicContentBoundsDirty = true;
}

void GAFBoxLayoutView::removeChild(Node * child, bool cleanup)
{
    GAFLayoutView::removeChild(child, cleanup);

    m_layoutDirty = m_dynamicContentBoundsDirty = true;
}

void GAFBoxLayoutView::removeAllChildrenWithCleanup(bool cleanup)
{
    GAFLayoutView::removeAllChildrenWithCleanup(cleanup);

    m_layoutDirty = m_dynamicContentBoundsDirty = true;
}

void GAFBoxLayoutView::setExternalTransform(const cocos2d::AffineTransform& transform)
{
    GAFLayoutView::setExternalTransform(transform);

    m_layoutDirty = m_dynamicContentBoundsDirty = _transformDirty;
}

bool GAFBoxLayoutView::processOwnCustomProperties(const CustomPropertiesMap_t& customProperties)
{
    bool cpChanged = GAFLayoutView::processOwnCustomProperties(customProperties);

    auto direction = Direction::toEnum(customProperties.at("direction"));
    auto horizontalAlign = HorizontalAlign::toEnum(customProperties.at("horizontalAlign"));
    auto verticalAlign = VerticalAlign::toEnum(customProperties.at("verticalAlign"));
    auto gap = std::stof(customProperties.at("gap"));
    auto marginTop = std::stof(customProperties.at("marginTop"));
    auto marginRight = std::stof(customProperties.at("marginRight"));
    auto marginBottom = std::stof(customProperties.at("marginBottom"));
    auto marginLeft = std::stof(customProperties.at("marginLeft"));

    auto gapMode = MarginGapMode::toEnum(customProperties.at("gapMode"));
    auto marginMode = MarginGapMode::toEnum(customProperties.at("marginMode"));

    if (m_direction != direction)
    {
        m_direction = direction;
        cpChanged = true;
    }

    if (m_horizontalAlign != horizontalAlign)
    {
        m_horizontalAlign = horizontalAlign;
        cpChanged = true;
    }

    if (m_verticalAlign != verticalAlign)
    {
        m_verticalAlign = verticalAlign;
        cpChanged = true;
    }

    if (m_gap != gap)
    {
        m_gap = gap;
        cpChanged = true;
    }

    if (m_marginTop != marginTop)
    {
        m_marginTop = marginTop;
        cpChanged = true;
    }

    if (m_marginRight != marginRight)
    {
        m_marginRight = marginRight;
        cpChanged = true;
    }

    if (m_marginBottom != marginBottom)
    {
        m_marginBottom = marginBottom;
        cpChanged = true;
    }

    if (m_marginLeft != marginLeft)
    {
        m_marginLeft = marginLeft;
        cpChanged = true;
    }

    if (m_gapMode != gapMode)
    {
        m_gapMode = gapMode;
        cpChanged = true;
    }

    if (m_marginMode != marginMode)
    {
        m_marginMode = marginMode;
        cpChanged = true;
    }

    if (cpChanged)
        m_layoutDirty = m_dynamicContentBoundsDirty = true;

    return cpChanged;
}

void GAFBoxLayoutView::processStates(cocos2d::Node* out, uint32_t frameIndex, GAFAnimationFrameConstPtr frame)
{
    m_layoutDirty = true; // allways update chindren position

    const GAFAnimationFrame::SubobjectStates_t& states = frame->getObjectStates();
    ObjectsStatesPositions_t childrenToAlign;
    for (GAFSubobjectStateConstPtr state : states)
    {
        GAFObject* child = m_displayList[state->objectIdRef];

        if (child && state->isVisible())
        {
            cocos2d::AffineTransform stateMatrix = state->affineTransform;
            childrenToAlign.emplace_back(child, state, stateMatrix);

            child->setLastVisibleInFrame(frameIndex + 1);
        }
    }

    processEmbededChildren(out, childrenToAlign);
    childrenToAlign.clear();

    for (auto child : _children)
    {
        GAFObject* subObject = dynamic_cast<GAFObject*>(child);

        if (subObject && subObject->isVisible())
            childrenToAlign.emplace_back(subObject, nullptr, cocos2d::AffineTransformMakeIdentity());
    }

    processDynamicChildren(out, childrenToAlign);

    m_layoutDirty = false;
}

cocos2d::AffineTransform& GAFBoxLayoutView::processGAFTimelineStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx, const CustomPropertiesMap_t& customProperties)
{
    if (m_layoutDirty)
        return GAFLayoutView::processGAFTimelineStateTransform(child, mtx, customProperties);

    return mtx;
}

cocos2d::AffineTransform& GAFBoxLayoutView::processGAFImageStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx)
{
    if (m_layoutDirty)
        return GAFLayoutView::processGAFImageStateTransform(child, mtx);

    return mtx;
}

cocos2d::AffineTransform& GAFBoxLayoutView::processGAFTextFieldStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx)
{
    if (m_layoutDirty)
        return GAFLayoutView::processGAFTextFieldStateTransform(child, mtx);

    return mtx;
}

cocos2d::AffineTransform& GAFBoxLayoutView::addAdditionalTransformations(cocos2d::AffineTransform& mtx) const
{
    if (m_scaleAlignedChildren && m_layoutDirty)
    {
        cocos2d::AffineTransform scaleMtx = cocos2d::AffineTransformMakeIdentity();
        cocos2d::Vec2 fittingScale(getFittingScale());
        scaleMtx = cocos2d::AffineTransformScale(scaleMtx, fittingScale.x, fittingScale.y);
        affineTransformSetFrom(mtx, AffineTransformConcat(mtx, scaleMtx));
    }
    
    return mtx;
}

void GAFBoxLayoutView::processEmbededChildren(cocos2d::Node* out, ObjectsStatesPositions_t& objects)
{
    bool tempLayoutDirty = m_layoutDirty;

    m_layoutDirty = true;
    for (auto& childAndMtx : objects)
    {
        GAFObject* child = std::get<0>(childAndMtx);
        const GAFSubobjectStateConstPtr state = std::get<1>(childAndMtx);
        cocos2d::AffineTransform& stateMatrix = std::get<2>(childAndMtx);

        assert(state);
        preProcessGAFObject(out, child, state, stateMatrix);
    }

    calculatePositions(objects);

    for (auto& childAndMtx : objects)
    {
        GAFObject* child = std::get<0>(childAndMtx);
        const GAFSubobjectStateConstPtr state = std::get<1>(childAndMtx);
        cocos2d::AffineTransform& stateMatrix = std::get<2>(childAndMtx);

        if (child->getCharType() == GAFCharacterType::Timeline)
        {
            processGAFTimeline(out, child, state, stateMatrix);
        }
        else if (child->getCharType() == GAFCharacterType::Texture)
        {
            processGAFImage(out, child, state, stateMatrix);
        }
        else if (child->getCharType() == GAFCharacterType::TextField)
        {
            processGAFTextField(out, child, state, stateMatrix);
        }
    }

    m_layoutDirty = tempLayoutDirty;
}

void GAFBoxLayoutView::processDynamicChildren(cocos2d::Node* out, ObjectsStatesPositions_t& objects)
{
    if (m_layoutDirty)
        calculatePositions(objects);

    for (auto& childAndMtx : objects)
    {
        GAFObject* child = std::get<0>(childAndMtx);
        cocos2d::AffineTransform& stateMatrix = std::get<2>(childAndMtx);
        if (!m_layoutDirty)
            stateMatrix = child->getExternalTransform();

        if (child->getCharType() == GAFCharacterType::Timeline)
        {
            CustomPropertiesMap_t props;

            processGAFTimelineStateTransform(child, stateMatrix, props);
        }
        else if (child->getCharType() == GAFCharacterType::Texture)
        {
            processGAFImageStateTransform(child, stateMatrix);
        }
        else if (child->getCharType() == GAFCharacterType::TextField)
        {
            processGAFTextFieldStateTransform(child, stateMatrix);
        }

        child->setExternalTransform(stateMatrix);
    }
}

void GAFBoxLayoutView::calculatePositions(ObjectsStatesPositions_t& objects) const
{
    const cocos2d::Rect bb = getFlashInternalBoundingBox();
    cocos2d::Rect childrenBB = cocos2d::Rect::ZERO;

    const cocos2d::Point fittingScale = getFittingScale();
    const cocos2d::Point gapScale = getGapScale();
    const cocos2d::Point marginScale = getMarginScale();

    cocos2d::Size maxLineSize;
    switch (m_direction)
    {
    case Direction::tiledByWidth:
        maxLineSize.setSize(bb.size.width - (m_marginLeft + m_marginRight) * marginScale.x, std::numeric_limits<float>::max());
        break;
    case Direction::tiledByHeight:
        maxLineSize.setSize(std::numeric_limits<float>::max(), bb.size.height - (m_marginTop + m_marginBottom) * marginScale.y);
        break;
    default:
        maxLineSize.setSize(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    }

    cocos2d::Size firstLineChildSize = cocos2d::Size::ZERO;
    cocos2d::Point currPos = cocos2d::Point::ZERO;
    cocos2d::Point  currMinPos = cocos2d::Point::ZERO;
    cocos2d::Size currLineSize = cocos2d::Size::ZERO;
    cocos2d::Point  childTopLeft = cocos2d::Point::ZERO;
    cocos2d::Size contentSize = cocos2d::Size::ZERO;

    std::vector<cocos2d::AffineTransform*> lineChildren;

    auto alignLine = [](std::vector<cocos2d::AffineTransform*>& lineChildren, cocos2d::Point lineOffset)
    {
        for (auto mtx : lineChildren)
        {
            mtx->tx += lineOffset.x;
            mtx->ty += lineOffset.y;
        }
    };

    auto calculateLineOffset = [this, &bb, &currPos, &currMinPos, &currLineSize, &marginScale](bool isHorizontal)
    {
        cocos2d::Point rowOffset;
        if (isHorizontal)
        {
            if (m_horizontalAlign == HorizontalAlign::right)
                rowOffset.x = bb.getMaxX() - m_marginRight * marginScale.x - currLineSize.width;
            else if (m_horizontalAlign == HorizontalAlign::center)
                rowOffset.x = bb.getMinX() + (bb.size.width - currLineSize.width) / 2;
            else
                rowOffset.x = bb.getMinX() + m_marginLeft * marginScale.x;

            rowOffset.y = currPos.y - currMinPos.y;
        }
        else
        {
            if (m_verticalAlign == VerticalAlign::bottom)
                rowOffset.y = bb.getMaxY() - m_marginBottom * marginScale.y - currLineSize.height;
            else if (m_verticalAlign == VerticalAlign::center)
                rowOffset.y = bb.getMinY() + (bb.size.height - currLineSize.height) / 2;
            else
                rowOffset.y = bb.getMinY() + m_marginTop * marginScale.y;

            rowOffset.x = currPos.x - currMinPos.x;
        }

        return rowOffset;
    };

    bool firstChild = true;
    for (auto& childAndMtx : objects)
    {
        GAFObject* child = std::get<0>(childAndMtx);
        cocos2d::AffineTransform& stateMatrix = std::get<2>(childAndMtx);

        auto lastChild = (child == std::get<0>(*std::prev(objects.cend())));

        if (m_scaleAlignedChildren)
            affineTransformSetFrom(stateMatrix, cocos2d::AffineTransformScale(stateMatrix, fittingScale.x, fittingScale.y));

        cocos2d::Rect childBB = child->getInternalBoundingBox();
        childBB.origin -= child->getAnchorPointInPoints();
        childBB = cocos2d::RectApplyAffineTransform(cocosBoundsToFlash(childBB), stateMatrix);
        cocos2d::Point pivot = cocos2d::Point(stateMatrix.tx - childBB.getMinX(), stateMatrix.ty - childBB.getMinY());

        switch (m_direction)
        {
        case Direction::horizontal:
        case Direction::tiledByWidth:
        {
            bool newRowStarted = firstChild || (currPos.x + childBB.size.width > maxLineSize.width);
            if (newRowStarted) // need to start new row
            {
                // second loop, align row
                alignLine(lineChildren, calculateLineOffset(true));
                lineChildren.clear();

                contentSize.width = std::max(contentSize.width, currLineSize.width);

                if (!firstChild)
                    currPos.y += currLineSize.height + m_gap * gapScale.y;

                childTopLeft.x = 0.0;
                currLineSize.width = firstLineChildSize.width = childBB.size.width;
                currLineSize.height = firstLineChildSize.height = childBB.size.height;
            }
            else // the child can be placed to current row
            {
                childTopLeft.x = currPos.x;
                currLineSize.width = std::max(currLineSize.width, childTopLeft.x + childBB.size.width);
                currLineSize.height = std::max(currLineSize.height, childBB.size.height);
            }

            currPos.x = childTopLeft.x + childBB.size.width + m_gap * gapScale.x; // calculate next child possible x-position
            stateMatrix.tx = childTopLeft.x + pivot.x;

            // first loop, align by first child
            switch (m_verticalAlign)
            {
            case VerticalAlign::bottom:
                childTopLeft.y = currPos.y + firstLineChildSize.height - childBB.size.height;
                currMinPos.y = std::min(currPos.y, childTopLeft.y);
                break;
            case VerticalAlign::center:
                childTopLeft.y = currPos.y + (firstLineChildSize.height - childBB.size.height) / 2;
                currMinPos.y = std::min(currPos.y, childTopLeft.y);
                break;
            default:
                childTopLeft.y = currPos.y;
                currMinPos.y = currPos.y;
            }

            stateMatrix.ty = childTopLeft.y + pivot.y;

            lineChildren.push_back(&stateMatrix);

            if (lastChild)
            {
                // second loop, align row
                alignLine(lineChildren, calculateLineOffset(true));
                lineChildren.clear();

                contentSize.width = std::max(contentSize.width, currLineSize.width);
                contentSize.height = currPos.y + currLineSize.height;
            }

            firstChild = false;
            break;
        }

        case Direction::vertical:
        case Direction::tiledByHeight:
        {
            bool newColumnStarted = firstChild || (currPos.y + childBB.size.height > maxLineSize.height);
            if (newColumnStarted) // need to start new column
            {
                // second loop, align column
                alignLine(lineChildren, calculateLineOffset(false));
                lineChildren.clear();

                contentSize.height = std::max(contentSize.height, currLineSize.height);

                if (!firstChild)
                    currPos.x += currLineSize.width + m_gap * gapScale.x;

                childTopLeft.y = 0.0;
                currLineSize.width = firstLineChildSize.width = childBB.size.width;
                currLineSize.height = firstLineChildSize.height = childBB.size.height;
            }
            else // the child can be placed to current row
            {
                childTopLeft.y = currPos.y;
                currLineSize.width = std::max(currLineSize.width, childBB.size.width);
                currLineSize.height = std::max(currLineSize.height, childTopLeft.y + childBB.size.height);
            }

            currPos.y = childTopLeft.y + childBB.size.height + m_gap * gapScale.y; // calculate next child possible y-position
            stateMatrix.ty = childTopLeft.y + pivot.y;

            // first loop, align by first child
            switch (m_horizontalAlign)
            {
            case HorizontalAlign::right:
                childTopLeft.x = currPos.x + firstLineChildSize.width - childBB.size.width;
                currMinPos.x = std::min(currPos.x, childTopLeft.x);
                break;
            case HorizontalAlign::center:
                childTopLeft.x = currPos.x + (firstLineChildSize.width - childBB.size.width) / 2;
                currMinPos.x = std::min(currPos.x, childTopLeft.x);
                break;
            default:
                childTopLeft.x = currPos.x;
                currMinPos.x = currPos.x;
            }

            stateMatrix.tx = childTopLeft.x + pivot.x;

            lineChildren.push_back(&stateMatrix);

            if (lastChild)
            {
                // second loop, align column
                alignLine(lineChildren, calculateLineOffset(false));
                lineChildren.clear();

                contentSize.width = currPos.x + currLineSize.width;
                contentSize.height = std::max(contentSize.height, currLineSize.height);
            }

            firstChild = false;
            break;
        }
        default:
            CCLOGERROR("Unknown direction");
        }
    }

    cocos2d::Point contentOffset = cocos2d::Point::ZERO;

    if (m_direction == Direction::horizontal || m_direction == Direction::tiledByWidth)
    {
        if (m_verticalAlign == VerticalAlign::bottom)
            contentOffset.y = bb.getMaxY() - m_marginBottom * marginScale.y - contentSize.height;
        else if (m_verticalAlign == VerticalAlign::center)
            contentOffset.y = bb.getMinY() + (bb.size.height - contentSize.height) / 2;
        else
            contentOffset.y = bb.getMinY() + m_marginTop * marginScale.y;
    }
    else
    {
        if (m_horizontalAlign == HorizontalAlign::right)
            contentOffset.x = bb.getMaxX() - m_marginRight * marginScale.x - contentSize.width;
        else if (m_horizontalAlign == HorizontalAlign::center)
            contentOffset.x = bb.getMinX() + (bb.size.width - contentSize.width) / 2;
        else
            contentOffset.x = bb.getMinX() + m_marginLeft * marginScale.x;
    }

    for (auto& childAndMtx : objects)
    {
        cocos2d::AffineTransform& stateMatrix = std::get<2>(childAndMtx);

        stateMatrix.tx += contentOffset.x;
        stateMatrix.ty += contentOffset.y;
    }
}

cocos2d::Rect GAFBoxLayoutView::getDynamicContentBounds() const
{
    if (!m_dynamicContentBoundsDirty) return m_dynamicContentBounds;

    cocos2d::Rect bb = cocosBoundsToFlash(GAFLayoutView::getInternalBoundingBox());
    cocos2d::Rect childrenBB = cocos2d::Rect::ZERO;

    cocos2d::Point fittingScale = getFittingScale();
    cocos2d::Point gapScale = getGapScale();
    cocos2d::Point marginScale = getMarginScale();

    cocos2d::Size maxLineSize;
    switch (m_direction)
    {
    case Direction::tiledByWidth:
        maxLineSize.setSize(bb.size.width - (m_marginLeft + m_marginRight) * marginScale.x, std::numeric_limits<float>::max());
        break;
    case Direction::tiledByHeight:
        maxLineSize.setSize(std::numeric_limits<float>::max(), bb.size.height - (m_marginTop - m_marginBottom) * marginScale.y);
        break;
    default:
        maxLineSize.setSize(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    }

    cocos2d::Rect currLineBB = cocos2d::Rect::ZERO;

    bool first = true;
    for (int i = 0; i < _children.size(); ++i)
    {
        auto child = _children.at(i);

        bool last = (i == _children.size() - 1);
        bool skip = (!child->isVisible() || child == m_container);

        if (skip)
        {
            if (last)
                childrenBB = childrenBB.unionWithRect(currLineBB);

            continue;
        }

        cocos2d::Rect childBB = child->getFlashBoundingBox();
        if (m_scaleAlignedChildren)
            childBB.setRect(
                childBB.origin.x * fittingScale.x, childBB.origin.y * fittingScale.y,
                childBB.size.width * fittingScale.x, childBB.size.height * fittingScale.y);
        switch (m_direction)
        {
            case Direction::horizontal:
            case Direction::tiledByWidth:
            {
                bool startNewLine = (first || currLineBB.size.width + m_gap * gapScale.x + childBB.size.width > maxLineSize.width);
                if (startNewLine)
                {
                    childrenBB = childrenBB.unionWithRect(currLineBB);

                    if (!first)
                        currLineBB.origin.y += m_gap * gapScale.y + currLineBB.size.height;
                    
                    currLineBB.size.setSize(childBB.size.width, childBB.size.height);
                }
                else
                {
                    currLineBB.size.width += m_gap * gapScale.x + childBB.size.width;
                    currLineBB.size.height = std::max(currLineBB.size.height, childBB.size.height);
                }
                break;
            }
            case Direction::vertical:
            case Direction::tiledByHeight:
            {
                bool startNewLine = (first || currLineBB.size.height + m_gap * gapScale.y + childBB.size.height > maxLineSize.height);
                if (startNewLine)
                {
                    childrenBB = childrenBB.unionWithRect(currLineBB);

                    if (!first)
                        currLineBB.origin.x += m_gap * gapScale.x + currLineBB.size.width;

                    currLineBB.size.setSize(childBB.size.width, childBB.size.height);
                }
                else
                {
                    currLineBB.size.width = std::max(currLineBB.size.width, childBB.size.width);
                    currLineBB.size.height += m_gap * gapScale.y + childBB.size.height;
                }
                break;
            }
            default:
                CCLOGERROR("Unknown direction");
        }

        if (last)
            childrenBB = childrenBB.unionWithRect(currLineBB);

        first = false;
    }

    childrenBB.size.width += (m_marginLeft + m_marginRight) * marginScale.x;
    childrenBB.size.height += (m_marginTop + m_marginBottom) * marginScale.y;

    if (m_horizontalAlign == HorizontalAlign::right)
        childrenBB.origin.x = bb.getMaxX() - childrenBB.size.width;
    else if (m_horizontalAlign == HorizontalAlign::center)
        childrenBB.origin.x = bb.getMinX() + (bb.size.width - childrenBB.size.width) / 2;
    else
        childrenBB.origin.x = bb.origin.x;

    if (m_verticalAlign == VerticalAlign::bottom)
        childrenBB.origin.y = bb.getMaxY() - childrenBB.size.height;
    else if (m_verticalAlign == VerticalAlign::center)
        childrenBB.origin.y = bb.getMinY() + (bb.size.height - childrenBB.size.height) / 2;
    else
        childrenBB.origin.y = bb.origin.y;

    m_dynamicContentBounds = childrenBB;

    m_dynamicContentBoundsDirty = false;

    return m_dynamicContentBounds;
}

cocos2d::Point GAFBoxLayoutView::getGapScale() const
{
    if (m_marginMode == MarginGapMode::percents)
        return m_internalScale;
    else if (m_marginMode == MarginGapMode::proportional)
        return getFittingScale();
    else
        return cocos2d::Point(1, 1);
}

cocos2d::Point GAFBoxLayoutView::getMarginScale() const
{
    if (m_gapMode == MarginGapMode::percents)
        return m_internalScale;
    else if (m_gapMode == MarginGapMode::proportional)
        return getFittingScale();
    else
        return cocos2d::Point(1, 1);
}

NS_GAF_END

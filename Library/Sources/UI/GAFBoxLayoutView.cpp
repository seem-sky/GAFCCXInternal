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
    , m_horizontalAlign(HorizontalAlign::center)
    , m_verticalAlign(VerticalAlign::center)
    , m_usePercents(false)
    , m_dynamicContentBounds(cocos2d::Rect::ZERO)
    , m_dynamicContentBoundsDirty(false)
{
}

GAFBoxLayoutView * GAFBoxLayoutView::create(GAFAsset * anAsset, GAFTimeline * timeline)
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

bool GAFBoxLayoutView::init(GAFAsset* anAnimationData, GAFTimeline* timeline)
{
    bool ret = GAFLayoutView::init(anAnimationData, timeline);

    if (ret)
        m_container->setLocalZOrder(-1);

    return ret;
}

cocos2d::Rect GAFBoxLayoutView::getInternalBoundingBox() const
{
    auto result = GAFLayoutView::getInternalBoundingBox();
    result = result.unionWithRect(getDynamicContentBounds());

    return result;
}

void GAFBoxLayoutView::addChild(Node* child, int localZOrder, int tag)
{
    GAFLayoutView::addChild(child, localZOrder, tag);

    m_dynamicContentBoundsDirty = true;
}

void GAFBoxLayoutView::addChild(Node* child, int localZOrder, const std::string& name)
{
    GAFLayoutView::addChild(child, localZOrder, name);

    m_dynamicContentBoundsDirty = true;
}

void GAFBoxLayoutView::removeChild(Node * child, bool cleanup)
{
    GAFLayoutView::removeChild(child, cleanup);

    m_dynamicContentBoundsDirty = true;
}

void GAFBoxLayoutView::removeAllChildrenWithCleanup(bool cleanup)
{
    GAFLayoutView::removeAllChildrenWithCleanup(cleanup);

    m_dynamicContentBoundsDirty = true;
}

void GAFBoxLayoutView::processOwnCustomProperties(const CustomPropertiesMap_t& customProperties)
{
    m_direction = Direction::toEnum(customProperties.at("direction"));
    m_horizontalAlign = HorizontalAlign::toEnum(customProperties.at("horizontalAlign"));
    m_verticalAlign = VerticalAlign::toEnum(customProperties.at("verticalAlign"));

    m_usePercents = customProperties.at("alignMode") == "percents";

    m_gap = std::stof(customProperties.at("gap"));
    m_marginTop = std::stof(customProperties.at("marginTop"));
    m_marginRight = std::stof(customProperties.at("marginRight"));
    m_marginBottom = std::stof(customProperties.at("marginBottom"));
    m_marginLeft = std::stof(customProperties.at("marginLeft"));

    m_dynamicContentBoundsDirty = true;
}

void GAFBoxLayoutView::processStates(cocos2d::Node* out, uint32_t frameIndex, const GAFAnimationFrame* frame)
{
    ObjectsStatesPositions_t childrenToAlign;
    const GAFAnimationFrame::SubobjectStates_t& states = frame->getObjectStates();
    for (const GAFSubobjectState* state : states)
    {
        GAFObject* child = m_displayList[state->objectIdRef];

        if (!child)
            continue;

        cocos2d::AffineTransform stateMatrix = state->affineTransform;

        childrenToAlign.emplace_back(child, state, stateMatrix);

        if (state->isVisible())
            child->setLastVisibleInFrame(frameIndex + 1);
    }

    processChildren(out, childrenToAlign);
    childrenToAlign.clear();

    for (auto child : _children)
    {
        GAFObject* subObject = dynamic_cast<GAFObject*>(child);
        if (!subObject) continue;

        childrenToAlign.emplace_back(subObject, nullptr, cocos2d::AffineTransformMakeIdentity());
    }

    processChildren(out, childrenToAlign);
}

void GAFBoxLayoutView::processChildren(cocos2d::Node* out, ObjectsStatesPositions_t& objects)
{
    cocos2d::Rect bb = GAFLayoutView::getInternalBoundingBox();
    cocos2d::Rect childrenBB = cocos2d::Rect::ZERO;

    cocos2d::Size maxLineSize;
    switch (m_direction)
    {
    case Direction::tiledByWidth:
        maxLineSize.setSize(bb.size.width - m_marginLeft - m_marginRight, std::numeric_limits<float>::max());
        break;
    case Direction::tiledByHeight:
        maxLineSize.setSize(std::numeric_limits<float>::max(), bb.size.height - m_marginTop - m_marginBottom);
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
            mtx->tx += lineOffset.y;
        }
    };

    auto calculateLineOffset = [this, &bb, &currPos, &currMinPos, &currLineSize](bool isHorizontal)
    {
        cocos2d::Point rowOffset;
        if (isHorizontal)
        {
            if (m_horizontalAlign == HorizontalAlign::right)
                rowOffset.x = bb.getMaxX() - m_marginRight - currLineSize.width;
            else if (m_horizontalAlign == HorizontalAlign::center)
                rowOffset.x = bb.getMinX() + (bb.size.width - currLineSize.width) / 2;
            else
                rowOffset.x = bb.getMinX() + m_marginLeft;

            rowOffset.y = currPos.y - currMinPos.y;
        }
        else
        {
            if (m_verticalAlign == VerticalAlign::bottom)
                rowOffset.y = bb.getMaxY() - m_marginBottom - currLineSize.height;
            else if (m_verticalAlign == VerticalAlign::center)
                rowOffset.y = bb.getMinY() + (bb.size.height - currLineSize.height) / 2;
            else
                rowOffset.y = bb.getMinY() + m_marginTop;

            rowOffset.x = currPos.x - currMinPos.x;
        }

        return rowOffset;
    };

    bool firstChild = true;
    for (auto& childAndMtx : objects)
    {
        GAFObject* child = std::get<0>(childAndMtx);
        const GAFSubobjectState* state = std::get<1>(childAndMtx);
        cocos2d::AffineTransform& stateMatrix = std::get<2>(childAndMtx);

        if (state)
            preProcessGAFObject(out, child, state, stateMatrix);

        auto lastChild = (child == std::get<0>(*std::prev(objects.cend())));
        bool skipChild = (state ? !state->isVisible() : !child->isVisible());
        if (skipChild)
        {
            if (lastChild)
            {
                // second loop, align row
                bool isHorizontalLine = (m_direction == Direction::horizontal || m_direction == Direction::tiledByWidth);

                alignLine(lineChildren, calculateLineOffset(isHorizontalLine));
                lineChildren.clear();

                contentSize.width = std::max(contentSize.width, currLineSize.width);
                contentSize.height = currPos.y + currLineSize.height;
            }

            continue;
        }

        cocos2d::Rect childBB = cocos2d::RectApplyAffineTransform(child->getInternalBoundingBox(), stateMatrix);
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
                    currPos.y += currLineSize.height + m_gap;

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

            currPos.x = childTopLeft.x + childBB.size.width + m_gap; // calculate next child possible x-position
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
                    currPos.x += currLineSize.width + m_gap;

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

            currPos.y = childTopLeft.y + childBB.size.height + m_gap; // calculate next child possible y-position
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
            contentOffset.y = bb.getMaxY() - m_marginBottom - contentSize.height;
        else if (m_verticalAlign == VerticalAlign::center)
            contentOffset.y = bb.getMinY() + (bb.size.height - contentSize.height) / 2;
        else
            contentOffset.y = bb.getMinY() + m_marginTop;
    }
    else
    {
        if (m_horizontalAlign == HorizontalAlign::right)
            contentOffset.x = bb.getMaxX() - m_marginRight - contentSize.width;
        else if (m_horizontalAlign == HorizontalAlign::center)
            contentOffset.x = bb.getMinX() + (bb.size.width - contentSize.width) / 2;
        else
            contentOffset.x = bb.getMinX() + m_marginLeft;
    }

    for (auto& childAndMtx : objects)
    {
        GAFObject* child = std::get<0>(childAndMtx);
        const GAFSubobjectState* state = std::get<1>(childAndMtx);
        cocos2d::AffineTransform& stateMatrix = std::get<2>(childAndMtx);

        stateMatrix.tx += contentOffset.x;
        stateMatrix.ty += contentOffset.y;

        if (state)
        {
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
        else
        {
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
}

cocos2d::Rect GAFBoxLayoutView::getDynamicContentBounds() const
{
    if (!m_dynamicContentBoundsDirty) return m_dynamicContentBounds;

    cocos2d::Rect bb = GAFLayoutView::getInternalBoundingBox();
    cocos2d::Rect childrenBB = cocos2d::Rect::ZERO;

    cocos2d::Size maxLineSize;
    switch (m_direction)
    {
    case Direction::tiledByWidth:
        maxLineSize.setSize(bb.size.width - m_marginLeft - m_marginRight, std::numeric_limits<float>::max());
        break;
    case Direction::tiledByHeight:
        maxLineSize.setSize(std::numeric_limits<float>::max(), bb.size.height - m_marginTop - m_marginBottom);
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

        auto childBB = child->getBoundingBox();

        switch (m_direction)
        {
            case Direction::horizontal:
            case Direction::tiledByWidth:
            {
                bool startNewLine = (first || currLineBB.size.width + m_gap + childBB.size.width > maxLineSize.width);
                if (startNewLine)
                {
                    childrenBB = childrenBB.unionWithRect(currLineBB);

                    if (!first)
                        currLineBB.origin.y += m_gap + currLineBB.size.height;
                    
                    currLineBB.size.setSize(childBB.size.width, childBB.size.height);
                }
                else
                {
                    currLineBB.size.width += m_gap + childBB.size.width;
                    currLineBB.size.height = std::max(currLineBB.size.height, childBB.size.height);
                }
                break;
            }
            case Direction::vertical:
            case Direction::tiledByHeight:
            {
                bool startNewLine = (first || currLineBB.size.height + m_gap + childBB.size.height > maxLineSize.height);
                if (startNewLine)
                {
                    childrenBB = childrenBB.unionWithRect(currLineBB);

                    if (!first)
                        currLineBB.origin.x += m_gap + currLineBB.size.width;

                    currLineBB.size.setSize(childBB.size.width, childBB.size.height);
                }
                else
                {
                    currLineBB.size.width = std::max(currLineBB.size.width, childBB.size.width);
                    currLineBB.size.height += m_gap + childBB.size.height;
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

    childrenBB.size.width += m_marginLeft + m_marginRight;
    childrenBB.size.height += m_marginTop + m_marginBottom;

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

NS_GAF_END

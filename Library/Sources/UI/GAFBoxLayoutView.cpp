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
    auto unscaledInternalBounds = m_timeline->getRect();
    auto actualInternalBounds = getInternalBoundingBox();
    auto actualChildrenBounds = getDynamicContentBounds();

    float currHPos;
    if (m_horizontalAlign == HorizontalAlign::left)
    {
        currHPos = actualInternalBounds.origin.x + m_marginLeft;
    }
    else if (m_horizontalAlign == HorizontalAlign::right)
    {
        currHPos = actualInternalBounds.origin.x + actualInternalBounds.size.width - m_marginRight;
    }
    else
    {
        currHPos = (actualInternalBounds.size.width - actualChildrenBounds.size.width) / 2;
    }

    float currVPos;
    if (m_verticalAlign == VerticalAlign::top)
    {
        currVPos = actualInternalBounds.origin.y + m_marginTop;
    }
    else if (m_verticalAlign == VerticalAlign::bottom)
    {
        currVPos = actualInternalBounds.origin.y + actualInternalBounds.size.height - m_marginBottom;
    }
    else
    {
        currVPos = (actualInternalBounds.size.height - actualChildrenBounds.size.height) / 2;
    }

    cocos2d::Point currPos(currHPos, currVPos);

    const GAFAnimationFrame::SubobjectStates_t& states = frame->getObjectStates();
    for (const GAFSubobjectState* state : states)
    {
        GAFObject* subObject = m_displayList[state->objectIdRef];

        if (!subObject)
            continue;

        cocos2d::AffineTransform stateMatrix = state->affineTransform;
        preProcessGAFObject(out, subObject, state, stateMatrix);

        if (!state->isVisible())
            continue;

        layoutChild(subObject, stateMatrix, currPos, actualInternalBounds);

        if (subObject->getCharType() == GAFCharacterType::Timeline)
        {
            processGAFTimeline(out, subObject, state, stateMatrix);
        }
        else if (subObject->getCharType() == GAFCharacterType::Texture)
        {
            processGAFImage(out, subObject, state, stateMatrix);
        }
        else if (subObject->getCharType() == GAFCharacterType::TextField)
        {
            processGAFTextField(out, subObject, state, stateMatrix);
        }

        if (state->isVisible())
        {
            subObject->setLastVisibleInFrame(frameIndex + 1);
        }
    }

    currPos.set(currHPos, currVPos);
    for (int i = 0; i < _children.size(); ++i)
    {
        cocos2d::Node* child = _children.at(i);

        GAFObject* subObject = dynamic_cast<GAFObject*>(child);
        if (!subObject) continue;

        cocos2d::AffineTransform stateMatrix = cocos2d::AffineTransformMakeIdentity();

        layoutChild(subObject, stateMatrix, currPos, actualInternalBounds);

        if (subObject->getCharType() == GAFCharacterType::Timeline)
        {
            CustomPropertiesMap_t props;

            processGAFTimelineStateTransform(subObject, stateMatrix, props);
        }
        else if (subObject->getCharType() == GAFCharacterType::Texture)
        {
            processGAFImageStateTransform(subObject, stateMatrix);
        }
        else if (subObject->getCharType() == GAFCharacterType::TextField)
        {
            processGAFTextFieldStateTransform(subObject, stateMatrix);
        }

        subObject->setExternalTransform(stateMatrix);

        //subObject->setLastVisibleInFrame(frameIndex + 1);
    }
}

cocos2d::Point& GAFBoxLayoutView::layoutChild(const GAFObject* subObject, cocos2d::AffineTransform& stateMatrix, cocos2d::Point& currPos, const cocos2d::Rect& actualInternalBounds) const
{
    auto childActualBounds = cocos2d::RectApplyAffineTransform(subObject->getInternalBoundingBox(), stateMatrix);
    if (m_direction == Direction::horizontal)
    {
        if (m_horizontalAlign == HorizontalAlign::right)
        {
            stateMatrix.tx = currPos.x - childActualBounds.size.width + (stateMatrix.tx - childActualBounds.origin.x);
            currPos.x -= childActualBounds.size.width + m_gap;
        }
        else
        {
            stateMatrix.tx = currPos.x + (stateMatrix.tx - childActualBounds.origin.x);
            currPos.x += childActualBounds.size.width + m_gap;
        }

        if (m_verticalAlign == VerticalAlign::top)
        {
            stateMatrix.ty = currPos.y + (stateMatrix.ty - childActualBounds.origin.y);
        }
        else if (m_verticalAlign == VerticalAlign::bottom)
        {
            stateMatrix.ty =
                actualInternalBounds.origin.y
                + actualInternalBounds.size.height
                - m_marginBottom
                - childActualBounds.size.height
                + (stateMatrix.ty - childActualBounds.origin.y);
        }
        else
        {
            stateMatrix.ty =
                actualInternalBounds.origin.y + m_marginTop
                + ((actualInternalBounds.size.height - m_marginTop - m_marginBottom) / 2)
                - (childActualBounds.size.height / 2)
                + (stateMatrix.ty - childActualBounds.origin.y);
        }
    }
    else
    {
        if (m_horizontalAlign == VerticalAlign::bottom)
        {
            stateMatrix.ty = currPos.y - childActualBounds.size.height + (stateMatrix.ty - childActualBounds.origin.y);
            currPos.y -= childActualBounds.size.height + m_gap;
        }
        else
        {
            stateMatrix.ty = currPos.y + (stateMatrix.ty - childActualBounds.origin.y);
            currPos.y += childActualBounds.size.height + m_gap;
        }

        if (m_horizontalAlign == HorizontalAlign::left)
        {
            stateMatrix.tx = currPos.x + (stateMatrix.tx - childActualBounds.origin.x);
        }
        else if (m_horizontalAlign == HorizontalAlign::right)
        {
            stateMatrix.tx =
                actualInternalBounds.origin.x
                + actualInternalBounds.size.width
                - m_marginRight
                - childActualBounds.size.width
                + (stateMatrix.tx - childActualBounds.origin.x);
        }
        else
        {
            stateMatrix.tx =
                actualInternalBounds.origin.x + m_marginLeft
                + ((actualInternalBounds.size.width - m_marginLeft - m_marginRight) / 2)
                - (childActualBounds.size.width / 2)
                + (stateMatrix.tx - childActualBounds.origin.x);
        }
    }

    return currPos;
}

cocos2d::Rect GAFBoxLayoutView::getDynamicContentBounds() const
{
    if (!m_dynamicContentBoundsDirty) return m_dynamicContentBounds;

    float w = 0;
    float h = 0;
    uint32_t childCounter = 0;
    for (int i = 0; i < _children.size(); ++i)
    {
        auto child = _children.at(i);
        if (!child->isVisible() || child == m_container) continue;

        auto childBB = child->getBoundingBox();

        if (m_direction == Direction::horizontal)
        {
            if (childCounter > 0)
                w += m_gap;

            w += childBB.size.width;
            h = std::max(h, childBB.size.height);
        }
        else
        {
            if (childCounter > 0)
                h += m_gap;

            w = std::max(w, childBB.size.width);
            h += childBB.size.height;
        }

        ++childCounter;
    }

    w += m_marginLeft + m_marginRight;
    h += m_marginTop + m_marginBottom;

    m_dynamicContentBounds.setRect(0, 0, w, h);

    m_dynamicContentBoundsDirty = false;

    return m_dynamicContentBounds;
}

NS_GAF_END

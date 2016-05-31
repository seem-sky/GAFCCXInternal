#include "GAFPrecompiled.h"
#include "GAFBoxLayoutView.h"
#include "GAFObject.h"
#include "GAFTimeline.h"
#include "GAFSubobjectState.h"
#include "GAFAnimationFrame.h"

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

void GAFBoxLayoutView::processOwnCustomProperties(const CustomPropertiesMap_t& customProperties)
{
    m_direction = toEnumDirection(customProperties.at("direction"));
    m_horizontalAlign = toEnumHorizontalAlign(customProperties.at("horizontalAlign"));
    m_verticalAlign = toEnumVerticalAlign(customProperties.at("verticalAlign"));

    m_usePercents = customProperties.at("units") == "percents";

    m_gap = std::stof(customProperties.at("gap"));
    m_marginTop = std::stof(customProperties.at("marginTop"));
    m_marginRight = std::stof(customProperties.at("marginRight"));
    m_marginBottom = std::stof(customProperties.at("marginBottom"));
    m_marginLeft = std::stof(customProperties.at("marginLeft"));
}

void GAFBoxLayoutView::processStates(cocos2d::Node* out, uint32_t frameIndex, const GAFAnimationFrame* frame)
{
    auto unscaledInternalBounds = m_timeline->getRect();
    auto actualInternalBounds = getInternalBoundingBox();

    auto currLeft = actualInternalBounds.origin.x + m_marginLeft;
    auto currTop = actualInternalBounds.origin.y + m_marginTop;

    cocos2d::Point currTopLeft(currLeft, currTop);

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

        layoutChild(subObject, stateMatrix, currTopLeft, actualInternalBounds);

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

    currTopLeft.set(currLeft, currTop);
    for (int i = 0; i < _children.size(); ++i)
    {
        cocos2d::Node* child = _children.at(i);

        GAFObject* subObject = dynamic_cast<GAFObject*>(child);
        if (!subObject) continue;

        cocos2d::AffineTransform stateMatrix = cocos2d::AffineTransformMakeIdentity();

        layoutChild(subObject, stateMatrix, currTopLeft, actualInternalBounds);

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

cocos2d::Point& GAFBoxLayoutView::layoutChild(const GAFObject* subObject, cocos2d::AffineTransform& stateMatrix, cocos2d::Point& currentTopLeft, const cocos2d::Rect& actualInternalBounds) const
{
    auto childActualBounds = cocos2d::RectApplyAffineTransform(subObject->getInternalBoundingBox(), stateMatrix);
    if (m_direction == Direction::horizontal)
    {
        stateMatrix.tx = currentTopLeft.x + (stateMatrix.tx - childActualBounds.origin.x);
        currentTopLeft.x += childActualBounds.size.width + m_gap;

        if (m_verticalAlign == VerticalAlign::top)
        {
            stateMatrix.ty = currentTopLeft.y;
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
        stateMatrix.ty = currentTopLeft.y + (stateMatrix.ty - childActualBounds.origin.y);
        currentTopLeft.y += childActualBounds.size.height + m_gap;

        if (m_horizontalAlign == HorizontalAlign::left)
        {
            stateMatrix.tx = currentTopLeft.x;
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

    return currentTopLeft;
}

NS_GAF_END

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

        auto childActualBounds = cocos2d::RectApplyAffineTransform(subObject->getInternalBoundingBox(), stateMatrix);
        if (m_direction == Direction::horizontal)
        {
            stateMatrix.tx = currLeft + (stateMatrix.tx - childActualBounds.origin.x);
            currLeft += childActualBounds.size.width + m_gap;

            if (m_verticalAlign == VerticalAlign::top)
            {
                stateMatrix.ty = currTop;
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
            stateMatrix.ty = currTop + (stateMatrix.ty - childActualBounds.origin.y);
            currTop += childActualBounds.size.height + m_gap;

            if (m_horizontalAlign == HorizontalAlign::left)
            {
                stateMatrix.tx = currLeft;
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
}

NS_GAF_END

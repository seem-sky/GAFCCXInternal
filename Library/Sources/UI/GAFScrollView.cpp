#include "GAFPrecompiled.h"
#include "GAFScrollView.h"
#include "GAFTimeline.h"
#include <GAFUtils.h>

NS_GAF_BEGIN

GAFScrollView::GAFScrollView()
    : m_innerContainer(nullptr)
    , m_scrollBarH(nullptr)
    , m_scrollBarV(nullptr)
{
}

GAFScrollView* GAFScrollView::create(GAFAsset* anAsset, GAFTimeline* timeline)
{
    GAFScrollView* ret = new GAFScrollView();

    if (ret && ret->init(anAsset, timeline))
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_RELEASE(ret);
    return nullptr;
}

bool GAFScrollView::init(GAFAsset* anAnimationData, GAFTimeline* timeline)
{
    bool ret = GAFComponentView::init(anAnimationData, timeline);
    if (ret)
    {
        setClippingEnabled(true);

        m_innerContainer = getObjectByName("container");
        CCASSERT(m_innerContainer, "Error. No container found in this GAFScrollView");

        m_scrollBarH = getObjectByName("scrollBarH");
        m_scrollBarV = getObjectByName("scrollBarV");
    }

    return ret;
}

void GAFScrollView::onEnter()
{
    GAFComponentView::onEnter();

    scheduleUpdate();

    if (m_scrollBarH) m_scrollBarH->scheduleUpdate();
    if (m_scrollBarV) m_scrollBarV->scheduleUpdate();
}

void GAFScrollView::processOwnCustomProperties(const CustomPropertiesMap_t& customProperties)
{
    GAFComponentView::processOwnCustomProperties(customProperties);
}

void GAFScrollView::processGAFTimeline(cocos2d::Node* out, GAFObject* child, const GAFSubobjectState* state, cocos2d::AffineTransform& mtx)
{
    if (child == m_innerContainer)
    {
        affineTransformSetFrom(mtx, cocos2d::AffineTransformScale(mtx, m_internalScale.x, m_internalScale.y));
    }

    GAFLayoutView::processGAFTimeline(out, child, state, mtx);
}

NS_GAF_END

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
    GAFLayoutView::onEnter();

    scheduleUpdate();

    if (m_scrollBarH) m_scrollBarH->scheduleUpdate();
    if (m_scrollBarV) m_scrollBarV->scheduleUpdate();
}

void GAFScrollView::processGAFTimeline(cocos2d::Node* out, GAFObject* child, const GAFSubobjectState* state, cocos2d::AffineTransform& mtx)
{
    if (child == m_scrollBarH || child == m_scrollBarV)
    {
        cocos2d::Vec3 internalScale;
        auto transform = GAFLayoutView::getNodeToParentTransform();
        transform.getScale(&internalScale);
        cocos2d::Vec3 inverseScale(1.0f / internalScale.x, 1.0f / internalScale.y, 1.0f / internalScale.z);
        if (child == m_scrollBarH)
        {
            mtx.b *= inverseScale.y;
            mtx.d *= inverseScale.y;
        }
        else
        {
            mtx.a *= inverseScale.x;
            mtx.c *= inverseScale.x;
        }
    }

    GAFLayoutView::processGAFTimeline(out, child, state, mtx);
}

NS_GAF_END

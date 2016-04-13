#include "GAFPrecompiled.h"
#include "GAFComponent.h"
#include "GAFObject.h"

NS_GAF_BEGIN
NS_GAF_UI_BEGIN

GAFComponent::GAFComponent(GAFObject *view)
: m_view(view)
{
    m_view->setAnchorPoint(cocos2d::Vec2(0.0f, 1.0f));
    GAFComponent::addChild(m_view);
}

GAFComponent::~GAFComponent()
{
}

bool GAFComponent::init()
{
    if (Widget::init())
    {
        return true;
    }

    return false;
}

cocos2d::Size GAFComponent::getVirtualRendererSize() const
{
    return m_view->getContentSize();
}

bool GAFComponent::hitTest(const cocos2d::Vec2 &pt, const cocos2d::Camera* camera, cocos2d::Vec3 *p) const
{
    cocos2d::Rect rect = m_view->getBoundingBoxForCurrentFrame();
    auto localTouchBeganPosition = m_view->getParent()->convertToNodeSpace(pt);

    return rect.containsPoint(localTouchBeganPosition);
}

NS_GAF_UI_END
NS_GAF_END
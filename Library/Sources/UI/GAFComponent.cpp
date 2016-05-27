#include "GAFPrecompiled.h"
#include "GAFComponent.h"
#include "GAFObject.h"

NS_GAF_BEGIN
NS_GAF_UI_BEGIN

GAFComponent::GAFComponent(GAFObject *view)
: m_view(view)
{
    //m_view->setAnchorPoint(cocos2d::Vec2(0.0f, 1.0f));
    //GAFComponent::addChild(m_view);
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

const cocos2d::Vec2& GAFComponent::getPosition() const
{
    return m_view->getPosition();
}

void GAFComponent::setPosition(float x, float y)
{
    m_view->setPosition(x, y);
}

void GAFComponent::getPosition(float* x, float* y) const
{
    m_view->getPosition(x , y);
}

float GAFComponent::getPositionX() const
{
    return m_view->getPositionX();
}

void GAFComponent::setPositionX(float x)
{
    m_view->setPositionX(x);
}

float GAFComponent::getPositionY() const
{
    return m_view->getPositionY();
}

void GAFComponent::setPositionY(float y)
{
    m_view->setPositionY(y);
}

float GAFComponent::getPositionZ() const
{
    return m_view->getPositionZ();
}

void GAFComponent::setPositionZ(float z)
{
    m_view->setPositionZ(z);
}

const cocos2d::Vec2& GAFComponent::getAnchorPoint() const
{
    return m_view->getAnchorPoint();
}

void GAFComponent::setAnchorPoint(const cocos2d::Vec2& anchorPoint)
{
    m_view->setAnchorPoint(anchorPoint);
}

const cocos2d::Vec2& GAFComponent::getAnchorPointInPoints() const
{
    return m_view->getAnchorPointInPoints();
}

const cocos2d::Size& GAFComponent::getContentSize() const
{
    return m_view->getContentSize();
}

void GAFComponent::update(float delta)
{
    m_view->update(delta);
}

void GAFComponent::updateTransform()
{
    m_view->updateTransform();
}

const cocos2d::Mat4& GAFComponent::getNodeToParentTransform() const
{
    return m_view->getNodeToParentTransform();
}

cocos2d::AffineTransform GAFComponent::getNodeToParentAffineTransform() const
{
    return m_view->getNodeToParentAffineTransform();
}

cocos2d::Mat4 GAFComponent::getNodeToParentTransform(Node* ancestor) const
{
    return m_view->cocos2d::Node::getNodeToParentTransform(ancestor);
}

cocos2d::AffineTransform GAFComponent::getNodeToParentAffineTransform(Node* ancestor) const
{
    return m_view->cocos2d::Node::getNodeToParentAffineTransform(ancestor);
}

void GAFComponent::setNodeToParentTransform(const cocos2d::Mat4& transform)
{
    m_view->setNodeToParentTransform(transform);
}

cocos2d::AffineTransform GAFComponent::nodeToParentTransform() const
{
    return m_view->nodeToParentTransform();
}

const cocos2d::Mat4& GAFComponent::getParentToNodeTransform() const
{
    return m_view->getParentToNodeTransform();
}

cocos2d::AffineTransform GAFComponent::getParentToNodeAffineTransform() const
{
    return m_view->getParentToNodeAffineTransform();
}

cocos2d::AffineTransform GAFComponent::parentToNodeTransform() const
{
    return m_view->parentToNodeTransform();
}

cocos2d::Mat4 GAFComponent::getNodeToWorldTransform() const
{
    return m_view->getNodeToWorldTransform();
}

cocos2d::AffineTransform GAFComponent::getNodeToWorldAffineTransform() const
{
    return m_view->getNodeToWorldAffineTransform();
}

cocos2d::AffineTransform GAFComponent::nodeToWorldTransform() const
{
    return m_view->nodeToWorldTransform();
}

cocos2d::Mat4 GAFComponent::getWorldToNodeTransform() const
{
    return m_view->getWorldToNodeTransform();
}

cocos2d::AffineTransform GAFComponent::getWorldToNodeAffineTransform() const
{
    return m_view->getWorldToNodeAffineTransform();
}

cocos2d::AffineTransform GAFComponent::worldToNodeTransform() const
{
    return m_view->worldToNodeTransform();
}

void GAFComponent::setContentSize(const cocos2d::Size& contentSize)
{
    m_view->setContentSize(contentSize);
}

bool GAFComponent::hitTest(const cocos2d::Vec2 &pt, const cocos2d::Camera* camera, cocos2d::Vec3 *p) const
{
    cocos2d::Rect rect = m_view->getBoundingBoxForCurrentFrame();
    auto localTouchBeganPosition = m_view->getParent()->convertToNodeSpace(pt);

    return rect.containsPoint(localTouchBeganPosition);
}

NS_GAF_UI_END
NS_GAF_END
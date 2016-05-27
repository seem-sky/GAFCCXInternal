#pragma once
#include "GAFUIMacros.h"
#include <ui/UIWidget.h>

NS_CC_BEGIN
class EventListenerTouchOneByOne;
class Camera;
NS_CC_END

NS_GAF_BEGIN
class GAFObject;

NS_GAF_UI_BEGIN
class EventListenerTouchOneByOne;

class GAFComponent : public cocos2d::ui::Widget
{
public:

    virtual ~GAFComponent() = 0;

    virtual cocos2d::Size getVirtualRendererSize() const override;

    virtual bool hitTest(const cocos2d::Vec2 &pt, const cocos2d::Camera* camera, cocos2d::Vec3 *p) const override;

    virtual const cocos2d::Vec2& getPosition() const override;
    virtual void setPosition(float x, float y) override;
    virtual void getPosition(float* x, float* y) const override;

    virtual float getPositionX() const override;
    virtual void setPositionX(float x) override;
    
    virtual float getPositionY() const override;
    virtual void setPositionY(float y) override;
    
    virtual float getPositionZ() const override;
    virtual void setPositionZ(float z) override;
    
    virtual const cocos2d::Vec2& getAnchorPoint() const override;
    virtual void setAnchorPoint(const cocos2d::Vec2& anchorPoint) override;
    virtual const cocos2d::Vec2& getAnchorPointInPoints() const override;

    virtual const cocos2d::Size& getContentSize() const override;
    virtual void setContentSize(const cocos2d::Size& contentSize) override;

    virtual void update(float delta) override;

    virtual void updateTransform() override;

    virtual const cocos2d::Mat4& getNodeToParentTransform() const override;
    virtual cocos2d::AffineTransform getNodeToParentAffineTransform() const override;

    virtual cocos2d::Mat4 getNodeToParentTransform(Node* ancestor) const override;

    virtual cocos2d::AffineTransform getNodeToParentAffineTransform(Node* ancestor) const override;

    virtual void setNodeToParentTransform(const cocos2d::Mat4& transform) override;

    /** @deprecated use getNodeToParentTransform() instead */
    CC_DEPRECATED_ATTRIBUTE virtual cocos2d::AffineTransform nodeToParentTransform() const override;

    virtual const cocos2d::Mat4& getParentToNodeTransform() const override;
    virtual cocos2d::AffineTransform getParentToNodeAffineTransform() const override;

    /** @deprecated Use getParentToNodeTransform() instead */
    CC_DEPRECATED_ATTRIBUTE virtual cocos2d::AffineTransform parentToNodeTransform() const override;

    virtual cocos2d::Mat4 getNodeToWorldTransform() const override;
    virtual cocos2d::AffineTransform getNodeToWorldAffineTransform() const override;

    /** @deprecated Use getNodeToWorldTransform() instead */
    CC_DEPRECATED_ATTRIBUTE virtual cocos2d::AffineTransform nodeToWorldTransform() const override;

    virtual cocos2d::Mat4 getWorldToNodeTransform() const override;
    virtual cocos2d::AffineTransform getWorldToNodeAffineTransform() const override;

    /** @deprecated Use getWorldToNodeTransform() instead */
    CC_DEPRECATED_ATTRIBUTE virtual cocos2d::AffineTransform worldToNodeTransform() const override;

protected:
    explicit GAFComponent(GAFObject* view);
    GAFComponent(const GAFComponent &) = delete;
    GAFComponent &operator=(const GAFComponent &) = delete;

    virtual bool init() override;

protected:
    GAFObject* m_view;
};

NS_GAF_UI_END
NS_GAF_END
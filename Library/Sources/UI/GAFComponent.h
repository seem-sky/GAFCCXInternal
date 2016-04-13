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
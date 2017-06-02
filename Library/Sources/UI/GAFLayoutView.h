#pragma once
#include "GAFComponentView.h"
#include "GAFCustomProperties.h"

NS_GAF_BEGIN

class GAFLayoutView : public GAFComponentView
{
public:
    GAFLayoutView();
    virtual ~GAFLayoutView() = 0;

    virtual const cocos2d::Mat4& getNodeToParentTransform() const override;
    virtual cocos2d::Rect getInternalBoundingBox() const override;

protected:
    mutable cocos2d::Vec2 m_internalScale;
    cp::FittingMode m_fittingMode;
    bool m_scaleAlignedChildren;

    virtual cocos2d::AffineTransform& addAdditionalTransformations(cocos2d::AffineTransform& mtx) const override;

    virtual cocos2d::Vec2 getInternalScale() const;
    virtual cocos2d::Vec2 getFittingScale() const;

    virtual bool allNecessaryFieldsExist(cp::GAFCustomPropertiesConstPtr customProperties) const override;
    virtual bool processOwnCustomProperties(cp::GAFCustomPropertiesConstPtr customProperties) override;
};

NS_GAF_END

#pragma once
#include "GAFComponentView.h"

NS_GAF_BEGIN

class GAFLayoutView : public GAFComponentView
{
public:
    GAFLayoutView();
    ~GAFLayoutView() = 0;

    virtual const cocos2d::Mat4& getNodeToParentTransform() const override;
    virtual cocos2d::Rect getInternalBoundingBox() const override;
    virtual cocos2d::Vec3 getInternalScale() const;

protected:
    mutable cocos2d::Vec3 m_internalScale;

    virtual bool allNecessaryFieldsExist(const CustomPropertiesMap_t& customProperties) const override;
};

NS_GAF_END

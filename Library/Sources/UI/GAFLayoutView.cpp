#include "GAFPrecompiled.h"
#include "GAFLayoutView.h"

NS_GAF_BEGIN

GAFLayoutView::GAFLayoutView()
{
}

GAFLayoutView::~GAFLayoutView()
{
}

/*const cocos2d::Mat4& GAFLayoutView::getNodeToParentTransform() const
{
    bool transformDirty = _transformDirty;
    if (transformDirty)
    {
        _transform = GAFObject::getNodeToParentTransform();
        _transformDirty = transformDirty;

        _transform.getScale(&m_internalScale);

        cocos2d::Vec3 inverseScale(1.0f / m_internalScale.x, 1.0f / m_internalScale.y, 1.0f / m_internalScale.z);
        _transform.scale(inverseScale);

        _transformDirty = false;
    }

    return _transform;
}*/

/*cocos2d::Rect GAFLayoutView::getInternalBoundingBox() const
{
    cocos2d::AffineTransform scaleMtx = cocos2d::AffineTransformMakeIdentity();
    scaleMtx = cocos2d::AffineTransformScale(scaleMtx, getInternalScale().x, getInternalScale().y);
    auto unscaledInternalBounds = GAFObject::getInternalBoundingBox();
    return RectApplyAffineTransform(unscaledInternalBounds, scaleMtx);
}*/

/*cocos2d::Vec3 GAFLayoutView::getInternalScale() const
{
    if (_transformDirty)
        getNodeToParentTransform();

    return m_internalScale;
}*/

bool GAFLayoutView::allNecessaryFieldsExist(const CustomPropertiesMap_t & customProperties) const
{
    bool allFieldsExist =
        (customProperties.find("alignLeft") != customProperties.end())
        && (customProperties.find("alignRight") != customProperties.end())
        && (customProperties.find("alignTop") != customProperties.end())
        && (customProperties.find("alignBottom") != customProperties.end())
        && (customProperties.find("units") != customProperties.end());

    return allFieldsExist;
}

NS_GAF_END

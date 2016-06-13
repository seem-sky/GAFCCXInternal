#include "GAFPrecompiled.h"
#include "GAFCanvasView.h"
#include "GAFObject.h"
#include "GAFSubobjectState.h"
#include "GAFTimeline.h"
#include "GAFUtils.h"

NS_GAF_BEGIN

GAFCanvasView::GAFCanvasView()
    : m_internalScale(1.0f, 1.0f, 1.0f)
{
}

GAFCanvasView * GAFCanvasView::create(GAFAsset * anAsset, GAFTimeline * timeline)
{
    GAFCanvasView* ret = new GAFCanvasView();

    if (ret && ret->init(anAsset, timeline))
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_RELEASE(ret);
    return nullptr;
}

cocos2d::AffineTransform& GAFCanvasView::changeTransformAccordingToCustomProperties(GAFObject* child, cocos2d::AffineTransform& mtx, const CustomPropertiesMap_t& customProperties) const
{
    auto unscaledInternalBounds = m_timeline->getRect();
    auto actualInternalBounds = getInternalBoundingBox();
    auto childActualBounds = RectApplyAffineTransform(child->getInternalBoundingBox(), mtx);

    bool usePercents = customProperties.at("units") == "percents";
    bool alignLeft = to_bool(customProperties.at("alignLeft"));
    bool alignRight = to_bool(customProperties.at("alignRight"));
    bool alignTop = to_bool(customProperties.at("alignTop"));
    bool alignBottom = to_bool(customProperties.at("alignBottom"));

    
    bool anyOfAlignSet = alignLeft || alignRight || alignTop || alignBottom;

    if (anyOfAlignSet)
    {
        float childAdditionalScaleX = 1.0f;
        float childAdditionalScaleY = 1.0f;

        float leftDistance = childActualBounds.origin.x - unscaledInternalBounds.origin.x;
        if (usePercents)
            leftDistance = leftDistance / unscaledInternalBounds.size.width * actualInternalBounds.size.width;

        if (alignRight)
        {
            float rightDistance = unscaledInternalBounds.origin.x + unscaledInternalBounds.size.width - (childActualBounds.origin.x + childActualBounds.size.width);
            if (usePercents)
                rightDistance = rightDistance / unscaledInternalBounds.size.width * actualInternalBounds.size.width;

            if (alignLeft)
                childAdditionalScaleX = (actualInternalBounds.size.width - leftDistance - rightDistance) / childActualBounds.size.width;
        }

        float finalTx = actualInternalBounds.origin.x + leftDistance + (mtx.tx - childActualBounds.origin.x) * childAdditionalScaleX;

        float topDistance = childActualBounds.origin.y - unscaledInternalBounds.origin.y;
        if (usePercents)
            topDistance = topDistance / unscaledInternalBounds.size.height * actualInternalBounds.size.height;

        if (alignBottom)
        {
            float bottomDistance = unscaledInternalBounds.origin.y + unscaledInternalBounds.size.height - (childActualBounds.origin.y + childActualBounds.size.height);
            if (usePercents)
                bottomDistance = bottomDistance / unscaledInternalBounds.size.height * actualInternalBounds.size.height;

            if (alignTop)
                childAdditionalScaleY = (actualInternalBounds.size.height - topDistance - bottomDistance) / childActualBounds.size.height;
        }

        float finalTy = actualInternalBounds.origin.y + topDistance + (mtx.ty - childActualBounds.origin.y) * childAdditionalScaleY;

        affineTransformSetFrom(mtx, cocos2d::AffineTransformScale(mtx, childAdditionalScaleX, childAdditionalScaleY));
        mtx.tx = finalTx;
        mtx.ty = finalTy;
    }
    else
    {
        cocos2d::AffineTransform scaleMtx = cocos2d::AffineTransformMakeIdentity();
        scaleMtx = cocos2d::AffineTransformScale(scaleMtx, getInternalScale().x, getInternalScale().y);
        affineTransformSetFrom(mtx, AffineTransformConcat(mtx, scaleMtx));
    }
    

    return mtx;
}

const cocos2d::Mat4& GAFCanvasView::getNodeToParentTransform() const
{
    bool transformDirty = _transformDirty;
    if (transformDirty)
    {
        _transform = GAFLayoutView::getNodeToParentTransform();
        _transformDirty = transformDirty;

        _transform.getScale(&m_internalScale);

        cocos2d::Vec3 inverseScale(1.0f / m_internalScale.x, 1.0f / m_internalScale.y, 1.0f / m_internalScale.z);
        _transform.scale(inverseScale);

        _transformDirty = false;
    }

    return _transform;
}

cocos2d::Rect GAFCanvasView::getInternalBoundingBox() const
{
    cocos2d::AffineTransform scaleMtx = cocos2d::AffineTransformMakeIdentity();
    scaleMtx = cocos2d::AffineTransformScale(scaleMtx, getInternalScale().x, getInternalScale().y);
    auto unscaledInternalBounds = GAFLayoutView::getInternalBoundingBox();
    return RectApplyAffineTransform(unscaledInternalBounds, scaleMtx);
}

cocos2d::Vec3 GAFCanvasView::getInternalScale() const
{
    if (_transformDirty)
        getNodeToParentTransform();

    return m_internalScale;
}

cocos2d::AffineTransform& GAFCanvasView::addAdditionalTransformations(cocos2d::AffineTransform& mtx) const
{
    cocos2d::AffineTransform scaleMtx = cocos2d::AffineTransformMakeIdentity();
    scaleMtx = cocos2d::AffineTransformScale(scaleMtx, getInternalScale().x, getInternalScale().y);
    affineTransformSetFrom(mtx, AffineTransformConcat(mtx, scaleMtx));
    return mtx;
}

cocos2d::AffineTransform & GAFCanvasView::processGAFTimelineStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx, const CustomPropertiesMap_t& customProperties)
{
    GAFObject::processGAFTimelineStateTransform(child, mtx, customProperties);
    return mtx;
}

cocos2d::AffineTransform& GAFCanvasView::processGAFImageStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx)
{
    GAFObject::processGAFImageStateTransform(child, mtx);
    addAdditionalTransformations(mtx);

    return mtx;
}

cocos2d::AffineTransform& GAFCanvasView::processGAFTextFieldStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx)
{
    GAFObject::processGAFImageStateTransform(child, mtx);
    addAdditionalTransformations(mtx);

    return mtx;
}

NS_GAF_END

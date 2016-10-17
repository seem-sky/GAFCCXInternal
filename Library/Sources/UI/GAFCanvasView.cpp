#include "GAFPrecompiled.h"
#include "GAFCanvasView.h"
#include "GAFObject.h"
#include "GAFTimeline.h"
#include "GAFUtils.h"

NS_GAF_BEGIN

GAFCanvasView::GAFCanvasView()
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
    auto unscaledInternalBounds = getFlashInitialInternalBoundingBox();
    auto actualInternalBounds = getFlashInternalBoundingBox();
    auto childActualBounds = RectApplyAffineTransform(child->getFlashInitialInternalBoundingBox(), mtx);

    bool usePercents = customProperties.at("alignMode") == "percents";
    bool useProportional = customProperties.at("alignMode") == "proportional";
    bool alignLeft = to_bool(customProperties.at("alignLeft"));
    bool alignRight = to_bool(customProperties.at("alignRight"));
    bool alignTop = to_bool(customProperties.at("alignTop"));
    bool alignBottom = to_bool(customProperties.at("alignBottom"));

    cocos2d::Vec2 fittingScale(getFittingScale());

    bool anyOfAlignSet = alignLeft || alignRight || alignTop || alignBottom;

    if (anyOfAlignSet)
    {
        float childAdditionalScaleX = 1.0f;
        float childAdditionalScaleY = 1.0f;

        float leftDistance = childActualBounds.origin.x - unscaledInternalBounds.origin.x;
        if (usePercents)
            leftDistance = leftDistance / unscaledInternalBounds.size.width * actualInternalBounds.size.width;
        else if (useProportional)
            leftDistance *= fittingScale.x;

        if (alignRight)
        {
            float rightDistance = unscaledInternalBounds.origin.x + unscaledInternalBounds.size.width - (childActualBounds.origin.x + childActualBounds.size.width);
            if (usePercents)
                rightDistance = rightDistance / unscaledInternalBounds.size.width * actualInternalBounds.size.width;
            else if (useProportional)
                rightDistance *= fittingScale.x;

            if (alignLeft)
            {
                childAdditionalScaleX = (actualInternalBounds.size.width - leftDistance - rightDistance) / childActualBounds.size.width;
            }
            else
            {
                if (m_scaleAlignedChildren)
                    childAdditionalScaleX = fittingScale.x;

                leftDistance = actualInternalBounds.size.width - rightDistance - childActualBounds.size.width * childAdditionalScaleX;
            }
        }
        else if (m_scaleAlignedChildren)
        {
            childAdditionalScaleX = fittingScale.x;
        }

        float finalTx = actualInternalBounds.origin.x + leftDistance + (mtx.tx - childActualBounds.origin.x) * childAdditionalScaleX;

        float topDistance = childActualBounds.origin.y - unscaledInternalBounds.origin.y;
        if (usePercents)
            topDistance = topDistance / unscaledInternalBounds.size.height * actualInternalBounds.size.height;
        else if (useProportional)
            topDistance *= fittingScale.y;

        if (alignBottom)
        {
            float bottomDistance = unscaledInternalBounds.origin.y + unscaledInternalBounds.size.height - (childActualBounds.origin.y + childActualBounds.size.height);
            if (usePercents)
                bottomDistance = bottomDistance / unscaledInternalBounds.size.height * actualInternalBounds.size.height;
            else if (useProportional)
                bottomDistance *= fittingScale.y;

            if (alignTop)
            {
                childAdditionalScaleY = (actualInternalBounds.size.height - topDistance - bottomDistance) / childActualBounds.size.height;
            }
            else
            {
                if (m_scaleAlignedChildren)
                    childAdditionalScaleY = fittingScale.y;

                topDistance = actualInternalBounds.size.height - bottomDistance - childActualBounds.size.height * childAdditionalScaleY;
            }
        }
        else if (m_scaleAlignedChildren)
        {
            childAdditionalScaleY = fittingScale.y;
        }

        float finalTy = actualInternalBounds.origin.y + topDistance + (mtx.ty - childActualBounds.origin.y) * childAdditionalScaleY;

        affineTransformSetFrom(mtx, cocos2d::AffineTransformScale(mtx, childAdditionalScaleX, childAdditionalScaleY));
        mtx.tx = finalTx;
        mtx.ty = finalTy;
    }
    else
    {
        cocos2d::AffineTransform scaleMtx = cocos2d::AffineTransformMakeIdentity();
        scaleMtx = cocos2d::AffineTransformScale(scaleMtx, fittingScale.x, fittingScale.y);
        affineTransformSetFrom(mtx, AffineTransformConcat(mtx, scaleMtx));
    }

    return mtx;
}

cocos2d::AffineTransform & GAFCanvasView::processGAFTimelineStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx, const CustomPropertiesMap_t& customProperties)
{
    GAFObject::processGAFTimelineStateTransform(child, mtx, customProperties);
    return mtx;
}

cocos2d::AffineTransform& GAFCanvasView::processGAFImageStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx)
{
    addAdditionalTransformations(mtx);
    GAFObject::processGAFImageStateTransform(child, mtx);

    return mtx;
}

cocos2d::AffineTransform& GAFCanvasView::processGAFTextFieldStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx)
{
    addAdditionalTransformations(mtx);
    GAFObject::processGAFTextFieldStateTransform(child, mtx);

    return mtx;
}

NS_GAF_END

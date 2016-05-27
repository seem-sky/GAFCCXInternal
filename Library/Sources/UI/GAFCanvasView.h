#pragma once
#include "GAFMacros.h"
#include "GAFLayoutView.h"

NS_GAF_BEGIN

class GAFCanvasView : public GAFLayoutView
{
public:
    GAFCanvasView();

    static GAFCanvasView* create(GAFAsset* anAsset, GAFTimeline* timeline);

protected:
    virtual cocos2d::AffineTransform& changeTransformAccordingToCustomProperties(GAFObject* child, const GAFSubobjectState* state, cocos2d::AffineTransform& mtx, const CustomPropertiesMap_t& customProperties) const override;
    virtual cocos2d::AffineTransform& addAdditionalTransformations(cocos2d::AffineTransform& mtx) const override;

    virtual cocos2d::AffineTransform& processGAFTimelineStateTransform(GAFObject* child, const GAFSubobjectState* state, cocos2d::AffineTransform& mtx) override;
    virtual cocos2d::AffineTransform& processGAFImageStateTransform(GAFObject* child, const GAFSubobjectState* state, cocos2d::AffineTransform& mtx) override;
    virtual cocos2d::AffineTransform& processGAFTextFieldStateTransform(GAFObject* child, const GAFSubobjectState* state, cocos2d::AffineTransform& mtx) override;
};

NS_GAF_END

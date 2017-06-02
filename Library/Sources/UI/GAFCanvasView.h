#pragma once
#include "GAFMacros.h"
#include "GAFLayoutView.h"

NS_GAF_BEGIN

class GAFCanvasView : public GAFLayoutView
{
public:
    GAFCanvasView();

    static GAFCanvasView* create(GAFAssetConstPtr anAsset, GAFTimelineConstPtr timeline);

protected:

    virtual cocos2d::AffineTransform& changeTransformAccordingToCustomProperties(GAFObject* child, cocos2d::AffineTransform& mtx, cp::GAFCustomPropertiesConstPtr customProperties) const override;

    virtual cocos2d::AffineTransform& processGAFTimelineStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx, cp::GAFCustomPropertiesConstPtr customProperties) override;
    virtual cocos2d::AffineTransform& processGAFImageStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx) override;
    virtual cocos2d::AffineTransform& processGAFTextFieldStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx) override;
};

NS_GAF_END

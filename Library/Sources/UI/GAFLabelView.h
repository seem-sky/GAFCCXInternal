#pragma once
#include "GAFMacros.h"
#include "GAFObject.h"
#include "GAFTextField.h"

NS_GAF_BEGIN

class GAFLabelView : public GAFObject
{
public:
    GAFLabelView();
    ~GAFLabelView();

    static GAFLabelView* create(GAFAsset* anAsset, GAFTimeline* timeline);
    virtual cocos2d::Rect getInternalBoundingBox() const override;
    virtual bool init(GAFAsset* anAnimationData, GAFTimeline* timeline) override;

protected:
    virtual void processOwnCustomProperties(const CustomPropertiesMap_t& customProperties) override;

    GAFTextField* m_textField;
};

NS_GAF_END

#pragma once

#include "GAFObject.h"

NS_GAF_BEGIN

gaf_fwd_this(GAFTextData);

class GAFTextField : public GAFObject
{
protected:
    cocos2d::Label* m_label;

    bool m_useTextBounds;

public:
    GAFTextField();
    virtual ~GAFTextField();

    void initWithTextData(const GAFTextDataConstPtr data);

    void setText(const std::string& text);

    void setUseTextBounds(bool value);
    bool getUseTextBounds() const;

    virtual cocos2d::Rect getInternalBoundingBox() const override;
};

NS_GAF_END
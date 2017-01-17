#include "GAFPrecompiled.h"
#include "GAFLabelView.h"
#include "GAFUtils.h"

gaf::GAFLabelView::GAFLabelView()
    : m_textField(nullptr)
{
}

gaf::GAFLabelView::~GAFLabelView()
{
    CC_SAFE_RELEASE(m_textField);
}

gaf::GAFLabelView* gaf::GAFLabelView::create(GAFAsset* anAsset, GAFTimeline* timeline)
{
    GAFLabelView* ret = new GAFLabelView();

    if (ret && ret->init(anAsset, timeline))
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_RELEASE(ret);
    return nullptr;
}

cocos2d::Rect gaf::GAFLabelView::getInternalBoundingBox() const
{
    CCASSERT(m_textField, "Error. No textfield found in this GAFLabelView");

    return m_textField->getBoundingBox();
}

bool gaf::GAFLabelView::init(GAFAsset* anAnimationData, GAFTimeline* timeline)
{
    bool ret = GAFObject::init(anAnimationData, timeline);
    if (ret)
    {
        m_textField = dynamic_cast<GAFTextField*>(getObjectByName("tf"));
        CCASSERT(m_textField, "Error. No textfield found in this GAFLabelView");

        CC_SAFE_RETAIN(m_textField);
    }

    return ret;
}

void gaf::GAFLabelView::processOwnCustomProperties(const CustomPropertiesMap_t& customProperties)
{
    CCASSERT(m_textField, "Error. No textfield found in this GAFLabelView");

    bool hasUseTextBoundsCP = customProperties.find("useTextBounds") != customProperties.end();
    CCASSERT(!hasUseTextBoundsCP, "Warning. Used old version of GAFUILabel.");

    bool useTextBounds = hasUseTextBoundsCP ? to_bool(customProperties.at("useTextBounds")) : false;

    m_textField->setUseTextBounds(useTextBounds);
}
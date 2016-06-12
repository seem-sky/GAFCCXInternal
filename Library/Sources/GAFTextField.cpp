#include "GAFPrecompiled.h"
#include "GAFTextField.h"

NS_GAF_BEGIN

GAFTextField::GAFTextField()
{
    m_charType = GAFCharacterType::TextField;
}

GAFTextField::~GAFTextField()
{
    CC_SAFE_RELEASE(m_label);
}

// TODO: Factory
void GAFTextField::initWithTextData(GAFTextData const* data)
{
    m_label = cocos2d::Label::createWithSystemFont(data->m_text, data->m_textFormat.m_font, data->m_textFormat.m_size);

    CC_SAFE_RETAIN(m_label);
    
    m_label->setDimensions(data->m_width, data->m_height);
    m_label->setTextColor(cocos2d::Color4B(data->m_textFormat.m_color));
    m_label->setHorizontalAlignment(data->m_textFormat.getTextAlignForCocos());

    cocos2d::Point anchor = cocos2d::Point(data->m_pivot.x / data->m_width, 1 - data->m_pivot.y / data->m_height);
    m_label->setAnchorPoint(anchor);

    addChild(m_label);
}

void GAFTextField::setText(const std::string& text)
{
    m_label->setString(text);
}

cocos2d::Rect GAFTextField::getBoundingBox() const
{
    cocos2d::Rect rect(m_label->getBoundingBox());
    return RectApplyAffineTransform(rect, getNodeToParentAffineTransform());
}

NS_GAF_END
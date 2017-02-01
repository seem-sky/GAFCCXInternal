#include "GAFPrecompiled.h"
#include "GAFLayoutView.h"
#include "GAFUtils.h"

NS_GAF_BEGIN

GAFLayoutView::GAFLayoutView()
    : m_internalScale(1.0f, 1.0f)
    , m_fittingMode(FittingMode::none)
    , m_scaleAlignedChildren(true)
{
}

GAFLayoutView::~GAFLayoutView()
{
}

cocos2d::AffineTransform & GAFLayoutView::addAdditionalTransformations(cocos2d::AffineTransform & mtx) const
{
    cocos2d::AffineTransform scaleMtx = cocos2d::AffineTransformMakeIdentity();
    cocos2d::Vec2 fittingScale(getFittingScale());
    scaleMtx = cocos2d::AffineTransformScale(scaleMtx, fittingScale.x, fittingScale.y);
    affineTransformSetFrom(mtx, AffineTransformConcat(mtx, scaleMtx));
    return mtx;
}

const cocos2d::Mat4& GAFLayoutView::getNodeToParentTransform() const
{
    bool transformDirty = _transformDirty;
    if (transformDirty)
    {
        _transform = GAFComponentView::getNodeToParentTransform();
        _transformDirty = transformDirty;

        cocos2d::Vec3 innnerScale;
        _transform.getScale(&innnerScale);

        m_internalScale.set(innnerScale.x, innnerScale.y);

        innnerScale.set(1.0f / innnerScale.x, 1.0f / innnerScale.y, 1.0f / innnerScale.z);
        _transform.scale(innnerScale);

        _transformDirty = false;
    }

    return _transform;
}

cocos2d::Vec2 GAFLayoutView::getInternalScale() const
{
    if (_transformDirty)
        getNodeToParentTransform();

    return m_internalScale;
}

cocos2d::Vec2 GAFLayoutView::getFittingScale() const
{
    cocos2d::Vec2 fittingScale;
    auto internalScale = getInternalScale();
    switch (m_fittingMode)
    {
    case FittingMode::none:
        fittingScale.set(internalScale);
        break;
    case FittingMode::minimum:
        fittingScale.x = fittingScale.y = (std::abs(internalScale.x) < std::abs(internalScale.y)) ? internalScale.x : internalScale.y;
        break;
    case FittingMode::maximum:
        fittingScale.x = fittingScale.y = (std::abs(internalScale.x) > std::abs(internalScale.y)) ? internalScale.x : internalScale.y;
        break;
    case FittingMode::horizontal:
        fittingScale.x = fittingScale.y = internalScale.x;
        break;
    case FittingMode::vertical:
        fittingScale.x = fittingScale.y = internalScale.x;
        break;
    default:
        fittingScale.set(internalScale);
    }

    return fittingScale;
}

cocos2d::Rect GAFLayoutView::getInternalBoundingBox() const
{
    cocos2d::AffineTransform scaleMtx = cocos2d::AffineTransformMakeIdentity();
    scaleMtx = cocos2d::AffineTransformScale(scaleMtx, getInternalScale().x, getInternalScale().y);
    cocos2d::Rect unscaledInternalBounds = GAFComponentView::getInternalBoundingBox();
    return RectApplyAffineTransform(unscaledInternalBounds, scaleMtx);
}

bool GAFLayoutView::allNecessaryFieldsExist(const CustomPropertiesMap_t & customProperties) const
{
    bool allFieldsExist =
        (customProperties.find("alignLeft") != customProperties.end())
        && (customProperties.find("alignRight") != customProperties.end())
        && (customProperties.find("alignTop") != customProperties.end())
        && (customProperties.find("alignBottom") != customProperties.end())
        && (customProperties.find("alignMode") != customProperties.end());

    return allFieldsExist;
}

bool GAFLayoutView::processOwnCustomProperties(const CustomPropertiesMap_t& customProperties)
{
    bool cpChanged = false;
    auto fittingMode = FittingMode::toEnum(customProperties.at("fittingMode"));
    auto scaleAlignedChildren = to_bool(customProperties.at("scaleAlignedChildren"));

    if (m_fittingMode != fittingMode)
    {
        m_fittingMode = fittingMode;
        cpChanged = true;
    }
    
    if (m_scaleAlignedChildren != scaleAlignedChildren)
    {
        m_scaleAlignedChildren = scaleAlignedChildren;
        cpChanged = true;
    }

    return cpChanged;
}

NS_GAF_END

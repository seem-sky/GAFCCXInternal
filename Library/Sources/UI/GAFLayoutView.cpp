#include "GAFPrecompiled.h"
#include "GAFLayoutView.h"
#include "GAFUtils.h"

NS_GAF_BEGIN

GAFLayoutView::GAFLayoutView()
    : m_internalScale(1.0f, 1.0f)
    , m_fittingMode(cp::FittingMode::none)
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
    case cp::FittingMode::none:
        fittingScale.set(internalScale);
        break;
    case cp::FittingMode::minimum:
        fittingScale.x = fittingScale.y = (std::abs(internalScale.x) < std::abs(internalScale.y)) ? internalScale.x : internalScale.y;
        break;
    case cp::FittingMode::maximum:
        fittingScale.x = fittingScale.y = (std::abs(internalScale.x) > std::abs(internalScale.y)) ? internalScale.x : internalScale.y;
        break;
    case cp::FittingMode::horizontal:
        fittingScale.x = fittingScale.y = internalScale.x;
        break;
    case cp::FittingMode::vertical:
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

bool GAFLayoutView::allNecessaryFieldsExist(cp::GAFCustomPropertiesConstPtr customProperties) const
{
    bool allFieldsExist = customProperties
        && customProperties->has<cp::CPEnum::alignTop>()
        && customProperties->has<cp::CPEnum::alignRight>()
        && customProperties->has<cp::CPEnum::alignBottom>()
        && customProperties->has<cp::CPEnum::alignLeft>()
        && customProperties->has<cp::CPEnum::alignMode>();

    return allFieldsExist;
}

bool GAFLayoutView::processOwnCustomProperties(cp::GAFCustomPropertiesConstPtr customProperties)
{
    bool cpChanged = false;
    auto fittingMode = customProperties->get<cp::CPEnum::fittingMode>();
    auto scaleAlignedChildren = customProperties->get<cp::CPEnum::scaleAlignedChildren>();

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

#include "GAFPrecompiled.h"
#include "GAFSubobjectState.h"
#include "GAFFilterData.h"
#include "GAFAsset.h"

#include "GAFCollections.h"

NS_GAF_BEGIN

GAFSubobjectState::GAFSubobjectState()
{
}

GAFSubobjectState::~GAFSubobjectState()
{
}

bool GAFSubobjectState::initEmpty(unsigned int ref)
{
    objectIdRef = ref;
    zIndex = 0;
    affineTransform = cocos2d::AffineTransformMake(1, 0, 0, 1, 0, 0);
    _colorOffsets[0] = _colorOffsets[1] = _colorOffsets[2] = _colorOffsets[3] = 0;
    _colorMults[GAFCTI_A] = -1;
    _colorMults[GAFCTI_R] = _colorMults[GAFCTI_G] = _colorMults[GAFCTI_B] = 1;
    return true;
}

void GAFSubobjectState::ctxMakeIdentity()
{
    _colorOffsets[0] = _colorOffsets[1] = _colorOffsets[2] = _colorOffsets[3] = 0;
    _colorMults[GAFCTI_R] = _colorMults[GAFCTI_G] = _colorMults[GAFCTI_B] = 1;
}

void GAFSubobjectState::pushFilter(GAFFilterDataConstPtr filter)
{
    m_filters.push_back(filter);
}

const Filters_t& GAFSubobjectState::getFilters() const
{
    return m_filters;
}

NS_GAF_END
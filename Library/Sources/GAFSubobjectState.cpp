#include "GAFPrecompiled.h"
#include "GAFSubobjectState.h"

#include "GAFCollections.h"
#include "GAFCustomProperties.h"

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
    m_colorOffsets[0] = m_colorOffsets[1] = m_colorOffsets[2] = m_colorOffsets[3] = 0;
    m_colorMults[GAFCTI_A] = -1;
    m_colorMults[GAFCTI_R] = m_colorMults[GAFCTI_G] = m_colorMults[GAFCTI_B] = 1;
    return true;
}

void GAFSubobjectState::ctxMakeIdentity()
{
    m_colorOffsets[0] = m_colorOffsets[1] = m_colorOffsets[2] = m_colorOffsets[3] = 0;
    m_colorMults[GAFCTI_R] = m_colorMults[GAFCTI_G] = m_colorMults[GAFCTI_B] = 1;
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
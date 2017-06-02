#pragma once

#include "GAFCollections.h"

NS_GAF_BEGIN

enum GAFColorTransformIndex
{
    GAFCTI_R,
    GAFCTI_G,
    GAFCTI_B,
    GAFCTI_A
};

gaf_fwd_this(GAFSubobjectState);

namespace cp
{
    gaf_fwd_this(GAFCustomProperties);
}


class GAFSubobjectState
{
    friend class GAFLoader;

private:
    Filters_t       m_filters;
    float           m_colorMults[4];
    float           m_colorOffsets[4];

    cp::GAFCustomPropertiesPtr m_customProperties;

public:
    uint32_t objectIdRef = IDNONE;
    uint32_t maskObjectIdRef = IDNONE;

    int zIndex;
    cocos2d::AffineTransform affineTransform;

    GAFSubobjectState();
    ~GAFSubobjectState();

    bool initEmpty(unsigned int objectIdRef);

    inline float * colorMults()
    {
        return &m_colorMults[0];
    }
    inline float * colorOffsets()
    {
        return &m_colorOffsets[0];
    }

    inline const float * colorMults() const 
    {
        return &m_colorMults[0];
    }

    inline const float * colorOffsets() const 
    {
        return &m_colorOffsets[0];
    }

    inline bool isVisible() const
    {
        return (m_colorMults[GAFCTI_A] > std::numeric_limits<float>::epsilon()) || (m_colorOffsets[GAFCTI_A] > std::numeric_limits<float>::epsilon());
    }
    
    cp::GAFCustomPropertiesConstPtr getCustomProperties() const
    {
        return m_customProperties;
    }

    void                ctxMakeIdentity();

    void                pushFilter(GAFFilterDataConstPtr filter);
    const Filters_t&    getFilters() const;
}; // GAFSubobjectState

NS_GAF_END
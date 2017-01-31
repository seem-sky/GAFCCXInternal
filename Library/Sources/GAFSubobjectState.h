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

class GAFSubobjectState
{
private:
    Filters_t       m_filters;
    float           _colorMults[4];
    float           _colorOffsets[4];
    
    std::vector<uint32_t> m_customPropertiesValueIdxs;

public:
    uint32_t objectIdRef = IDNONE;
    uint32_t maskObjectIdRef = IDNONE;

    int zIndex;
    cocos2d::AffineTransform affineTransform;

    bool initEmpty(unsigned int objectIdRef);

    ~GAFSubobjectState();
    GAFSubobjectState();

    inline float * colorMults()
    {
        return &_colorMults[0];
    }
    inline float * colorOffsets()
    {
        return &_colorOffsets[0];
    }

    inline const float * colorMults() const 
    {
        return &_colorMults[0];
    }

    inline const float * colorOffsets() const 
    {
        return &_colorOffsets[0];
    }

    inline bool isVisible() const
    {
        return (_colorMults[GAFCTI_A] > std::numeric_limits<float>::epsilon()) || (_colorOffsets[GAFCTI_A] > std::numeric_limits<float>::epsilon());
    }
    
    const std::vector<uint32_t>& getCustomPropertiesValueIdxs() const
    {
        return m_customPropertiesValueIdxs;
    }
    
    std::vector<uint32_t>& getCustomPropertiesValueIdxs()
    {
        return m_customPropertiesValueIdxs;
    }

    void                ctxMakeIdentity();

    void                pushFilter(GAFFilterDataConstPtr filter);
    const Filters_t&    getFilters() const;
}; // GAFSubobjectState

NS_GAF_END
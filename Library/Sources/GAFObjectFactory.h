#pragma once
#include "GAFMacros.h"
NS_GAF_BEGIN
    class GAFTimeline;
    class GAFTextData;
class GAFTextureAtlasElement;
class GAFAsset;
class GAFObject;

class GafObjectFactory
{
public:
    static GAFObject* create(GAFAsset* asset, uint32_t id, GAFTextureAtlasElement* txElemet, bool isMask);
    static GAFObject* create(GAFAsset* asset, uint32_t id, GAFTextData* txtData, bool isMask);
    static GAFObject* create(GAFAsset* asset, uint32_t id, GAFTimeline* timeline, bool isMask);
};

NS_GAF_END
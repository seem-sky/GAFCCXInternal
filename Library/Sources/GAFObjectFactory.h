#pragma once
#include "GAFMacros.h"
#include "GAFCollections.h"

NS_GAF_BEGIN

class GAFObject;
gaf_fwd_this(GAFTimeline);
gaf_fwd_this(GAFTextData);
gaf_fwd_this(GAFTextureAtlasElement);
gaf_fwd_this(GAFAsset);

class GafObjectFactory
{
private:
    typedef std::map<std::string, std::function<gaf::GAFObject*(GAFAssetConstPtr, GAFTimelineConstPtr)>> FactorySignatures_t;
public:
    static GAFObject* create(GAFAssetConstPtr asset, GAFTextureAtlasElementConstPtr txElemet, bool isMask = false, uint32_t id = IDNONE);
    static GAFObject* create(GAFAssetConstPtr asset, GAFTextDataConstPtr txtData, bool isMask = false, uint32_t id = IDNONE);
    static GAFObject* create(GAFAssetConstPtr asset, GAFTimelineConstPtr timeline, bool isMask = false, uint32_t id = IDNONE);
};

NS_GAF_END
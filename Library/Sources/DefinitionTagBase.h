#pragma once

NS_GAF_BEGIN

gaf_fwd_this(GAFStream);
gaf_fwd_this(GAFAsset);
gaf_fwd_this(GAFTimeline);

class DefinitionTagBase
{
public:
    virtual ~DefinitionTagBase() {};

    virtual void read(GAFStreamPtr, GAFAssetPtr, GAFTimelinePtr) = 0;
};

NS_GAF_END
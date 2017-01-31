#pragma once

NS_GAF_BEGIN

forward_this(GAFStream);
forward_this(GAFAsset);
forward_this(GAFTimeline);

class DefinitionTagBase
{
public:
    virtual ~DefinitionTagBase() {};

    virtual void read(GAFStreamPtr, GAFAssetPtr, GAFTimelinePtr) = 0;
};

NS_GAF_END
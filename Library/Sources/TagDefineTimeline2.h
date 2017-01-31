#pragma once

#include "TagDefineTimeline.h"

NS_GAF_BEGIN

gaf_fwd_this(GAFLoader);

class TagDefineTimeline2 : public TagDefineTimeline
{
public:
    explicit TagDefineTimeline2(GAFLoaderPtr loader);
    
    virtual void read(GAFStreamPtr, GAFAssetPtr, GAFTimelinePtr) override;
};

NS_GAF_END
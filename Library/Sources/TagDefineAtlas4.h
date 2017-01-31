#pragma once
#include "TagDefineAtlas3.h"

NS_GAF_BEGIN

class TagDefineAtlas4 : public gaf::TagDefineAtlas3
{
public:
    virtual void read(GAFStreamPtr, GAFAssetPtr, GAFTimelinePtr) override;
};

NS_GAF_END
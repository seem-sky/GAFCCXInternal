#pragma once

#include "DefinitionTagBase.h"

NS_GAF_BEGIN

class TagDefineSounds : public DefinitionTagBase
{
public:

    virtual void read(GAFStreamPtr, GAFAssetPtr, GAFTimelinePtr) override;

};

NS_GAF_END
#pragma once

#include "DefinitionTagBase.h"

NS_GAF_BEGIN

class TagDefineExternalObjects : public DefinitionTagBase
{
public:

    virtual void read(GAFStream*, GAFAsset*, GAFTimeline*) override;

};

NS_GAF_END
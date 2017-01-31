#pragma once

#include "DefinitionTagBase.h"

NS_GAF_BEGIN

gaf_fwd_this(GAFLoader);

class TagDefineTimeline : public DefinitionTagBase
{
protected:
    GAFLoaderWPtr m_loader; // weak

public:
    explicit TagDefineTimeline(GAFLoaderPtr loader);

    virtual void read(GAFStreamPtr, GAFAssetPtr, GAFTimelinePtr) override;

};

NS_GAF_END
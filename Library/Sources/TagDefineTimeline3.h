#pragma once

#include "TagDefineTimeline.h"

NS_GAF_BEGIN

class GAFLoader;

class TagDefineTimeline3 : public TagDefineTimeline
{
public:
    TagDefineTimeline3(GAFLoader* loader);

    virtual void read(GAFStream*, GAFAsset*, GAFTimeline*) override;
};

NS_GAF_END
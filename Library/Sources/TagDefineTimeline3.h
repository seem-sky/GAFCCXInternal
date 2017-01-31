#pragma once

#include "TagDefineTimeline.h"

NS_GAF_BEGIN

class GAFLoader;

class TagDefineTimeline3 : public TagDefineTimeline
{
public:
    explicit TagDefineTimeline3(GAFLoaderPtr loader);

    virtual void read(GAFStreamPtr, GAFAssetPtr, GAFTimelinePtr) override;
};

NS_GAF_END
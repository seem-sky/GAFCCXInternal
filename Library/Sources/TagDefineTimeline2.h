#pragma once

#include "TagDefineTimeline.h"

NS_GAF_BEGIN

class GAFLoader;

class TagDefineTimeline2 : public TagDefineTimeline
{
private:
    void parseCustomProperties(GAFTimeline* tl, const std::string& jsonStr);
    
public:
    TagDefineTimeline2(GAFLoader* loader);

    virtual void read(GAFStream*, GAFAsset*, GAFTimeline*) override;
};

NS_GAF_END
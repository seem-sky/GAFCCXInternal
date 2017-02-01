#include "GAFPrecompiled.h"
#include "TagDefineTimeline2.h"
#include "PrimitiveDeserializer.h"
#include "GAFLoader.h"
#include "GAFStream.h"
#include "GAFAsset.h"
#include "GAFTimeline.h"

#include "json/document.h"

NS_GAF_BEGIN

TagDefineTimeline2::TagDefineTimeline2(GAFLoaderPtr loader) : TagDefineTimeline(loader)
{
}

void TagDefineTimeline2::read(GAFStreamPtr in, GAFAssetPtr asset, GAFTimelinePtr timeline)
{
    unsigned int id = in->readU32();
    unsigned int framesCount = in->readU32();
    cocos2d::Rect aabb;
    cocos2d::Point pivot;
    
    PrimitiveDeserializer::deserialize(in, &aabb);
    PrimitiveDeserializer::deserialize(in, &pivot);
    
    auto tl = std::make_shared<GAFTimeline>(timeline, id, aabb, pivot, framesCount);
    
    //////////////////////////////////////////////////////////////////////////
    
    std::string temp;
    in->readString(&temp);
    tl->setLinkageName(temp);
    
    in->readString(&temp);
    tl->setBaseClass(temp);
    
    auto ldr = m_loader.lock();
    assert(ldr);

    ldr->loadTags(in, asset, tl);
    
    asset->pushTimeline(id, tl);

    if (id == 0)
        asset->setRootTimeline(0);
}

NS_GAF_END
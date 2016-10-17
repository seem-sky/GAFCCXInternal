#include "GAFPrecompiled.h"
#include "TagDefineTimeline3.h"
#include "PrimitiveDeserializer.h"
#include "GAFLoader.h"
#include "GAFStream.h"
#include "GAFAsset.h"
#include "GAFTimeline.h"

#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/prettywriter.h"

NS_GAF_BEGIN

TagDefineTimeline3::TagDefineTimeline3(GAFLoader* loader) : TagDefineTimeline(loader)
{
}

void TagDefineTimeline3::read(GAFStream* in, GAFAsset* asset, GAFTimeline* timeline)
{
    unsigned int id = in->readU32();
    unsigned int framesCount = in->readU32();
    cocos2d::Rect aabb;
    cocos2d::Point pivot;

    PrimitiveDeserializer::deserialize(in, &aabb);
    PrimitiveDeserializer::deserialize(in, &pivot);

    GAFTimeline *tl = new GAFTimeline(timeline, id, aabb, pivot, framesCount);  // will be released in assset dtor

    //////////////////////////////////////////////////////////////////////////

    std::string temp;
    in->readString(&temp);
    tl->setLinkageName(temp);

    in->readString(&temp);
    tl->setBaseClass(temp);

    CustomProperties_t properties;
    m_loader->readCustomProperties(in, &properties);
    tl->setCustomProperties(properties);

    m_loader->loadTags(in, asset, tl);

    asset->pushTimeline(id, tl);
    if (id == 0)
    {
        asset->setRootTimeline(0u);
    }
}

NS_GAF_END
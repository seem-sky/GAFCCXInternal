#include "GAFPrecompiled.h"
#include "TagDefineExternalObjects.h"

#include "GAFStream.h"
#include "GAFAsset.h"
#include "GAFTimeline.h"
#include "GAFSoundInfo.h"

NS_GAF_BEGIN

void TagDefineExternalObjects::read(GAFStream* in, GAFAsset* asset, GAFTimeline* timeline)
{
    uint32_t count = in->readU32();

    for (uint32_t i = 0; i < count; ++i)
    {
        uint32_t id = in->readU32();
        std::string name;
        in->readString(&name);
        timeline->pushExternalObject(id, name);
    }
}

NS_GAF_END
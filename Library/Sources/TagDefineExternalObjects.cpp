#include "GAFPrecompiled.h"
#include "TagDefineExternalObjects.h"

#include "GAFStream.h"
#include "GAFAsset.h"
#include "GAFTimeline.h"

NS_GAF_BEGIN

void TagDefineExternalObjects::read(GAFStream* in, GAFAsset*, GAFTimeline* timeline)
{
    uint32_t count = in->readU32();

    for (uint32_t i = 0; i < count; ++i)
    {
        uint32_t objectIdRef = in->readU32();

        std::string name;
        in->readString(&name);

        ExternalObject* externalObj = new ExternalObject(objectIdRef, name);

        timeline->pushExternalObject(externalObj);
    }
}

NS_GAF_END

#include "GAFPrecompiled.h"
#include "TagDefineExternalObjects2.h"
#include "GAFLoader.h"
#include "GAFStream.h"
#include "GAFAsset.h"
#include "GAFTimeline.h"

NS_GAF_BEGIN

TagDefineExternalObjects2::TagDefineExternalObjects2(GAFLoader* loader) :
    m_loader(loader)
{
}

void TagDefineExternalObjects2::read(GAFStream* in, GAFAsset*, GAFTimeline* timeline)
{
    uint32_t count = in->readU32();

    for (uint32_t i = 0; i < count; ++i)
    {
        uint32_t objectIdRef = in->readU32();

        std::string name;
        in->readString(&name);

        ExternalObject* externalObj = new ExternalObject(objectIdRef, name);

        CustomProperties_t properties;
        m_loader->readCustomProperties(in, &properties);
        externalObj->setCustomProperties(properties);

        timeline->pushExternalObject(externalObj);
    }
}

NS_GAF_END

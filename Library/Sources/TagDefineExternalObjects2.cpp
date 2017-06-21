#include "GAFPrecompiled.h"
#include "TagDefineExternalObjects2.h"
#include "GAFLoader.h"
#include "GAFStream.h"
#include "GAFAsset.h"
#include "GAFTimeline.h"

NS_GAF_BEGIN

TagDefineExternalObjects2::TagDefineExternalObjects2(GAFLoaderPtr loader)
    : m_loader(loader)
{
}

void TagDefineExternalObjects2::read(GAFStreamPtr in, GAFAssetPtr, GAFTimelinePtr timeline)
{
    uint32_t count = in->readU32();

    for (uint32_t i = 0; i < count; ++i)
    {
        uint32_t objectIdRef = in->readU32();

        std::string name;
        in->readString(&name);

        auto externalObj = std::make_shared<ExternalObject>(objectIdRef, name);

        auto ldr = m_loader.lock();
        assert(ldr);

        std::string jsonStr;
        in->readString(&jsonStr);
        if (!jsonStr.empty())
            ldr->setCustomProperties(GAFCharacterType::External, objectIdRef, std::move(jsonStr));

        timeline->pushExternalObject(externalObj);
    }
}

NS_GAF_END

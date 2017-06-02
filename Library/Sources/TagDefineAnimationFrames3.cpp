#include "GAFPrecompiled.h"
#include "TagDefineAnimationFrames3.h"

#include "GAFStream.h"
#include "GAFLoader.h"
#include "GAFTimeline.h"
#include "GAFSubobjectState.h"

gaf::TagDefineAnimationFrames3::TagDefineAnimationFrames3(GAFLoaderPtr loader)
    : TagDefineAnimationFrames2(loader)
{
}

gaf::GAFSubobjectStatePtr gaf::TagDefineAnimationFrames3::extractState(GAFStreamPtr in, GAFTimelinePtr timeline) const
{
    auto state = TagDefineAnimationFrames2::extractState(in, timeline);

    auto ldr = m_loader.lock();
    assert(ldr);

    size_t customPropertiesCount = in->readU16();
    if (customPropertiesCount > 0)
    {
        std::vector<size_t> indices;
        for (size_t i = 0; i < customPropertiesCount; ++i)
            indices.push_back(in->readU32());

        ldr->setCustomPropertiesIndices(state, std::move(indices));
    }

    return state;
}


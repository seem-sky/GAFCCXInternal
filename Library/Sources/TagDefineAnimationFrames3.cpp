#include "GAFPrecompiled.h"
#include "TagDefineAnimationFrames3.h"

#include "GAFStream.h"
#include "GAFLoader.h"
#include "GAFSubobjectState.h"

gaf::TagDefineAnimationFrames3::TagDefineAnimationFrames3(GAFLoaderPtr loader)
    : TagDefineAnimationFrames2(loader)
{
}

gaf::GAFSubobjectStatePtr gaf::TagDefineAnimationFrames3::extractState(GAFStreamPtr in, GAFTimelinePtr timeline) const
{
    auto state = TagDefineAnimationFrames2::extractState(in, timeline);

    uint16_t customPropertiesCount = in->readU16();
    for (uint16_t i = 0; i < customPropertiesCount; ++i)
    {
        uint32_t idx = in->readU32();
        state->getCustomPropertiesValueIdxs().push_back(idx);
    }

    return state;
}


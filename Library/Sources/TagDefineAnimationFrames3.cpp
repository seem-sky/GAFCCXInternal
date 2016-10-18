#include "GAFPrecompiled.h"
#include "TagDefineAnimationFrames3.h"

#include "GAFStream.h"
#include "GAFLoader.h"
#include "GAFTimeline.h"
#include "GAFSubobjectState.h"

gaf::TagDefineAnimationFrames3::TagDefineAnimationFrames3(GAFLoader* loader)
    : TagDefineAnimationFrames2(loader)
{
}

gaf::GAFSubobjectState* gaf::TagDefineAnimationFrames3::extractState(GAFStream* in, GAFTimeline* timeline) const
{
    GAFSubobjectState* state = TagDefineAnimationFrames2::extractState(in, timeline);

    uint16_t customPropertiesCount = in->readU16();
    for (uint16_t i = 0; i < customPropertiesCount; ++i)
    {
        uint32_t idx = in->readU32();
        state->getCustomPropertiesValueIdxs().push_back(idx);
    }

    return state;
}


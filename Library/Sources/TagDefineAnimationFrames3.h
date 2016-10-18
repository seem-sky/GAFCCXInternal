#pragma once
#pragma once

#include "TagDefineAnimationFrames2.h"

NS_GAF_BEGIN
class GAFLoader;
class GAFSubobjectState;

class TagDefineAnimationFrames3 : public TagDefineAnimationFrames2
{
protected:
    GAFSubobjectState* extractState(GAFStream* in, GAFTimeline* timeline) const override;

public:
    explicit TagDefineAnimationFrames3(GAFLoader* loader);
};

NS_GAF_END
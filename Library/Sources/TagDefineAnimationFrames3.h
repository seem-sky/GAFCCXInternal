#pragma once
#pragma once

#include "TagDefineAnimationFrames2.h"

NS_GAF_BEGIN

class TagDefineAnimationFrames3 : public TagDefineAnimationFrames2
{
protected:
    virtual GAFSubobjectStatePtr extractState(GAFStreamPtr in, GAFTimelinePtr timeline) const override;

public:
    explicit TagDefineAnimationFrames3(GAFLoaderPtr loader);
};

NS_GAF_END
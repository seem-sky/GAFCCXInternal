#pragma once
#include "GAFTimelineAction.h"

NS_GAF_BEGIN

class GAFTextureAtlas;

gaf_fwd_this(GAFSubobjectState);
gaf_fwd_this(GAFAnimationFrame);

class GAFAnimationFrame
{
public:
    typedef std::vector<GAFSubobjectStateConstPtr> SubobjectStates_t;
    typedef std::vector<GAFTimelineAction> TimelineActions_t;
private:
    SubobjectStates_t       m_subObjectStates;
    TimelineActions_t       m_timelineActions;
public:
    GAFAnimationFrame();
    ~GAFAnimationFrame();
    const SubobjectStates_t& getObjectStates() const;
    const TimelineActions_t& getTimelineActions() const;

    void    pushObjectState(GAFSubobjectStateConstPtr);
    void    pushTimelineAction(GAFTimelineAction action);
    void    sortStatesByZIndex();
};

NS_GAF_END
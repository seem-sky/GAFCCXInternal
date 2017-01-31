#include "GAFPrecompiled.h"
#include "GAFAnimationFrame.h"
#include "GAFSubobjectState.h"

NS_GAF_BEGIN

GAFAnimationFrame::GAFAnimationFrame()
{
}

GAFAnimationFrame::~GAFAnimationFrame()
{
}

const GAFAnimationFrame::SubobjectStates_t& GAFAnimationFrame::getObjectStates() const
{
    return m_subObjectStates;
}

const GAFAnimationFrame::TimelineActions_t & GAFAnimationFrame::getTimelineActions() const
{
    return m_timelineActions;
}

void GAFAnimationFrame::pushObjectState(GAFSubobjectStateConstPtr state)
{
    m_subObjectStates.push_back(state);
}

void GAFAnimationFrame::pushTimelineAction(GAFTimelineAction action)
{
    m_timelineActions.push_back(action);
}

void GAFAnimationFrame::sortStatesByZIndex()
{
    std::sort(m_subObjectStates.begin(), m_subObjectStates.end(),
        [](gaf::GAFSubobjectStateConstPtr state1, gaf::GAFSubobjectStateConstPtr state2)
        {
            return state1->zIndex < state2->zIndex;
        });
}

NS_GAF_END
#include "GAFPrecompiled.h"
#include "GAFAnimationFrame.h"
#include "GAFSubobjectState.h"
#include "GAFTextureAtlas.h"

NS_GAF_BEGIN

GAFAnimationFrame::GAFAnimationFrame()
{

}

GAFAnimationFrame::~GAFAnimationFrame()
{
    for (SubobjectStates_t::iterator i = m_subObjectStates.begin(), e = m_subObjectStates.end(); i != e; ++i)
    {
        (*i)->release();
    }
}

const GAFAnimationFrame::SubobjectStates_t& GAFAnimationFrame::getObjectStates() const
{
    return m_subObjectStates;
}

const GAFAnimationFrame::TimelineActions_t & GAFAnimationFrame::getTimelineActions() const
{
    return m_timelineActions;
}

void GAFAnimationFrame::pushObjectState(GAFSubobjectState* state)
{
    m_subObjectStates.push_back(state);
    state->addRef();
}

void GAFAnimationFrame::pushTimelineAction(GAFTimelineAction action)
{
    m_timelineActions.push_back(action);
}

void GAFAnimationFrame::sortStatesByZIndex()
{
    std::sort(m_subObjectStates.begin(), m_subObjectStates.end(),
        [](gaf::GAFSubobjectState* state1, gaf::GAFSubobjectState* state2)
    {
        return state1->zIndex < state2->zIndex;
    });
}

NS_GAF_END
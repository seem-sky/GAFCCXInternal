#include "GAFPrecompiled.h"
#include "GAFObject.h"
#include "GAFButton.h"
#include "GAFUIMacros.h"
#include <GAFCollections.h>

NS_GAF_BEGIN
NS_GAF_UI_BEGIN

const std::string GAFButton::STATE_NONE_STR = "state_none";
const std::string GAFButton::STATE_OVER_STR = "state_over";
const std::string GAFButton::STATE_DOWN_STR = "state_down";
const std::string GAFButton::STATE_DISABLED_STR = "state_disabled";
const std::string GAFButton::STATE_SELECTED_STR = "state_selected";
const std::string GAFButton::STATE_SELECTED_OVER_STR = "state_selected_over";
const std::string GAFButton::STATE_SELECTED_DOWN_STR = "state_selected_down";
const std::string GAFButton::STATE_SELECTED_DISABLED_STR = "state_selected_disabled";
const std::string GAFButton::STATE_UNDEFINED_STR = "state_undefined";

const std::string GAFButton::HIT_OBJECT_NAME = "touchBG";

GAFButton::GAFButton(gaf::GAFObject* view)
: GAFComponent(view)
, m_selected(false)
, m_checkBoxMode(false)
, m_currentState(State::STATE_UNDEFINED)
, m_nextState(State::STATE_UNDEFINED)
, m_inTransition(false)
{
}

GAFButton::~GAFButton()
{
}

bool GAFButton::init()
{
    bool result = GAFComponent::init();

    if (result)
    {
        setTouchEnabled(true);
        m_view->start();
        auto hitObject = m_view->getObjectByName(HIT_OBJECT_NAME);
        if (hitObject)
        {
            m_hitZone = hitObject->getBoundingBox();
        }
        else
        {
            m_hitZone = m_view->getInternalBoundingBoxForCurrentFrame();
        }
    }

    return result;
}

uint32_t GAFButton::getFrameByLabel(const std::string& name)
{
    return m_view->getStartFrame(name);
}

GAFButton::TransitionInfo_t GAFButton::getTransitionInfo()
{
    std::string transitionLabel = stateToString(m_currentState) + "_" + stateToString(m_nextState);

    if (getFrameByLabel(transitionLabel) != IDNONE)
        return std::make_tuple(true, std::move(transitionLabel), false);

    transitionLabel = stateToString(m_nextState) + "_" + stateToString(m_currentState);
    if (getFrameByLabel(transitionLabel) != IDNONE)
    {
        return std::make_tuple(true, std::move(transitionLabel), true);
    }

    return std::make_tuple(false, "", false);
}

void GAFButton::changeState(State state)
{
    if (m_nextState == state) return;

    if (m_inTransition)
        m_currentState = m_nextState;

    m_nextState = state;

    auto transitionInfo = getTransitionInfo();

    if (std::get<0>(transitionInfo))
    {
        auto label = std::get<1>(transitionInfo);
        auto reversed = std::get<2>(transitionInfo);
        m_view->setReversed(reversed);

        if (!m_inTransition)
        {
            m_view->setAnimationFinishedPlayDelegate(
                [this] (gaf::GAFObject*)
                {
                    this->setState();
                });

            m_inTransition = true;
        }

        m_view->playSequence(label, false);
    }
    else
    {
        setState();
    }
}

void GAFButton::setState()
{
    m_currentState = m_nextState;
    m_inTransition = false;
    m_view->setAnimationFinishedPlayDelegate(nullptr);
    m_view->playSequence(stateToString(m_nextState), true);
}

void GAFButton::setSelected(bool value)
{
    m_selected = value;

    if (m_selected)
    {
        changeState(!_touchEnabled ? State::STATE_SELECTED_DISABLED : State::STATE_SELECTED);
    }
    else
    {
        changeState(!_touchEnabled ? State::STATE_DISABLED : State::STATE_NONE);
    }
}

void GAFButton::setEnabled(bool value)
{
    GAFComponent::setEnabled(value);

    if (!_touchEnabled)
    {
        changeState(m_selected ? State::STATE_SELECTED_DISABLED : State::STATE_DISABLED);
        setTouchEnabled(false);
    }
    else
    {
        changeState(m_selected ? State::STATE_SELECTED : State::STATE_NONE);
        setTouchEnabled(true);
    }
}

const std::string& GAFButton::stateToString(State state)
{
    switch (state)
    {
    case State::STATE_NONE:
        return STATE_NONE_STR;
    case State::STATE_OVER:
        return STATE_OVER_STR;
    case State::STATE_DOWN:
        return STATE_DOWN_STR;
    case State::STATE_DISABLED:
        return STATE_DISABLED_STR;
    case State::STATE_SELECTED:
        return STATE_SELECTED_STR;
    case State::STATE_SELECTED_OVER:
        return STATE_SELECTED_OVER_STR;
    case State::STATE_SELECTED_DOWN:
        return STATE_SELECTED_DOWN_STR;
    case State::STATE_SELECTED_DISABLED:
        return STATE_SELECTED_DISABLED_STR;
    case State::STATE_UNDEFINED:
        return STATE_SELECTED_DISABLED_STR;
    }

    return STATE_SELECTED_DISABLED_STR;
}

void GAFButton::setCheckBoxMode(bool value)
{
    m_checkBoxMode = value;
}

void GAFButton::onPressStateChangedToNormal()
{
    changeState(m_selected ? State::STATE_SELECTED : State::STATE_NONE);
}

void GAFButton::onPressStateChangedToPressed()
{
    changeState(m_selected ? State::STATE_SELECTED_DOWN : State::STATE_DOWN);

    if (m_checkBoxMode) m_selected = !m_selected;
}

void GAFButton::onPressStateChangedToDisabled()
{
    changeState(m_selected ? State::STATE_SELECTED_DISABLED : State::STATE_DISABLED);
}

bool GAFButton::hitTest(const cocos2d::Vec2 &pt, const cocos2d::Camera* camera, cocos2d::Vec3 *p) const
{
    auto localTouchBeganPosition = m_view->convertToNodeSpace(pt);

    return m_hitZone.containsPoint(localTouchBeganPosition);
}

NS_GAF_UI_END
NS_GAF_END
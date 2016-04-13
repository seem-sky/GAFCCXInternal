#include "GAFPrecompiled.h"
#include "GAFObject.h"
#include "GAFButton.h"
#include "GAFUIMacros.h"
#include <GAFCollections.h>

NS_GAF_BEGIN
NS_GAF_UI_BEGIN

const char GAFButton::STATE_NONE_STR[] = "state_none";
const char GAFButton::STATE_OVER_STR[] = "state_over";
const char GAFButton::STATE_DOWN_STR[] = "state_down";
const char GAFButton::STATE_DISABLED_STR[] = "state_disabled";
const char GAFButton::STATE_SELECTED_STR[] = "state_selected";
const char GAFButton::STATE_SELECTED_OVER_STR[] = "state_selected_over";
const char GAFButton::STATE_SELECTED_DOWN_STR[] = "state_selected_down";
const char GAFButton::STATE_SELECTED_DISABLED_STR[] = "state_selected_disabled";

const char GAFButton::HIT_OBJECT_NAME[] = "touchBG";

GAFButton::GAFButton(gaf::GAFObject* view)
: GAFComponent(view)
, m_selected(false)
, m_checkBoxMode(false)
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
        checkFrameLabels();
        changeFrame(State::STATE_NONE);
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

void GAFButton::checkFrameLabels()
{
    auto stateFrame = getFrameByLabel(STATE_NONE_STR);
    if (stateFrame == IDNONE) stateFrame = static_cast<uint32_t>(State::STATE_NONE);
    m_frames.emplace(State::STATE_NONE, stateFrame);

    stateFrame = getFrameByLabel(STATE_OVER_STR);
    if (stateFrame == IDNONE) stateFrame = static_cast<uint32_t>(State::STATE_OVER);
    m_frames.emplace(State::STATE_OVER, stateFrame);

    stateFrame = getFrameByLabel(STATE_DOWN_STR);
    if (stateFrame == IDNONE) stateFrame = static_cast<uint32_t>(State::STATE_DOWN);
    m_frames.emplace(State::STATE_DOWN, stateFrame);

    stateFrame = getFrameByLabel(STATE_DISABLED_STR);
    if (stateFrame == IDNONE) stateFrame = static_cast<uint32_t>(State::STATE_DISABLED);
    m_frames.emplace(State::STATE_DISABLED, stateFrame);

    stateFrame = getFrameByLabel(STATE_SELECTED_STR);
    if (stateFrame == IDNONE) stateFrame = static_cast<uint32_t>(State::STATE_SELECTED);
    m_frames.emplace(State::STATE_SELECTED, stateFrame);

    stateFrame = getFrameByLabel(STATE_SELECTED_OVER_STR);
    if (stateFrame == IDNONE) stateFrame = static_cast<uint32_t>(State::STATE_SELECTED_OVER);
    m_frames.emplace(State::STATE_SELECTED_OVER, stateFrame);

    stateFrame = getFrameByLabel(STATE_SELECTED_DOWN_STR);
    if (stateFrame == IDNONE) stateFrame = static_cast<uint32_t>(State::STATE_SELECTED_DOWN);
    m_frames.emplace(State::STATE_SELECTED_DOWN, stateFrame);

    stateFrame = getFrameByLabel(STATE_SELECTED_DISABLED_STR);
    if (stateFrame == IDNONE) stateFrame = static_cast<uint32_t>(State::STATE_SELECTED_DISABLED);
    m_frames.emplace(State::STATE_SELECTED_DISABLED, stateFrame);
}

uint32_t GAFButton::getFrameByLabel(const std::string& name)
{
    return m_view->getStartFrame(name);
}

void GAFButton::changeFrame(State state)
{
    m_view->gotoAndStop(m_frames[state]);
}

void GAFButton::setSelected(bool value)
{
    m_selected = value;

    if (m_selected)
    {
        changeFrame(!_touchEnabled ? State::STATE_SELECTED_DISABLED : State::STATE_SELECTED);
    }
    else
    {
        changeFrame(!_touchEnabled ? State::STATE_DISABLED : State::STATE_NONE);
    }
}

void GAFButton::setEnabled(bool value)
{
    GAFComponent::setEnabled(value);

    if (!_touchEnabled)
    {
        changeFrame(m_selected ? State::STATE_SELECTED_DISABLED : State::STATE_DISABLED);
        setTouchEnabled(false);
    }
    else
    {
        changeFrame(m_selected ? State::STATE_SELECTED : State::STATE_NONE);
        setTouchEnabled(true);
    }
}

void GAFButton::setCheckBoxMode(bool value)
{
    m_checkBoxMode = value;
}

void GAFButton::onPressStateChangedToNormal()
{
    if (!m_selected)
    {
        changeFrame(State::STATE_NONE);
    }
    else if (m_checkBoxMode)
    {
        changeFrame(State::STATE_SELECTED);
    }
}

void GAFButton::onPressStateChangedToPressed()
{
    changeFrame(m_selected ? State::STATE_SELECTED_DOWN : State::STATE_DOWN);

    if (m_checkBoxMode) m_selected = !m_selected;
}

void GAFButton::onPressStateChangedToDisabled()
{
    changeFrame(m_selected ? State::STATE_SELECTED_DISABLED : State::STATE_DISABLED);
}

bool GAFButton::hitTest(const cocos2d::Vec2 &pt, const cocos2d::Camera* camera, cocos2d::Vec3 *p) const
{
    auto localTouchBeganPosition = m_view->convertToNodeSpace(pt);

    return m_hitZone.containsPoint(localTouchBeganPosition);
}

NS_GAF_UI_END
NS_GAF_END
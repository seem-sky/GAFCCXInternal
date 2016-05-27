#pragma once
#include "GAFComponent.h"

NS_GAF_BEGIN

class GAFObject;

NS_GAF_UI_BEGIN

class GAFButton : public GAFComponent
{
public:
    enum class State : uint8_t
    {
        STATE_NONE = 0,
        STATE_OVER = 1,
        STATE_DOWN = 2,
        STATE_DISABLED = 3,
        STATE_SELECTED = 4,
        STATE_SELECTED_OVER = 5,
        STATE_SELECTED_DOWN = 6,
        STATE_SELECTED_DISABLED = 7,
        STATE_UNDEFINED
    };

protected:
    static const std::string STATE_NONE_STR;
    static const std::string STATE_OVER_STR;
    static const std::string STATE_DOWN_STR;
    static const std::string STATE_DISABLED_STR;
    static const std::string STATE_SELECTED_STR;
    static const std::string STATE_SELECTED_OVER_STR;
    static const std::string STATE_SELECTED_DOWN_STR;
    static const std::string STATE_SELECTED_DISABLED_STR;
    static const std::string STATE_UNDEFINED_STR;

    static const std::string HIT_OBJECT_NAME;

    bool m_selected;
    bool m_checkBoxMode;

    GAFObject* m_hitObject;

    State m_currentState;
    State m_nextState;
    bool m_inTransition;

    GAF_CREATE_COMPONENT_CTORS(GAFButton)

    virtual bool init() override;

public:
    virtual ~GAFButton() override;

    bool isSelected() const { return m_selected; }
    virtual void setSelected(bool value);

    bool isCheckBoxMode() const { return m_checkBoxMode; }
    virtual void setCheckBoxMode(bool value);

    virtual void setEnabled(bool value) override;

protected:
    typedef std::tuple<bool, std::string, bool> TransitionInfo_t;

    static const std::string& stateToString(State state);

    uint32_t getFrameByLabel(const std::string& name);
    TransitionInfo_t getTransitionInfo();
    void changeState(State state);

    //call back function called widget's state changed to normal.
    virtual void onPressStateChangedToNormal() override;
    //call back function called widget's state changed to selected.
    virtual void onPressStateChangedToPressed() override;
    //call back function called widget's state changed to dark.
    virtual void onPressStateChangedToDisabled() override;

    virtual bool hitTest(const cocos2d::Vec2 &pt, const cocos2d::Camera* camera, cocos2d::Vec3 *p) const override;

private:
    void setState();
};

NS_GAF_UI_END
NS_GAF_END
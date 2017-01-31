#pragma once
#include "GAFMacros.h"
#include "GAFCollections.h"

NS_GAF_BEGIN

typedef std::vector<std::string> ActionParams_t;

gaf_fwd_this(GAFTimelineAction);

class GAFTimelineAction
{
public:
    GAFTimelineAction();

    enum ParameterIndex
    {
        PI_FRAME = 0,
        PI_EVENT_TYPE = 0,
        PI_EVENT_BUBBLES,
        PI_EVENT_CANCELABLE,
        PI_EVENT_DATA
    };

    void setAction(GAFActionType type, const ActionParams_t& params, const std::string& scope);
    GAFActionType getType();
    const std::string& getParam(ParameterIndex idx) const;

private:
    GAFActionType m_type;
    ActionParams_t m_params;
    std::string m_scope;
};

NS_GAF_END
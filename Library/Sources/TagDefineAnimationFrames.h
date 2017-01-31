#pragma once

#include "DefinitionTagBase.h"

NS_GAF_BEGIN

forward_this(GAFSubobjectState);

class TagDefineAnimationFrames : public DefinitionTagBase
{
private:
    using States_t = std::map<unsigned int, GAFSubobjectStatePtr>;
    States_t m_currentStates;
    
    GAFSubobjectStateUPtr extractState(GAFStreamPtr in) const;

public:
    
    ~TagDefineAnimationFrames();
    
    virtual void read(GAFStreamPtr, GAFAssetPtr, GAFTimelinePtr) override;

};

NS_GAF_END

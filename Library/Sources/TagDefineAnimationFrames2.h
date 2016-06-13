#pragma once

#include "DefinitionTagBase.h"

NS_GAF_BEGIN

class GAFSubobjectState;

class TagDefineAnimationFrames2 : public DefinitionTagBase
{
private:
    GAFSubobjectState* extractState(GAFStream* in);
    
    typedef std::map<unsigned int, GAFSubobjectState*> States_t;
    States_t m_currentStates;
    
    uint8_t m_version;
    
public:
	
    TagDefineAnimationFrames2(uint8_t version);
    ~TagDefineAnimationFrames2();
    
    virtual void read(GAFStream*, GAFAsset*, GAFTimeline*) override;

};

NS_GAF_END
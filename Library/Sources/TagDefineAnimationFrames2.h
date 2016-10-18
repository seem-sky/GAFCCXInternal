#pragma once

#include "DefinitionTagBase.h"

NS_GAF_BEGIN
    class GAFLoader;
    class GAFSubobjectState;

class TagDefineAnimationFrames2 : public DefinitionTagBase
{
protected:
    virtual GAFSubobjectState* extractState(GAFStream* in, GAFTimeline* timeline) const;
    
    typedef std::unordered_map<unsigned int, GAFSubobjectState*> States_t;
    States_t m_currentStates;

    GAFLoader*  m_loader; // weak

public:
    explicit TagDefineAnimationFrames2(GAFLoader* loader);
    ~TagDefineAnimationFrames2();
    
    virtual void read(GAFStream*, GAFAsset*, GAFTimeline*) override;

};

NS_GAF_END
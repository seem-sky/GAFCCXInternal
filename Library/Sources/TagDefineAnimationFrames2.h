#pragma once

#include "DefinitionTagBase.h"
#include "GAFLoader.h"

NS_GAF_BEGIN
forward_this(GAFLoader);
forward_this(GAFSubobjectState);

class TagDefineAnimationFrames2 : public DefinitionTagBase
{
protected:
    virtual GAFSubobjectStatePtr extractState(GAFStreamPtr in, GAFTimelinePtr timeline) const;
    
    typedef std::unordered_map<unsigned int, GAFSubobjectStatePtr> States_t;
    States_t m_currentStates;

    GAFLoaderWPtr m_loader; // weak

public:
    explicit TagDefineAnimationFrames2(GAFLoaderPtr loader);
    ~TagDefineAnimationFrames2();
    
    virtual void read(GAFStreamPtr, GAFAssetPtr, GAFTimelinePtr) override;

};

NS_GAF_END
#pragma once
#include "TagDefineExternalObjects.h"

NS_GAF_BEGIN

class GAFLoader;

class TagDefineExternalObjects2 : public TagDefineExternalObjects
{
protected:
    GAFLoader*  m_loader; // weak

public:
    explicit TagDefineExternalObjects2(GAFLoader*);
    virtual void read(GAFStream*, GAFAsset*, GAFTimeline*) override;

};

NS_GAF_END

#pragma once
#include "TagDefineExternalObjects.h"
#include "GAFLoader.h"

NS_GAF_BEGIN

forward_this(GAFLoader);

class TagDefineExternalObjects2 : public TagDefineExternalObjects
{
protected:
    GAFLoaderWPtr  m_loader; // weak

public:
    explicit TagDefineExternalObjects2(GAFLoaderPtr);
    virtual void read(GAFStreamPtr, GAFAssetPtr, GAFTimelinePtr) override;

};

NS_GAF_END

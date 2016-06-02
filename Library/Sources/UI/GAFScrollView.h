#pragma once
#include "GAFComponentView.h"

NS_GAF_BEGIN

class GAFScrollView : public GAFComponentView
{
protected:
    GAFObject* m_innerContainer;
    GAFObject* m_scrollBarH;
    GAFObject* m_scrollBarV;

public:
    GAFScrollView();

    static GAFScrollView* create(GAFAsset* anAsset, GAFTimeline* timeline);
    virtual bool init(GAFAsset* anAnimationData, GAFTimeline* timeline) override;

    inline GAFObject* getInnerContainer() const { return m_innerContainer; }
    GAFObject* getScrollBarH() const { return m_scrollBarH; }
    GAFObject* getScrollBarV() const { return m_scrollBarV; }

};

NS_GAF_END

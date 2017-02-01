#pragma once
#include "GAFLayoutView.h"

NS_GAF_BEGIN

class GAFScrollView : public GAFLayoutView
{
protected:
    GAFObject* m_innerContainer;
    GAFObject* m_scrollBarH;
    GAFObject* m_scrollBarV;

public:
    GAFScrollView();

    static GAFScrollView* create(GAFAssetConstPtr anAsset, GAFTimelineConstPtr timeline);
    virtual bool init(GAFAssetConstPtr anAnimationData, GAFTimelineConstPtr timeline) override;

    inline GAFObject* getInnerContainer() const { return m_innerContainer; }
    GAFObject* getScrollBarH() const { return m_scrollBarH; }
    GAFObject* getScrollBarV() const { return m_scrollBarV; }

    virtual void onEnter() override;

protected:
    virtual bool processOwnCustomProperties(const CustomPropertiesMap_t& customProperties) override;
    virtual void processGAFTimeline(cocos2d::Node* out, GAFObject* child, GAFSubobjectStateConstPtr state, cocos2d::AffineTransform& mtx) override;
};

NS_GAF_END

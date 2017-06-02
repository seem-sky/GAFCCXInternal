#pragma once
#include "GAFLayoutView.h"

NS_GAF_BEGIN
    class GAFBoxLayoutView : public GAFLayoutView
    {
    public:
        GAFBoxLayoutView();

        static GAFBoxLayoutView* create(GAFAssetConstPtr anAsset, GAFTimelineConstPtr timeline);
        virtual bool init(GAFAssetConstPtr anAnimationData, GAFTimelineConstPtr timeline) override;
        virtual cocos2d::Rect getInternalBoundingBox() const override;

        virtual void addChild(Node* child, int localZOrder, int tag) override;
        virtual void addChild(Node* child, int localZOrder, const std::string& name) override;
        virtual void removeChild(Node* child, bool cleanup = true) override;
        virtual void removeAllChildrenWithCleanup(bool cleanup) override;

        virtual void setExternalTransform(const cocos2d::AffineTransform& transform) override;
    protected:
        using ObjectStatePosition_t = std::tuple<GAFObject*, const GAFSubobjectStateConstPtr, cocos2d::AffineTransform>;
        using ObjectsStatesPositions_t = std::vector<ObjectStatePosition_t>;
        using Objects_t = std::vector<ObjectStatePosition_t>;

        cp::Direction m_direction;
        int m_gap;
        int m_marginTop;
        int m_marginRight;
        int m_marginBottom;
        int m_marginLeft;

        cp::GapMode m_gapMode;
        cp::MarginMode m_marginMode;

        cp::HorizontalAlign m_horizontalAlign;
        cp::VerticalAlign m_verticalAlign;

        mutable cocos2d::Rect m_dynamicContentBounds;
        mutable bool m_dynamicContentBoundsDirty;
        mutable bool m_layoutDirty;

        virtual bool processOwnCustomProperties(cp::GAFCustomPropertiesConstPtr customProperties) override;
        virtual void processStates(cocos2d::Node* out, uint32_t frameIndex, GAFAnimationFrameConstPtr frame) override;
        virtual cocos2d::AffineTransform& processGAFTimelineStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx, cp::GAFCustomPropertiesConstPtr customProperties) override;
        virtual cocos2d::AffineTransform& processGAFImageStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx) override;
        virtual cocos2d::AffineTransform& processGAFTextFieldStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx) override;

        virtual cocos2d::AffineTransform& addAdditionalTransformations(cocos2d::AffineTransform& mtx) const override;

        virtual void processEmbededChildren(cocos2d::Node* out, ObjectsStatesPositions_t& objects);
        virtual void processDynamicChildren(cocos2d::Node* out, ObjectsStatesPositions_t& objects);
        virtual void calculatePositions(ObjectsStatesPositions_t& objects) const;

        virtual cocos2d::Rect getDynamicContentBounds() const;
        virtual cocos2d::Point getGapScale() const;
        virtual cocos2d::Point getMarginScale() const;
    };

NS_GAF_END

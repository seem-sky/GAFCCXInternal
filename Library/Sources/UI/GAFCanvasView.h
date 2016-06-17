#pragma once
#include "GAFMacros.h"
#include "GAFLayoutView.h"

NS_GAF_BEGIN

class GAFCanvasView : public GAFLayoutView
{
    struct FittingMode
    {
        enum Enum
        {
            none,
            minimum,
            maximum,
            horizontal,
            vertical,
            unknown
        };

        static Enum toEnum(const std::string& fittingMode)
        {
            if (fittingMode == toString(none))
                return none;
            else if (fittingMode == toString(minimum))
                return minimum;
            else if (fittingMode == toString(maximum))
                return maximum;
            else if (fittingMode == toString(horizontal))
                return horizontal;
            else if (fittingMode == toString(vertical))
                return vertical;
            else
                return unknown;
        }

        static std::string toString(Enum fittingMode)
        {
            switch (fittingMode)
            {
            case none:
                return "none";
            case minimum:
                return "minimum";
            case maximum:
                return "maximum";
            case horizontal:
                return "horizontal";
            case vertical:
                return "vertical";
            default:
                return "unknown";
            }
        }
    };

public:
    GAFCanvasView();

    static GAFCanvasView* create(GAFAsset* anAsset, GAFTimeline* timeline);

protected:
    mutable cocos2d::Vec2 m_internalScale;
    FittingMode::Enum m_fittingMode;
    bool m_scaleAlignedChildren;

    virtual cocos2d::AffineTransform& changeTransformAccordingToCustomProperties(GAFObject* child, cocos2d::AffineTransform& mtx, const CustomPropertiesMap_t& customProperties) const override;
    virtual cocos2d::AffineTransform& addAdditionalTransformations(cocos2d::AffineTransform& mtx) const override;

    virtual cocos2d::AffineTransform& processGAFTimelineStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx, const CustomPropertiesMap_t& customProperties) override;
    virtual cocos2d::AffineTransform& processGAFImageStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx) override;
    virtual cocos2d::AffineTransform& processGAFTextFieldStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx) override;

    virtual void processOwnCustomProperties(const CustomPropertiesMap_t& customProperties) override;

    virtual const cocos2d::Mat4& getNodeToParentTransform() const override;
    virtual cocos2d::Rect getInternalBoundingBox() const override;
    virtual cocos2d::Vec2 getInternalScale() const;
    virtual cocos2d::Vec2 getFittingScale() const;
};

NS_GAF_END

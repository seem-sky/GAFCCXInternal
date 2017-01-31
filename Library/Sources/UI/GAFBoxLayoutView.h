#pragma once
#include "GAFLayoutView.h"

NS_GAF_BEGIN

class GAFBoxLayoutView : public GAFLayoutView
{
    struct Direction
    {
        enum Enum
        {
            horizontal,
            vertical,
            tiledByWidth,
            tiledByHeight,
            unknown
        };

        static Enum toEnum(const std::string& direction)
        {
            if (direction == toString(horizontal))
                return horizontal;
            else if (direction == toString(vertical))
                return vertical;
            else if (direction == toString(tiledByWidth))
                return tiledByWidth;
            else if (direction == toString(tiledByHeight))
                return tiledByHeight;
            else
                return unknown;
        }

        static std::string toString(Enum direction)
        {
            switch (direction)
            {
            case horizontal:
                return "horizontal";
            case vertical:
                return "vertical";
            case tiledByWidth:
                return "tiledByWidth";
            case tiledByHeight:
                return "tiledByHeight";
            default:
                return "unknown";
            }
        }
    };

    struct HorizontalAlign
    {
        enum Enum
        {
            left,
            center,
            right,
            unknown
        };

        static Enum toEnum(const std::string& align)
        {
            if (align == toString(left))
                return left;
            else if (align == toString(center))
                return center;
            else if (align == toString(right))
                return right;
            else
                return unknown;
        }

        static std::string toString(Enum align)
        {
            switch (align)
            {
            case left:
                return "left";
            case center:
                return "center";
            case right:
                return "right";
            default:
                return "unknown";
            }
        }
    };

    struct VerticalAlign
    {
        enum Enum
        {
            top,
            center,
            bottom,
            unknown
        };

        static Enum toEnum(const std::string& align)
        {
            if (align == toString(top))
                return top;
            else if (align == toString(center))
                return center;
            else if (align == toString(bottom))
                return bottom;
            else
                return unknown;
        }

        static std::string toString(Enum align)
        {
            switch (align)
            {
            case top:
                return "top";
            case center:
                return "center";
            case bottom:
                return "bottom";
            default:
                return "unknown";
            }
        }
    };

    struct MarginGapMode
    {
        enum Enum
        {
            pixels,
            percents,
            proportional,
            unknown
        };

        static Enum toEnum(const std::string& mode)
        {
            if (mode == toString(pixels))
                return pixels;
            else if (mode == toString(percents))
                return percents;
            else if (mode == toString(proportional))
                return proportional;
            else
                return unknown;
        }

        static std::string toString(Enum mode)
        {
            switch (mode)
            {
            case pixels:
                return "pixels";
            case percents:
                return "percents";
            case proportional:
                return "proportional";
            default:
                return "unknown";
            }
        }
    };

public:
    GAFBoxLayoutView();

    static GAFBoxLayoutView* create(GAFAssetConstPtr anAsset, GAFTimelineConstPtr timeline);
    virtual bool init(GAFAssetConstPtr anAnimationData, GAFTimelineConstPtr timeline) override;
    virtual cocos2d::Rect getInternalBoundingBox() const override;

    virtual void addChild(Node* child, int localZOrder, int tag) override;
    virtual void addChild(Node* child, int localZOrder, const std::string &name) override;
    virtual void removeChild(Node* child, bool cleanup = true) override;
    virtual void removeAllChildrenWithCleanup(bool cleanup) override;

protected:
    typedef std::tuple<GAFObject*, const GAFSubobjectStateConstPtr, cocos2d::AffineTransform> ObjectStatePosition_t;
    typedef std::vector<ObjectStatePosition_t> ObjectsStatesPositions_t;

    Direction::Enum m_direction;
    float m_gap;
    float m_marginTop;
    float m_marginRight;
    float m_marginBottom;
    float m_marginLeft;

    MarginGapMode::Enum m_gapMode;
    MarginGapMode::Enum m_marginMode;

    HorizontalAlign::Enum m_horizontalAlign;
    VerticalAlign::Enum m_verticalAlign;

    mutable cocos2d::Rect m_dynamicContentBounds;
    mutable bool m_dynamicContentBoundsDirty;

    virtual void processOwnCustomProperties(const CustomPropertiesMap_t& customProperties) override;
    virtual void processStates(cocos2d::Node* out, uint32_t frameIndex, GAFAnimationFrameConstPtr frame) override;
    virtual cocos2d::AffineTransform& addAdditionalTransformations(cocos2d::AffineTransform& mtx) const override;

    virtual void processChildren(cocos2d::Node* out, ObjectsStatesPositions_t& objects);

    virtual cocos2d::Rect getDynamicContentBounds() const;
    virtual cocos2d::Point getGapScale() const;
    virtual cocos2d::Point getMarginScale() const;
};

NS_GAF_END

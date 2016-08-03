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
            unknown
        };

        static Enum toEnum(const std::string& direction)
        {
            if (direction == toString(horizontal))
                return horizontal;
            else if (direction == toString(vertical))
                return vertical;
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


public:
    GAFBoxLayoutView();

    static GAFBoxLayoutView* create(GAFAsset* anAsset, GAFTimeline* timeline);
    virtual bool init(GAFAsset* anAnimationData, GAFTimeline* timeline) override;
    virtual cocos2d::Rect getInternalBoundingBox() const override;

    virtual void addChild(Node* child, int localZOrder, int tag) override;
    virtual void addChild(Node* child, int localZOrder, const std::string &name) override;
    virtual void removeChild(Node* child, bool cleanup = true) override;
    virtual void removeAllChildrenWithCleanup(bool cleanup) override;

protected:
    Direction::Enum m_direction;
    float m_gap;
    float m_marginTop;
    float m_marginRight;
    float m_marginBottom;
    float m_marginLeft;

    HorizontalAlign::Enum m_horizontalAlign;
    VerticalAlign::Enum m_verticalAlign;

    bool m_usePercents;

    mutable cocos2d::Rect m_dynamicContentBounds;
    mutable bool m_dynamicContentBoundsDirty;

    virtual void processOwnCustomProperties(const CustomPropertiesMap_t& customProperties) override;
    virtual void processStates(cocos2d::Node* out, uint32_t frameIndex, const GAFAnimationFrame* frame) override;

    virtual cocos2d::Point& layoutChild(const GAFObject* subObject, cocos2d::AffineTransform& stateMatrix, cocos2d::Point& currPos, const cocos2d::Rect& actualInternalBounds) const;

    virtual cocos2d::Rect getDynamicContentBounds() const;
};

NS_GAF_END

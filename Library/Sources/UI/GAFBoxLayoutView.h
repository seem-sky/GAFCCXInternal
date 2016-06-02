#pragma once
#include "GAFLayoutView.h"

NS_GAF_BEGIN
class GAFBoxLayoutView : public GAFLayoutView
{
    enum class Direction : uint8_t
    {
        horizontal,
        vertical,
        unknown
    };

    enum class HorizontalAlign : uint8_t
    {
        left,
        center,
        right,
        unknown
    };

    enum class VerticalAlign : uint8_t
    {
        top,
        center,
        bottom,
        unknown
    };

    static Direction toEnumDirection(const std::string& direction)
    {
        if (direction == toString(Direction::horizontal))
            return Direction::horizontal;
        else if (direction == toString(Direction::vertical))
            return Direction::vertical;
        else
            return Direction::unknown;
    }

    static HorizontalAlign toEnumHorizontalAlign(const std::string& align)
    {
        if (align == toString(HorizontalAlign::left))
            return HorizontalAlign::left;
        else if (align == toString(HorizontalAlign::center))
            return HorizontalAlign::center;
        else if (align == toString(HorizontalAlign::right))
            return HorizontalAlign::right;
        else
            return HorizontalAlign::unknown;
    }

    static VerticalAlign toEnumVerticalAlign(const std::string& align)
    {
        if (align == toString(VerticalAlign::top))
            return VerticalAlign::top;
        else if (align == toString(VerticalAlign::center))
            return VerticalAlign::center;
        else if (align == toString(VerticalAlign::bottom))
            return VerticalAlign::bottom;
        else
            return VerticalAlign::unknown;
    }

    static std::string toString(Direction direction)
    {
        switch (direction)
        {
        case Direction::horizontal:
            return "horizontal";
        case Direction::vertical:
            return "horizontal";
        default:
            return "unknown";
        }
    }

    static std::string toString(HorizontalAlign align)
    {
        switch (align)
        {
        case HorizontalAlign::left:
            return "left";
        case HorizontalAlign::center:
            return "center";
        case HorizontalAlign::right:
            return "right";
        default:
            return "unknown";
        }
    }

    static std::string toString(VerticalAlign align)
    {
        switch (align)
        {
        case VerticalAlign::top:
            return "top";
        case VerticalAlign::center:
            return "center";
        case VerticalAlign::bottom:
            return "bottom";
        default:
            return "unknown";
        }
    }

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
    Direction m_direction;
    float m_gap;
    float m_marginTop;
    float m_marginRight;
    float m_marginBottom;
    float m_marginLeft;

    HorizontalAlign m_horizontalAlign;
    VerticalAlign m_verticalAlign;

    bool m_usePercents;

    mutable cocos2d::Rect m_dynamicContentBounds;
    mutable bool m_dynamicContentBoundsDirty;

    virtual void processOwnCustomProperties(const CustomPropertiesMap_t& customProperties) override;
    virtual void processStates(cocos2d::Node* out, uint32_t frameIndex, const GAFAnimationFrame* frame) override;

    virtual cocos2d::Point& layoutChild(const GAFObject* subObject, cocos2d::AffineTransform& stateMatrix, cocos2d::Point& currentTopLeft, const cocos2d::Rect& actualInternalBounds) const;

    virtual cocos2d::Rect getDynamicContentBounds() const;
};

NS_GAF_END

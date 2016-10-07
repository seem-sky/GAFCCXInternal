#pragma once
#include "GAFComponentView.h"

NS_GAF_BEGIN

class GAFLayoutView : public GAFComponentView
{
protected:
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
    GAFLayoutView();
    virtual ~GAFLayoutView() = 0;

    //virtual const cocos2d::Mat4& getNodeToParentTransform() const override;
    //virtual cocos2d::Rect getInternalBoundingBox() const override;
    //virtual cocos2d::Vec3 getInternalScale() const;

protected:
    mutable cocos2d::Vec2 m_internalScale;
    FittingMode::Enum m_fittingMode;
    bool m_scaleAlignedChildren;

    virtual cocos2d::AffineTransform& addAdditionalTransformations(cocos2d::AffineTransform& mtx) const override;

    virtual const cocos2d::Mat4& getNodeToParentTransform() const override;

    virtual cocos2d::Vec2 getInternalScale() const;
    virtual cocos2d::Vec2 getFittingScale() const;
    virtual cocos2d::Rect getInternalBoundingBox() const override;

    virtual bool allNecessaryFieldsExist(const CustomPropertiesMap_t& customProperties) const override;
    virtual void processOwnCustomProperties(const CustomPropertiesMap_t& customProperties) override;
};

NS_GAF_END

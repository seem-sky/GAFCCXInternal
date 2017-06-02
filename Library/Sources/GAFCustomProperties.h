#pragma once
#include <bitset>
#include <tuple>

NS_GAF_BEGIN

//#define invalid_enum(__enum__) (__enum__(std::numeric_limits<std::underlying_type<__enum__>::type>::max()))

    namespace cp
    {
        extern const char kCPAlignMode[];
        extern const char kCPAlignTop[];
        extern const char kCPAlignRight[];
        extern const char kCPAlignBottom[];
        extern const char kCPAlignLeft[];
        extern const char kCPScaleAlignedChildren[];
        extern const char kCPFittingMode[];
        extern const char kCPMarginTop[];
        extern const char kCPMarginRight[];
        extern const char kCPMarginBottom[];
        extern const char kCPMarginLeft[];
        extern const char kCPMarginMode[];
        extern const char kCPGap[];
        extern const char kCPGapMode[];
        extern const char kCPDirection[];
        extern const char kCPVerticalAlign[];
        extern const char kCPHorizontalAlign[];
        extern const char kCPUseTextBounds[];

        extern const char kAlignModePixels[];
        extern const char kAlignModePercents[];
        extern const char kAlignModeProportional[];

        extern const char kFittingModeNone[];
        extern const char kFittingModeMinimum[];
        extern const char kFittingModeMaximum[];
        extern const char kFittingModeHorizontal[];
        extern const char kFittingModeVertical[];

        extern const char kMarginModePixels[];
        extern const char kMarginModePercents[];
        extern const char kMarginModeProportional[];

        extern const char kGapModePixels[];
        extern const char kGapModePercents[];
        extern const char kGapModeProportional[];

        extern const char kDirectionHorizontal[];
        extern const char kDirectionVertical[];
        extern const char kDirectionTiledByWidth[];
        extern const char kDirectionTiledByHeight[];

        extern const char kVerticalAlignTop[];
        extern const char kVerticalAlignCenter[];
        extern const char kVerticalAlignBottom[];

        extern const char kHorizontalAlignLeft[];
        extern const char kHorizontalAlignCenter[];
        extern const char kHorizontalAlignRight[];

        template<typename E>
        constexpr auto to_integral(E e) -> typename std::underlying_type<E>::type
        {
            return static_cast<typename std::underlying_type<E>::type>(e);
        }

        //#define invalid_enum(__enum__) (__enum__(std::numeric_limits<std::underlying_type<__enum__>::type>::max()))
        template<typename E>
        constexpr E invalid_enum()
        {
            return E(std::numeric_limits<typename std::underlying_type<E>::type>::max());
        }
        
        template<typename Enum>
        Enum toEnum(const std::string& value)
        {
            return invalid_enum<Enum>();
        }

        enum class AlignMode : uint8_t
        {
            pixels,
            percents,
            proportional
        };

        template<>
        inline AlignMode toEnum<AlignMode>(const std::string& value)
        {
            if (value == kAlignModePixels)
                return AlignMode::pixels;
            else if (value == kAlignModePercents)
                return AlignMode::percents;
            else if (value == kAlignModeProportional)
                return AlignMode::proportional;
            else
            {
                assert(false);
                return invalid_enum<AlignMode>();
            }
        }

        enum class FittingMode : uint8_t
        {
            none,
            minimum,
            maximum,
            horizontal,
            vertical
        };

        template<>
        inline FittingMode toEnum<FittingMode>(const std::string& value)
        {
            if (value == kFittingModeNone)
                return FittingMode::none;
            else if (value == kFittingModeMinimum)
                return FittingMode::minimum;
            else if (value == kFittingModeMaximum)
                return FittingMode::maximum;
            else if (value == kFittingModeHorizontal)
                return FittingMode::horizontal;
            else if (value == kFittingModeVertical)
                return FittingMode::vertical;
            else
            {
                assert(false);
                return invalid_enum<FittingMode>();
            }
        }

        enum class MarginMode : uint8_t
        {
            pixels,
            percents,
            proportional
        };

        template<>
        inline MarginMode toEnum<MarginMode>(const std::string& value)
        {
            if (value == kMarginModePixels)
                return MarginMode::pixels;
            else if (value == kMarginModePercents)
                return MarginMode::percents;
            else if (value == kMarginModeProportional)
                return MarginMode::proportional;
            else
            {
                assert(false);
                return invalid_enum<MarginMode>();
            }
        }

        enum class GapMode : uint8_t
        {
            pixels,
            percents,
            proportional
        };

        template<>
        inline GapMode toEnum<GapMode>(const std::string& value)
        {
            if (value == kGapModePixels)
                return GapMode::pixels;
            else if (value == kGapModePercents)
                return GapMode::percents;
            else if (value == kGapModeProportional)
                return GapMode::proportional;
            else
            {
                assert(false);
                return invalid_enum<GapMode>();
            }
        }

        enum class Direction : uint8_t
        {
            horizontal,
            vertical,
            tiledByWidth,
            tiledByHeight
        };

        template<>
        inline Direction toEnum<Direction>(const std::string& value)
        {
            if (value == kDirectionHorizontal)
                return Direction::horizontal;
            else if (value == kDirectionVertical)
                return Direction::vertical;
            else if (value == kDirectionTiledByWidth)
                return Direction::tiledByWidth;
            else if (value == kDirectionTiledByHeight)
                return Direction::tiledByHeight;
            else
            {
                assert(false);
                return invalid_enum<Direction>();
            }
        }

        enum class VerticalAlign : uint8_t
        {
            top,
            center,
            bottom
        };

        template<>
        inline VerticalAlign toEnum<VerticalAlign>(const std::string& value)
        {
            if (value == kVerticalAlignTop)
                return VerticalAlign::top;
            else if (value == kVerticalAlignCenter)
                return VerticalAlign::center;
            else if (value == kVerticalAlignBottom)
                return VerticalAlign::bottom;
            else
            {
                assert(false);
                return invalid_enum<VerticalAlign>();
            }
        }

        enum class HorizontalAlign : uint8_t
        {
            left,
            center,
            right
        };

        template<>
        inline HorizontalAlign toEnum<HorizontalAlign>(const std::string& value)
        {
            if (value == kHorizontalAlignLeft)
                return HorizontalAlign::left;
            else if (value == kHorizontalAlignCenter)
                return HorizontalAlign::center;
            else if (value == kHorizontalAlignRight)
                return HorizontalAlign::right;
            else
            {
                assert(false);
                return invalid_enum<HorizontalAlign>();
            }
        }

        enum class CPEnum
        {
            alignMode,
            alignTop,
            alignRight,
            alignBottom,
            alignLeft,
            scaleAlignedChildren,
            fittingMode,
            marginTop,
            marginRight,
            marginBottom,
            marginLeft,
            marginMode,
            gap,
            gapMode,
            direction,
            verticalAlign,
            horizontalAlign,
            useTextBounds
        };

        template<>
        inline CPEnum toEnum<CPEnum>(const std::string& value)
        {
            if (value == kCPAlignMode)
                return CPEnum::alignMode;
            else if (value == kCPAlignTop)
                return CPEnum::alignTop;
            else if (value == kCPAlignRight)
                return CPEnum::alignRight;
            else if (value == kCPAlignBottom)
                return CPEnum::alignBottom;
            else if (value == kCPAlignLeft)
                return CPEnum::alignLeft;
            else if (value == kCPScaleAlignedChildren)
                return CPEnum::scaleAlignedChildren;
            else if (value == kCPFittingMode)
                return CPEnum::fittingMode;
            else if (value == kCPMarginTop)
                return CPEnum::marginTop;
            else if (value == kCPMarginRight)
                return CPEnum::marginRight;
            else if (value == kCPMarginBottom)
                return CPEnum::marginBottom;
            else if (value == kCPMarginLeft)
                return CPEnum::marginLeft;
            else if (value == kCPMarginMode)
                return CPEnum::marginMode;
            else if (value == kCPGap)
                return CPEnum::gap;
            else if (value == kCPGapMode)
                return CPEnum::gapMode;
            else if (value == kCPDirection)
                return CPEnum::direction;
            else if (value == kCPVerticalAlign)
                return CPEnum::verticalAlign;
            else if (value == kCPHorizontalAlign)
                return CPEnum::horizontalAlign;
            else if (value == kCPUseTextBounds)
                return CPEnum::useTextBounds;
            else
            {
                assert(false);
                return invalid_enum<CPEnum>();
            }
        }

        template<CPEnum e>
        struct CPType
        {
        };

        template<>
        struct CPType<CPEnum::alignMode>
        {
            using type = AlignMode;
        };

        template<>
        struct CPType<CPEnum::alignTop>
        {
            using type = bool;
        };

        template<>
        struct CPType<CPEnum::alignRight>
        {
            using type = bool;
        };

        template<>
        struct CPType<CPEnum::alignBottom>
        {
            using type = bool;
        };

        template<>
        struct CPType<CPEnum::alignLeft>
        {
            using type = bool;
        };

        template<>
        struct CPType<CPEnum::scaleAlignedChildren>
        {
            using type = bool;
        };

        template<>
        struct CPType<CPEnum::fittingMode>
        {
            using type = FittingMode;
        };

        template<>
        struct CPType<CPEnum::marginTop>
        {
            using type = int;
        };

        template<>
        struct CPType<CPEnum::marginRight>
        {
            using type = int;
        };

        template<>
        struct CPType<CPEnum::marginBottom>
        {
            using type = int;
        };

        template<>
        struct CPType<CPEnum::marginLeft>
        {
            using type = int;
        };

        template<>
        struct CPType<CPEnum::marginMode>
        {
            using type = MarginMode;
        };

        template<>
        struct CPType<CPEnum::gap>
        {
            using type = int;
        };

        template<>
        struct CPType<CPEnum::gapMode>
        {
            using type = GapMode;
        };

        template<>
        struct CPType<CPEnum::direction>
        {
            using type = Direction;
        };

        template<>
        struct CPType<CPEnum::verticalAlign>
        {
            using type = VerticalAlign;
        };

        template<>
        struct CPType<CPEnum::horizontalAlign>
        {
            using type = HorizontalAlign;
        };

        template<>
        struct CPType<CPEnum::useTextBounds>
        {
            using type = bool;
        };

        gaf_fwd_this(GAFCustomProperties);

        class GAFCustomProperties
        {
            std::tuple<
                CPType<CPEnum::alignMode>::type,
                CPType<CPEnum::alignTop>::type,
                CPType<CPEnum::alignRight>::type,
                CPType<CPEnum::alignBottom>::type,
                CPType<CPEnum::alignLeft>::type,
                CPType<CPEnum::scaleAlignedChildren>::type,
                CPType<CPEnum::fittingMode>::type,
                CPType<CPEnum::marginTop>::type,
                CPType<CPEnum::marginRight>::type,
                CPType<CPEnum::marginBottom>::type,
                CPType<CPEnum::marginLeft>::type,
                CPType<CPEnum::marginMode>::type,
                CPType<CPEnum::gap>::type,
                CPType<CPEnum::gapMode>::type,
                CPType<CPEnum::direction>::type,
                CPType<CPEnum::verticalAlign>::type,
                CPType<CPEnum::horizontalAlign>::type,
                CPType<CPEnum::useTextBounds>::type>
                m_CPs;

            std::bitset<std::tuple_size<decltype(m_CPs)>::value> m_availableCPs;

        public:
            template<CPEnum prop>
            bool has() const;

            template<CPEnum prop>
            typename CPType<prop>::type get() const;

            template<CPEnum prop, typename T>
            void set(T value);
        };

        template<CPEnum prop>
        bool GAFCustomProperties::has() const
        {
            return m_availableCPs.test(to_integral(prop));
        }

        template<CPEnum prop>
        typename CPType<prop>::type GAFCustomProperties::get() const
        {
            return std::get<to_integral(prop)>(m_CPs);
        }

        template<CPEnum prop, typename T>
        void GAFCustomProperties::set(T value)
        {
            std::get<to_integral(prop)>(m_CPs) = value;
            m_availableCPs.set(to_integral(prop));
        }
    }


NS_GAF_END

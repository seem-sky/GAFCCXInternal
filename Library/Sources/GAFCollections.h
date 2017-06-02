#pragma once

#include <memory>
#include <map>
#include <unordered_map>
#include "GAFMacros.h"

NS_GAF_BEGIN

class GAFSprite;
class GAFObject;
gaf_fwd_this(GAFTextureAtlas);
gaf_fwd_this(GAFAnimationFrame);
gaf_fwd_this(GAFFilterData);
gaf_fwd_this(GAFTimeline);
gaf_fwd_this(ExternalObject);
gaf_fwd_this(GAFTextData);
gaf_fwd_this(GAFSoundInfo);
gaf_fwd_this(GAFAnimationSequence);

enum class GAFFilterType : uint32_t
{
    DropShadow = 0,
    Blur = 1,
    Glow = 2,
    ColorMatrix = 6
};

enum class GAFCharacterType : uint32_t
{
    Texture = 0,
    TextField,
    Timeline,
    External
};

enum class GAFObjectType : uint32_t
{
    None = 0,
    MovieClip,
    Mask
};

enum class GAFRotation : int8_t
{
    CCW_90 = -1,
    NONE = 0,
    CW_90 = 1,
};

enum class GAFActionType : int32_t
{
    //0 - stop, 1 - play, 2 - gotoAndStop, 3 - gotoAndPlay, 4 - dispatchEvent
    None = -1,
    Stop = 0,
    Play,
    GotoAndStop,
    GotoAndPlay,
    DispatchEvent,
};

enum
{
    GAFFirstFrameIndex = 0
};

gaf_fwd_this(GAFAnyInterface);

class GAFAnyInterface
{
public:
    ~GAFAnyInterface() = default;
};

template <typename T>
class GAFAny : public GAFAnyInterface
{
public:
    explicit GAFAny(const T& data) : data(data) {}
    T data;
};

using AnimationObjectEx_t = std::tuple<uint32_t, GAFCharacterType>;
using TextureAtlases_t = std::vector<GAFTextureAtlasConstPtr>;
using AnimationMasks_t = std::unordered_map<uint32_t, AnimationObjectEx_t>;      // Object id -> Element Atlas Id, Type
using AnimationObjects_t = std::unordered_map<uint32_t, AnimationObjectEx_t>;    // Object id -> Element Atlas Id, Type
using AnimationFrames_t = std::vector<GAFAnimationFrameConstPtr>;

using Filters_t = std::vector<GAFFilterDataConstPtr>;
using Timelines_t = std::unordered_map<uint32_t, GAFTimelinePtr>;
using TextsData_t = std::unordered_map<uint32_t, GAFTextDataConstPtr>;

using CaptureObjects_t = std::unordered_map<uint32_t, int>;      // Object id -> capture flags

using AnimationSequences_t = std::unordered_map<std::string, GAFAnimationSequenceConstPtr>;
using NamedParts_t = std::multimap<std::string, uint32_t>;
using ExternalObjects_t = std::unordered_map<uint32_t, ExternalObjectConstPtr>;

using SoundInfos_t = std::unordered_map<uint32_t, GAFSoundInfoConstPtr>;

static const uint32_t IDNONE = UINT_MAX;

NS_GAF_END

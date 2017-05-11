#pragma once

#include "GAFSoundInfo.h"

NS_GAF_BEGIN

class GAFSprite;
class GAFObject;

using GAFSequenceDelegate_t = std::function<void(GAFObject* object, const std::string& sequenceName)>;
using GAFAnimationFinishedPlayDelegate_t = std::function<void(GAFObject* obj)>;
using GAFAnimationStartedNextLoopDelegate_t = std::function<void(GAFObject* obj)>;
using GAFTextureLoadDelegate_t = std::function<std::string(const std::string&)>;
using GAFFramePlayedDelegate_t = std::function<void(GAFObject* obj, uint32_t frame)>;
using GAFObjectControlDelegate_t = std::function<void(GAFObject* object, const GAFSprite* subobject)>;
using GAFSoundDelegate_t = std::function<void(GAFSoundInfoConstPtr sound, int32_t repeat, GAFSoundInfo::SyncEvent syncEvent)>;
using GAFPreloadSoundDelegate_t = std::function<void(const std::string& soundPath)>;

NS_GAF_END

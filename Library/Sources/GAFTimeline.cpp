#include "GAFPrecompiled.h"
#include "GAFTimeline.h"
#include "GAFTextureAtlas.h"
#include "GAFAnimationFrame.h"
#include "GAFAnimationSequence.h"

NS_GAF_BEGIN

ExternalObject::ExternalObject() : ExternalObject(IDNONE, "")
{
}

ExternalObject::ExternalObject(uint32_t objectIdRef, const std::string& name)
    : m_objectIdRef(objectIdRef)
    , m_name(name)
{
}

GAFTimeline::GAFTimeline(GAFTimelineConstPtr parent, uint32_t id, const cocos2d::Rect& aabb, cocos2d::Point& pivot, uint32_t framesCount)
    : m_id(id)
    , m_aabb(aabb)
    , m_pivot(pivot)
    , m_framesCount(framesCount)
    , m_parent(parent)
{
}

GAFTimeline::~GAFTimeline()
{
}

void GAFTimeline::pushTextureAtlas(GAFTextureAtlasConstPtr atlas)
{
    m_textureAtlases.push_back(atlas);
}

void GAFTimeline::pushAnimationMask(uint32_t objectId, uint32_t elementAtlasIdRef, GAFCharacterType charType)
{
    m_animationMasks[objectId] = ::std::make_tuple(elementAtlasIdRef, charType);
}

void GAFTimeline::pushAnimationObject(uint32_t objectId, uint32_t elementAtlasIdRef, GAFCharacterType charType)
{
    m_animationObjects[objectId] = ::std::make_tuple(elementAtlasIdRef, charType);
}

void GAFTimeline::pushAnimationFrame(GAFAnimationFrameConstPtr frame)
{
    m_animationFrames.push_back(frame);
}

void GAFTimeline::pushAnimationSequence(const std::string& nameId, int start, int end)
{
    auto seq = ::std::make_shared<GAFAnimationSequence>();
    seq->name = nameId;
    seq->startFrameNo = start;
    seq->endFrameNo = end;

    m_animationSequences[nameId] = seq;
}

void GAFTimeline::pushNamedPart(uint32_t objectIdRef, const std::string& name)
{
    m_namedParts.insert(std::pair<std::string, uint32_t>(name, objectIdRef));
}

void GAFTimeline::pushTextData(uint32_t objectIdRef, GAFTextDataConstPtr textField)
{
    m_textsData[objectIdRef] = textField;
}

void GAFTimeline::pushExternalObject(ExternalObjectConstPtr externalObj)
{
    m_externalObjects[externalObj->getObjectIdRef()] = externalObj;
}

void GAFTimeline::setSceneFps(unsigned int v)
{
    m_sceneFps = v;
}

void GAFTimeline::setSceneWidth(unsigned int v)
{
    m_sceneWidth = v;
}

void GAFTimeline::setSceneHeight(unsigned int v)
{
    m_sceneHeight = v;
}

void GAFTimeline::setSceneColor(const cocos2d::Color4B& v)
{
    m_sceneColor = v;
}

const AnimationObjects_t& GAFTimeline::getAnimationObjects() const
{
    return m_animationObjects;
}

const AnimationMasks_t& GAFTimeline::getAnimationMasks() const
{
    return m_animationMasks;
}

const AnimationFrames_t& GAFTimeline::getAnimationFrames() const
{
    return m_animationFrames;
}

const AnimationSequences_t& GAFTimeline::getAnimationSequences() const
{
    return m_animationSequences;
}

const NamedParts_t& GAFTimeline::getNamedParts() const
{
	return m_namedParts;
}

TextsData_t const& GAFTimeline::getTextsData() const
{
    return m_textsData;
}

const TextureAtlases_t& GAFTimeline::getTextureAtlases() const
{
    return m_textureAtlases;
}

const ExternalObjects_t& GAFTimeline::getExternalObjects() const
{
    return m_externalObjects;
}

void GAFTimeline::setCustomProperties(const CustomProperties_t& properties)
{
    m_customProperties = properties;
}

const CustomProperties_t& GAFTimeline::getCustomProperties() const
{
    return m_customProperties;
}

GAFAnimationSequenceConstPtr GAFTimeline::getSequence(const std::string& name) const
{
    AnimationSequences_t::const_iterator it = m_animationSequences.find(name);

    if (it != m_animationSequences.cend())
        return it->second;

    return nullptr;
}

GAFAnimationSequenceConstPtr GAFTimeline::getSequenceByLastFrame(size_t frame) const
{
    if (m_animationSequences.empty())
        return nullptr;

    for (AnimationSequences_t::const_iterator i = m_animationSequences.cbegin(), e = m_animationSequences.cend(); i != e; ++i)
    {
        if (i->second->endFrameNo == frame + 1)
            return i->second;
    }

    return nullptr;
}

GAFAnimationSequenceConstPtr GAFTimeline::getSequenceByFirstFrame(size_t frame) const
{
    if (m_animationSequences.empty())
    {
        return nullptr;
    }

    for (AnimationSequences_t::const_iterator i = m_animationSequences.begin(), e = m_animationSequences.end(); i != e; ++i)
    {
        if (i->second->startFrameNo == frame)
        {
            return i->second;
        }
    }

    return nullptr;
}

GAFTextureAtlasConstPtr GAFTimeline::getTextureAtlas() const
{
    return m_currentTextureAtlas;
}

void GAFTimeline::setLinkageName(const std::string &linkageName)
{
    m_linkageName = linkageName;
}

void GAFTimeline::setBaseClass(const std::string &baseClass)
{
    m_baseClass = baseClass;
}

void GAFTimeline::setOriginClass(const std::string &originClass)
{
    m_originClass = originClass;
}

uint32_t GAFTimeline::getFramesCount() const
{
    return m_framesCount;
}

const cocos2d::Rect& GAFTimeline::getRect() const
{
    return m_aabb;
}

const cocos2d::Point& GAFTimeline::getPivot() const
{
    return m_pivot;
}

const std::string& GAFTimeline::getLinkageName() const
{
    return m_linkageName;
}

const std::string& GAFTimeline::getBaseClass() const
{
    return m_baseClass;
}

const std::string& GAFTimeline::getOriginClass() const
{
    return m_originClass;
}

GAFTimelineConstPtr GAFTimeline::getParent() const
{
    return m_parent.lock();
}

void GAFTimeline::loadImages(float desiredAtlasScale)
{
    if (m_textureAtlases.empty())
    {
        m_currentTextureAtlas = nullptr;
        m_usedAtlasContentScaleFactor = desiredAtlasScale;
        return;
    }
    _chooseTextureAtlas(desiredAtlasScale);
}

void GAFTimeline::_chooseTextureAtlas(float desiredAtlasScale)
{
    float atlasScale = m_textureAtlases[0]->getScale();

    m_currentTextureAtlas = m_textureAtlases[0];

    const size_t count = m_textureAtlases.size();
    
    for (size_t i = 1; i < count; ++i)
    {
        float as = m_textureAtlases[i]->getScale();
        if (fabs(atlasScale - desiredAtlasScale) > fabs(as - desiredAtlasScale))
        {
            m_currentTextureAtlas = m_textureAtlases[i];
            atlasScale = as;
        }
    }

    m_usedAtlasContentScaleFactor = atlasScale;
}

float GAFTimeline::usedAtlasScale() const
{
    return m_usedAtlasContentScaleFactor;
}

NS_GAF_END
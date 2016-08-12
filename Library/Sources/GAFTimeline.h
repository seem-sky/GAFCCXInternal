#pragma once

#include "GAFCollections.h"
#include "GAFDelegates.h"

NS_GAF_BEGIN

class GAFTextureAtlas;

class ExternalObject
{
private:
    uint32_t           m_objectIdRef;
    std::string        m_name;
    CustomProperties_t m_customProperties;

public:
    ExternalObject();
    ExternalObject(uint32_t objectIdRef, const std::string& name);

    uint32_t getObjectIdRef() const { return m_objectIdRef; }
    void setObjectIdRef(uint32_t objectIdRef) { m_objectIdRef = objectIdRef; }

    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    const CustomProperties_t& getCustomProperties() const { return m_customProperties; }
    void setCustomProperties(const CustomProperties_t& customProperties) { m_customProperties = customProperties; }
};

class GAFTimeline : public cocos2d::Ref
{
private:
    TextureAtlases_t        m_textureAtlases;
    AnimationMasks_t        m_animationMasks;
    AnimationObjects_t      m_animationObjects;
    AnimationFrames_t       m_animationFrames;
    AnimationSequences_t    m_animationSequences;
    NamedParts_t            m_namedParts;
    TextsData_t             m_textsData;
    ExternalObjects_t       m_externalObjects;

    uint32_t                m_id;
    cocos2d::Rect           m_aabb;
    cocos2d::Point          m_pivot;

    unsigned int            m_sceneFps;
    unsigned int            m_sceneWidth;
    unsigned int            m_sceneHeight;
    cocos2d::Color4B        m_sceneColor;

    uint32_t                m_framesCount;

    std::string             m_linkageName;
    std::string             m_baseClass;
    std::string             m_originClass;

    GAFTextureAtlas*        m_currentTextureAtlas;
    GAFTextureLoadDelegate_t m_textureLoadDelegate;

    float                   m_usedAtlasContentScaleFactor;

    GAFTimeline*            m_parent; // weak

    void                    _chooseTextureAtlas(float desiredAtlasScale);
public:
    GAFTimeline(GAFTimeline* parent, uint32_t id, const cocos2d::Rect& aabb, cocos2d::Point& pivot, uint32_t framesCount);
    virtual ~GAFTimeline();

    void                        pushTextureAtlas(GAFTextureAtlas* atlas);
    void                        pushAnimationMask(uint32_t objectId, uint32_t elementAtlasIdRef, GAFCharacterType charType);
    void                        pushAnimationObject(uint32_t objectId, uint32_t elementAtlasIdRef, GAFCharacterType charType);
    void                        pushAnimationFrame(GAFAnimationFrame* frame);
    void                        pushAnimationSequence(const std::string& nameId, int start, int end);
    void                        pushNamedPart(uint32_t objectIdRef, const std::string& name);
    void                        pushTextData(uint32_t objectIdRef, GAFTextData* textField);
    void                        pushExternalObject(ExternalObject* externalObj);

    void                        setSceneFps(unsigned int);
    void                        setSceneWidth(unsigned int);
    void                        setSceneHeight(unsigned int);
    void                        setSceneColor(const cocos2d::Color4B&);

    void                        setLinkageName(const std::string& linkageName);
    void                        setBaseClass(const std::string& baseClass);
    void                        setOriginClass(const std::string& originClass);

    const AnimationObjects_t&   getAnimationObjects() const;
    const AnimationMasks_t&     getAnimationMasks() const;
    const AnimationFrames_t&	getAnimationFrames() const;
    const AnimationSequences_t& getAnimationSequences() const;
    const NamedParts_t&         getNamedParts() const;
    const TextsData_t&          getTextsData() const;
    const TextureAtlases_t&     getTextureAtlases() const;
    const ExternalObjects_t&    getExternalObjects() const;
    uint32_t                    getFramesCount() const;

    const cocos2d::Rect&        getRect() const;
    const cocos2d::Point&       getPivot() const;

    const std::string&          getLinkageName() const;
    const std::string&          getBaseClass() const;
    const std::string&          getOriginClass() const;

    /// get GAFAnimationSequence by name specified in editor
    const GAFAnimationSequence* getSequence(const std::string& name) const;
    /// get GAFAnimationSequence by last frame number in sequence	
    const GAFAnimationSequence* getSequenceByLastFrame(size_t frame) const;
    /// get GAFAnimationSequence by first frame number in sequence	
    const GAFAnimationSequence* getSequenceByFirstFrame(size_t frame) const;

    GAFTimeline*                getParent() const;

    GAFTextureAtlas*            getTextureAtlas();
    void                        loadImages(float desiredAtlasScale);

    float                       usedAtlasScale() const;

    // Custom flash properties;
public:
    void setCustomProperties(const CustomProperties_t& properties);
    const CustomProperties_t& getCustomProperties() const;
    
private:
    CustomProperties_t m_customProperties;
    

    // Custom fields functionality
public:
    void appendUserData(const std::string& K, GAFAnyInterface* V) { m_userData[K] = V; }

    template<class T> T getUserData(const std::string& K) 
    {
        CustomData_t::const_iterator it = m_userData.find(K);
        if (it == m_userData.end()) return T();
        
        return reinterpret_cast<GAFAny<T>*>(it->second)->data;
    }

private:    
    typedef std::unordered_map<std::string, GAFAnyInterface*> CustomData_t;
    CustomData_t m_userData;
};

NS_GAF_END
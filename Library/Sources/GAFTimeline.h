#pragma once

#include "GAFCollections.h"
#include "GAFDelegates.h"

NS_GAF_BEGIN

class ExternalObject
{
private:
    uint32_t           m_objectIdRef;
    std::string        m_name;

public:
    ExternalObject();
    ExternalObject(uint32_t objectIdRef, const std::string& name);

    uint32_t getObjectIdRef() const { return m_objectIdRef; }
    void setObjectIdRef(uint32_t objectIdRef) { m_objectIdRef = objectIdRef; }

    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
};

gaf_fwd_this(GAFStencil);

class GAFStencil
{
public:
    using bitset_t = std::vector<bool>;

private:
    float m_scale = 1.f;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    bitset_t m_bitset;

public:
    float getScale() const { return m_scale; }
    void setScale(float scale) { m_scale = scale; }

    uint32_t getWidth() const { return m_width; }
    void setWidth(uint32_t width) { m_width = width; }

    uint32_t getHeight() const { return m_height; }
    void setHeight(uint32_t height) { m_height = height; }

    const bitset_t& getBitset() const { return m_bitset; }

    template<typename T>
    void setBits(size_t bitIdx, T bits);
};

template<typename T>
void GAFStencil::setBits(size_t bitIdx, T bits)
{
    const size_t bitsNum = sizeof(T) * CHAR_BIT;
    const size_t requestedSize = bitIdx + bitsNum;
    if (requestedSize > m_bitset.size())
        m_bitset.resize(requestedSize);

    size_t offset = bitsNum - 1;
    for (size_t i = bitIdx; i < requestedSize; ++i, --offset)
        m_bitset[i] = bits >> offset & 0x1;
}

gaf_fwd_this(GAFTimeline);
gaf_fwd_this(GAFTextureAtlas);
gaf_fwd_this(GAFAnimationSequence);

class GAFTimeline
{
    friend class GAFAsset;
    friend class GAFLoader;

private:
    TextureAtlases_t        m_textureAtlases;
    AnimationMasks_t        m_animationMasks;
    AnimationObjects_t      m_animationObjects;
    AnimationFrames_t       m_animationFrames;
    AnimationSequences_t    m_animationSequences;
    NamedParts_t            m_namedParts;
    TextsData_t             m_textsData;
    ExternalObjects_t       m_externalObjects;
    GAFStencilConstPtr      m_stencil;

private:
    uint32_t                m_id;
    cocos2d::Rect           m_aabb;
    cocos2d::Point          m_pivot;

    uint32_t                m_sceneFps = 0;
    uint32_t                m_sceneWidth = 0;
    uint32_t                m_sceneHeight = 0;
    cocos2d::Color4B        m_sceneColor;

    uint32_t                m_framesCount;

    std::string             m_linkageName;
    std::string             m_baseClass;
    std::string             m_originClass;

    GAFTextureAtlasConstPtr     m_currentTextureAtlas;
    GAFTextureLoadDelegate_t    m_textureLoadDelegate;

    float                   m_usedAtlasContentScaleFactor = 1.f;

    GAFTimelineConstWPtr    m_parent; // weak

    void                    _chooseTextureAtlas(float desiredAtlasScale);
public:
    GAFTimeline(GAFTimelineConstPtr parent, uint32_t id, const cocos2d::Rect& aabb, cocos2d::Point& pivot, uint32_t framesCount);
    virtual ~GAFTimeline();

    uint32_t                    id() const;
    void                        pushTextureAtlas(GAFTextureAtlasConstPtr atlas);
    void                        pushAnimationMask(uint32_t objectId, uint32_t elementAtlasIdRef, GAFCharacterType charType);
    void                        pushAnimationObject(uint32_t objectId, uint32_t elementAtlasIdRef, GAFCharacterType charType);
    void                        pushAnimationFrame(GAFAnimationFrameConstPtr frame);
    void                        pushAnimationSequence(const std::string& nameId, int start, int end);
    void                        pushNamedPart(uint32_t objectIdRef, const std::string& name);
    void                        pushTextData(uint32_t objectIdRef, GAFTextDataConstPtr textField);
    void                        pushExternalObject(ExternalObjectConstPtr externalObj);

    void                        setSceneFps(unsigned int);
    void                        setSceneWidth(unsigned int);
    void                        setSceneHeight(unsigned int);
    void                        setSceneColor(const cocos2d::Color4B&);

    void                        setLinkageName(const std::string& linkageName);
    void                        setBaseClass(const std::string& baseClass);
    void                        setOriginClass(const std::string& originClass);

    GAFStencilConstPtr          getStencil() const;
    void                        setStencil(const GAFStencilConstPtr gafStencil);

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
    GAFAnimationSequenceConstPtr getSequence(const std::string& name) const;
    /// get GAFAnimationSequence by last frame number in sequence	
    GAFAnimationSequenceConstPtr getSequenceByLastFrame(size_t frame) const;
    /// get GAFAnimationSequence by first frame number in sequence	
    GAFAnimationSequenceConstPtr getSequenceByFirstFrame(size_t frame) const;

    GAFTimelineConstPtr         getParent() const;

    GAFTextureAtlasConstPtr     getTextureAtlas() const;
    void                        loadImages(float desiredAtlasScale);

    float                       usedAtlasScale() const;

    // Custom fields functionality
public:
    void appendUserData(const std::string& K, GAFAnyInterfacePtr V) { m_userData[K] = V; }

    template<class T> T getUserData(const std::string& K) 
    {
        CustomData_t::const_iterator it = m_userData.find(K);
        if (it == m_userData.end()) return T();
        
        return std::static_pointer_cast<GAFAny<T>>(it->second)->data;
    }

private:
    using CustomData_t = std::unordered_map<std::string, GAFAnyInterfacePtr>;
    CustomData_t m_userData;
};

NS_GAF_END
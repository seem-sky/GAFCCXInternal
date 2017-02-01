#pragma once

#include "GAFCollections.h"
#include "GAFResourcesInfo.h"
#include "GAFHeader.h"
#include "GAFTimeline.h"

#include "GAFDelegates.h"

NS_GAF_BEGIN

class GAFTextureAtlas;
class GAFObject;
class GAFAssetTextureManager;
class GAFTimelineAction;

gaf_fwd_this(GAFLoader);
gaf_fwd_this(GAFAsset);
gaf_fwd_this(GAFTimeline);
gaf_fwd_this(GAFTimelineAction);
gaf_fwd_this(GAFAssetTextureManager);

class GAFAsset : public std::enable_shared_from_this<GAFAsset>
{
    friend class GAFObject;
private:
    GAFHeader               m_header;
    Timelines_t             m_timelines;
    GAFTimelineConstPtr     m_rootTimeline;
    SoundInfos_t            m_soundInfos;
    TextureAtlases_t        m_textureAtlases; // custom regions
    GAFTextureAtlasConstPtr m_currentTextureAtlas;
    GAFAssetConstPtr        m_libraryAsset;

    
    GAFTextureLoadDelegate_t    m_textureLoadDelegate = nullptr;
    GAFAssetTextureManagerPtr   m_textureManager;

    GAFSoundDelegate_t      m_soundDelegate = nullptr;

    uint32_t                m_sceneFps = 60;
    uint32_t                m_sceneWidth = 0;
    uint32_t                m_sceneHeight = 0;
    cocos2d::Color4B        m_sceneColor;

    float                   m_desiredAtlasScale = 1.f;

    std::string             m_gafFileName;

    enum class State : uint8_t
    {
        Normal = 0,
        DryRun
    };

    State                   m_state = State::Normal; // avoid to pass this parameter to public methods to prevent usage

    int                     _majorVersion = 0;
    int                     _minorVersion = 0;

private:
    GAFAsset();

    void setRootTimeline(GAFTimelineConstPtr tl);

    void parseReferences(std::vector<GAFResourcesInfoConstPtr>&dest);
    void loadTextures(const std::string& filePath, GAFTextureLoadDelegate_t delegate, cocos2d::ZipFile* bundle = nullptr);
    void _chooseTextureAtlas(float desiredAtlasScale);

public:
    /// Initializes asset with bGAF data

    bool                        initWithGAFFile(const std::string& filePath, GAFTextureLoadDelegate_t delegate, GAFLoaderPtr customLoader = nullptr);

    bool                        initWithGAFBundle(const std::string& zipfilePath, const std::string& entryFile, GAFTextureLoadDelegate_t delegate, GAFLoaderPtr customLoader = nullptr);

    void                        pushTimeline(uint32_t timelineIdRef, GAFTimelinePtr t);
    void                        pushSound(uint32_t id, GAFSoundInfoConstPtr sound);
    void                        soundEvent(const GAFTimelineAction& action) const;

    void                        pushTextureAtlas(GAFTextureAtlasPtr atlas);

    void                        setHeader(GAFHeader& h);
    const GAFHeader&            getHeader() const;
    
    bool                        setRootTimeline(const std::string& name);
    bool                        setRootTimeline(uint32_t id);
    GAFTimelineConstPtr         getRootTimeline() const;
    GAFTimelineConstPtr         getTimelineByName(const std::string& name) const;

    const Timelines_t&          getTimelines() const;

    static GAFAssetPtr          createWithBundle(const std::string& zipfilePath, const std::string& entryFile, GAFTextureLoadDelegate_t delegate, GAFLoaderPtr customLoader = nullptr);
    static GAFAssetPtr          createWithBundle(const std::string& zipfilePath, const std::string& entryFile);
    static GAFAssetPtr          create(const std::string& gafFilePath, GAFTextureLoadDelegate_t delegate, GAFLoaderPtr customLoader = nullptr);
    static GAFAssetPtr          create(const std::string& gafFilePath);

    static void                 getResourceReferences(const std::string& gafFilePath, std::vector<GAFResourcesInfoConstPtr> &dest);
    static void                 getResourceReferencesFromBundle(const std::string& zipfilePath, const std::string& entryFile, std::vector<GAFResourcesInfoConstPtr> &dest);
    
    //void                        useExternalTextureAtlas(std::vector<cocos2d::Texture2D*>& textures, GAFTextureAtlas::Elements_t& elements);
    void                        linkLibraryAsset(GAFAssetConstPtr library);
    GAFAssetConstPtr            getLibraryAsset() const;

    GAFTextureAtlasConstPtr     getTextureAtlas() const;
    void                        loadImages(float desiredAtlasScale);
    GAFSprite*                  getCustomRegion(const std::string& name);

    /// List of GAFAnimationFrame objects	
    static bool                 isAssetVersionPlayable(const char* version);

    GAFObject*                  createObject();
    GAFObject*                  createObjectAndRun(bool looped);

    /// Desired atlas scale. Default is 1.0f
    float                       desiredAtlasScale();
    /// Sets desired atlas scale. Will choose nearest atlas scale from available
    void                        setDesiredAtlasScale(float scale);

    void                        setTextureLoadDelegate(GAFTextureLoadDelegate_t delegate);
    void                        setSoundDelegate(GAFSoundDelegate_t delagate);

    GAFAssetTextureManagerPtr  getTextureManager() const;

    const unsigned int getSceneFps() const;
    const unsigned int getSceneWidth() const;
    const unsigned int getSceneHeight() const;
    const cocos2d::Color4B& getSceneColor() const;
    void setSceneFps(unsigned int);
    void setSceneWidth(unsigned int);
    void setSceneHeight(unsigned int);
    void setSceneColor(const cocos2d::Color4B&);

    const std::string&          getGAFFileName() const;
};

NS_GAF_END

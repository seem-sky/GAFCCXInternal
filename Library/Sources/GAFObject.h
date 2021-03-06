#pragma once

#include "GAFDelegates.h"
#include "GAFSprite.h"
#include "GAFCollections.h"
#include "GAFSubobjectState.h"

NS_GAF_BEGIN

gaf_fwd_this(GAFAsset);
gaf_fwd_this(GAFTimeline);
gaf_fwd_this(GAFFilterData);

namespace cp
{
    gaf_fwd_this(GAFCustomProperties);
}


struct GAFObjectClass
{
    enum Enum
    {
        UNKNOWN = -1,
        SOUND,
        BITMAP_DATA,
        SPRITE,
        MOVIE_CLIP,
        UI_COMPONENT,
        UI_VISUAL_COMPONENT,
        UI_LAYOUT,
        UI_CANVAS,
        UI_BOX_LAYOUT,
        UI_BUTTON,
        UI_SCROLL_VIEW,
        UI_TEXT_AREA,
        UI_LABEL,
        UI_PROGRESS_BAR
    };

    static std::string toString(Enum e)
    {
        switch (e)
        {
        case GAFObjectClass::SOUND:
            return "flash.media.Sound";

        case GAFObjectClass::BITMAP_DATA:
            return "flash.display.BitmapData";

        case GAFObjectClass::SPRITE:
            return "flash.display.Sprite";

        case GAFObjectClass::MOVIE_CLIP:
            return "flash.display.MovieClip";

        case GAFObjectClass::UI_COMPONENT:
            return "sv.GAFUIComponent";

        case GAFObjectClass::UI_VISUAL_COMPONENT:
            return "sv.GAFUIVisualComponent";

        case GAFObjectClass::UI_LAYOUT:
            return "sv.GAFUILayout";

        case GAFObjectClass::UI_CANVAS:
            return "sv.GAFUICanvas";

        case GAFObjectClass::UI_BOX_LAYOUT:
            return "sv.GAFUIBoxLayout";

        case GAFObjectClass::UI_BUTTON:
            return "sv.GAFUIButton";

        case GAFObjectClass::UI_SCROLL_VIEW:
            return "sv.GAFUIScrollView";

        case GAFObjectClass::UI_TEXT_AREA:
            return "sv.GAFUITextArea";

        case GAFObjectClass::UI_LABEL:
            return "sv.GAFUILabel";

        case GAFObjectClass::UI_PROGRESS_BAR:
            return "sv.GAFUIProgressBar";

        case GAFObjectClass::UNKNOWN:
        default:
            return "unknown";
        }
    }
};

class GAFObject : public GAFSprite
{
public:

    using DisplayList_t = std::vector<GAFObject*>;
    using MaskList_t = std::vector<cocos2d::ClippingNode*>;

    using GAFObjectUpdateCallback = std::function<void(float dt)>;
    using GAFVisibilityCallback_t = std::function<void(bool)>;

private:
    GAFSequenceDelegate_t                   m_sequenceDelegate;
    GAFAnimationFinishedPlayDelegate_t      m_animationFinishedPlayDelegate;
    GAFAnimationStartedNextLoopDelegate_t   m_animationStartedNextLoopDelegate;
    GAFFramePlayedDelegate_t                m_framePlayedDelegate;

    uint32_t                                m_totalFrameCount;
    uint32_t                                m_currentSequenceStart;
    uint32_t                                m_currentSequenceEnd;

    bool                                    m_isRunning;
    bool                                    m_isLooped;
    bool                                    m_isReversed;

    double                                  m_timeDelta;
    uint32_t                                m_fps;
    bool                                    m_skipFpsCheck;

    bool                                    m_animationsSelectorScheduled;

    bool                                    m_isInResetState;
    bool                                    m_visibility;
    GAFVisibilityCallback_t                 m_visibilityCallback;

    std::string                           m_objectName;
    int32_t m_previousFrameIndex;

private:
    void constructObject();
    GAFObject* _instantiateObject(uint32_t id, GAFCharacterType type, uint32_t reference, bool isMask);
    
    /// schedule/unschedule
    /// @note this function is automatically called in start/stop
    void enableTick(bool val);

protected:
    cocos2d::Node*                          m_container;
    GAFObject*                              m_timelineParentObject;
    GAFAssetConstPtr                        m_asset;
    GAFTimelineConstPtr                     m_timeline;
    DisplayList_t                           m_displayList;
    MaskList_t                              m_masks;
    GAFCharacterType                        m_charType;
    GAFObjectType                           m_objectType;
    uint32_t                                m_currentFrame;
    uint32_t                                m_showingFrame; // Frame number that is valid from the beginning of realize frame
    uint32_t                                m_lastVisibleInFrame; // Last frame that object was visible in
    Filters_t                               m_parentFilters;
    cocos2d::Vec4                           m_parentColorTransforms[2];

    GAFFilterDataConstPtr                   m_customFilter;

    bool                                    m_isManualColor;
    bool                                    m_isManualPosition;
    bool                                    m_isManualScale;

    GAFObjectUpdateCallback                 m_updateEventListener;

    cocos2d::AffineTransform AffineTransformFlashToCocos(const cocos2d::AffineTransform& aTransform) const;

    void  setTimelineParentObject(GAFObject* obj) { m_timelineParentObject = obj; }
    
    void  processAnimations(float dt);

    void  instantiateObject(const AnimationObjects_t& objs, const AnimationMasks_t& masks);

    virtual void realizeFrame(cocos2d::Node* out, uint32_t frameIndex);
    virtual void processStates(cocos2d::Node* out, uint32_t frameIndex, GAFAnimationFrameConstPtr frame);
    virtual void rearrangeSubobject(cocos2d::Node* out, cocos2d::Node* child, int zIndex);
    virtual void preProcessGAFObject(cocos2d::Node* out, GAFObject* child, GAFSubobjectStateConstPtr state, cocos2d::AffineTransform& mtx);
    virtual void processGAFObject(cocos2d::Node* out, GAFObject* child, GAFSubobjectStateConstPtr state, cocos2d::AffineTransform& mtx);
    virtual void processGAFTimeline(cocos2d::Node* out, GAFObject* child, GAFSubobjectStateConstPtr state, cocos2d::AffineTransform& mtx);
    virtual void processGAFImage(cocos2d::Node* out, GAFObject* child, GAFSubobjectStateConstPtr state, cocos2d::AffineTransform& mtx);
    virtual void processGAFTextField(cocos2d::Node* out, GAFObject* child, GAFSubobjectStateConstPtr state, cocos2d::AffineTransform& mtx);
    virtual void postProcessGAFObject(cocos2d::Node* out, GAFObject* child, GAFSubobjectStateConstPtr state, cocos2d::AffineTransform& mtx);

    virtual cocos2d::AffineTransform& processGAFTimelineStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx, cp::GAFCustomPropertiesConstPtr customProperties);
    virtual cocos2d::AffineTransform& processGAFImageStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx);
    virtual cocos2d::AffineTransform& processGAFTextFieldStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx);

    virtual cocos2d::AffineTransform& changeTransformAccordingToCustomProperties(GAFObject* child, cocos2d::AffineTransform& mtx, cp::GAFCustomPropertiesConstPtr customProperties) const;
    virtual cocos2d::AffineTransform& addAdditionalTransformations(cocos2d::AffineTransform& mtx) const;

    // returns true if custom properties were changed
    virtual bool processOwnCustomProperties(cp::GAFCustomPropertiesConstPtr customProperties);
    virtual bool allNecessaryFieldsExist(cp::GAFCustomPropertiesConstPtr customProperties) const;

    virtual GAFObject* encloseNewTimeline(uint32_t reference);

    void        step();
    static void step(GAFObject* obj) { obj->step(); } // call step from derived class through a pointer
    bool        isCurrentFrameLastInSequence() const;
    uint32_t    nextFrame();
    
    void changeVisibility(bool value);

public:
    void addUpdateListener(const GAFObjectUpdateCallback& callback);

    /// @note do not forget to call setSequenceDelegate(nullptr) before deleting your subscriber
    void setSequenceDelegate(GAFSequenceDelegate_t delegate);

    /// @note do not forget to call setAnimationFinishedPlayDelegate(nullptr) before deleting your subscriber
    void setAnimationFinishedPlayDelegate(GAFAnimationFinishedPlayDelegate_t delegate);

    /// @note do not forget to call setAnimationStartedNextLoopDelegate(nullptr) before deleting your subscriber
    void setAnimationStartedNextLoopDelegate(GAFAnimationStartedNextLoopDelegate_t delegate);

    /// @note do not forget to call setFramePlayedDelegate(nullptr) before deleting your subscriber
    void setFramePlayedDelegate(GAFFramePlayedDelegate_t delegate);

#if COCOS2D_VERSION < 0x00030200
    void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, bool flags) override;
#else
    void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
#endif

#if COCOS2D_VERSION < 0x00030200
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, bool flags) override
#else
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override
#endif
    {
        (void)flags;
        (void)renderer;
        (void)transform;
    }
    
    virtual void setLocator(bool locator) override
    {
        m_container->setVisible(!locator);
    };
    
    virtual void update(float delta) override;
    
    void setVisibilityCallback(const GAFVisibilityCallback_t& callback);

public:
    void        processAnimation();
    // Playback accessing
    void        start();
    void        stop();

    /// Pauses animation including enclosed timelines
    void        pauseAnimation();

    /// Resumes animation including enclosed timelines
    void        resumeAnimation();


    bool        isDone() const;
    bool        getIsAnimationRunning() const;

    bool        isLooped() const;
    void        setLooped(bool looped, bool recursive = false);

    bool        isReversed() const;
    void        setReversed(bool reversed, bool fromCurrentFrame = true);

    uint32_t    getTotalFrameCount() const;
    uint32_t    getCurrentFrameIndex() const;

    bool        setFrame(uint32_t index);

    /// Plays specified frame and then stops excluding enclosed timelines
    bool        gotoAndStop(const std::string& frameLabel);
    /// Plays specified frame and then stops excluding enclosed timelines
    bool        gotoAndStop(uint32_t frameNumber);

    /// Plays animation from specified frame excluding enclosed timelines
    bool        gotoAndPlay(const std::string& frameLabel);
    /// Plays animation from specified frame excluding enclosed timelines
    bool        gotoAndPlay(uint32_t frameNumber);

    uint32_t    getStartFrame(const std::string& frameLabel);
    uint32_t    getEndFrame(const std::string& frameLabel);

    /// Plays animation sequence with specified name
    /// @param name a sequence name
    /// @param looped if true - sequence should play in cycle
    /// @param resume if true - animation will be played immediately, if false - playback will be paused after the first frame is shown
    /// @param hint specific animation playback parameters

    bool        playSequence(const std::string& name, bool looped, bool resume = true);

    /// Stops playing an animation as a sequence
    void        clearSequence();

    void        setAnimationRunning(bool value, bool recurcive);

public:
    GAFObject();
    virtual ~GAFObject();

    static GAFObject* create(GAFAssetConstPtr anAsset, GAFTimelineConstPtr timeline);

    virtual bool init(GAFAssetConstPtr anAnimationData, GAFTimelineConstPtr timeline);

    bool hasSequences() const;

    bool isVisibleInCurrentFrame();

    const AnimationSequences_t& getSequences() const;
    GAFTimelineConstPtr getTimeLine() const { return m_timeline; }
    DisplayList_t& getDisplayList() { return m_displayList; }
    const DisplayList_t& getDisplayList() const { return m_displayList; }
    GAFAssetConstPtr getAsset() const { return m_asset; }
    GAFCharacterType getCharType() const { return m_charType; }
    void setLastVisibleInFrame(uint32_t frame) { m_lastVisibleInFrame = frame; }
    uint32_t getLastVisibleInFrame() const { return m_lastVisibleInFrame; }

    virtual const cocos2d::Mat4& getNodeToParentTransform() const override;
    virtual cocos2d::AffineTransform getNodeToParentAffineTransform() const override;

    virtual cocos2d::Rect getBoundingBox() const override;
    virtual cocos2d::Rect getInternalBoundingBox() const;

    virtual cocos2d::Rect getFlashBoundingBox() const override;
    virtual cocos2d::Rect getFlashInternalBoundingBox() const;

    virtual cocos2d::Rect getFlashInitialInternalBoundingBox() const;

    virtual cocos2d::Rect getInternalBoundingBoxForCurrentFrame() const;
    virtual cocos2d::Rect getBoundingBoxForCurrentFrame() const;

    virtual void setColor(const cocos2d::Color3B& color) override;
    virtual void setOpacity(GLubyte opacity) override;

    virtual void setPosition(const cocos2d::Vec2& position) override;
    virtual void setPosition(float x, float y) override;

    virtual void setScaleZ(float scaleZ) override;
    virtual void setScaleX(float scaleX) override;
    virtual void setScaleY(float scaleY) override;
    virtual void setScale(float scaleX, float scaleY) override;
    virtual void setScale(float scale) override;

    template <typename FilterSubtype>
    void setCustomFilter(const FilterSubtype* filter)
    {
        if (filter)
            m_customFilter = std::make_shared<const FilterSubtype>(*filter);
    }

    //////////////////////////////////////////////////////////////////////////
    // Accessors

    // Searches for an object by given string
    // @param object name e.g. "head" or object path e.g. "knight.body.arm"
    // @note it can slow down the real-time performance
    // @returns instance of GAFObject or null. Warning: the instance could be invalidated when the system catches EVENT_COME_TO_FOREGROUND event
    GAFObject* getObjectByName(const std::string& name);
    const GAFObject* getObjectByName(const std::string& name) const;

    cocos2d::Vector<GAFObject*> getObjectsByName(const std::string& name);

    GAFObject* getObjectByNameForCurrentFrame(const std::string& name);
    const GAFObject* getObjectByNameForCurrentFrame(const std::string& name) const;

    const std::string& getObjectName() const { return m_objectName; }

    uint32_t getFps() const;

    void setFps(uint32_t value);

    void setFpsLimitations(bool fpsLimitations);

    bool isManualPosition() const { return m_isManualPosition; }
};

NS_GAF_END

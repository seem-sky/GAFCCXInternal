#include "GAFPrecompiled.h"
#include "GAFObject.h"
#include "GAFAsset.h"
#include "GAFTimeline.h"
#include "GAFTextureAtlas.h"
#include "GAFMovieClip.h"
#include "GAFAnimationFrame.h"
#include "GAFAnimationSequence.h"
#include "GAFSubobjectState.h"

#include <math/TransformUtils.h>
#include "GAFObjectFactory.h"
#include "GAFUtils.h"

#define ENABLE_RUNTIME_FILTERS 0

NS_GAF_BEGIN

static const AnimationSequences_t s_emptySequences = AnimationSequences_t();

cocos2d::AffineTransform GAFObject::AffineTransformFlashToCocos(const cocos2d::AffineTransform& aTransform) const
{
    cocos2d::AffineTransform transform = aTransform;
    transform.b = -transform.b;
    transform.c = -transform.c;
    transform.ty = getAnchorPointInPoints().y - transform.ty;
    return transform;
}

GAFObject::GAFObject()
    : m_timelineParentObject(nullptr)
    , m_container(nullptr)
    , m_totalFrameCount(0)
    , m_currentSequenceStart(0)
    , m_currentSequenceEnd(0)
    , m_isRunning(false)
    , m_isLooped(false)
    , m_isReversed(false)
    , m_timeDelta(0.0)
    , m_fps(0)
    , m_skipFpsCheck(false)
    , m_asset(nullptr)
    , m_timeline(nullptr)
    , m_currentFrame(GAFFirstFrameIndex)
    , m_showingFrame(GAFFirstFrameIndex)
    , m_lastVisibleInFrame(0)
    , m_objectType(GAFObjectType::None)
    , m_animationsSelectorScheduled(false)
    , m_isInResetState(false)
    , m_customFilter(nullptr)
    , m_isManualColor(false)
    , m_isManualPosition(false)
    , m_isManualScale(false)
    , m_objectName("")
    , m_previousFrameIndex(-1)
    , m_visibility(false)
    , m_visibilityCallback(nullptr)
{
#if GAF_ENABLE_SHADER_MANAGER_AUTOMATIC_INITIALIZATION
    GAFShaderManager::Initialize();
#endif

    m_charType = GAFCharacterType::Timeline;
    m_parentColorTransforms[0] = cocos2d::Vec4::ONE;
    m_parentColorTransforms[1] = cocos2d::Vec4::ZERO;
}

void GAFObject::addUpdateListener(const GAFObjectUpdateCallback & callback)
{
    m_updateEventListener = callback;
}

GAFObject::~GAFObject()
{
    enableTick(false);
    GAF_SAFE_RELEASE_ARRAY_WITH_NULL_CHECK(MaskList_t, m_masks);
    GAF_SAFE_RELEASE_ARRAY_WITH_NULL_CHECK(DisplayList_t, m_displayList);
}

GAFObject* GAFObject::create(GAFAssetConstPtr anAsset, GAFTimelineConstPtr timeline)
{
    GAFObject* ret = new GAFObject();

    if (ret && ret->init(anAsset, timeline))
    {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_RELEASE(ret);
    return nullptr;
}

bool GAFObject::init(GAFAssetConstPtr anAnimationData, GAFTimelineConstPtr timeline)
{
    CC_ASSERT(anAnimationData && "anAssetData data should not be nil");
    CC_ASSERT(timeline && "Timeline data should not be nil");

    if (!anAnimationData || !timeline)
        return false;

    if (m_asset != anAnimationData)
        m_asset = anAnimationData;

    if (m_timeline != timeline)
        m_timeline = timeline;

    m_container = cocos2d::Node::create();
    addChild(m_container);
    m_container->setContentSize(getContentSize());

    m_currentSequenceStart = m_currentFrame = GAFFirstFrameIndex;

    m_currentSequenceEnd = m_totalFrameCount = m_timeline->getFramesCount();

    constructObject();

    return true;
}

void GAFObject::constructObject()
{
    cocos2d::Rect size = m_timeline->getRect();

    setContentSize(cocos2d::Size(size.size.width, size.size.height));

    GAF_SAFE_RELEASE_ARRAY_WITH_NULL_CHECK(DisplayList_t, m_displayList);

    m_fps = m_asset->getSceneFps();

    m_animationsSelectorScheduled = false;

    instantiateObject(m_timeline->getAnimationObjects(), m_timeline->getAnimationMasks());
}

GAFObject* GAFObject::_instantiateObject(uint32_t id, GAFCharacterType type, uint32_t reference, bool isMask)
{
    GAFObject* result = nullptr;
    if (type == GAFCharacterType::External)
    {
        assert(m_asset->getLibraryAsset());
        ExternalObjects_t::const_iterator externalTl = m_timeline->getExternalObjects().find(reference);
        assert(externalTl != m_timeline->getExternalObjects().end());
        GAFTimelineConstPtr externalTimeline = m_asset->getLibraryAsset()->getTimelineByName(externalTl->second->getName());
        result = GafObjectFactory::create(m_asset->getLibraryAsset(), externalTimeline, isMask, id);
    }
    else if (type == GAFCharacterType::Timeline)
    {
        const Timelines_t& timelines = m_asset->getTimelines();

        CC_ASSERT(reference != IDNONE && "Invalid object reference.");

        Timelines_t::const_iterator tl = timelines.find(reference);

        CC_ASSERT(tl != timelines.cend() && "Invalid object reference.");

        result = GafObjectFactory::create(m_asset, tl->second, isMask, id);

        //result = encloseNewTimeline(reference);
    }
    else if (type == GAFCharacterType::TextField)
    {
        TextsData_t::const_iterator it = m_timeline->getTextsData().find(reference);
        if (it != m_timeline->getTextsData().end())
        {
            result = GafObjectFactory::create(m_asset, it->second, isMask, id);
        }
    }
    else if (type == GAFCharacterType::Texture)
    {
        auto atlas = m_timeline->getTextureAtlas();
        const GAFTextureAtlas::Elements_t& elementsMap = atlas->getElements();
        GAFTextureAtlas::Elements_t::const_iterator elIt = elementsMap.find(reference); // Search for atlas element by its xref
        GAFTextureAtlasElementConstPtr txElemet;
        if (elIt != elementsMap.end())
        {
            txElemet = elIt->second;

            result = GafObjectFactory::create(m_asset, txElemet, isMask, id);
        }
        else
        {
            CCLOGERROR("Texture atlas element with idx: %d not found.", reference);
        }
    }

    if (result)
    {
        result->retain();
        result->setTimelineParentObject(this);
    }

    return result;
}

void GAFObject::instantiateObject(const AnimationObjects_t& objs, const AnimationMasks_t& masks)
{
    uint32_t maxIdx = 0;
    for (AnimationObjects_t::const_iterator it = objs.begin(), e = objs.end(); it != e; ++it)
    {
        maxIdx = std::max(it->first, maxIdx);
    }
    for (AnimationMasks_t::const_iterator it = masks.begin(), e = masks.end(); it != e; ++it)
    {
        maxIdx = std::max(it->first, maxIdx);
    }
    maxIdx += 1;

    m_displayList.resize(maxIdx);
    m_masks.resize(maxIdx);

    for (AnimationObjects_t::const_iterator i = objs.begin(), e = objs.end(); i != e; ++i)
    {
        GAFCharacterType charType = std::get<1>(i->second);
        uint32_t reference = std::get<0>(i->second);
        uint32_t objectId = i->first;

        CCASSERT(m_displayList[objectId] == nullptr, "Object is already created. Memory will be leaked.");
        m_displayList[objectId] = _instantiateObject(objectId, charType, reference, false);
    }
    for (AnimationMasks_t::const_iterator i = masks.begin(), e = masks.end(); i != e; ++i)
    {
        GAFCharacterType charType = std::get<1>(i->second);
        uint32_t reference = std::get<0>(i->second);
        uint32_t objectId = i->first;

        CCASSERT(m_displayList[objectId] == nullptr, "Object is already created. Memory will be leaked.");
        GAFObject* stencil = _instantiateObject(objectId, charType, reference, true);
        m_displayList[objectId] = stencil;
        cocos2d::ClippingNode* mask = cocos2d::ClippingNode::create(stencil);
        mask->retain();
        mask->setAlphaThreshold(0.1f);
        m_masks[objectId] = mask;
    }

    // Set names
    const NamedParts_t& np = m_timeline->getNamedParts();
    for (NamedParts_t::const_iterator i = np.begin(), e = np.end(); i != e; ++i)
    {
        uint32_t id = i->second;
        if (m_displayList[id] != nullptr)
        {
            m_displayList[id]->m_objectName = i->first;
        }
    }
}

GAFObject* GAFObject::encloseNewTimeline(uint32_t reference)
{
    const Timelines_t& timelines = m_asset->getTimelines();

    CC_ASSERT(reference != IDNONE && "Invalid object reference.");

    Timelines_t::const_iterator tl = timelines.find(reference);

    CC_ASSERT(tl != timelines.cend() && "Invalid object reference.");

    GAFObject* newObject = new GAFObject();
    newObject->init(m_asset, tl->second);
    return newObject;
}

void GAFObject::update(float delta)
{
    GAFSprite::update(delta);

    if (m_updateEventListener)
        m_updateEventListener(delta);
}

void GAFObject::processAnimation()
{
    realizeFrame(m_container, m_currentFrame);
}

void GAFObject::setAnimationRunning(bool value, bool recursive)
{
    m_isRunning = value;

    if (recursive)
    {
        for (auto obj : m_displayList)
        {
            if (obj == nullptr)
                continue;

            obj->setAnimationRunning(value, recursive);
        }
    }
}

bool GAFObject::getIsAnimationRunning() const
{
    return m_isRunning;
}

void GAFObject::setSequenceDelegate(GAFSequenceDelegate_t delegate)
{
    m_sequenceDelegate = delegate;
}

void GAFObject::setAnimationFinishedPlayDelegate(GAFAnimationFinishedPlayDelegate_t delegate)
{
    m_animationFinishedPlayDelegate = delegate;
}

void GAFObject::setAnimationStartedNextLoopDelegate(GAFAnimationStartedNextLoopDelegate_t delegate)
{
    m_animationStartedNextLoopDelegate = delegate;
}

void GAFObject::setFramePlayedDelegate(GAFFramePlayedDelegate_t delegate)
{
    m_framePlayedDelegate = delegate;
}

void GAFObject::start()
{
    enableTick(true);

    if (!m_isRunning)
    {
        m_currentFrame = m_isReversed ? m_totalFrameCount - 1 : GAFFirstFrameIndex;
        setAnimationRunning(true, true);
    }
}

void GAFObject::stop()
{
    enableTick(false);
    if (m_isRunning)
    {
        setFrame(GAFFirstFrameIndex);
        setAnimationRunning(false, true);
    }
}

void GAFObject::processAnimations(float dt)
{
    if (m_skipFpsCheck)
    {
        step();
    }
    else
    {
        m_timeDelta += dt;
        double frameTime = 1.0 / m_fps;
        while (m_timeDelta >= frameTime)
        {
            m_timeDelta -= frameTime;
            step();
        }
    }
}

void GAFObject::pauseAnimation()
{
    if (m_isRunning)
    {
        setAnimationRunning(false, false);

        if (m_animationFinishedPlayDelegate)
        {
            m_animationFinishedPlayDelegate(this);
        }
    }
}

void GAFObject::resumeAnimation()
{
    if (!m_isRunning)
    {
        setAnimationRunning(true, false);
    }
}

bool GAFObject::isDone() const
{
    if (m_isLooped)
    {
        return false;
    }

    return isCurrentFrameLastInSequence();

    /*if (!m_isReversed)
    {
    return m_currentFrame > m_totalFrameCount;
    }
    else
    {
    return m_currentFrame < GAFFirstFrameIndex - 1;
    }*/
}

bool GAFObject::isLooped() const
{
    return m_isLooped;
}

void GAFObject::setLooped(bool looped, bool recursive /*= false*/)
{
    m_isLooped = looped;

    if (recursive)
    {
        for (auto obj : m_displayList)
        {
            if (obj == nullptr)
            {
                continue;
            }
            obj->setLooped(looped, recursive);
        }
    }
}

bool GAFObject::isReversed() const
{
    return m_isReversed;
}

void GAFObject::setReversed(bool reversed, bool fromCurrentFrame /* = true */)
{
    m_isReversed = reversed;
    if (!fromCurrentFrame)
    {
        m_currentFrame = reversed ? m_currentSequenceEnd - 1 : m_currentSequenceStart;
    }

    for (auto obj : m_displayList)
    {
        if (obj == nullptr)
        {
            continue;
        }
        obj->setReversed(reversed, fromCurrentFrame);
    }
}

uint32_t GAFObject::getTotalFrameCount() const
{
    return m_totalFrameCount;
}

uint32_t GAFObject::getCurrentFrameIndex() const
{
    return m_showingFrame;
}

bool GAFObject::setFrame(uint32_t index)
{
    if (index < m_totalFrameCount)
    {
        m_showingFrame = m_currentFrame = index;
        processAnimation();
        return true;
    }
    return false;
}

bool GAFObject::gotoAndStop(const std::string& frameLabel)
{
    uint32_t f = getStartFrame(frameLabel);
    if (IDNONE == f)
    {
        uint32_t frameNumber = atoi(frameLabel.c_str());
        if (frameNumber == 0)
        {
            return false;
        }
        return gotoAndStop(frameNumber - 1);
    }
    return gotoAndStop(f);
}

bool GAFObject::gotoAndStop(uint32_t frameNumber)
{
    if (setFrame(frameNumber))
    {
        m_isRunning = false;

        if (m_animationFinishedPlayDelegate)
        {
            m_animationFinishedPlayDelegate(this);
        }

        return true;
    }
    return false;
}

bool GAFObject::gotoAndPlay(const std::string& frameLabel)
{
    uint32_t f = getStartFrame(frameLabel);
    if (IDNONE == f)
    {
        uint32_t frameNumber = atoi(frameLabel.c_str());
        if (frameNumber == 0)
        {
            return false;
        }
        return gotoAndPlay(frameNumber - 1);
    }
    return gotoAndPlay(f);
}

bool GAFObject::gotoAndPlay(uint32_t frameNumber)
{
    if (setFrame(frameNumber))
    {
        m_isRunning = true;
        return true;
    }
    return false;
}

uint32_t GAFObject::getStartFrame(const std::string& frameLabel)
{
    if (!m_asset)
        return IDNONE;

    auto seq = m_timeline->getSequence(frameLabel);
    if (seq)
        return seq->startFrameNo;

    return IDNONE;

}

uint32_t GAFObject::getEndFrame(const std::string& frameLabel)
{
    if (!m_asset)
        return IDNONE;

    auto seq = m_timeline->getSequence(frameLabel);
    if (seq)
        return seq->endFrameNo;

    return IDNONE;
}

bool GAFObject::playSequence(const std::string& name, bool looped, bool resume /*= true*/)
{
    if (!m_asset || !m_timeline)
        return false;

    if (name.empty())
        return false;

    uint32_t s = getStartFrame(name);
    uint32_t e = getEndFrame(name);

    if (IDNONE == s || IDNONE == e)
    {
        return false;
    }

    m_currentSequenceStart = s;
    m_currentSequenceEnd = e;

    uint32_t frame = m_isReversed ? (e - 1) : (s);
    setFrame(frame);

    setLooped(looped, false);

    if (resume)
        resumeAnimation();
    else
        stop();

    return true;
}

void GAFObject::clearSequence()
{
    m_currentSequenceStart = GAFFirstFrameIndex;
    m_currentSequenceEnd = m_totalFrameCount;
}

void GAFObject::step()
{
    m_showingFrame = m_currentFrame;

    if (!getIsAnimationRunning())
    {
        processAnimation();
        return;
    }

    if (m_sequenceDelegate && m_timeline)
    {
        GAFAnimationSequenceConstPtr seq;
        if (!m_isReversed)
            seq = m_timeline->getSequenceByLastFrame(m_currentFrame);
        else
            seq = m_timeline->getSequenceByFirstFrame(m_currentFrame + 1);

        if (seq)
            m_sequenceDelegate(this, seq->name);
    }

    if (isCurrentFrameLastInSequence())
    {
        if (m_isLooped)
        {
            if (m_animationStartedNextLoopDelegate)
                m_animationStartedNextLoopDelegate(this);
        }
        else
        {
            setAnimationRunning(false, false);

            if (m_animationFinishedPlayDelegate)
                m_animationFinishedPlayDelegate(this);
        }
    }

    processAnimation();

    m_showingFrame = m_currentFrame;
    m_currentFrame = nextFrame();

    if (m_framePlayedDelegate)
    {
        m_framePlayedDelegate(this, m_currentFrame);
    }
}

bool GAFObject::isCurrentFrameLastInSequence() const
{
    if (m_isReversed)
        return m_currentFrame == m_currentSequenceStart;
    return m_currentFrame == m_currentSequenceEnd - 1;
}

uint32_t GAFObject::nextFrame()
{
    if (!getIsAnimationRunning())
    {
        return m_currentFrame;
    }

    if (isCurrentFrameLastInSequence())
    {
        if (!m_isLooped)
            return m_currentFrame;

        if (m_isReversed)
            return m_currentSequenceEnd - 1;
        else
            return m_currentSequenceStart;
    }

    return m_currentFrame + (m_isReversed ? -1 : 1);
}

bool GAFObject::hasSequences() const
{
    return !m_timeline->getAnimationSequences().empty();
}

const AnimationSequences_t& GAFObject::getSequences() const
{
    if (m_timeline)
        return m_timeline->getAnimationSequences();
    return s_emptySequences;
}

static cocos2d::Rect GAFCCRectUnion(const cocos2d::Rect& src1, const cocos2d::Rect& src2)
{
    float thisLeftX = src1.origin.x;
    float thisRightX = src1.origin.x + src1.size.width;
    float thisTopY = src1.origin.y + src1.size.height;
    float thisBottomY = src1.origin.y;

    if (thisRightX < thisLeftX)
    {
        std::swap(thisRightX, thisLeftX); // This rect has negative width
    }

    if (thisTopY < thisBottomY)
    {
        std::swap(thisTopY, thisBottomY); // This rect has negative height
    }

    float otherLeftX = src2.origin.x;
    float otherRightX = src2.origin.x + src2.size.width;
    float otherTopY = src2.origin.y + src2.size.height;
    float otherBottomY = src2.origin.y;

    if (otherRightX < otherLeftX)
        std::swap(otherRightX, otherLeftX); // Other rect has negative width

    if (otherTopY < otherBottomY)
        std::swap(otherTopY, otherBottomY); // Other rect has negative height

    float combinedLeftX = std::min(thisLeftX, otherLeftX);
    float combinedRightX = std::max(thisRightX, otherRightX);
    float combinedTopY = std::max(thisTopY, otherTopY);
    float combinedBottomY = std::min(thisBottomY, otherBottomY);

    return cocos2d::Rect(combinedLeftX, combinedBottomY, combinedRightX - combinedLeftX, combinedTopY - combinedBottomY);
}

cocos2d::Rect GAFObject::getInternalBoundingBoxForCurrentFrame() const
{
    cocos2d::Rect result = cocos2d::Rect::ZERO;

    const AnimationFrames_t& animationFrames = m_timeline->getAnimationFrames();

    if (animationFrames.size() <= m_currentFrame)
        return result;

    auto currentFrame = animationFrames[m_currentFrame];

    const GAFAnimationFrame::SubobjectStates_t& states = currentFrame->getObjectStates();

    bool isFirstObj = true;
    for (auto state : states)
    {
        GAFObject* subObject = m_displayList[state->objectIdRef];

        if (!subObject)
            continue;

        if (state->isVisible())
        {
            cocos2d::Rect bb;
            if (subObject->m_charType == GAFCharacterType::Timeline)
                bb = subObject->getBoundingBoxForCurrentFrame();
            else
                bb = subObject->getBoundingBox();

            if (isFirstObj)
                result = bb;
            else
                result = GAFCCRectUnion(result, bb);
        }

        isFirstObj = false;
    }

    for (Node* subObject : _children)
    {
        if (subObject == m_container || !subObject->isVisible())
            continue;

        cocos2d::Rect bb = subObject->getBoundingBox();
        result = GAFCCRectUnion(result, bb);
    }

    return result;
}

cocos2d::Rect GAFObject::getBoundingBoxForCurrentFrame() const
{
    return cocos2d::RectApplyTransform(getInternalBoundingBoxForCurrentFrame(), getNodeToParentTransform());
}

cocos2d::Mat4 const& GAFObject::getNodeToParentTransform() const
{
    if (_transformDirty)
    {
        if (m_charType == GAFCharacterType::Timeline)
        {
            cocos2d::Mat4 tmp;
            cocos2d::CGAffineToGL(m_externalTransform, tmp.m);
            _transform = cocos2d::Node::getNodeToParentTransform() * tmp;
        }
        else
        {
            GAFSprite::getNodeToParentTransform();
        }
    }
    
    return _transform;
}

cocos2d::AffineTransform GAFObject::getNodeToParentAffineTransform() const
{
    cocos2d::AffineTransform ret;
    cocos2d::GLToCGAffine(getNodeToParentTransform().m, &ret);

    return ret;
}

cocos2d::Rect GAFObject::getBoundingBox() const
{
    cocos2d::Rect rect = getInternalBoundingBox();
    return RectApplyAffineTransform(rect, getNodeToParentAffineTransform());
}

cocos2d::Rect GAFObject::getInternalBoundingBox() const
{
    if (m_charType == GAFCharacterType::Timeline)
        return flashBoundsToCocos(m_timeline->getRect());
    else
        return cocos2d::Rect(0, 0, _contentSize.width, _contentSize.height);
}

cocos2d::Rect GAFObject::getFlashBoundingBox() const
{
    cocos2d::Rect rect = getInternalBoundingBox();
    return cocosBoundsToFlash(RectApplyAffineTransform(rect, getNodeToParentAffineTransform()));
}

cocos2d::Rect GAFObject::getFlashInternalBoundingBox() const
{
    return cocosBoundsToFlash(getInternalBoundingBox());
}

cocos2d::Rect GAFObject::getFlashInitialInternalBoundingBox() const
{
    return cocosBoundsToFlash(GAFObject::getInternalBoundingBox());
}

void GAFObject::setColor(const cocos2d::Color3B& color)
{
   if (m_isManualColor && color == cocos2d::Color3B::WHITE)
   {
       m_isManualColor = false;
   }
   else
   {
       m_isManualColor = true;
   }

   Node::setColor(color);
}

void GAFObject::setOpacity(GLubyte opacity)
{
    m_isManualColor = true;
    Node::setOpacity(opacity);

    if (m_container)
    {
        const auto& children = m_container->getChildren();
        for (const auto &child : children)
            child->setOpacity(opacity);
    }
}

void GAFObject::setPosition(const cocos2d::Vec2& position)
{
    setPosition(position.x, position.y);
}

void GAFObject::setPosition(float x, float y)
{
    m_isManualPosition = true;
    Node::setPosition(x, y);
}

void GAFObject::setScaleZ(float scaleZ)
{
    m_isManualScale = true;
    Node::setScaleZ(scaleZ);
}

void GAFObject::setScaleX(float scaleX)
{
    m_isManualScale = true;
    Node::setScaleX(scaleX);
}

void GAFObject::setScaleY(float scaleY)
{
    m_isManualScale = true;
    Node::setScaleY(scaleY);
}

void GAFObject::setScale(float scaleX, float scaleY)
{
    m_isManualScale = true;
    Node::setScale(scaleX, scaleY);
}

void GAFObject::setScale(float scale)
{
    m_isManualScale = true;
    Node::setScale(scale);
}

void GAFObject::rearrangeSubobject(cocos2d::Node* out, cocos2d::Node* child, int zIndex)
{
    cocos2d::Node* parent = child->getParent();
    //child->setCameraMask(getCameraMask());
    if (parent != out)
    {
        child->removeFromParentAndCleanup(false);
        out->addChild(child, zIndex);
    }
    else
    {
        //if (child->getLocalZOrder() != zIndex)
            static_cast<GAFObject*>(child)->_transformUpdated = true;

        child->setLocalZOrder(zIndex);
    }
}

void GAFObject::preProcessGAFObject(cocos2d::Node* out, GAFObject* child, const GAFSubobjectStateConstPtr state, cocos2d::AffineTransform& mtx)
{
    (void)out;
    (void)mtx;

    if (state->colorMults()[GAFColorTransformIndex::GAFCTI_A] >= 0.f && child->m_isInResetState)
        child->m_currentFrame = child->m_currentSequenceStart;

    child->m_isInResetState = state->colorMults()[GAFColorTransformIndex::GAFCTI_A] < 0.f;
}

void GAFObject::processGAFObject(cocos2d::Node * out, GAFObject * child, GAFSubobjectStateConstPtr state, cocos2d::AffineTransform & mtx)
{
    if (!state->isVisible())
        return;

    if (child->m_charType == GAFCharacterType::Timeline)
    {
        processGAFTimeline(out, child, state, mtx);
    }
    else if (child->m_charType == GAFCharacterType::Texture)
    {
        processGAFImage(out, child, state, mtx);
    }
    else if (child->m_charType == GAFCharacterType::TextField)
    {
        processGAFTextField(out, child, state, mtx);
    }
}

void GAFObject::processGAFTimeline(cocos2d::Node* out, GAFObject* child, const GAFSubobjectStateConstPtr state, cocos2d::AffineTransform& mtx)
{
    if (!child->m_isInResetState)
    {
        child->processOwnCustomProperties(state->getCustomProperties());
        processGAFTimelineStateTransform(child, mtx, state->getCustomProperties());
        child->setExternalTransform(mtx);

        child->m_parentFilters.clear();
        if (m_customFilter)
        {
            child->m_parentFilters.push_back(m_customFilter);
        }

        const Filters_t& filters = state->getFilters();
        child->m_parentFilters.insert(child->m_parentFilters.end(), filters.begin(), filters.end());

        const float* cm = state->colorMults();
        child->m_parentColorTransforms[0] = cocos2d::Vec4(
            m_parentColorTransforms[0].x * cm[0],
            m_parentColorTransforms[0].y * cm[1],
            m_parentColorTransforms[0].z * cm[2],
            m_parentColorTransforms[0].w * cm[3]);
        child->m_parentColorTransforms[1] = cocos2d::Vec4(state->colorOffsets()) + m_parentColorTransforms[1];

        if (m_masks[state->objectIdRef])
        {
            rearrangeSubobject(out, m_masks[state->objectIdRef], state->zIndex);
        }
        else
        {
            //subObject->removeFromParentAndCleanup(false);
            if (state->maskObjectIdRef == IDNONE)
            {
                rearrangeSubobject(out, child, state->zIndex);
            }
            else
            {
                // If the state has a mask, then attach it 
                // to the clipping node. Clipping node will be attached on its state
                auto mask = m_masks[state->maskObjectIdRef];
                CCASSERT(mask, "Error. No mask found for this ID");
                if (mask)
                    rearrangeSubobject(mask, child, state->zIndex);
            }
        }

        child->step();
    }
}

void GAFObject::processGAFImage(cocos2d::Node* out, GAFObject* child, const GAFSubobjectStateConstPtr state, cocos2d::AffineTransform& mtx)
{
    cocos2d::Vec2 prevAP = child->getAnchorPoint();
    cocos2d::Size  prevCS = child->getContentSize();

#if ENABLE_RUNTIME_FILTERS
    if (child->m_objectType == GAFObjectType::MovieClip)
    {
        // Validate sprite type (w/ or w/o filter)
        const Filters_t& filters = state->getFilters();
        GAFFilterData* filter = NULL;

        GAFMovieClip* mc = static_cast<GAFMovieClip*>(child);

        if (m_customFilter)
        {
            filter = m_customFilter;
        }
        else if (m_parentFilters.size() > 0)
        {
            filter = *m_parentFilters.begin();
        }
        else if (filters.size() > 0)
        {
            filter = *filters.begin();
        }

        if (filter)
        {
            filter->apply(mc);
        }

        if (!filter || filter->getType() != GAFFilterType::Blur)
        {
            mc->setBlurFilterData(nullptr);
        }

        if (!filter || filter->getType() != GAFFilterType::ColorMatrix)
        {
            mc->setColorMarixFilterData(nullptr);
        }

        if (!filter || filter->getType() != GAFFilterType::Glow)
        {
            mc->setGlowFilterData(nullptr);
        }

        if (!filter || filter->getType() != GAFFilterType::DropShadow)
        {
            GAFDropShadowFilterData::reset(mc);
        }
    }
#endif

    cocos2d::Size newCS = child->getContentSize();
    cocos2d::Vec2 newAP = cocos2d::Vec2(((prevAP.x - 0.5f) * prevCS.width) / newCS.width + 0.5f,
        ((prevAP.y - 0.5f) * prevCS.height) / newCS.height + 0.5f);
    child->setAnchorPoint(newAP);

    if (m_masks[state->objectIdRef])
    {
        rearrangeSubobject(out, m_masks[state->objectIdRef], state->zIndex);
    }
    else
    {
        //subObject->removeFromParentAndCleanup(false);
        if (state->maskObjectIdRef == IDNONE)
        {
            rearrangeSubobject(out, child, state->zIndex);
        }
        else
        {
            // If the state has a mask, then attach it 
            // to the clipping node. Clipping node will be attached on its state
            auto mask = m_masks[state->maskObjectIdRef];
            CCASSERT(mask, "Error. No mask found for this ID");
            if (mask)
                rearrangeSubobject(mask, child, state->zIndex);
        }
    }

    processGAFImageStateTransform(child, mtx);
    child->setExternalTransform(mtx);

    if (child->m_objectType == GAFObjectType::MovieClip)
    {
        GAFMovieClip* mc = static_cast<GAFMovieClip*>(child);
        cocos2d::Vec4 colorMults(
            state->colorMults()[0] * m_parentColorTransforms[0].x * _displayedColor.r / 255,
            state->colorMults()[1] * m_parentColorTransforms[0].y * _displayedColor.g / 255,
            state->colorMults()[2] * m_parentColorTransforms[0].z * _displayedColor.b / 255,
            state->colorMults()[3] * m_parentColorTransforms[0].w * _displayedOpacity / 255
        );
        cocos2d::Vec4 colorOffsets(
            state->colorOffsets()[0] + m_parentColorTransforms[1].x,
            state->colorOffsets()[1] + m_parentColorTransforms[1].y,
            state->colorOffsets()[2] + m_parentColorTransforms[1].z,
            state->colorOffsets()[3] + m_parentColorTransforms[1].w
        );

        mc->setColorTransform(colorMults, colorOffsets);
    }
}

void GAFObject::processGAFTextField(cocos2d::Node* out, GAFObject* child, const GAFSubobjectStateConstPtr state, cocos2d::AffineTransform& mtx)
{
    //GAFTextField *tf = static_cast<GAFTextField*>(subObject);
    rearrangeSubobject(out, child, state->zIndex);

    processGAFTextFieldStateTransform(child, mtx);
    child->setExternalTransform(mtx);
}

void GAFObject::postProcessGAFObject(cocos2d::Node* out, GAFObject* child, const GAFSubobjectStateConstPtr state, cocos2d::AffineTransform& mtx)
{
    (void)out;
    (void)child;
    (void)state;
    (void)mtx;
}

cocos2d::AffineTransform& GAFObject::processGAFTimelineStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx, cp::GAFCustomPropertiesConstPtr customProperties)
{
    if (child->m_isManualPosition || child->m_isManualScale)
    {
        //affineTransformSetFrom(mtx, cocos2d::AffineTransform::IDENTITY);
        if (child->m_isManualPosition)
        {
            mtx.tx = 0.f; // getTimeLine()->getPivot().x;
            mtx.ty = 0.f; // getTimeLine()->getPivot().y;
        }
        
        if (child->m_isManualScale)
        {
            cocos2d::Vec2 scale = affineTransformGetScale(mtx);
            cocos2d::Vec2 inverseScale(1.0f / scale.x, 1.0f / scale.y);
            affineTransformSetFrom(mtx, cocos2d::AffineTransformScale(mtx, inverseScale.x, inverseScale.y));
        }
    }
    else
    {
        if (allNecessaryFieldsExist(customProperties))
            changeTransformAccordingToCustomProperties(child, mtx, customProperties);
        else
            addAdditionalTransformations(mtx);
    }

    float csf = m_timeline->usedAtlasScale();
    mtx.tx *= csf;
    mtx.ty *= csf;
    affineTransformSetFrom(mtx, AffineTransformFlashToCocos(mtx));

    mtx.tx /= child->getScaleX();
    mtx.ty /= child->getScaleY();

    return mtx;
}

cocos2d::AffineTransform& GAFObject::processGAFImageStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx)
{
    affineTransformSetFrom(mtx, AffineTransformFlashToCocos(mtx));

    float curScaleX = child->getScaleX();
    if (fabs(curScaleX - 1.0) > std::numeric_limits<float>::epsilon())
        mtx.a *= curScaleX;

    float curScaleY = child->getScaleY();
    if (fabs(curScaleY - 1.0) > std::numeric_limits<float>::epsilon())
        mtx.d *= curScaleY;

    return mtx;
}

cocos2d::AffineTransform& GAFObject::processGAFTextFieldStateTransform(GAFObject* child, cocos2d::AffineTransform& mtx)
{
    (void)child;

    if (child->m_isManualPosition)
    {
        mtx.tx = child->getPositionX(); // getTimeLine()->getPivot().x;
        mtx.ty = -child->getPositionY(); // getTimeLine()->getPivot().y;
    }

    if (child->m_isManualScale)
    {
        cocos2d::Vec2 ab(mtx.a, mtx.b);
        ab.normalize();
        ab.scale(_scaleX);

        cocos2d::Vec2 cd(mtx.c, mtx.d);
        cd.normalize();
        cd.scale(_scaleY);

        mtx.a = ab.x;
        mtx.b = ab.y;
        mtx.c = cd.x;
        mtx.d = cd.y;
    }

    affineTransformSetFrom(mtx, AffineTransformFlashToCocos(mtx));

    /*if (isFlippedX() || isFlippedY())
    {
        float flipMulX = isFlippedX() ? -1.f : 1.f;
        float flipOffsetX = isFlippedX() ? getContentSize().width - m_asset->getHeader().frameSize.getMinX() : 0;
        float flipMulY = isFlippedY() ? -1.f : 1.f;
        float flipOffsetY = isFlippedY() ? -getContentSize().height + m_asset->getHeader().frameSize.getMinY() : 0;

        cocos2d::AffineTransform flipCenterTransform = cocos2d::AffineTransformMake(flipMulX, 0, 0, flipMulY, flipOffsetX, flipOffsetY);
        affineTransformSetFrom(mtx, AffineTransformConcat(mtx, flipCenterTransform));
    }*/

    return mtx;
}

cocos2d::AffineTransform& GAFObject::changeTransformAccordingToCustomProperties(GAFObject* child, cocos2d::AffineTransform& mtx, cp::GAFCustomPropertiesConstPtr customProperties) const
{
    (void)child;
    (void)customProperties;

    return mtx;
}

cocos2d::AffineTransform& GAFObject::addAdditionalTransformations(cocos2d::AffineTransform& mtx) const
{
    return mtx;
}

bool GAFObject::processOwnCustomProperties(cp::GAFCustomPropertiesConstPtr customProperties)
{
    (void)customProperties;

    return false;
}

bool GAFObject::allNecessaryFieldsExist(cp::GAFCustomPropertiesConstPtr customProperties) const
{
    (void)customProperties;

    return false;
}

void GAFObject::realizeFrame(cocos2d::Node* out, uint32_t frameIndex)
{
    const AnimationFrames_t& animationFrames = m_timeline->getAnimationFrames();

    if (animationFrames.size() <= frameIndex)
    {
        return;
    }

    auto currentFrame = animationFrames[frameIndex];

    processStates(out, frameIndex, currentFrame);

    GAFAnimationFrame::TimelineActions_t timelineActions = currentFrame->getTimelineActions();
    for (GAFTimelineAction action : timelineActions)
    {
        switch (action.getType())
        {
        case GAFActionType::Stop:
            pauseAnimation();
            break;
        case GAFActionType::Play:
            resumeAnimation();
            break;
        case GAFActionType::GotoAndStop:
            gotoAndStop(action.getParam(GAFTimelineAction::PI_FRAME));
            break;
        case GAFActionType::GotoAndPlay:
            gotoAndPlay(action.getParam(GAFTimelineAction::PI_FRAME));
            break;
        case GAFActionType::DispatchEvent:
        {
            std::string type = action.getParam(GAFTimelineAction::PI_EVENT_TYPE);
            if (type.compare(GAFSoundInfo::SoundEvent) == 0 && m_previousFrameIndex != frameIndex)
            {
                m_asset->soundEvent(action);
            }
            else if (m_previousFrameIndex != frameIndex)
            {
                _eventDispatcher->dispatchCustomEvent(action.getParam(GAFTimelineAction::PI_EVENT_TYPE), &action);
            }
        }
        break;

        case GAFActionType::None:
        default:
            break;
        }
    }
    m_previousFrameIndex = frameIndex;
}

void GAFObject::processStates(cocos2d::Node* out, uint32_t frameIndex, GAFAnimationFrameConstPtr frame)
{
    const GAFAnimationFrame::SubobjectStates_t& states = frame->getObjectStates();
    for (auto state : states)
    {
        GAFObject* subObject = m_displayList[state->objectIdRef];

        if (!subObject)
            continue;

        cocos2d::AffineTransform stateMatrix = state->affineTransform;

        preProcessGAFObject(out, subObject, state, stateMatrix);

        processGAFObject(out, subObject, state, stateMatrix);

        postProcessGAFObject(out, subObject, state, stateMatrix);

        if (state->isVisible())
            subObject->m_lastVisibleInFrame = frameIndex + 1;
    }
}

uint32_t GAFObject::getFps() const
{
    return m_fps;
}

void GAFObject::setFps(uint32_t value)
{
    CCASSERT(value, "Error! Fps is set to zero.");
    m_fps = value;
}

void GAFObject::setFpsLimitations(bool fpsLimitations)
{
    m_skipFpsCheck = !fpsLimitations;
}

GAFObject* GAFObject::getObjectByName(const std::string& name)
{
    if (name.empty())
    {
        return nullptr;
    }

    std::stringstream ss(name);
    std::string item;
    typedef std::vector<std::string> StringVec_t;
    StringVec_t elems;
    while (std::getline(ss, item, '.'))
    {
        elems.push_back(item);
    }

    if (!elems.empty())
    {
        const NamedParts_t& np = m_timeline->getNamedParts();
        NamedParts_t::const_iterator it = np.find(elems[0]);

        if (it != np.end())
        {
            GAFObject* retval = m_displayList[it->second];

            if (elems.size() == 1)
            {
                return retval;
            }
            StringVec_t::iterator begIt = elems.begin() + 1;

            while (begIt != elems.end())
            {
                const NamedParts_t& np2 = retval->m_timeline->getNamedParts();
                NamedParts_t::const_iterator it2 = np2.find(*begIt);
                if (it2 != np2.end())
                {
                    retval = retval->m_displayList[it2->second];
                }
                else
                {
                    // Sequence is incorrect
                    //break;

                    // It is better to return nil instead of the last found object in a chain
                    return nullptr;
                }

                ++begIt;
            }

            return retval;
        }
    }
    return nullptr;
}

const GAFObject* GAFObject::getObjectByName(const std::string& name) const
{
    return const_cast<GAFObject*>(this)->getObjectByName(name);
}

cocos2d::Vector<GAFObject*> GAFObject::getObjectsByName(const std::string & name)
{
    cocos2d::Vector<GAFObject*> currentObjects;

    if (name.empty())
        return currentObjects;

    std::stringstream ss(name);
    std::string item;
    typedef std::vector<std::string> StringVec_t;
    StringVec_t elems;
    while (std::getline(ss, item, '.'))
        elems.push_back(item);

    currentObjects.pushBack(this);

    for (StringVec_t::const_iterator elIt = elems.cbegin(); elIt != elems.cend() && currentObjects.size() > 0; ++elIt)
    {
        cocos2d::Vector<GAFObject*> foundObjects;
        for (GAFObject* currentObject : currentObjects)
        {
            const NamedParts_t& np = currentObject->getTimeLine()->getNamedParts();

            std::pair<NamedParts_t::const_iterator, NamedParts_t::const_iterator> range = np.equal_range(*elIt);
            for (NamedParts_t::const_iterator npIt = range.first; npIt != range.second; ++npIt)
            {
                GAFObject* child = currentObject->getDisplayList().at(npIt->second);
                if (child)
                    foundObjects.pushBack(child);
            }
        }

        currentObjects = foundObjects;
    }

    return currentObjects;
}

GAFObject* GAFObject::getObjectByNameForCurrentFrame(const std::string& name)
{
    if (name.empty())
        return nullptr;

    std::stringstream ss(name);
    std::string item;
    typedef std::vector<std::string> StringVec_t;
    StringVec_t elems;
    while (std::getline(ss, item, '.'))
        elems.push_back(item);

    GAFObject* retval = nullptr;
    GAFObject* currentObj = this;

    for (StringVec_t::const_iterator elIt = elems.cbegin(); elIt != elems.cend() && currentObj; ++elIt)
    {
        const NamedParts_t& np = currentObj->getTimeLine()->getNamedParts();

        std::pair<NamedParts_t::const_iterator, NamedParts_t::const_iterator> range = np.equal_range(*elIt);

        GAFObject* foundChild = nullptr;

        for (NamedParts_t::const_iterator npIt = range.first; npIt != range.second; ++npIt)
        {
            GAFObject* child = currentObj->getDisplayList().at(npIt->second);

            if (child != nullptr && child->isVisibleInCurrentFrame())
            {
                foundChild = child;
                break;
            }
        }

        retval = currentObj = foundChild;
    }

    return retval;
}

const GAFObject* GAFObject::getObjectByNameForCurrentFrame(const std::string& name) const
{
    return const_cast<GAFObject*>(this)->getObjectByNameForCurrentFrame(name);
}

bool GAFObject::isVisibleInCurrentFrame()
{
    bool value = !(m_timelineParentObject && (m_timelineParentObject->getCurrentFrameIndex() + 1 != m_lastVisibleInFrame));
    GAFObject::changeVisibility(value);
    return value;
}

#if COCOS2D_VERSION < 0x00030200
void GAFObject::visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, bool flags)
#else
void GAFObject::visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
#endif
{
    if (isVisibleInCurrentFrame())
    {
        GAFSprite::visit(renderer, transform, flags);
    }
}

void GAFObject::enableTick(bool val)
{
    if (!m_animationsSelectorScheduled && val)
    {
        schedule(cocos2d::SEL_SCHEDULE(&GAFObject::processAnimations));

        m_animationsSelectorScheduled = true;
    }
    else if (m_animationsSelectorScheduled && !val)
    {
        unschedule(cocos2d::SEL_SCHEDULE(&GAFObject::processAnimations));

        m_animationsSelectorScheduled = false;
    }
}

void GAFObject::changeVisibility(bool value)
{
    if(m_visibility != value)
    {
        m_visibility = value;
        if(m_visibilityCallback)
        {
            m_visibilityCallback(value);
        }
    }
}

void GAFObject::setVisibilityCallback(const GAFVisibilityCallback_t &callback)
{
    m_visibilityCallback = callback;
}

NS_GAF_END

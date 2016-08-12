#include "GAFPrecompiled.h"
#include "GAFComponentView.h"
#include <base/CCStencilStateManager.hpp>

NS_GAF_BEGIN

GAFComponentView::GAFComponentView()
    : m_clippingEnabled(false)
    , m_clippingType(ClippingType::STENCIL)
    , m_clippingStencil(nullptr)
    , m_scissorOldState(false)
    , m_clippingOldRect(cocos2d::Rect::ZERO)
    , m_clippingRect(cocos2d::Rect::ZERO)
    , m_clippingRectDirty(true)
    , m_stencileStateManager(new cocos2d::StencilStateManager())
{
    
}

GAFComponentView::~GAFComponentView()
{
    CC_SAFE_RELEASE(m_clippingStencil);
    CC_SAFE_DELETE(m_stencileStateManager);
}

void GAFComponentView::setClippingEnabled(bool enabled)
{
    if (enabled == m_clippingEnabled) return;

    m_clippingEnabled = enabled;
    switch (m_clippingType)
    {
    case ClippingType::STENCIL:
        if (enabled)
        {
            m_clippingStencil = cocos2d::DrawNode::create();
            if (_running)
                m_clippingStencil->onEnter();

            m_clippingStencil->retain();
            //setStencilClippingRect(getInternalBoundingBox());
        }
        else
        {
            if (_running)
                m_clippingStencil->onExit();

            m_clippingStencil->release();
            m_clippingStencil = nullptr;
        }
        break;
    default:
        break;
    }
}

void GAFComponentView::setClippingType(ClippingType type)
{
    if (type == m_clippingType) return;

    bool clippingEnabled = isClippingEnabled();
    setClippingEnabled(false);
    m_clippingType = type;
    setClippingEnabled(clippingEnabled);
}

GAFComponentView::ClippingType GAFComponentView::getClippingType() const
{
    return m_clippingType;
}

bool GAFComponentView::isClippingEnabled() const
{
    return m_clippingEnabled;
}

void GAFComponentView::visit(cocos2d::Renderer* renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags)
{
    if (!isVisibleInCurrentFrame()) return;

    if (m_clippingEnabled)
    {
        switch (m_clippingType)
        {
        case ClippingType::STENCIL:
            stencilClippingVisit(renderer, parentTransform, parentFlags);
            break;
        case ClippingType::SCISSOR:
            scissorClippingVisit(renderer, parentTransform, parentFlags);
            break;
        default:
            break;
        }
    }
    else
    {
        GAFObject::visit(renderer, parentTransform, parentFlags);
    }
}

void GAFComponentView::onEnter()
{
    GAFObject::onEnter();
    if (m_clippingStencil)
        m_clippingStencil->onEnter();

    m_clippingRectDirty = true;
}

void GAFComponentView::onExit()
{
    GAFObject::onExit();
    if (m_clippingStencil)
        m_clippingStencil->onExit();
}

void GAFComponentView::setCameraMask(unsigned short mask, bool applyChildren)
{
    GAFObject::setCameraMask(mask, applyChildren);
    if (m_clippingStencil)
        m_clippingStencil->setCameraMask(mask, applyChildren);
}

void GAFComponentView::stencilClippingVisit(cocos2d::Renderer* renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags)
{
    if (!isVisibleInCurrentFrame())
        return;

    uint32_t flags = processParentFlags(parentTransform, parentFlags);

    // IMPORTANT:
    // To ease the migration to v3.0, we still support the Mat4 stack,
    // but it is deprecated and your code should not rely on it
    cocos2d::Director* director = cocos2d::Director::getInstance();
    CCASSERT(nullptr != director, "Director is null when setting matrix stack");
    director->pushMatrix(cocos2d::MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    director->loadMatrix(cocos2d::MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);
    //Add group command
    
    m_groupCommand.init(_globalZOrder);
    renderer->addCommand(&m_groupCommand);

    renderer->pushGroup(m_groupCommand.getRenderQueueID());

    m_beforeVisitCmdStencil.init(_globalZOrder);
    m_beforeVisitCmdStencil.func = CC_CALLBACK_0(cocos2d::StencilStateManager::onBeforeVisit, m_stencileStateManager);
    renderer->addCommand(&m_beforeVisitCmdStencil);

    m_clippingStencil->visit(renderer, _modelViewTransform, flags);

    m_afterDrawStencilCmd.init(_globalZOrder);
    m_afterDrawStencilCmd.func = CC_CALLBACK_0(cocos2d::StencilStateManager::onAfterDrawStencil, m_stencileStateManager);
    renderer->addCommand(&m_afterDrawStencilCmd);

    int i = 0;      // used by _children

    sortAllChildren();

    //
    // draw children and protectedChildren zOrder < 0
    //
    for (; i < _children.size(); ++i)
    {
        auto node = _children.at(i);

        if (node && node->getLocalZOrder() < 0)
            node->visit(renderer, _modelViewTransform, flags);
        else
            break;
    }

    //
    // draw self
    //
    this->draw(renderer, _modelViewTransform, flags);

    //
    // draw children zOrder >= 0
    //
    for (auto it = _children.cbegin() + i; it != _children.cend(); ++it)
        (*it)->visit(renderer, _modelViewTransform, flags);


    m_afterVisitCmdStencil.init(_globalZOrder);
    m_afterVisitCmdStencil.func = CC_CALLBACK_0(cocos2d::StencilStateManager::onAfterVisit, m_stencileStateManager);
    renderer->addCommand(&m_afterVisitCmdStencil);

    renderer->popGroup();

    director->popMatrix(cocos2d::MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void GAFComponentView::scissorClippingVisit(cocos2d::Renderer* renderer, const cocos2d::Mat4& parentTransform, uint32_t parentFlags)
{
    if (parentFlags & FLAGS_DIRTY_MASK)
        m_clippingRectDirty = true;

    m_beforeVisitCmdScissor.init(_globalZOrder);
    m_beforeVisitCmdScissor.func = CC_CALLBACK_0(GAFComponentView::onBeforeVisitScissor, this);
    renderer->addCommand(&m_beforeVisitCmdScissor);

    GAFObject::visit(renderer, parentTransform, parentFlags);

    m_afterVisitCmdScissor.init(_globalZOrder);
    m_afterVisitCmdScissor.func = CC_CALLBACK_0(GAFComponentView::onAfterVisitScissor, this);
    renderer->addCommand(&m_afterVisitCmdScissor);
}

void GAFComponentView::setStencilClippingRect(const cocos2d::Rect& rect)
{
    if (m_clippingEnabled && m_clippingType == ClippingType::STENCIL)
    {
        cocos2d::Vec2 clipRect[4];
        clipRect[0].set(rect.origin.x, -rect.origin.y);
        clipRect[1].set(rect.origin.x + rect.size.width, -rect.origin.y);
        clipRect[2].set(rect.origin.x + rect.size.width, -(rect.origin.y + rect.size.height));
        clipRect[3].set(rect.origin.x, -(rect.origin.y + rect.size.height));
        cocos2d::Color4F green(0.0f, 1.0f, 0.0f, 1.0f);
        m_clippingStencil->clear();
        m_clippingStencil->drawPolygon(clipRect, 4, green, 0, green);
    }
}

const cocos2d::Rect& GAFComponentView::getClippingRect()
{
    if (m_clippingRectDirty)
    {
        cocos2d::Vec2 worldPos = convertToWorldSpace(cocos2d::Vec2::ZERO);
        cocos2d::AffineTransform t = getNodeToWorldAffineTransform();
        float scissorWidth = _contentSize.width*t.a;
        float scissorHeight = _contentSize.height*t.d;
        cocos2d::Rect parentClippingRect;

        m_clippingRect.origin.x = worldPos.x - (scissorWidth * _anchorPoint.x);
        m_clippingRect.origin.y = worldPos.y - (scissorHeight * _anchorPoint.y);
        m_clippingRect.size.width = scissorWidth;
        m_clippingRect.size.height = scissorHeight;

        m_clippingRectDirty = false;
    }
    return m_clippingRect;
}

void GAFComponentView::realizeFrame(cocos2d::Node* out, uint32_t frameIndex)
{
    GAFObject::realizeFrame(out, frameIndex);
    if (m_clippingRectDirty)
        setStencilClippingRect(getInternalBoundingBox());
}

const cocos2d::Mat4 & GAFComponentView::getNodeToParentTransform() const
{
    if (_transformDirty) m_clippingRectDirty = true;

    return GAFObject::getNodeToParentTransform();
}

void GAFComponentView::onBeforeVisitScissor()
{
    auto glview = cocos2d::Director::getInstance()->getOpenGLView();
    // apply scissor test
    m_scissorOldState = glview->isScissorEnabled();
    if (false == m_scissorOldState)
    {
        glEnable(GL_SCISSOR_TEST);
    }

    // apply scissor box
    cocos2d::Rect clippingRect = getClippingRect();
    m_clippingOldRect = glview->getScissorRect();
    if (false == m_clippingOldRect.equals(clippingRect))
    {
        glview->setScissorInPoints(clippingRect.origin.x,
            clippingRect.origin.y,
            clippingRect.size.width,
            clippingRect.size.height);
    }
}

void GAFComponentView::onAfterVisitScissor()
{
    if (m_scissorOldState)
    {
        // revert scissor box
        if (false == m_clippingOldRect.equals(m_clippingRect))
        {
            auto glview = cocos2d::Director::getInstance()->getOpenGLView();
            glview->setScissorInPoints(m_clippingOldRect.origin.x,
                m_clippingOldRect.origin.y,
                m_clippingOldRect.size.width,
                m_clippingOldRect.size.height);
        }
    }
    else
    {
        // revert scissor test
        glDisable(GL_SCISSOR_TEST);
    }
}

    NS_GAF_END

#pragma once
#include "GAFObject.h"

NS_GAF_BEGIN

class GAFComponentView : public GAFObject
{
public:
    enum class ClippingType
    {
        STENCIL,
        SCISSOR
    };

    GAFComponentView();
    virtual ~GAFComponentView() = 0;

    virtual void setClippingEnabled(bool enabled);

    void setClippingType(ClippingType type);

    ClippingType getClippingType()const;

    virtual bool isClippingEnabled()const;

    virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags) override;

    virtual void onEnter() override;

    virtual void onExit() override;

    virtual void setCameraMask(unsigned short mask, bool applyChildren = true) override;

protected:
    void stencilClippingVisit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags);
    void scissorClippingVisit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags);

    void setStencilClippingRect(const cocos2d::Rect& rect);
    const cocos2d::Rect& getClippingRect();

    //clipping

    void onBeforeVisitScissor();
    void onAfterVisitScissor();

    //clipping
    bool m_clippingEnabled;
    ClippingType m_clippingType;
    cocos2d::DrawNode* m_clippingStencil;
    bool m_scissorOldState;
    cocos2d::Rect m_clippingOldRect;
    cocos2d::Rect m_clippingRect;
    bool m_clippingRectDirty;

    //clipping
    cocos2d::StencilStateManager* m_stencileStateManager;

    cocos2d::GroupCommand m_groupCommand;
    cocos2d::CustomCommand m_beforeVisitCmdStencil;
    cocos2d::CustomCommand m_afterDrawStencilCmd;
    cocos2d::CustomCommand m_afterVisitCmdStencil;
    cocos2d::CustomCommand m_beforeVisitCmdScissor;
    cocos2d::CustomCommand m_afterVisitCmdScissor;
};

NS_GAF_END

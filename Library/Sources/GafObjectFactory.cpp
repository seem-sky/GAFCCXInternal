#include "GAFPrecompiled.h"
#include "GAFObjectFactory.h"
#include "GAFAsset.h"
#include "GAFTextureAtlasElement.h"
#include "GAFAssetTextureManager.h"
#include "GAFMovieClip.h"
#include "GAFMask.h"
#include "GAFTextField.h"
#include "UI/GAFCanvasView.h"
#include "UI/GAFBoxLayoutView.h"

NS_GAF_BEGIN

GAFObject* GafObjectFactory::create(GAFAsset* asset, uint32_t id, GAFTextureAtlasElement* txElemet, bool isMask)
{
    GAFObject* result = nullptr;
    cocos2d::SpriteFrame * spriteFrame = nullptr;
    GAFAssetTextureManager* txMgr = asset->getTextureManager();
    cocos2d::Texture2D * texture = txMgr->getTextureById(txElemet->atlasIdx + 1);
    if (texture)
    {
        spriteFrame = cocos2d::SpriteFrame::createWithTexture(texture, txElemet->bounds);

        if (!isMask)
            result = new GAFMovieClip();
        else
            result = new GAFMask();

        result->initWithSpriteFrame(spriteFrame, txElemet->rotation, txElemet->scale9GridRect);
        result->objectIdRef = id;
        cocos2d::Vec2 pt = cocos2d::Vec2(0 - (0 - (txElemet->pivotPoint.x / result->getContentSize().width)),
            0 + (1 - (txElemet->pivotPoint.y / result->getContentSize().height)));
        result->setAnchorPoint(pt);

        if (txElemet->getScale() != 1.0f)
            result->setAtlasScale(1.0f / txElemet->getScale());

        result->setBlendFunc(cocos2d::BlendFunc::ALPHA_PREMULTIPLIED);
    }
    else
    {
        CCLOGERROR("Cannot add sub object with Id: %d, atlas with idx: %d not found.", id, txElemet->atlasIdx);
    }

    return result;
}

GAFObject* GafObjectFactory::create(GAFAsset* asset, uint32_t id, GAFTextData* txtData, bool isMask)
{
    GAFTextField* result = new GAFTextField();
    result->initWithTextData(txtData);
    result->objectIdRef = id;

    return result;
}

GAFObject* GafObjectFactory::create(GAFAsset* asset, uint32_t id, GAFTimeline* timeline, bool isMask)
{
    GAFObject* result = nullptr;
    std::string linkageName = timeline->getLinkageName();
    std::string baseClass = timeline->getBaseClass();

    if (baseClass == GAFObjectClass::toString(GAFObjectClass::UI_BOX_LAYOUT))
    {
        result = new GAFBoxLayoutView();
    }
    else if (baseClass == GAFObjectClass::toString(GAFObjectClass::UI_CANVAS))
    {
        result = new GAFCanvasView();
    }
    else if (baseClass == GAFObjectClass::toString(GAFObjectClass::UI_BUTTON))
    {
        result = new GAFObject();
    }
    else
    {
        result = new GAFObject();
    }
    
    result->init(asset, timeline);
    result->objectIdRef = id;

    return result;
}

NS_GAF_END

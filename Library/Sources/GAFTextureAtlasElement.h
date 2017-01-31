#pragma once

#include "GAFCollections.h"

NS_GAF_BEGIN

forward_this(GAFTextureAtlasElement);

class GAFTextureAtlasElement
{
public:
    std::string        linkageName;
    std::string        baseClass;
    std::string        originClass;
    cocos2d::Vec2      pivotPoint;
    cocos2d::Rect      bounds;
    
    unsigned int       atlasIdx;
    unsigned int       elementAtlasIdx;

    GAFRotation        rotation = GAFRotation::NONE;

    cocos2d::Rect      scale9GridRect;

    GAFTextureAtlasElement();

    void setScale(float s);
    const float getScale() const { return scale; }
    void setScaleX(float s);
    const float getScaleX() const { return scaleX; }
    void setScaleY(float s);
    const float getScaleY() const { return scaleY; }

private:
    float              scale = 1.f;
    float              scaleX = 1.f;
    float              scaleY = 1.f;
}; // GAFTextureAtlasElement

NS_GAF_END
#include "GAFPrecompiled.h"
#include "GAFTextureAtlasElement.h"

NS_GAF_BEGIN

GAFTextureAtlasElement::GAFTextureAtlasElement()
{
}

void GAFTextureAtlasElement::setScale(float s)
{
    scale = scaleX = scaleY = s;
}

void GAFTextureAtlasElement::setScaleX(float s)
{
    scale = scaleX = s;
}

void GAFTextureAtlasElement::setScaleY(float s)
{
    scale = scaleY = s;
}

NS_GAF_END
#include "GAFPrecompiled.h"
#include "GAFTextureAtlas.h"
#include "GAFTextureAtlasElement.h"

NS_GAF_BEGIN

GAFTextureAtlas::GAFTextureAtlas()
{
}

GAFTextureAtlas::~GAFTextureAtlas()
{
}

bool GAFTextureAtlas::compareAtlasesById(const AtlasInfo& ai1, const AtlasInfo& ai2)
{
    return ai1.id < ai2.id;
}

void GAFTextureAtlas::setScale(float val)
{
    m_scale = val;
}

float GAFTextureAtlas::getScale() const
{
    return m_scale;
}

void GAFTextureAtlas::pushAtlasInfo(const AtlasInfo& ai)
{
    m_atlasInfos.push_back(ai);
}

void GAFTextureAtlas::pushElement(uint32_t idx, GAFTextureAtlasElementConstPtr el)
{
    m_elements[idx] = el;
}

bool GAFTextureAtlas::swapElement(uint32_t idx, GAFTextureAtlasElementConstPtr el)
{
    Elements_t::iterator it = m_elements.find(idx);
    if (it != m_elements.end())
        m_elements.erase(it);

    pushElement(idx, el);
    return true;
}

const GAFTextureAtlas::Elements_t& GAFTextureAtlas::getElements() const
{
    return m_elements;
}

const GAFTextureAtlas::AtlasInfos_t& GAFTextureAtlas::getAtlasInfos() const
{
    return m_atlasInfos;
}

NS_GAF_END
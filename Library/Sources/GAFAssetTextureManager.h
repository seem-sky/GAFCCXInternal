#pragma once

#include "GAFTextureAtlas.h"
#include "GAFDelegates.h"

NS_GAF_BEGIN

gaf_fwd_this(GAFAssetTextureManager);

class GAFAssetTextureManager
{
public:
    GAFAssetTextureManager();
    ~GAFAssetTextureManager();

    void                    appendInfoFromTextureAtlas(GAFTextureAtlasConstPtr atlas);
    void                    loadImages(const std::string& dir, GAFTextureLoadDelegate_t delegate, cocos2d::ZipFile* bundle = nullptr);
    cocos2d::Texture2D*     getTextureById(uint32_t id);
    void                    preloadAllTextures();
    bool                    swapTexture(uint32_t id, cocos2d::Texture2D* texture);

    uint32_t                getMemoryConsumptionStat() const;

private:
    using ImagesMap_t = std::map<size_t, cocos2d::Image*>;
    using TexturesMap_t = std::map<size_t, cocos2d::Texture2D*>;

    bool isAtlasInfoPresent(const GAFTextureAtlas::AtlasInfo &ai);

    GAFTextureAtlas::AtlasInfos_t m_atlasInfos;

    ImagesMap_t m_images;
    TexturesMap_t m_textures;

    uint32_t m_memoryConsumption = 0;
};

NS_GAF_END

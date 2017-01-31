#pragma once

NS_GAF_BEGIN

forward_this(GAFTextureAtlas);
forward_this(GAFTextureAtlasElement);

class GAFTextureAtlas
{
public:

    struct AtlasInfo
    {
        struct Source
        {
            std::string source;
            float csf;
        };

        typedef std::vector<Source> Sources_t;

        Sources_t m_sources;
        unsigned int id;
    };

    static bool compareAtlasesById(const AtlasInfo& ai1, const AtlasInfo& ai2);

    using AtlasInfos_t = std::vector<AtlasInfo>;
    using Elements_t = std::map<uint32_t, GAFTextureAtlasElementConstPtr>;

    GAFTextureAtlas();
private:

    float           m_scale = 1.f;
    AtlasInfos_t    m_atlasInfos;
    Elements_t      m_elements;
public:
    ~GAFTextureAtlas();

    void    pushAtlasInfo(const AtlasInfo& ai);
    void    pushElement(uint32_t idx, GAFTextureAtlasElementConstPtr el);
    bool    swapElement(uint32_t idx, GAFTextureAtlasElementConstPtr el);

    void    setScale(float val);
    float   getScale() const;

    const Elements_t& getElements() const;
    const AtlasInfos_t& getAtlasInfos() const;
};

NS_GAF_END
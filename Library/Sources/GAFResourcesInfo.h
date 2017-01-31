#pragma once

NS_GAF_BEGIN

gaf_fwd_this(GAFResourcesInfo);

class GAFResourcesInfo
{
public:
    enum class ResourceId : uint16_t
    {
        Texture = 0,
        Font,
        Sound, // this one for future usage
    };

    ResourceId id;
};

gaf_fwd_this(GAFResourcesInfoTexture);

class GAFResourcesInfoTexture : public GAFResourcesInfo
{
public:
    GAFResourcesInfoTexture() { id = ResourceId::Texture; }
    GAFResourcesInfoTexture(std::string _source, float _csf)
    {
        id = ResourceId::Texture;
        source = _source;
        csf = _csf;
    }

    bool operator==(const GAFResourcesInfoTexture &other) const
    {
        return source.compare(other.source) == 0 && fabs(csf - other.csf) < std::numeric_limits<float>::epsilon();
    }

    std::string source;
    float csf;
};

gaf_fwd_this(GAFResourcesInfoFont);

class GAFResourcesInfoFont : public GAFResourcesInfo
{
public:
    GAFResourcesInfoFont() { id = ResourceId::Font; }
    GAFResourcesInfoFont(std::string _name)
    {
        id = ResourceId::Font;
        name = _name;
    }

    bool operator==(const GAFResourcesInfoFont &other) const
    {
        return name.compare(other.name) == 0;
    }

    std::string name;
};

NS_GAF_END
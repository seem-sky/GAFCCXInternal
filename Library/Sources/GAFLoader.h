#pragma once

#include "TagDefines.h"
#include "GAFCollections.h"

NS_GAF_BEGIN

forward_this(GAFLoader);

forward_this(GAFAsset);
forward_this(GAFTimeline);

forward_this(GAFStream);
forward_this(DefinitionTagBase);
forward_this(GAFHeader);
forward_this(GAFFile);

class GAFLoader : public ::std::enable_shared_from_this<GAFLoader>
{
public:
    using CustomPropertiesMap_t = std::map<uint32_t, CustomProperties_t>; // Custom properties set by timeline id

    static GAFLoaderPtr create();

protected:
    GAFLoader() = default;

private:
    GAFStreamPtr            m_stream;
    CustomPropertiesMap_t   m_customProperties;

    void                    _readHeaderEnd(GAFHeader&);
    void                    _readHeaderEndV4(GAFHeader&);

    void                    _registerTagLoadersV3();
    void                    _registerTagLoadersCommon();
    void                    _registerTagLoadersV4();

protected:
    using TagLoaders_t = std::map</*Tags::Enum*/ uint32_t, DefinitionTagBasePtr>;
    TagLoaders_t            m_tagLoaders;

    virtual void            _processLoad(GAFFilePtr file, GAFAssetPtr context);

public:
    virtual ~GAFLoader() {}

    bool                    loadFile(const std::string& fname, GAFAssetPtr context);
    bool                    loadData(const unsigned char* data, size_t len, GAFAssetPtr context);
    bool                    isFileLoaded() const;

    GAFStreamConstPtr       getStream() const;

    const GAFHeader&        getHeader() const;

    void                    registerTagLoader(unsigned int idx, DefinitionTagBasePtr);

    void                    loadTags(GAFStreamPtr in, GAFAssetPtr asset, GAFTimelinePtr timeline);
    void                    readCustomProperties(GAFStreamPtr in, CustomProperties_t& customProperties) const;

    const                   CustomProperties_t& getCustomProperties(uint32_t timeline) const;
    void                    setCustomProperties(uint32_t timeline, CustomProperties_t cp);
};

NS_GAF_END
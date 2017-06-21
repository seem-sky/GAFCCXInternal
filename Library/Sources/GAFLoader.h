#pragma once

#include "GAFCollections.h"
#include "json/document.h"
#include "json/stringbuffer.h"
#include "GAFSubobjectState.h"

NS_GAF_BEGIN

gaf_fwd_this(GAFLoader);

gaf_fwd_this(GAFAsset);
gaf_fwd_this(GAFTimeline);

gaf_fwd_this(GAFStream);
gaf_fwd_this(DefinitionTagBase);
gaf_fwd_this(GAFHeader);
gaf_fwd_this(GAFFile);

class GAFLoader : public std::enable_shared_from_this<GAFLoader>
{
public:
    using CustomPropertiesMap_t = std::map<GAFCharacterType, std::map<uint32_t, std::string>>; // Custom properties set by timeline id
    using CustomPropertiesIndices_t = std::map<GAFSubobjectStatePtr, std::vector<size_t>>;

    static GAFLoaderPtr create();

protected:
    GAFLoader() = default;

private:
    GAFStreamPtr            m_stream;
    CustomPropertiesMap_t   m_customProperties{ { GAFCharacterType::Timeline, {} }, { GAFCharacterType::External, {} } };
    CustomPropertiesIndices_t m_customPropertiesIndices;

    void                    _readHeaderEnd(GAFHeader&);
    void                    _readHeaderEndV4(GAFHeader&);

    void                    _registerTagLoadersV3();
    void                    _registerTagLoadersCommon();
    void                    _registerTagLoadersV4();

protected:
    using TagLoaders_t = std::map</*Tags::Enum*/ uint32_t, DefinitionTagBasePtr>;
    TagLoaders_t            m_tagLoaders;

    virtual void            _processLoad(GAFFilePtr file, GAFAssetPtr context);
    virtual void            _postProcessAsset(GAFAssetPtr asset);

public:
    virtual ~GAFLoader() {}

    bool                    loadFile(const std::string& fname, GAFAssetPtr context);
    bool                    loadData(const unsigned char* data, size_t len, GAFAssetPtr context);
    bool                    isFileLoaded() const;

    GAFStreamConstPtr       getStream() const;

    const GAFHeader&        getHeader() const;

    void                    registerTagLoader(unsigned int idx, DefinitionTagBasePtr);

    void                    loadTags(GAFStreamPtr in, GAFAssetPtr asset, GAFTimelinePtr timeline);

    const std::string&      getCustomProperties(GAFCharacterType type, uint32_t timeline) const;
    void                    setCustomProperties(GAFCharacterType type, uint32_t timeline, std::string&& cp);

    void                    setCustomPropertiesIndices(GAFSubobjectStatePtr state, std::vector<size_t>&& cpi);
};

NS_GAF_END
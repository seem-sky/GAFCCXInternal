#include "GAFPrecompiled.h"
#include "GAFLoader.h"

#include "GAFAsset.h"
#include "DefinitionTagBase.h"
#include "GAFHeader.h"
#include "GAFStream.h"
#include "GAFFile.h"
#include "GAFTimeline.h"
#include "GAFAnimationFrame.h"

#include "PrimitiveDeserializer.h"

#include "TagDefineAtlas.h"
#include "TagDefineAtlas3.h"
#include "TagDefineAtlas4.h"
#include "TagDefineAnimationMasks.h"
#include "TagDefineAnimationObjects.h"
#include "TagDefineAnimationFrames.h"
#include "TagDefineNamedParts.h"
#include "TagDefineSequences.h"
#include "TagDefineStage.h"
#include "TagDefineAnimationFrames2.h"
#include "TagDefineTimeline.h"
#include "TagDefineTimeline2.h"
#include "TagDefineTimeline3.h"
#include "TagDefineTextField.h"
#include "TagDefineSounds.h"
#include "TagDefineExternalObjects.h"
#include "TagDefineExternalObjects2.h"
#include "TagDefineAnimationFrames3.h"
#include "TagDefineStencil.h"
#include "GAFCustomProperties.h"

NS_GAF_BEGIN

GAFLoaderPtr GAFLoader::create()
{
    return std::shared_ptr<GAFLoader>(new GAFLoader);
}

void GAFLoader::_readHeaderEnd(GAFHeader& header)
{
    header.framesCount = m_stream->readU16();
    PrimitiveDeserializer::deserialize(m_stream, &header.frameSize);
    PrimitiveDeserializer::deserialize(m_stream, &header.pivot);
}

void GAFLoader::_readHeaderEndV4(GAFHeader& header)
{
    size_t scaleValuesCount = m_stream->readU32();
    while (scaleValuesCount)
    {
        float val = m_stream->readFloat();
        header.scaleValues.push_back(val);

        scaleValuesCount--;
    }

    size_t csfValuesCount = m_stream->readU32();
    while (csfValuesCount)
    {
        float val = m_stream->readFloat();
        header.csfValues.push_back(val);

        csfValuesCount--;
    }
}

void GAFLoader::_registerTagLoadersV3()
{
    m_tagLoaders[Tags::TagDefineAtlas] = std::make_shared<TagDefineAtlas>();
    m_tagLoaders[Tags::TagDefineAnimationMasks] = std::make_shared<TagDefineAnimationMasks>();
    m_tagLoaders[Tags::TagDefineAnimationObjects] = std::make_shared<TagDefineAnimationObjects>();
    m_tagLoaders[Tags::TagDefineAnimationFrames] = std::make_shared<TagDefineAnimationFrames>();
}

void GAFLoader::_registerTagLoadersV4()
{
    m_tagLoaders[Tags::TagDefineAnimationFrames2] = std::make_shared<TagDefineAnimationFrames2>(shared_from_this());
    m_tagLoaders[Tags::TagDefineAnimationObjects2] = std::make_shared<TagDefineAnimationObjects>();
    m_tagLoaders[Tags::TagDefineAnimationMasks2] = std::make_shared<TagDefineAnimationMasks>();
    m_tagLoaders[Tags::TagDefineAtlas2] = std::make_shared<TagDefineAtlas>();
    m_tagLoaders[Tags::TagDefineAtlas3] = std::make_shared<TagDefineAtlas3>();
    m_tagLoaders[Tags::TagDefineAtlas4] = std::make_shared<TagDefineAtlas4>();
    m_tagLoaders[Tags::TagDefineTextFields] = std::make_shared<TagDefineTextField>();
    m_tagLoaders[Tags::TagDefineTimeline] = std::make_shared<TagDefineTimeline>(shared_from_this());
    m_tagLoaders[Tags::TagDefineTimeline2] = std::make_shared<TagDefineTimeline2>(shared_from_this());
    m_tagLoaders[Tags::TagDefineTimeline3] = std::make_shared<TagDefineTimeline3>(shared_from_this());
    m_tagLoaders[Tags::TagDefineSounds] = std::make_shared<TagDefineSounds>();
    m_tagLoaders[Tags::TagDefineExternalObjects] = std::make_shared<TagDefineExternalObjects>();
    m_tagLoaders[Tags::TagDefineAnimationFrames3] = std::make_shared<TagDefineAnimationFrames3>(shared_from_this());
    m_tagLoaders[Tags::TagDefineExternalObjects2] = std::make_shared<TagDefineExternalObjects2>(shared_from_this());
    m_tagLoaders[Tags::TagDefineStencil] = std::make_shared<TagDefineStencil>();
}

void GAFLoader::_registerTagLoadersCommon()
{
    m_tagLoaders[Tags::TagDefineStage] = std::make_shared<TagDefineStage>();
    m_tagLoaders[Tags::TagDefineNamedParts] = std::make_shared<TagDefineNamedParts>();
    m_tagLoaders[Tags::TagDefineSequences] = std::make_shared<TagDefineSequences>();
}

void GAFLoader::loadTags(GAFStreamPtr in, GAFAssetPtr asset, GAFTimelinePtr timeline)
{
    bool tagEndRead = false;

    while (!in->isEndOfStream())
    {
        Tags::Enum tag = in->openTag();

        TagLoaders_t::iterator it = m_tagLoaders.find(tag);

        if (it != m_tagLoaders.end())
        {
            it->second->read(in, asset, timeline);
        }
        else
        {
            if (tag != Tags::TagEnd)
            {
                CCLOG("No tag parser for %d. Custom loader needed", tag);
            }
        }

        in->closeTag();

        if (tag == Tags::TagEnd)
        {
            tagEndRead = true;
            break;
        }
    }

    if (!tagEndRead)
    {
        //TODO: warning or error here
    }
}

const std::string& GAFLoader::getCustomProperties(GAFCharacterType type, uint32_t timeline) const
{
    auto itType = m_customProperties.find(type);
    assert(itType != m_customProperties.cend());

    const auto& cpMap = itType->second;
    auto it = cpMap.find(timeline);
    assert(it != cpMap.cend());

    return it->second;
}

void GAFLoader::setCustomProperties(GAFCharacterType type, uint32_t timeline, std::string&& cp)
{
    m_customProperties[type][timeline] = std::move(cp);
}

void GAFLoader::setCustomPropertiesIndices(GAFSubobjectStatePtr state, std::vector<size_t>&& cpi)
{
    m_customPropertiesIndices[state] = std::move(cpi);
}

bool GAFLoader::loadData(const unsigned char* data, size_t len, GAFAssetPtr context)
{
    auto file = std::make_shared<GAFFile>();

    bool retval = false;

    if (file->open(data, len))
    {
        retval = true;

        _processLoad(file, context);
    }

    return retval;
}

void GAFLoader::_processLoad(GAFFilePtr file, GAFAssetPtr context)
{
    m_stream = std::make_shared<GAFStream>(file);

    GAFHeader& header = m_stream->m_input->getHeader();

    GAFTimelinePtr timeline;
    if (header.getMajorVersion() >= 4)
    {
        _readHeaderEndV4(header);
        _registerTagLoadersV4();
    }
    else
    {
        _readHeaderEnd(header);
        _registerTagLoadersV3();

        timeline = std::make_shared<GAFTimeline>(nullptr, 0, header.frameSize, header.pivot, header.framesCount);  // will be released in assset dtor

        context->pushTimeline(0, timeline);
        context->setRootTimeline((uint32_t)0);
    }

    _registerTagLoadersCommon();

    context->setHeader(header);

    loadTags(m_stream, context, timeline);

    _postProcessAsset(context);
}

void GAFLoader::_postProcessAsset(GAFAssetPtr asset)
{
    for (auto t : asset->m_timelines)
    {
        for (auto f : t.second->m_animationFrames)
        {
            for (auto s : f->m_subObjectStates)
            {
                auto animObjectIt = t.second->getAnimationObjects().find(s->objectIdRef);
                if (animObjectIt == t.second->getAnimationObjects().cend())
                {
                    animObjectIt = t.second->getAnimationMasks().find(s->objectIdRef);
                    assert(animObjectIt != t.second->getAnimationMasks().cend());
                }

                const auto& animObjectKV = animObjectIt->second;

                auto itType = m_customProperties.find(animObjectKV.second);
                if (itType == m_customProperties.cend())
                    continue;

                const auto& cpMap = itType->second;
                auto it = cpMap.find(animObjectKV.first);
                if (it == cpMap.cend())
                    continue;

                const auto& cpsStr = it->second;
                assert(!cpsStr.empty());

                auto stateIt = m_customPropertiesIndices.find(std::const_pointer_cast<GAFSubobjectState>(s));
                if (stateIt == m_customPropertiesIndices.cend())
                    continue;

                const auto& indices = stateIt->second;

                rapidjson::Document doc;
                doc.Parse<0>(cpsStr.c_str());

                const rapidjson::Value& root = doc["cps"];
                assert(root.IsArray());

                auto cps = std::make_shared<cp::GAFCustomProperties>();

                for (rapidjson::SizeType i = 0; i < root.Size(); ++i)
                {
                    const rapidjson::Value& prop_value = root[i];
                    assert(prop_value.MemberCount() == 1);

                    rapidjson::Value::ConstMemberIterator firstMember = prop_value.MemberBegin();
                    std::string propName = firstMember->name.GetString();
                    const rapidjson::Value& values = firstMember->value;
                    assert(values.IsArray());

                    const uint32_t idx = indices[i];
                    const auto& value = values[idx];

                    if (propName == cp::kCPAlignMode)
                        cps->set<cp::CPEnum::alignMode>(cp::toEnum<cp::AlignMode>(value.GetString()));
                    else if (propName == cp::kCPAlignTop)
                        cps->set<cp::CPEnum::alignTop>(value.GetBool());
                    else if (propName == cp::kCPAlignRight)
                        cps->set<cp::CPEnum::alignRight>(value.GetBool());
                    else if (propName == cp::kCPAlignBottom)
                        cps->set<cp::CPEnum::alignBottom>(value.GetBool());
                    else if (propName == cp::kCPAlignLeft)
                        cps->set<cp::CPEnum::alignLeft>(value.GetBool());
                    else if (propName == cp::kCPScaleAlignedChildren)
                        cps->set<cp::CPEnum::scaleAlignedChildren>(value.GetBool());
                    else if (propName == cp::kCPFittingMode)
                        cps->set<cp::CPEnum::fittingMode>(cp::toEnum<cp::FittingMode>(value.GetString()));
                    else if (propName == cp::kCPMarginTop)
                        cps->set<cp::CPEnum::marginTop>(value.GetInt());
                    else if (propName == cp::kCPMarginRight)
                        cps->set<cp::CPEnum::marginRight>(value.GetInt());
                    else if (propName == cp::kCPMarginBottom)
                        cps->set<cp::CPEnum::marginBottom>(value.GetInt());
                    else if (propName == cp::kCPMarginLeft)
                        cps->set<cp::CPEnum::marginLeft>(value.GetInt());
                    else if (propName == cp::kCPMarginMode)
                        cps->set<cp::CPEnum::marginMode>(cp::toEnum<cp::MarginMode>(value.GetString()));
                    else if (propName == cp::kCPGap)
                        cps->set<cp::CPEnum::gap>(value.GetInt());
                    else if (propName == cp::kCPGapMode)
                        cps->set<cp::CPEnum::gapMode>(cp::toEnum<cp::GapMode>(value.GetString()));
                    else if (propName == cp::kCPDirection)
                        cps->set<cp::CPEnum::direction>(cp::toEnum<cp::Direction>(value.GetString()));
                    else if (propName == cp::kCPVerticalAlign)
                        cps->set<cp::CPEnum::verticalAlign>(cp::toEnum<cp::VerticalAlign>(value.GetString()));
                    else if (propName == cp::kCPHorizontalAlign)
                        cps->set<cp::CPEnum::horizontalAlign>(cp::toEnum<cp::HorizontalAlign>(value.GetString()));
                    else if (propName == cp::kCPUseTextBounds)
                        cps->set<cp::CPEnum::useTextBounds>(value.GetBool());
                    else
                        assert(false);
                }

                stateIt->first->m_customProperties = cps;
            }
        }
    }
}

bool GAFLoader::loadFile(const std::string& fname, GAFAssetPtr context)
{
    auto file = std::make_shared<GAFFile>();

    bool retval = false;

    if (file->open(fname, "rb"))
    {
        retval = true;

        _processLoad(file, context);
    }

    return retval;
}

bool GAFLoader::isFileLoaded() const
{
    return m_stream && m_stream->isEndOfStream();
}

GAFStreamConstPtr GAFLoader::getStream() const
{
    return m_stream;
}

const GAFHeader& GAFLoader::getHeader() const
{
    return m_stream->getInput()->getHeader();
}

void GAFLoader::registerTagLoader(unsigned int idx, DefinitionTagBasePtr tagptr)
{
    m_tagLoaders[static_cast<Tags::Enum>(idx)] = tagptr;
}

NS_GAF_END
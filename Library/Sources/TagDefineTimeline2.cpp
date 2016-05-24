#include "GAFPrecompiled.h"
#include "TagDefineTimeline2.h"
#include "PrimitiveDeserializer.h"
#include "GAFLoader.h"
#include "GAFStream.h"
#include "GAFAsset.h"
#include "GAFTimeline.h"

#include "json/document.h"
#include "json/stringbuffer.h"
#include "json/prettywriter.h"

NS_GAF_BEGIN

TagDefineTimeline2::TagDefineTimeline2(GAFLoader* loader) : TagDefineTimeline(loader)
{
}

void TagDefineTimeline2::read(GAFStream* in, GAFAsset* asset, GAFTimeline* timeline)
{
    unsigned int id = in->readU32();
    unsigned int framesCount = in->readU32();
    cocos2d::Rect aabb;
    cocos2d::Point pivot;

    PrimitiveDeserializer::deserialize(in, &aabb);
    PrimitiveDeserializer::deserialize(in, &pivot);

    GAFTimeline *tl = new GAFTimeline(timeline, id, aabb, pivot, framesCount);

    //////////////////////////////////////////////////////////////////////////

    std::string temp;
    in->readString(&temp);
    tl->setLinkageName(temp);

    in->readString(&temp);
    tl->setBaseClass(temp);
    
    in->readString(&temp);
    parseCustomProperties(tl, temp);

    m_loader->loadTags(in, asset, tl);

    asset->pushTimeline(id, tl);
    if (id == 0)
    {
        asset->setRootTimeline(0u);
    }
}

void TagDefineTimeline2::parseCustomProperties(GAFTimeline* tl, const std::string& jsonStr)
{
    //JSON // (format: {"cps" /* custom properties */: [{"n" /* name */: "propName1", "vs" /* values */: [val1, val2, val3, ...]}, [{"n": "propName2", "vs": [val1, val2, val3, ...]}, ...]})
    if (jsonStr.empty())
        return;
    
    rapidjson::Document doc;
    doc.Parse<0>(jsonStr.c_str());
    
    const rapidjson::Value& root = doc["cps"];
    assert(root.IsArray());
    
    for (rapidjson::SizeType i = 0; i < root.Size(); ++i)
    {
        const rapidjson::Value& prop_value = root[i];
        assert(prop_value.MemberCount() == 1);
        
        GAFTimeline::CustomProperty custom_prop;
        
        rapidjson::Value::ConstMemberIterator firstMember = prop_value.MemberBegin();
        custom_prop.name = firstMember->name.GetString();
        const rapidjson::Value& firstMemberValue = firstMember->value;
        assert(firstMemberValue.IsArray());
        for (rapidjson::SizeType j = 0; j < firstMemberValue.Size(); ++j)
        {
            if (firstMemberValue[i].IsString())
            {
                std::string value = firstMemberValue[j].GetString();
                custom_prop.possibleValues.push_back(value);
            }
            else
            {
                rapidjson::StringBuffer valueBuffer;
                rapidjson::PrettyWriter<rapidjson::StringBuffer> wr(valueBuffer);
                firstMemberValue[j].Accept(wr);
                custom_prop.possibleValues.push_back(valueBuffer.GetString());
            }
        }
        
        tl->pushCustomProperty(&custom_prop);
    }
}

NS_GAF_END
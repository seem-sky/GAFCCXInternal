#include "GAFPrecompiled.h"
#include "TagDefineAnimationFrames2.h"

#include "GAFStream.h"
#include "GAFAsset.h"
#include "GAFLoader.h"
#include "GAFTimeline.h"

#include "PrimitiveDeserializer.h"

#include "GAFSubobjectState.h"
#include "GAFAnimationFrame.h"
#include "GAFFilterData.h"

NS_GAF_BEGIN

TagDefineAnimationFrames2::TagDefineAnimationFrames2(GAFLoaderPtr loader)
    : m_loader(loader)
{
}

TagDefineAnimationFrames2::~TagDefineAnimationFrames2()
{
}

void TagDefineAnimationFrames2::read(GAFStreamPtr in, GAFAssetPtr asset, GAFTimelinePtr timeline)
{
    (void)asset;
    unsigned int count = in->readU32();

    //assert(!timeline->getAnimationObjects().empty());

    for (AnimationObjects_t::const_iterator i = timeline->getAnimationObjects().begin(), e = timeline->getAnimationObjects().end(); i != e; ++i)
    {
        unsigned int objectId = i->first;
        auto state = ::std::make_shared<GAFSubobjectState>();
        state->initEmpty(objectId);

        m_currentStates[objectId] = state;
    }

    unsigned int frameNumber = in->readU32();

    for (unsigned int i = 0; i < count; ++i)
    {
        char hasChangesInDisplayList = in->readUByte();
        char hasActions = in->readUByte();

        if (hasChangesInDisplayList)
        {
            unsigned int numObjects = in->readU32();

            for (unsigned int j = 0; j < numObjects; ++j)
            {
                GAFSubobjectStatePtr current_state = extractState(in, timeline);
                
                m_currentStates[current_state->objectIdRef] = current_state;
            }
        }

        auto frame = ::std::make_shared<GAFAnimationFrame>();

        for (States_t::iterator it = m_currentStates.begin(), ie = m_currentStates.end(); it != ie; ++it)
        {
            frame->pushObjectState(it->second);
        }

        frame->sortStatesByZIndex();

        if (hasActions)
        {   
            uint32_t actionsCount = in->readU32();
            for (uint32_t actionIdx = 0; actionIdx < actionsCount; actionIdx++)
            {
                GAFTimelineAction action;

                GAFActionType type = static_cast<GAFActionType>(in->readU32());
                std::string scope;
                in->readString(&scope);

                std::vector<std::string> params;

                unsigned int paramsLength = in->readU32();
                unsigned int startPosition = in->getPosition();
                while (paramsLength > in->getPosition() - startPosition)
                {
                    std::string paramValue;
                    in->readString(&paramValue);
                    params.push_back(paramValue);
                }

                action.setAction(type, params, scope);
                frame->pushTimelineAction(action);
            }
        }

        if (in->getPosition() < in->getTagExpectedPosition())
            frameNumber = in->readU32();

        timeline->pushAnimationFrame(frame);
    }

    m_currentStates.clear();
}

GAFSubobjectStatePtr TagDefineAnimationFrames2::extractState(GAFStreamPtr in, GAFTimelinePtr timeline) const
{
    (void)timeline;

    auto state = ::std::make_shared<GAFSubobjectState>();

    float ctx[7];

    char hasColorTransform = in->readUByte();
    char hasMasks = in->readUByte();
    char hasEffect = in->readUByte();

    state->objectIdRef = in->readU32();
    state->zIndex = in->readS32();
    state->colorMults()[GAFCTI_A] = in->readFloat();

    PrimitiveDeserializer::deserialize(in, &state->affineTransform);

    if (hasColorTransform)
    {
        in->readNBytesOfT(ctx, sizeof(float)* 7);

        float* ctxOff = state->colorOffsets();
        float* ctxMul = state->colorMults();

        ctxOff[GAFCTI_A] = ctx[0];

        ctxMul[GAFCTI_R] = ctx[1];
        ctxOff[GAFCTI_R] = ctx[2];

        ctxMul[GAFCTI_G] = ctx[3];
        ctxOff[GAFCTI_G] = ctx[4];

        ctxMul[GAFCTI_B] = ctx[5];
        ctxOff[GAFCTI_B] = ctx[6];
    }
    else
    {
        state->ctxMakeIdentity();
    }

    if (hasEffect)
    {
        unsigned char effects = in->readUByte();

        for (unsigned int e = 0; e < effects; ++e)
        {
            GAFFilterType type = static_cast<GAFFilterType>(in->readU32());

            if (type == GAFFilterType::Blur)
            {
                cocos2d::Size p;
                PrimitiveDeserializer::deserialize(in, &p);
                auto blurFilter = ::std::make_shared<GAFBlurFilterData>();
                blurFilter->blurSize = p;
                state->pushFilter(blurFilter);
            }
            else if (type == GAFFilterType::ColorMatrix)
            {
                auto colorFilter = ::std::make_shared<GAFColorMatrixFilterData>();
                for (unsigned int i = 0; i < 4; ++i)
                {
                    for (unsigned int j = 0; j < 4; ++j)
                    {
                        colorFilter->matrix[j * 4 + i] = in->readFloat();
                    }

                    colorFilter->matrix2[i] = in->readFloat() / 255.f;
                }

                state->pushFilter(colorFilter);
            }
            else if (type == GAFFilterType::Glow)
            {
                auto filter = ::std::make_shared<GAFGlowFilterData>();
                unsigned int clr = in->readU32();

                PrimitiveDeserializer::translateColor(filter->color, clr);
                filter->color.a = 1.f;

                PrimitiveDeserializer::deserialize(in, &filter->blurSize);

                filter->strength = in->readFloat();
                filter->innerGlow = in->readUByte() ? true : false;
                filter->knockout = in->readUByte() ? true : false;

                state->pushFilter(filter);
            }
            else if (type == GAFFilterType::DropShadow)
            {
                auto filter = ::std::make_shared<GAFDropShadowFilterData>();

                unsigned int clr = in->readU32();

                PrimitiveDeserializer::translateColor(filter->color, clr);
                filter->color.a = 1.f;

                PrimitiveDeserializer::deserialize(in, &filter->blurSize);
                filter->angle = in->readFloat();
                filter->distance = in->readFloat();
                filter->strength = in->readFloat();
                filter->innerShadow = in->readUByte() ? true : false;
                filter->knockout = in->readUByte() ? true : false;

                state->pushFilter(filter);
            }
        }
    }

    if (hasMasks)
    {
        state->maskObjectIdRef = in->readU32();
    }

    return state;
}

NS_GAF_END

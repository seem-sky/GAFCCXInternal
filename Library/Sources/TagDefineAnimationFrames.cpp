#include "GAFPrecompiled.h"
#include "TagDefineAnimationFrames.h"

#include "GAFStream.h"
#include "GAFAsset.h"
#include "GAFFile.h"
#include "GAFHeader.h"

#include "PrimitiveDeserializer.h"

#include "GAFSubobjectState.h"
#include "GAFAnimationFrame.h"
#include "GAFFilterData.h"

NS_GAF_BEGIN

TagDefineAnimationFrames::~TagDefineAnimationFrames()
{
}

void TagDefineAnimationFrames::read(GAFStreamPtr in, GAFAssetPtr asset, GAFTimelinePtr timeline)
{
    (void)asset;
    in->readU32(); // read count. Unused here

    if (timeline->getAnimationObjects().empty()) return;

    for (AnimationObjects_t::const_iterator i = timeline->getAnimationObjects().cbegin(), e = timeline->getAnimationObjects().cend(); i != e; ++i)
    {
        unsigned int objectId = i->first;
        auto state = std::make_shared<GAFSubobjectState>();
        state->initEmpty(objectId);

        m_currentStates[objectId] = state;
    }

    const unsigned short totalFrameCount = in->getInput()->getHeader().framesCount;

    unsigned int frameNumber = in->readU32();

    for (unsigned int i = 0; i < totalFrameCount; ++i)
    {
        if ((frameNumber - 1) == i)
        {
            unsigned int numObjects = in->readU32();

            using StatesList_t = std::list<GAFSubobjectStatePtr>;
            StatesList_t statesList;

            for (unsigned int j = 0; j < numObjects; ++j)
            {
                GAFSubobjectStatePtr state = extractState(in);

                statesList.push_back(state);
            }

            for (StatesList_t::iterator it = statesList.begin(), ie = statesList.end(); it != ie; ++it)
            {
                GAFSubobjectStatePtr st = *it;

                m_currentStates[st->objectIdRef] = st;
            }

            if (in->getPosition() < in->getTagExpectedPosition())
                frameNumber = in->readU32();
        }

        GAFAnimationFramePtr frame = std::make_shared<GAFAnimationFrame>();

        for (States_t::const_iterator it = m_currentStates.cbegin(), ie = m_currentStates.cend(); it != ie; ++it)
        {
            frame->pushObjectState(it->second);
        }

        frame->sortStatesByZIndex();

        timeline->pushAnimationFrame(frame);
    }
}

GAFSubobjectStateUPtr TagDefineAnimationFrames::extractState(GAFStreamPtr in) const
{
    auto state = std::make_unique<GAFSubobjectState>();

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
        in->readNBytesOfT(ctx, sizeof(float) * 7);

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
                auto filter = std::make_shared<GAFBlurFilterData>();
                filter->blurSize = p;
                state->pushFilter(filter);
            }
            else if (type == GAFFilterType::ColorMatrix)
            {
                auto filter = std::make_shared<GAFColorMatrixFilterData>();
                for (auto i = 0; i < 4; ++i)
                {
                    for (auto j = 0; j < 4; ++j)
                    {
                        filter->matrix[j * 4 + i] = in->readFloat();
                    }

                    filter->matrix2[i] = in->readFloat() / 255.f;
                }

                state->pushFilter(filter);
            }
            else if (type == GAFFilterType::Glow)
            {
                auto filter = std::make_shared<GAFGlowFilterData>();
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
                auto filter = std::make_shared<GAFDropShadowFilterData>();
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

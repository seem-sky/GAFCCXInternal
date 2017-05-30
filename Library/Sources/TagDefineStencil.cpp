#include "GAFPrecompiled.h"
#include "TagDefineStencil.h"
#include "GAFStream.h"
#include "GAFTimeline.h"

void gaf::TagDefineStencil::read(GAFStreamPtr in, GAFAssetPtr, GAFTimelinePtr timeline)
{
    auto stencil = std::make_shared<GAFStencil>();
    stencil->setScale(in->readFloat());
    stencil->setWidth(in->readU32());
    stencil->setHeight(in->readU32());

    const size_t bitsetBytesNum = static_cast<size_t>(std::ceilf(stencil->getWidth() * stencil->getHeight() / 8.f));
    for (size_t i = 0; i < bitsetBytesNum; ++i)
    {
        stencil->setBits(i << 3, in->readUByte());
    }

    timeline->setStencil(stencil);
}

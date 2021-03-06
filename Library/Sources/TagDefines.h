#pragma once

NS_GAF_BEGIN

struct Tags
{
    enum Enum
    {
        TagEnd = 0,
        TagDefineAtlas = 1,
        TagDefineAnimationMasks = 2,
        TagDefineAnimationObjects = 3,
        TagDefineAnimationFrames = 4,
        TagDefineNamedParts = 5,
        TagDefineSequences = 6,
        TagDefineTextFields = 7,
        TagDefineAtlas2 = 8,
        TagDefineStage = 9,
        TagDefineAnimationObjects2 = 10,
        TagDefineAnimationMasks2 = 11,
        TagDefineAnimationFrames2 = 12,
        TagDefineTimeline = 13,
        TagDefineSounds = 14,
        TagDefineAtlas3 = 15,
        TagDefineAtlas4 = 16,
        TagDefineTimeline2 = 17,
        TagDefineExternalObjects = 18,
        TagDefineAnimationFrames3 = 19,
        TagDefineTimeline3 = 20,
        TagDefineExternalObjects2 = 21,
        TagDefineStencil = 22,
    };

    static std::string toString(Enum e)
    {
        switch (e)
        {
        case Tags::TagEnd:
            return "TagEnd";

        case Tags::TagDefineAtlas:
            return "TagDefineAtlas";

        case Tags::TagDefineAnimationMasks:
            return "TagDefineAnimationMasks";

        case Tags::TagDefineAnimationObjects:
            return "TagDefineAnimationObjects";

        case Tags::TagDefineAnimationFrames:
            return "TagDefineAnimationFrames";

        case Tags::TagDefineNamedParts:
            return "TagDefineNamedParts";

        case Tags::TagDefineSequences:
            return "TagDefineSequences";

        case Tags::TagDefineStage:
            return "TagDefineStage";

        case Tags::TagDefineAtlas2:
            return "TagDefineAtlas2";

        case Tags::TagDefineTimeline:
            return "TagDefineTimeline";

        case Tags::TagDefineTextFields:
            return "TagDefineTextFields";

        case Tags::TagDefineAnimationMasks2:
            return "TagDefineAnimationMasks2";

        case Tags::TagDefineAnimationObjects2:
            return "TagDefineAnimationObjects2";

        case Tags::TagDefineAnimationFrames2:
            return "TagDefineAnimationFrames2";

        case Tags::TagDefineSounds:
            return "TagDefineSounds";

        case Tags::TagDefineAtlas3:
            return "TagDefineAtlas3";

        case Tags::TagDefineAtlas4:
            return "TagDefineAtlas4";

        case Tags::TagDefineTimeline2:
            return "TagDefineTimeline2";

        case Tags::TagDefineExternalObjects:
            return "TagDefineExternalObjects";
                
        case Tags::TagDefineAnimationFrames3:
            return "TagDefineAnimationFrames3";
                
        case Tags::TagDefineTimeline3:
            return "TagDefineTimeline3";

        case Tags::TagDefineExternalObjects2:
            return "TagDefineExternalObjects2";

        case Tags::TagDefineStencil:
            return "TagDefineStencil";

        default:
            return "This tag is not yet implemented";
        }
    }
};

NS_GAF_END
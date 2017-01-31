#pragma once

#include "TagDefines.h"

NS_GAF_BEGIN

gaf_fwd_this(GAFStream);
gaf_fwd_this(GAFFile);

class GAFStream
{
    friend class GAFLoader;
private:
    GAFFilePtr          m_input;
    unsigned char       m_currentByte = 0;
    unsigned char       m_unusedBits = 0;

    struct TagRecord
    {
        unsigned int expectedStreamPos;
        unsigned int tagSize;
        Tags::Enum tagType;
    };

    using TagStack_t = std::stack<TagRecord>;
    TagStack_t          m_tagStack;

public:
    explicit GAFStream(GAFFilePtr input);
    ~GAFStream();

    void                 readNBytesOfT(void* dest, unsigned int);
    bool                 readBool();
    unsigned int         readUint(unsigned short bitcount);
    int                  readSint(unsigned short bitcount);
    float                readFixed();
    float                readFixed8();

    float                readFloat();

    unsigned char        readUByte();
    char                 readSByte();

    unsigned char        readU8() { return readUByte(); }
    char                 readS8() { return readSByte(); }
    unsigned short       readU16();
    unsigned int         readU32();
    int                  readS32();

    void                 readString(std::string* out);

    GAFFileConstPtr      getInput() const;

    Tags::Enum           openTag();
    void                 closeTag();
    unsigned int         getTagLenghtOnStackTop() const;
    unsigned int         getTagExpectedPosition() const;
    unsigned int         getPosition() const;

    void                 align();

    bool                 isEndOfStream() const;
};

NS_GAF_END
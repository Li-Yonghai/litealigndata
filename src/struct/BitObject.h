#ifndef _BITOBJECT_H_
#define _BITOBJECT_H_
#include "struct/ColumnObject.h"

class  CBitObject : public CColumnObject
{
    public:
        CBitObject(litealigndata::elementTag tag);
        ~CBitObject();
    
    private:
        uint8_t m_bit_offset;  //xml 需加加这个字段在标签bit内


};

class  CSubBitObject : public CColumnObject
{
    public:
        CSubBitObject(litealigndata::elementTag tag);
        ~CSubBitObject();


};

#endif /* _BITOBJECT_H_ */
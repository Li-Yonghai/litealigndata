#ifndef _BASECASEFUN_H
#define _BASECASEFUN_H
#include <iostream>
#include "struct/ColumnObject.h"
#include "file/BaseFileFormat.h"
#include "file/Ctrldata/Ctrldata.h"
#include "file/Example/Example.h"

namespace testcase{

namespace CaseBaseCase{

class CBaseCaseFun
{
    public:
        CBaseCaseFun(){};
        virtual ~CBaseCaseFun(){};
        virtual bool MethodFun(std::map<std::string, litealigndata::autoType>& item, 
        CColumnObject* release_column_obj, std::string dirname, CBaseFileFormat *fileFormat);
        CAlignDataTypeDef m_AlignDataTypeDef;

};

};

}



#endif
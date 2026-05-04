#ifndef _BASECASEFUNACCULATE_H
#define _BASECASEFUNACCULATE_H

#include "BaseCaseFun.h"

namespace testcase{

namespace CaseBaseCase{

class CBaseCaseFunAcculate : public CBaseCaseFun
{
    public:
        CBaseCaseFunAcculate(){};
        virtual ~CBaseCaseFunAcculate(){};
        virtual bool MethodFun(std::map<std::string, litealigndata::autoType>& item, 
        CColumnObject* release_column_obj, std::string dirname, CBaseFileFormat *fileFormat);


};

};

}

#endif
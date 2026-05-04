#ifndef _BASECASEFUNMerge_H
#define _BASECASEFUNMerge_H

#include "BaseCaseFun.h"

namespace testcase{

namespace CaseBaseCase{

class CBaseCaseFunMerge : public CBaseCaseFun
{
    public:
        CBaseCaseFunMerge(){};
        virtual ~CBaseCaseFunMerge(){};
        virtual bool MethodFun(std::map<std::string, litealigndata::autoType>& item, 
        CColumnObject* release_column_obj, std::string dirname, CBaseFileFormat *fileFormat);


};

};

}



#endif
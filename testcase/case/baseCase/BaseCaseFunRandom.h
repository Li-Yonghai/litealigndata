#ifndef _BASECASEFUNRANDOM_H
#define _BASECASEFUNRANDOM_H

#include "BaseCaseFun.h"

namespace testcase{

namespace CaseBaseCase{

class CBaseCaseFunRandom : public CBaseCaseFun
{
    public:
        CBaseCaseFunRandom(){};
        virtual ~CBaseCaseFunRandom(){};
        virtual bool MethodFun(std::map<std::string, litealigndata::autoType>& item, 
        CColumnObject* release_column_obj, std::string dirname, CBaseFileFormat *fileFormat);


};

};

}



#endif
/*
 * =============================================================================
 *
 *       Filename:  CBaseCaseFunAcculate.cc
 *
 *       Description: 
 *
 *       Version:  1.0
 *       Created:  04/01, 2024
 *       Revision:  V1
 *       Compiler:  g++
 *
 *       Author:  keen.lee 
 *       Company
 *
 * =============================================================================
 */
#include "BaseCase.h"

using namespace testcase::CaseBaseCase;

bool CBaseCaseFunAcculate::MethodFun(std::map<std::string, litealigndata::autoType>& item, 
        CColumnObject* release_column_obj, std::string dirname, CBaseFileFormat *fileFormat)
{
    AcculateFunc::AcculateType type = ((AcculateFunc*)(release_column_obj->getFunc()))->getType();
    if(type == AcculateFunc::InvalidAcculate)
    {
        for(auto it = item.begin(); it != std::prev(item.end()); it++)
        {
            uint64_t value = atoi((it++)->first.c_str());
            uint64_t nextvalue = atoi((it--)->first.c_str());
            //printf("value:[%ld], nextvalue:[%ld]\n", value, nextvalue);
            if( (value > MAX_UINT64) || (nextvalue > MAX_UINT64) ||
                value < ((AcculateFunc*)(release_column_obj->getFunc()))->getinit() ||
                nextvalue < ((AcculateFunc*)(release_column_obj->getFunc()))->getinit() ||
                ((uint32_t)(nextvalue - value) != ((AcculateFunc*)(release_column_obj->getFunc()))->getstep() )
               )
                return false;
        }  
    }
    return true;
}
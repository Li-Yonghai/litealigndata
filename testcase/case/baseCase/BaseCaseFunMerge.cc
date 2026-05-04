/*
 * =============================================================================
 *
 *       Filename:  BaseCaseFunMerge.cc
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
#include "struct/ColumnObject.h"

using namespace testcase::CaseBaseCase;

bool CBaseCaseFunMerge::MethodFun(std::map<std::string, litealigndata::autoType>& item, 
                CColumnObject* release_column_obj, std::string dirname, CBaseFileFormat *fileFormat)
{
    litealigndata::autoType type = 
    (((CBaseFunc*)(release_column_obj->getFunc()))->getValue()).getType();
    if( type == litealigndata::AUTO_UINT64 )
    {
        uint8_t leftbit = 0;
        uint64_t value = 0;
        uint64_t subfuncvalue;
        CColumnObject::litealigndataObjlist sublist = release_column_obj->getSubColumn();
        for ( auto it = sublist.begin(); it != sublist.end(); ++it )
        {
            subColumnObject* subColumn = dynamic_cast<subColumnObject*>( *it );
            CColumnObject::LitealigndataObjectList& paraList = subColumn->getLitealigndataObjectList();
            for(auto plist = paraList.begin(); plist != paraList.end(); plist++)
            {
                std::string parameterName = ( *plist )->getObjectName();
                litealigndata::parameterTag tag = 
                ( litealigndata::parameterTag ) m_AlignDataTypeDef.FindRelativeTag( 
                    litealigndata::ParaTypeString, litealigndata::PARA_MAX, parameterName.c_str() );
                std::string ParameterValue = ( *plist )->getStrValue();
                switch ( tag )
                {
                    case litealigndata::PARA_Value:
                    subfuncvalue = (uint64_t)atol(ParameterValue.c_str());
                    value |= ( subfuncvalue << leftbit );
                    leftbit += subColumn->getBitlen();
                    break;
                    default:
                    break;
                }
            }
        }
        //printf("====value===========%lx\n", value);
        for(auto it = item.begin(); it != item.end(); it++)
        {
            //printf("====value=====value======%lx\n", atol(it->first.c_str()));
            if(value != (uint64_t)atol(it->first.c_str()))
                return false;
        }
    }
    else
    {

    }
    return true;
    
}
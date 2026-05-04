/*
 * =============================================================================
 *
 *       Filename:  BaseCaseFunRandom.cc
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

bool CBaseCaseFunRandom::MethodFun(std::map<std::string, litealigndata::autoType>& item, 
            CColumnObject* release_column_obj, std::string dirname, CBaseFileFormat *fileFormat)
{
    //printf("%s\n", release_column_obj->getObjectName().c_str());
    RandomFunc::RandomType type = ((RandomFunc*)(release_column_obj->getFunc()))->getType();
    if (type == RandomFunc::DefalutRandom)
    {
        RandomFunc::strlist defaultValues = 
        ((RandomFunc*)(release_column_obj->getFunc()))->getdefaultValues();
        // for(auto it : defaultValues)
        // {
        //     printf("--defaultValues-[%s]\n", it.c_str());
        // }
        // printf("item.size()====>[%lu]\n", item.size());
        for(auto it = item.begin(); it != item.end(); it++)
        {
            //printf("==actual value=[%s]\n", it->first.c_str());
            if(std::find(defaultValues.begin(), defaultValues.end(), it->first) == 
                    defaultValues.end())
                    return false;
        }
    }
    else if(type == RandomFunc::NatureRandom || 
    type == RandomFunc::PseudoRandom)
    {
            //printf("                 %ld\n", item.size());
        for(auto it = item.begin(); it != item.end(); it++)
        {
                //printf("%ld\n", (uint64_t )(atoi(it->first.c_str())));
            if((uint64_t )(atoi(it->first.c_str())) < (((RandomFunc*)(release_column_obj->getFunc()))->getlowRange()) || 
            (uint64_t )(atoi(it->first.c_str())) > (((RandomFunc*)(release_column_obj->getFunc()))->gethighRange()))
                return false;
        }
    }
    else if(type == RandomFunc::FileRandom)
    {
        std::string file;
        std::list<CAlignDataObject* >&  parameterlist = 
        ((RandomFunc*)(release_column_obj->getFunc()))->getparameterlist();
        auto it = parameterlist.begin();
        while ( it != parameterlist.end() )
        {
            std::string parameterName = ( *it )->getObjectName();
            litealigndata::parameterTag tag = 
            ( litealigndata::parameterTag ) m_AlignDataTypeDef.FindRelativeTag( litealigndata::ParaTypeString, litealigndata::PARA_MAX, parameterName.c_str() );
            if(tag == litealigndata::PARA_Value)
            {
                uint32_t end = (( *it )->getStrValue()).find("[");
                file = (( *it )->getStrValue()).substr(0, end);
                file = dirname + "/" + file + ".csv";
                //printf("======%s\n", file.c_str());
                break;
            }
            it++;
        }
        std::vector<std::string> Lines;
        fileFormat->GetAllLines(Lines, file);
        //  for(auto it : Lines)     
        // {
        //      printf("=======GetAllLines=======%s\n", (it).c_str());
        // }
        std::string str;
        for(auto it = item.begin(); it != item.end(); it++)
        {
            str = std::string("\"") + it->first.c_str() + std::string("\"");
            // printf("==============%s\n", it->first.c_str());
            // printf("=======str  %s=======\n", str.c_str());
            auto iter = std::find(Lines.begin(), Lines.end(), str);
            if(iter == Lines.end())
                return false;
        }
    }
    else if (type == RandomFunc::TableRandom)
    {
        //printf("type == RandomFunc::TableRandom\n");

        std::list<CAlignDataObject* >&  parameterlist = 
        release_column_obj->getLitealigndataObjectList();
        auto it = parameterlist.begin();
        
        std::string strTableName;
        std::string strColName;
        while ( it != parameterlist.end() )
        {
            std::string parameterName = ( *it )->getObjectName();
            litealigndata::parameterTag tag = 
            ( litealigndata::parameterTag ) m_AlignDataTypeDef.FindRelativeTag( 
                litealigndata::ParaTypeString, litealigndata::PARA_MAX, parameterName.c_str() );
            if(tag == litealigndata::PARA_Value)
            {
                std::string strValue = (( *it )->getStrValue());
                uint32_t RowNumber = 0;
                m_AlignDataTypeDef.ParserTableInfoFromString( strValue.c_str(), strValue.length(),
                                   strTableName, RowNumber, strColName );
                break;
            }
            it++;
        }
        //printf("%s,  %s\n", strTableName.c_str(), strColName.c_str());
        
        CInterfaceObj *interface = (CInterfaceObj *)(release_column_obj->getTable()->getParent());
        CInterfaceObj::GetNetObjectList interfaceTables  = interface->getMapLitealigndataObject();

        CTableObject* pObject = NULL;
        for ( auto it = interfaceTables.begin(); it != interfaceTables.end(); ++it)
        {
            //printf("it->first    %s\n", it->first.c_str());
            if(it->first == strTableName)
            {
                pObject = static_cast<CTableObject*>( it->second );
            }
        }
        //printf("CTableObject[%s]\n", strTableName.c_str());
        //printf("          %s\n", pObject->getObjectName().c_str());
        //uint16_t colIdx = release_column_obj->getColidx()-1;
        CColumnObject* dep_column_obj = NULL;
	    CAlignDataObject* Body = pObject->findChildObject("Body");
        CAlignDataObject* BodyData =  Body->findChildObject("BodyData");
	    std::list<CAlignDataObject*> obj = (BodyData)->getLitealigndataObjectList();
        for ( listInterface::iterator  it = obj.begin(); it != obj.end(); ++(it) )
        {
            dep_column_obj = static_cast<CColumnObject*>(*it);
            if(strColName == dep_column_obj->getObjectName())
            {
                break;
            }

        }
        //printf("dep_column_obj  %s\n", strColName.c_str());
        RandomFunc::strlist Dep_defaultValues;
        RandomFunc::RandomType type = ((RandomFunc*)(dep_column_obj->getFunc()))->getType();
        if (type == RandomFunc::DefalutRandom)
        {
            Dep_defaultValues = 
            ((RandomFunc*)(dep_column_obj->getFunc()))->getdefaultValues();
        }
        
        // for(auto it = Dep_defaultValues.begin(); it != Dep_defaultValues.end(); it++)
        // {
        //     printf("Dep_defaultValues[%s]\n", (*it).c_str());
        // }
        for(auto it = item.begin(); it != item.end(); it++)
        {
            //printf("table-item[%s]\n", it->first.c_str());
            if(std::find(Dep_defaultValues.begin(), Dep_defaultValues.end(), it->first) == 
                    Dep_defaultValues.end())
                    return false;
        }

    }
    return true;
}
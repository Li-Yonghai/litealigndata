/*
 * =============================================================================
 *
 *       Filename:  FunctionObject.cc
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
#include "fun/FunctionObject.h"

bool FunctionObject::Initialize( DOMNode* pNode, CAlignDataObject* pParent )
{
    if ( ! CAlignDataObject::Initialize( pNode, pParent ) )
        return false;

    std::string attrName;

    if ( !findAttriValue( litealigndata::ATTR_GenFuncName, attrName ) )
    {
        ACE_DEBUG((LM_ERROR, "the FunctionObject name: %s can not find the GenFuncName.\n", 
                   this->m_ObjectName.c_str()));
        return false;
    }

    litealigndata::funcTag tag = ( litealigndata::funcTag ) m_AlignDataTypeDef.FindRelativeTag( litealigndata::funcTagString, litealigndata::FUNC_MAX,
                  attrName.c_str() );

    if ( tag == litealigndata::FUNC_Invalid )
    {
        ACE_DEBUG((LM_ERROR, "can not find Valid FuncTag for funcName: %s.\n", attrName.c_str()));
        return false;
    }

    m_pFunc = FuncFactory::Instance().createBaseFuncObject( tag );

    if ( m_pFunc == NULL )
    {
        ACE_DEBUG((LM_ERROR, "can not create the Func instance: %s in the FunctionObject::Initialize.\n",
                   litealigndata::funcTagString[tag]));
        return false;
    }

    m_pFunc->setColumn( this );

    return m_pFunc->Initialize();

}

bool FunctionObject::processOjbect( CAlignDataObject* root )
{
    return getFunc()->process();
}

bool RedefineRowFunc::process()
{

    ACE_DEBUG((LM_DEBUG, "start process data by RedefineRowFunc for FunctionObject: %s.\n", 
               m_pColumn->getObjectName().c_str()));

    if ( !CBaseFunc::process() )
        return false;

    alignDataList::iterator it = m_parameterlist.begin();

    while ( it != m_parameterlist.end() )
    {
        CParameterObject* para = ( ( CParameterObject* )( *it ) );

        litealigndata::parameterTag tag = para->getTag();

        std::string strParaValue = para->getStrValue();

        if ( strParaValue.empty() )
        {
            ACE_DEBUG((LM_ERROR, "he function name: %s 's parameter is empty.\n",
                       getColumn()->getObjectName().c_str(), para->getObjectName().c_str()));
            return false;
        }

        switch ( tag )
        {
        case litealigndata::PARA_SourceType:
        {
            m_sourcetype = ( RandomFunc::RandomType ) m_AlignDataTypeDef.FindRelativeTag( RandomFunc::RandomName, RandomFunc::RandomMax,
                           strParaValue.c_str() );

            if ( m_sourcetype == RandomFunc::InvalidRandom )
            {
                ACE_DEBUG((LM_ERROR, "the SourceType: %s do not find valid RandomType for FunctionName: %s.\n",
                           strParaValue.c_str(), getColumn()->getObjectName().c_str()));
                return false;
            }

            break;
        }

        case litealigndata::PARA_TotNumber:
        {
            m_iTotoalNum = strtoull( strParaValue.c_str(), NULL, 10 );
            m_leftRecordsnumber = m_iTotoalNum;
            break;
        }

        case litealigndata::PARA_FileNumber:
        {
            m_filenumber = strtoull( strParaValue.c_str(), NULL, 10 );
            break;
        }

        case litealigndata::PARA_Percent:
        {
            if ( m_sourcetype == RandomFunc::DefalutRandom )
            {
                strArray strlist;
                m_AlignDataTypeDef.splitValue( strParaValue.c_str(), strlist );

                strArray::const_iterator arrayIt;

                for ( arrayIt = strlist.begin(); arrayIt != strlist.end(); ++arrayIt )
                {
                    m_listPercent.push_back( strtoull( arrayIt->c_str(), NULL, 10 ) );
                }
            }
            else if ( m_sourcetype == RandomFunc::FileRandom )
            {
                CAlignDataObject* pDimensionFile = NULL;
                CAlignDataObject* pDimensionColumn = NULL;

                if ( !getColumn()->getParser()->anyzeDepDimensionInfo( para->getObjectName(), para->getStrValue(),
                        pDimensionFile, pDimensionColumn ) )
                {
                    return false;
                }

                if ( pDimensionFile != NULL && pDimensionColumn != NULL )
                {
                    getColumn()->getParser()->addDimensionInfo( pDimensionFile, pDimensionColumn );  //for loading DimensionData.
                    //storage DimensionInfo.
                    DependencyInfo info;
                    info.m_pTableInfo = pDimensionFile;
                    info.m_pColumnInfo = pDimensionColumn;
                    m_DepDimensionList.push_back( info );
                }
            }
        }

        default:
            break;
        }

        ++it;
    }

    return true;
}

void RedefineRowFunc::CalulateRowbaseOnPercent( uint64_t percent )
{
    uint32_t currentNumber = double ( percent ) / 10000 * m_iTotoalNum;

    if ( m_leftRecordsnumber >= currentNumber )
    {
        getValue().setNumber( currentNumber );
        m_leftRecordsnumber -= currentNumber;
    }
    else
    {
        getValue().setNumber( m_leftRecordsnumber );
        m_leftRecordsnumber = 0;
    }
}

void RedefineRowFunc::CalculateRowNumber( const CXmlParser::DimensionfileDatas* pDimensionfileData,
        DimensionFileColumn* pColumn )
{
    uint32_t MaxRowNumber = pDimensionfileData->size();
    uint32_t colIdx = pColumn->getMappingidx() - 1;

    if ( MaxRowNumber >= m_filenumber )
    {
        const CXmlParser::Dimensioncolvalues* pRowdata = ( *pDimensionfileData )[m_currentFileNum - 1];
        const baseType* pValue = ( *pRowdata )[colIdx];
        uint64_t percent = ( ( numberType* )( pValue ) )->getValue();
        CalulateRowbaseOnPercent( percent );
    }
    else
    {
        if ( m_currentFileNum <= MaxRowNumber )
        {
            const CXmlParser::Dimensioncolvalues* pRowdata = ( *pDimensionfileData )[m_currentFileNum - 1];
            const baseType* pValue = ( *pRowdata )[colIdx];
            uint64_t percent = ( ( numberType* )( pValue ) )->getValue();

            CalulateRowbaseOnPercent( percent );
        }
        else
        {
            CalculateOutofRownumber();
        }
    }
}



void RedefineRowFunc::calculateRowFordefault()
{
    uint32_t MaxRowNumber = m_listPercent.size();

    if ( MaxRowNumber >= m_filenumber )
    {
        uint64_t percent = m_listPercent[m_currentFileNum - 1];
        CalulateRowbaseOnPercent( percent );
    }
    else
    {
        if ( m_currentFileNum <= MaxRowNumber )
        {
            uint64_t percent = m_listPercent[m_currentFileNum - 1];
            CalulateRowbaseOnPercent( percent );
        }
        else
        {
            CalculateOutofRownumber();
        }
    }
}

void RedefineRowFunc::CalculateOutofRownumber()
{
    if ( m_leftRecordsnumber <= 0 )
    {
        getValue().setNumber( 0 );
    }
    else
    {
        uint32_t leftFileNumber = m_filenumber - m_currentFileNum + 1;
        uint32_t currentFileNumber = m_leftRecordsnumber / leftFileNumber;
        m_leftRecordsnumber -= currentFileNumber;
        getValue().setNumber( currentFileNumber );
    }
}



bool RedefineRowFunc::Calculate()
{

    ACE_DEBUG((LM_DEBUG, "start Calculate data by RedefineRowFunc for FunctionObject: %s.\n", 
               m_pColumn->getObjectName().c_str()));

    if ( !CBaseFunc::Calculate() )
        return false;

    m_currentFileNum++;

    if ( m_currentFileNum > m_filenumber )
    {
        getValue().setNumber( 0 );
        return true;
    }

    if ( m_sourcetype == RandomFunc::FileRandom )
    {
        if ( m_DepDimensionList.empty() )
            return false;

        RandomFunc::dependencylist::const_iterator it = m_DepDimensionList.begin();

        DimensionalFile* pTable = ( DimensionalFile* ) it->m_pTableInfo;

        DimensionFileColumn* pColumn = ( DimensionFileColumn* ) it->m_pColumnInfo;

        if ( pTable == NULL || pColumn == NULL )
        {
            ACE_DEBUG((LM_ERROR, "there is no depended dimensionFile and columns for the function %s.\n",
                       getColumn()->getObjectName().c_str()));
            return false;
        }

        CXmlParser* parser = getColumn()->getParser();

        const CXmlParser::DimensionfileDatas* pDimensionfileData = parser->findDimensionFileDatas( pTable->getObjectName() );

        if ( pDimensionfileData == NULL || pDimensionfileData->empty() )
        {
            ACE_DEBUG((LM_ERROR, "can not find the the proper dimension Data (%s) for function %s in the RedefineRowFunc::Caculate().\n",
                       pTable->getObjectName().c_str(), getColumn()->getObjectName().c_str()));
            return false;
        }

        CalculateRowNumber( pDimensionfileData, pColumn );
    }
    else if ( m_sourcetype == RandomFunc::DefalutRandom )
    {
        calculateRowFordefault();
    }

    ACE_DEBUG((LM_DEBUG, "the result is %Q. Caculated by RedefineRowFunc for FunctionObject: %s.\n", 
               getValue().getnumber(), m_pColumn->getObjectName().c_str()));

    return true;
}
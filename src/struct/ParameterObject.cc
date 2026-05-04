/*
 * =============================================================================
 *
 *       Filename:  CParameterObject.cc
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
#include <iostream>
#include "fun/BaseFuncClass.h"
#include "fun/FunctionObject.h"
#include "struct/ColumnObject.h"
#include "struct/ParameterObject.h"

CParameterObject::CParameterObject( litealigndata::elementTag tag ): CAlignDataObject( tag ), m_pTable( NULL ), m_pFunc( NULL )
{

}

CParameterObject::CParameterObject( const CParameterObject& rhs ): CAlignDataObject( rhs )
{

}

CParameterObject& CParameterObject::operator=( const CParameterObject& rhs )
{
    if ( &rhs == this )
        return *this;

    CAlignDataObject::operator= (rhs);

    return *this;
}

void CParameterObject::Initialize()
{
    CAlignDataObject::Initialize();
    
    CAlignDataObject* parents = m_pParent;
    
    while ( parents != NULL )
    {
        if ( parents->getElementType() == litealigndata::ELEMENT_Table )
        {
            m_pTable = static_cast<CTableObject*>( parents );
            break;
        }
    
        parents = parents->getParent();
    }

    litealigndata::elementTag eletype = this->m_pParent->getElementType();

    CBaseFunc* func = NULL;

    if ( eletype == litealigndata::ELEMENT_Column )
    {
        CColumnObject* pColumn = static_cast<CColumnObject* >( m_pParent );
        func = pColumn->getFunc();
    }
    else if ( eletype == litealigndata::ELEMENT_SubColumn )
    {
        subColumnObject* pColumn = static_cast<subColumnObject* >( m_pParent );
        func = pColumn->getFunc();
    }
    else if ( eletype == litealigndata::ELEMENT_Function )
    {
        FunctionObject* pFunction = static_cast<FunctionObject* >( m_pParent );
        func = pFunction->getFunc();
    }

    if ( func == NULL )
    {
        ACE_DEBUG((LM_ERROR, "Current CParameterObject %s get the func is NULL.\n", this->m_ObjectName.c_str()));
        return;
    }

    func->addParameter( this );

    m_pFunc = func;    
}

bool CParameterObject::Initialize( DOMNode* pNode, CAlignDataObject* pParent )
{
    if ( !CAlignDataObject::Initialize( pNode, pParent ) )
        return false;

    CAlignDataObject* parents = m_pParent;

    while ( parents != NULL )
    {
        if ( parents->getElementType() == litealigndata::ELEMENT_Table )
        {
            m_pTable = static_cast<CTableObject*>( parents );
            break;
        }

        parents = parents->getParent();
    }

    if ( m_pParent->getElementType() != litealigndata::ELEMENT_Function && m_pTable == NULL )
    {
        ACE_DEBUG((LM_ERROR, "do not find the Parent Table element for current parameter = %s\n", 
            this->m_ObjectName.c_str()));
        return false;
    }

    m_tag = ( litealigndata::parameterTag ) m_AlignDataTypeDef.FindRelativeTag( litealigndata::ParaTypeString, litealigndata::PARA_MAX,
            getObjectName().c_str() );

    if ( m_tag == litealigndata::PARA_Invalid )
    {
        ACE_DEBUG((LM_ERROR, "the parameter: %s do not be defined.\n", getObjectName().c_str()));
        return false;
    }

    litealigndata::elementTag eletype = this->m_pParent->getElementType();

    CBaseFunc* func = NULL;

    if ( eletype == litealigndata::ELEMENT_Column )
    {
        CColumnObject* pColumn = static_cast<CColumnObject* >( m_pParent );
        func = pColumn->getFunc();
    }
    else if ( eletype == litealigndata::ELEMENT_SubColumn )
    {
        subColumnObject* pColumn = static_cast<subColumnObject* >( m_pParent );
        func = pColumn->getFunc();
    }
    else if ( eletype == litealigndata::ELEMENT_Function )
    {
        FunctionObject* pFunction = static_cast<FunctionObject* >( m_pParent );
        func = pFunction->getFunc();
    }

    if ( func == NULL )
    {
        ACE_DEBUG((LM_ERROR, "Current CParameterObject %s get the func is NULL.\n", this->m_ObjectName.c_str()));
        return false;
    }

    func->addParameter( this );

    m_pFunc = func;
    return true;

}

bool CParameterObject::processOjbect( CAlignDataObject* pRoot )
{
    if ( !m_pParent )
    {
        ACE_DEBUG((LM_ERROR, "Current CParameterObject %s does not have the Parent Node.\n", 
                   this->m_ObjectName.c_str()));
        return false;
    }

    if ( this->m_strValue.empty() )
    {
    }

    if (
        ( m_pFunc->getTag() == litealigndata::FUNC_Random ) &&
        ( ( ( ( RandomFunc* ) m_pFunc )->getType() == RandomFunc::TableRandom && m_tag == litealigndata::PARA_Value ) ||
          ( ( ( RandomFunc* ) m_pFunc )->getType() == RandomFunc::MappingRandom && m_tag == litealigndata::PARA_DepColumn ) ||
          ( ( ( RandomFunc* ) m_pFunc )->getType() == RandomFunc::LessRandom && m_tag == litealigndata::PARA_DepColumn ) ||
          ( ( ( RandomFunc* ) m_pFunc )->getType() == RandomFunc::MoreRandom && m_tag == litealigndata::PARA_DepColumn ) )
    )
    {
        std::string strTableName;
        uint32_t RowNumber = 0;
        m_AlignDataTypeDef.ParserTableInfoFromString( m_strValue.c_str(), m_strValue.length(),
                                   strTableName, RowNumber, m_strColName );

        CAlignDataObject* tableParent = m_pTable->getParent();

        if ( tableParent == NULL )
        {
            ACE_DEBUG((LM_ERROR, "can not find the parent node for the table object = %s \n",
                    m_pTable->getObjectName().c_str()));
            return false;
        }

        if ( !strTableName.empty() && !m_strColName.empty() )
        {
            DependencyInfo info;
            info.m_pTableInfo = tableParent->findChildObject( strTableName );

            CTableObject* pTable = static_cast<CTableObject*>( info.m_pTableInfo );

            if ( pTable == NULL )
            {
                ACE_DEBUG((LM_ERROR, "current depended table: %s does not exists, please check the configuration of CParameterObject = %s.\n",
                           strTableName.c_str(), m_ObjectName.c_str()));
                return false;
            }

            info.m_pColumnInfo = pTable->findColumnObject( m_strColName );

            if ( info.m_pColumnInfo == NULL )
            {
                ACE_DEBUG((LM_ERROR, "current depended column info: %s do not exists, please check the configuration of CParameterObject = %s.\n",
                           strTableName.c_str(), m_ObjectName.c_str()));
                return false;
            }

            info.m_rowNum = RowNumber;

            if ( pTable != m_pTable )
            {
                if ( m_pTable->getToCreateTableNum() != pTable->getToCreateTableNum() )
                {
                    ACE_DEBUG((LM_ERROR, "depended table(%s)'s file number: %d is not the same as Current Table(%s)'s file number: %d.\n",
                               pTable->getObjectName().c_str(), pTable->getToCreateTableNum(),
                               m_pTable->getObjectName().c_str(), m_pTable->getToCreateTableNum()));
                    return false;
                }

                m_pTable->addDependencyInfo( info );
                pTable->addDependedtable( m_pTable );//保存pTable被引用的表
            }

            if ( m_pFunc->getTag() == litealigndata::FUNC_Random )
            {
                ( ( RandomFunc* ) m_pFunc )->AddDendencyInfo( info );
            }
        }
    }

    return true;
}

CParameterObject::~CParameterObject()
{

}


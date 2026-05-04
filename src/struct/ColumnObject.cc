/*
 * =============================================================================
 *
 *       Filename:  CColumnObject.cc
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

#include "sub/parser/XmlParser.h"
#include "fun/BaseFuncClass.h"
#include "struct/TableObject.h"
#include "struct/ColumnObject.h"

CColumnObject::CColumnObject( litealigndata::elementTag tag ) :
    CAlignDataObject( tag ), m_pTable( NULL ), m_pFunc( NULL ), m_colidx(0), m_colfieldValue(""), isLocalKey( false )
{
    m_subColumn.clear();
}

CColumnObject::CColumnObject( const CColumnObject& rhs ) :
    CAlignDataObject( rhs ), m_subColumn( rhs.m_subColumn)
    , m_colidx( rhs.m_colidx ), m_colfieldValue( rhs.m_colfieldValue ), 
    isLocalKey( rhs.isLocalKey )
{
    m_pFunc = FuncFactory::Instance().createBaseFuncObject( *(rhs.m_pFunc) );
}

CColumnObject& CColumnObject::operator=( const CColumnObject& rhs )
{
    if ( &rhs == this )
        return *this;
        
    CAlignDataObject::operator= (rhs);
    m_subColumn= rhs.m_subColumn;
    m_offset= rhs.m_offset; 
    m_colidx= rhs.m_colidx; 
    m_colfieldValue= rhs.m_colfieldValue; 
    isLocalKey= rhs.isLocalKey;
    m_pFunc = FuncFactory::Instance().createBaseFuncObject( *(rhs.m_pFunc) );

    return *this;
}

void CColumnObject::Initialize()
{
    CAlignDataObject::Initialize();
    m_pFunc->setColumn( this );
    //m_pTable = dynamic_cast<CTableObject*>(this->m_pParent);
}

bool CColumnObject::GetColumnData( int rownumber, autoValue& value )
{
    if ( getElementType() != litealigndata::ELEMENT_Column )
        return false;

    if ( m_pTable == NULL )
        return false;

    uint8_t* psrc = ( uint8_t* )( m_pTable->getData() ) + rownumber * m_pTable->getTableHeader().entry_size + getOffset();

    value.setData( psrc );
    value.setType( getType() );

    return true;
}
bool CColumnObject::PreInitials( DOMNode* pNode, CAlignDataObject* pParent )
{
    bool ret = CAlignDataObject::Initialize( pNode, pParent );

    if ( ret == false )
    {
        ACE_DEBUG((LM_ERROR, "Current CColumnObject: %s initialize failed.\n", this->m_ObjectName.c_str()));
        return false;
    }

    //get the table object parent.
    CAlignDataObject* parents = this->m_pParent;

    while ( parents != NULL )
    {
        if ( parents->getElementType() == litealigndata::ELEMENT_Table )
        {
            m_pTable = static_cast<CTableObject*>( parents );
            break;
        }

        parents = parents->getParent();
    }

    if ( m_pTable == NULL )
    {
        ACE_DEBUG((LM_ERROR, "the CColumnObject name: %s have no the tableOjbect as its parents node.\n", 
                   this->m_ObjectName.c_str()));
        return false;
    }

    //get the attrName of GenFuncName;
    std::string attrName;

    if ( !findAttriValue( litealigndata::ATTR_GenFuncName, attrName ) )
    {
    }

    litealigndata::funcTag tag = ( litealigndata::funcTag ) m_AlignDataTypeDef.FindRelativeTag( litealigndata::funcTagString, litealigndata::FUNC_MAX,
                  attrName.c_str() );

    if ( tag == litealigndata::FUNC_Invalid )
    {
    }

    m_pFunc = FuncFactory::Instance().createBaseFuncObject( tag );

    if ( m_pFunc == NULL )
    {
        ACE_DEBUG((LM_ERROR, "can not create the Func instance: %s.\n", litealigndata::funcTagString[tag]));
        return false;
    }

    m_pFunc->setColumn( this );

    return ret;
}

bool CColumnObject::Initialize( DOMNode* pNode, CAlignDataObject* pParent )
{
    if ( !PreInitials( pNode, pParent ) )
        return false;

    //get the column index.
    std::string strColidx;

    if ( !findAttriValue( litealigndata::ATTR_ColIdx, strColidx ) )
    {
    }
    else
       this->m_colidx = atoi( strColidx.c_str() );

    std::string strColfieldValue;
    if ( findAttriValue( litealigndata::ATTR_ColValue, strColfieldValue ) )
    {
        this->m_colfieldValue = strColfieldValue;
    }

    Initialize();
	processItem();
    return true;//m_pFunc->Initialize();
}
bool CColumnObject::processItem()
{
    return m_pFunc->Initialize();
}
   

bool CColumnObject::processOjbect( CAlignDataObject* pRoot )
{
    //Calculate the offset.
    std::string tableid;
    getTable()->findAttriValue( litealigndata::ATTR_tableid, tableid );

    //m_offset = m_pParser->GetColumnOffset( tableid, m_colidx, getTable());

    //if(0 != m_colidx)
    //{
       /// if ( m_offset == 0xFFFE )
       // {
       //     ACE_DEBUG((LM_ERROR, "the CColumnObject %s get offset failed.\n", this->getObjectName().c_str()));
       //     return false;
       // }
  //  }

    //std::cout << "Current Table id:  " << atoi(tableid.c_str()) << "offset: " << m_offset << std::endl;
    ACE_DEBUG((LM_DEBUG, "Current Table id: %u, offset: %u.\n", 
               atoi(tableid.c_str()), m_offset));

    return getFunc()->process();
}

CColumnObject::~CColumnObject()
{
    delete m_pFunc;
}

void CColumnObject::ProduceData( autoValue* pValue )
{

}

bool CColumnObject::createData()
{
    // if(0 == m_colidx)
    // { // virtural collumn
    //     return true;
    // }
    // uint8_t  table_id = getTable()->getTableHeader().table_id;
    // tabledef* AllTableDefine = getParser()->GetAllTableDefine();
    // if(!AllTableDefine[table_id].tableinfo[m_colidx].depenColumnFlag)
    // {
        if ( !getFunc()->Calculate() )
             return false;
    // }

    return true;
}

subColumnObject::subColumnObject( litealigndata::elementTag tag ) :
    CColumnObject( tag ), m_bitlen( 0 ),isDelimiter(false)
{

}

bool subColumnObject::Initialize( DOMNode* pNode, CAlignDataObject* pParent )
{
    if ( !CColumnObject::PreInitials( pNode, pParent ) )
        return false;

    if ( m_pParent->getElementType() == litealigndata::ELEMENT_Column || m_pParent->getElementType() == litealigndata::ELEMENT_SubColumn )
    {
        CColumnObject* pColumnObject = dynamic_cast<CColumnObject*>( m_pParent );

        if ( pColumnObject == NULL )
        {
            ACE_DEBUG((LM_ERROR, "subColumnObject %s dynamic_cast failed!\n", this->m_ObjectName.c_str()));
            return false;
        }

        pColumnObject->addSubColumn( this );

        MergeFunc* pMergeFunc = dynamic_cast<MergeFunc*>( pColumnObject->getFunc() );

        if ( pMergeFunc == NULL )
        {
            ACE_DEBUG((LM_ERROR, "subColumnObject %s 's parent func must be MergeFunc!\n", this->m_ObjectName.c_str()));
            return false;
        }

        pMergeFunc->addSubFunc( getFunc() );

    }

    return m_pFunc->Initialize();
}

bool subColumnObject::processOjbect( CAlignDataObject* pRoot )
{
    if ( !getFunc()->process() )
        return false;

    litealigndata::autoType parentColumnType = ( ( CColumnObject* )getParent() )->getFunc()->getValue().getType();

    std::string strBitlen;
    bool ret = findAttriValue( litealigndata::ATTR_BitLen, strBitlen );
    m_bitlen = atoi( strBitlen.c_str() );

    if ( parentColumnType != litealigndata::AUTO_IPV4V6 && parentColumnType !=litealigndata:: AUTO_IMSIBCD 
    && parentColumnType != litealigndata::AUTO_IPV6 && parentColumnType != litealigndata::AUTO_IPV6_RSA_CU 
    && parentColumnType != litealigndata::AUTO_IPV4_IPV6 )
    {
        if ( !ret )
        {
            ACE_DEBUG((LM_ERROR, "SubColumnObject %s initialize failed, should have the bitLen attribute.\n",
                       this->m_ObjectName.c_str()));
            return false;
        }
    }

    std::string strValue;
    ret = findAttriValue( litealigndata::ATTR_misc, strValue );
    if(ret)
    {
        auto retv = strValue.find("delimiter");
        if(std::string::npos != retv){
            setIsDelimiter(true);
        }
    }

    return true;
}

subColumnObject::~subColumnObject()
{
}

bool subColumnObject::createData()
{
    return getFunc()->Calculate();
}

bool DimensionFileColumn::Initialize( DOMNode* pNode, CAlignDataObject* pParent )
{
    if ( !CAlignDataObject::Initialize( pNode, pParent ) )
        return false;

    std::string strype;

    if ( !findAttriValue( litealigndata::ATTR_Type, strype ) )
    {
        ACE_DEBUG((LM_ERROR, "DimensionFileColumn name: %s should have the type attribute.\n",
                   this->m_ObjectName.c_str()));
        return false;
    }

    m_type = ( litealigndata::autoType ) m_AlignDataTypeDef.FindRelativeTag( litealigndata::autoTypeString, litealigndata::AUTO_MAX,
                                           strype.c_str() );

    if ( m_type == 0 )
    {
        ACE_DEBUG((LM_ERROR, "DimensionFileColumn::Initialize %s can not find the relative data type for the value %s.\n",
                   this->m_ObjectName.c_str(), strype.c_str()));
        return false;
    }

    std::string strColIdx;

    if ( !findAttriValue( litealigndata::ATTR_ColIdx, strColIdx ) )
    {
        ACE_DEBUG((LM_ERROR, "DimensionFileColumn name: %s should have the colIdx attribute.\n", this->m_ObjectName.c_str()));
        return false;
    }

    m_colidx = atoi( strColIdx.c_str() );

    return true;

}

const char* DimensionFileColumn::GetColvaluestringforhash( baseType* value, char* strvalue )
{
    litealigndata::autoType type = value->getType();

    switch ( type )
    {
    case litealigndata::AUTO_UINT64:
    case litealigndata::AUTO_UINT32:
    case litealigndata::AUTO_UINT16:
    case litealigndata::AUTO_UINT8:
    case litealigndata::AUTO_INT64:
    case litealigndata::AUTO_INT32:
    case litealigndata::AUTO_INT16:
    case litealigndata::AUTO_INT8:
    case litealigndata::AUTO_IPV4:
    case litealigndata::AUTO_IMSIBCD:
        sprintf( strvalue, "%lu", ( ( numberType* )value )->getValue() );
        break;

    case litealigndata::AUTO_ByteArray:
    {
        sprintf( strvalue, "%s", ( ( CharArray* )value )->getValue() );
        break;
    }

    case litealigndata::AUTO_IPV6_RSA_CU:
    case litealigndata::AUTO_IPV6:
    case litealigndata::AUTO_IPV4_IPV6:
    {
        sprintf( strvalue, "%lu%lu", ( ( IPtype* )value )->getValue().hl.high, ( ( IPtype* )value )->getValue().hl.low );
        break;
    }

    default:
        return strvalue;

    }

    return strvalue;
}
/*
 * =============================================================================
 *
 *       Filename:  CAlignDataObject.cc
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
#include <thread>
#include <future>
#include <vector>
#include <atomic>
#include <stddef.h>
#include <stdlib.h>
#include "com/Comm.h"
#include "struct/BitObject.h"
#include "struct/TableObject.h"
#include "fun/FunctionObject.h"
#include "struct/DataStructure.h"
#include "struct/ColumnObject.h"
#include "conf/AligndataConfig.h"
#include "xercesc/dom/DOMAttr.hpp"
#include "struct/ParameterObject.h"
#include "xercesc/dom/DOMError.hpp"
#include "struct/AlignDataObject.h"
#include "xercesc/dom/DOMLocator.hpp"
#include "xercesc/dom/DOMDocument.hpp"
#include "xercesc/dom/DOMNodeList.hpp"
#include "xercesc/dom/DOMNamedNodeMap.hpp"

CAlignDataObject::CAlignDataObject( litealigndata::elementTag tag ) :
    m_offset(0), m_elementType( tag ), m_pParent( NULL ), m_pParser( NULL ), percentage(-1),has_percent(false),has_percent_child(false)
        ,pPercent_selected(NULL)
{

}
CAlignDataObject::CAlignDataObject( const CAlignDataObject& rhs ) :
    m_ObjectName( rhs.m_ObjectName ),  m_strValue( rhs.m_strValue), m_elementType( rhs.m_elementType),
    m_pParser( rhs.m_pParser), m_attributes( rhs.m_attributes ),
    m_attrLitealigndataObject ( rhs.m_attrLitealigndataObject),
    percentage(rhs.percentage),has_percent(rhs.has_percent),has_percent_child(rhs.has_percent_child),pPercent_selected(rhs.pPercent_selected)
{
    for ( auto it = rhs.m_Children.begin(); it != rhs.m_Children.end(); ++it )
    {
        CAlignDataObject* pObj = CAlignDataObjectFactory::Instance().createalignDataObject( **it );
        m_Children.push_back(pObj);
        pObj->setParent(this);
    }
}

CAlignDataObject& CAlignDataObject::operator=( const CAlignDataObject& rhs )
{
    if ( &rhs == this )
        return *this;

    m_ObjectName= rhs.m_ObjectName;
    m_strValue= rhs.m_strValue;
    m_elementType= rhs.m_elementType;
    m_pParser= rhs.m_pParser;
    m_attributes= rhs.m_attributes;
    m_attrLitealigndataObject = rhs.m_attrLitealigndataObject;

    for ( auto it = rhs.m_Children.begin(); it != rhs.m_Children.end(); ++it )
    {
        CAlignDataObject* pObj = CAlignDataObjectFactory::Instance().createalignDataObject( **it );
        m_Children.push_back(pObj);
        pObj->setParent(this);
    }

    return *this;
}

CAlignDataObject::~CAlignDataObject() 
{

}

void CAlignDataObject::Initialize()
{
    for ( auto it = m_Children.begin(); it != m_Children.end(); ++it )
    {
        (*it)->setParent(this);
        (*it)->Initialize();
    }
}

bool CAlignDataObject::Initialize( DOMNode* pNode, CAlignDataObject* pParent )
{
    m_pParent = pParent;
    char path[256] = {'\0'};
    char *value_ptr = NULL;
    if ( pParent != NULL )
        pParent->AddChildLitealigndataObject( this );

    //Get the Litealigndata ObjectName
    ACE_DEBUG((LM_DEBUG, "elementName = %s,elemtype = %u.\n", litealigndata::elementTagString[m_elementType], m_elementType));

    if ( pNode->hasAttributes() )
    {
        DOMNamedNodeMap* pAttributes = pNode->getAttributes();
        const XMLSize_t nSize = pAttributes->getLength();

        for ( XMLSize_t i = 0; i < nSize; ++i )
        {
            DOMAttr* pAttributeNode = ( DOMAttr* ) pAttributes->item( i );

            // get attribute name
            StrX attrName( pAttributeNode->getName() );

            litealigndata::AttrTag tag = ( litealigndata::AttrTag ) m_AlignDataTypeDef.FindRelativeTag( litealigndata::AttrTagString, litealigndata::ATTR_Max, attrName );

            if ( tag == litealigndata::ATTR_Invalid )
            {
                //char tmp[64]={'\0'};
                //wchar_to_char(tmp, pAttributeNode->getName());
                ACE_DEBUG((LM_ERROR, "can not find the attribute Tag for attrName = %s.\n", 
                    XMLString::transcode(pAttributeNode->getName())));
                return false;
            }

            StrX value( pAttributeNode->getValue() );
            value_ptr = (char*)value;
           
            if( tag == litealigndata::ATTR_UNION_Flag )
            {
                m_union_flag = value;
            }
            if ( tag == litealigndata::ATTR_Name )
            {
                m_ObjectName = value;
                //        std::cout << "alignDataObject::Initialize --" << m_ObjectName << std::endl;
            }

            if ( tag == litealigndata::ATTR_Percentage )
            {
                percentage = atof(value_ptr);
                has_percent = true;
                getParent()->has_percent_child = true;
            }
            
            if ( tag == litealigndata::ATTR_offset )
            {
                m_offset = atof(value_ptr);
            }


            if ( tag ==  litealigndata::ATTR_SeedFile )
            {
                struct stat s_buf;
                int len_value_ptr =  strlen(value_ptr);

                if(-1 == stat(value_ptr, &s_buf))
                {
                     ACE_DEBUG((LM_ERROR, "can not find the path of value= %s.\n", value_ptr));
                }
 
                if(S_ISDIR(s_buf.st_mode))
                {
                     if(value_ptr[len_value_ptr-1] == '/')
                     {
                         sprintf(path, "%s%s%s%s%s%s", "for file in `ls ", value_ptr, "*csv", "`; do cat $file >> ", value_ptr, "aligndata_Example_Feed.csv; done");

                     }
                     else
                     {
                         sprintf(path, "%s%s%s%s%s%s", "for file in `ls ", value_ptr, "/*csv", "`; do cat $file >> ", value_ptr, "aligndata_Example_Feed.csv; done");
                     }
                     int i=system(path);
                     sprintf(path, "%s%s", value_ptr, "aligndata_Example_Feed.csv\0"); 
                     value_ptr = path;

                }
             }
             if ( tag ==  litealigndata::ATTR_Confile )
             {

             }
            AddAttribute( tag, value_ptr );

            ACE_DEBUG((LM_DEBUG, "	attributName = %s,attrValue = %s.\n", ( const char* ) attrName, value_ptr));

            //add AttributeFunction
            char* pstr = m_AlignDataTypeDef.trim( value_ptr );

            //this is a object;
            if ( pstr[0] == '$' )
            {
                std::string objectname( ++pstr );
                CAlignDataObject* pobj = getParser()->findlitealigndataObjectFileInMapper( objectname, litealigndata::ELEMENT_Function );

                if ( pobj == NULL )
                {
                    ACE_DEBUG((LM_ERROR, "the Function Object %s have not been defined in the xml file.\n", objectname.c_str()));
                    return false;
                }

                AddAttributeObject( tag, pobj );
            }
        }
    }

    SetElementValue( pNode );
    return true;
}

void CAlignDataObject::AddChildLitealigndataObject( CAlignDataObject* pChild )
{
    m_Children.push_back( pChild );
}

void CAlignDataObject::AddAttribute( litealigndata::AttrTag tag, std::string name )
{
    this->m_attributes.insert( std::make_pair( tag, name ) );
}

void CAlignDataObject:: AddAttributeObject( litealigndata::AttrTag tag, CAlignDataObject* object )
{
    if ( object != NULL )
        m_attrLitealigndataObject.insert( std::make_pair( tag, object ) );
}

bool CAlignDataObject::recurse_process( bool (CAlignDataObject::*pFunc)() )
{
    //process current node
    if(false ==(this->*pFunc)())
    {
         return false;
    }

    //recurse child nodes
    {
        CAlignDataObject::LitealigndataObjectList& childList = this->getLitealigndataObjectList();
        CAlignDataObject::LitealigndataObjectList::iterator it;
        
        for ( it = childList.begin(); it != childList.end(); ++it )
        {
            if ( false == (*it)->recurse_process(pFunc) )
                return false;
        }

        if ( this->getElementType() != litealigndata::ELEMENT_Parameter || 
        this->getElementType() == litealigndata::ELEMENT_FuncParameter )
        {
            ACE_DEBUG((LM_DEBUG, "start Producing data, the objectName : %s.\n", getObjectName().c_str()));
            if ( this->getParent() )
                ACE_DEBUG((LM_DEBUG, "parentName : %s.\n", this->getParent()->getObjectName().c_str()));
            ACE_DEBUG((LM_DEBUG, "-----------------------------------------\n"));
        }
    }
    
    return true;
}


bool CAlignDataObject::initPercent()
{
    CAlignDataObject::LitealigndataObjectList& childList = this->getLitealigndataObjectList();
    CAlignDataObject::LitealigndataObjectList::iterator it;
    int flag_Once = 0;
    unsigned int sum = 0;

    for ( it = childList.begin(); it != childList.end(); ++it )
    {
        CAlignDataObject* pChlidObj =(*it);
        if( pChlidObj->isPercentSet() )
        {
            pChlidObj->PercentCount.rows_to_create = (getNumRowsToCreate()) * (pChlidObj->getPercent())/100.0;
            pChlidObj->PercentCount.rows_created   = 0;

            sum += pChlidObj->PercentCount.rows_to_create;

           // let parent talbe flag he has a percentage child     
           CAlignDataObject* pTable = getParser()->GetTheSpecialObject(pChlidObj, 
           litealigndata::ELEMENT_Table);
           if(0 == flag_Once)
           {   
               flag_Once = true;
               percent_it = it;
           }
        }
    }

    unsigned int leftNum = getNumRowsToCreate()-sum;
    if( leftNum && has_percent_child )
    {
        for (auto iter = childList.rbegin(); iter != childList.rend(); iter++)
        {
            CAlignDataObject* pChlidObj =(*iter);
            if( pChlidObj->isPercentSet() )
            {
                pChlidObj->PercentCount.rows_to_create += leftNum;
                break;
            }
        }
    }

    return true;
}

bool CAlignDataObject::clearPercent()
{
    CAlignDataObject::LitealigndataObjectList& childList = this->getLitealigndataObjectList();
    CAlignDataObject::LitealigndataObjectList::iterator it;

    for ( it = childList.begin(); it != childList.end(); ++it )
    {
        CAlignDataObject* pChlidObj =(*it);
        if( pChlidObj->isPercentSet() )
        {            
            pChlidObj->PercentCount.rows_created   = 0;
        }
    }

    return true;
}




CInterfaceObj::CInterfaceObj( litealigndata::elementTag tag ) :
    CAlignDataObject( tag ), bytes_lastPoint(0), packets_lastPoint(0),isToBeDuplicated(false),
        duplicateIfnStart(0),duplicateIfnNum(0)
{
    // TODO Auto-generated constructor stub
    m_mapLitealigndataObject.clear();
    
    memset( &m_bfullTable, 0, sizeof( m_bfullTable ) );

}

bool CInterfaceObj::Initialize( DOMNode* pNode, CAlignDataObject* pParent )
{
    CAlignDataObject::Initialize( pNode, pParent );
    setStatsFile();

    return true;
}

bool CInterfaceObj::addObjectInMapper( std::string str, CAlignDataObject* pLitealigndataObject )
{
    switch ( pLitealigndataObject->getElementType() )
    {
    case litealigndata::ELEMENT_Table:
    {
        std::pair<GetNetObjectList::iterator, bool> ret;
        ret = m_mapLitealigndataObject.insert( std::make_pair( str, pLitealigndataObject ) );
        return ret.second;
    }

    default:
        return true;
    }
}

const CAlignDataObject* CInterfaceObj::findLitealigndataObject( const std::string& objname ) const
{
    GetNetObjectList::const_iterator it = m_mapLitealigndataObject.find( objname );

    if ( it != m_mapLitealigndataObject.end() )
        return it->second;

    return NULL;
}

CAlignDataObject* CInterfaceObj::findLitealigndataObject( const std::string& objname )
{
    GetNetObjectList::const_iterator it = m_mapLitealigndataObject.find( objname );

    if ( it != m_mapLitealigndataObject.end() )
        return it->second;

    return NULL;
}

bool CInterfaceObj::findTableObjects(int tableId, std::vector<CAlignDataObject*>& tableList)
{
    GetNetObjectList::iterator itor;
    CTableObject* pObject = NULL;
    for ( itor = m_mapLitealigndataObject.begin(); itor != m_mapLitealigndataObject.end(); ++itor )
    {
        litealigndata::elementTag elementType = itor->second->getElementType();
        if(elementType == litealigndata::ELEMENT_Table)
        {
            pObject = static_cast<CTableObject*>( itor->second );
            if((pObject->getTableHeader().table_id == tableId) || (tableId == -1))
            {
                tableList.push_back(itor->second);
            }
        }
    }
    return true;
}

CInterfaceObj::~CInterfaceObj() 
{
//     auto it = m_mapLitealigndataObject.begin();
//     while(it != m_mapLitealigndataObject.end())
//     {
//         free((CAlignDataObject*)(it->second));
//         m_mapLitealigndataObject.erase(it++);
//     }

    for (auto it = m_mapLitealigndataObject.begin(); it != m_mapLitealigndataObject.end(); ++it )
	{
        CTableObject* obj = static_cast<CTableObject*>( it->second );
        delete obj;
		m_mapLitealigndataObject.erase(it);
	}
 }

bool CInterfaceObj::StartProduceData()
{
    // set new ACE log file for each thread
    char curLog[256] = {0};
    std::string xmlfile = getParser()->getXmlFile();
    m_AlignDataTypeDef.setNewLogFile(xmlfile, curLog);

    CTableObject* pObject = NULL;
    for ( auto it = m_mapLitealigndataObject.begin(); it != m_mapLitealigndataObject.end(); ++it)
    {
    	pObject = static_cast<CTableObject*>( it->second );
    	if ( !pObject->fillAlltheTableData() )
    	return false;
    }

    while ( !checkalltheTablefull() )
    {
        auto it = m_mapLitealigndataObject.begin();
        while (it != m_mapLitealigndataObject.end() )
        {
            pObject = static_cast<CTableObject*>( it->second);
            if(pObject->getCreatedTableNum() < pObject->getToCreateTableNum())
            {
                if ( !pObject->notifyAlldepenedTable(m_buffev) )
                {
                    return false;
                }
            }
            it++;
        }
        for ( auto& it : m_mapLitealigndataObject)
        {
            pObject = static_cast<CTableObject*>( it.second );
            pObject->setStatus( CTableObject::NotReady );
            pObject->setCurFilledCount(0);
            pObject->setNextFilledPosition(0);
        }
    }
    for (auto it = m_mapLitealigndataObject.begin(); it != m_mapLitealigndataObject.end(); ++it)
        m_bfullTable[pObject->getTableHeader().table_id] = false; 

    return true;
}

void CInterfaceObj::setStatsFile()
{
    std::string logPath = "./output/log/";
    std::string xml = this->getParser()->getXmlFile();
    auto n = xml.rfind('/');    
    std::string templateName = xml.substr(n+1);    
    auto x = templateName.rfind('.');    
    std::string traffic = templateName.substr(0,x);
    time_t cur_time;
    time( &cur_time );
    tm time = *gmtime(&cur_time);
    sprintf( m_statsFile, "%sstats_%02d-%02d-%d_%s_pid%d.log", logPath.c_str(), 
        time.tm_mon +1, time.tm_mday, time.tm_year + 1900,       
        traffic.c_str(), getpid());
}

CAlignDataTypeDef m_AlignDataTypeDef;
CAlignDataObject* CAlignDataObjectFactory::createalignDataObject( DOMNode* pNode )
{
    StrX x( pNode->getNodeName() );

    litealigndata::elementTag elementType = ( litealigndata::elementTag ) m_AlignDataTypeDef.FindRelativeTag( litealigndata::elementTagString,
                             litealigndata::ELEMENT_Max,
                             ( const char* ) x );

    ACE_DEBUG((LM_DEBUG, "alignDataObjectFactory::createalignDataObject--%s.\n", ( const char* )x));

    return createalignDataObject(elementType);
}

CAlignDataObject* CAlignDataObjectFactory::createalignDataObject( CAlignDataObject& obj )
{
    litealigndata::elementTag elementType = obj.getElementType();

    switch ( elementType )
    {
    case litealigndata::ELEMENT_Table:
        return new CTableObject( dynamic_cast<CTableObject&>(obj) );

    //case ELEMENT_Interface:
        //return new CInterfaceObj( dynamic_cast<CInterfaceObj&>(obj) );
    case  litealigndata::ELEMENT_Head:
        return new CHead(dynamic_cast<CHead&>(obj));

    case  litealigndata::ELEMENT_Body:
        return new CBody(dynamic_cast<CBody&>(obj));

    case litealigndata::ELEMENT_Struct:
        return new CStruct( dynamic_cast<CStruct&>(obj) );

    case litealigndata::ELEMENT_Union:
        return new CUnion( dynamic_cast<CUnion&>(obj) );

    case litealigndata::ELEMENT_Bit:
        return new CBitObject( dynamic_cast<CBitObject&>(obj) );

    case litealigndata::ELEMENT_Subbit:
        return new CSubBitObject( dynamic_cast<CSubBitObject&>(obj) );

    case litealigndata::ELEMENT_Column:
        return new CColumnObject( dynamic_cast<CColumnObject&>(obj) );

    case litealigndata::ELEMENT_Parameter:
        return new CParameterObject( dynamic_cast<CParameterObject&>(obj) );

    case litealigndata::ELEMENT_SubColumn:
        return new subColumnObject( dynamic_cast<subColumnObject&>(obj) );

    case litealigndata::ELEMENT_DimensionalFile:
        return new DimensionalFile( dynamic_cast<DimensionalFile&>(obj) );

    case litealigndata::ELEMENT_FileColumn:
        return new DimensionFileColumn( dynamic_cast<DimensionFileColumn&>(obj) );

    case litealigndata::ELEMENT_Function:
        return new FunctionObject( dynamic_cast<FunctionObject&>(obj) );

    default:
        return new CAlignDataObject( obj );
    }
}

CAlignDataObject* CAlignDataObjectFactory::createalignDataObject( litealigndata::elementTag elementType )
{
    switch ( elementType )
    {
    case litealigndata::ELEMENT_Table:
        return new CTableObject( elementType );

    case  litealigndata::ELEMENT_Head:
        return new CHead(elementType);

    case  litealigndata::ELEMENT_Body:
        return new CBody(elementType);

    case litealigndata::ELEMENT_Struct:
        return new CStruct( elementType );

    case litealigndata::ELEMENT_Union:
        return new CUnion( elementType );

    case litealigndata::ELEMENT_Bit:
        return new CBitObject( elementType );

    case litealigndata::ELEMENT_Subbit:
        return new CSubBitObject( elementType );
        

    case litealigndata::ELEMENT_Interface:
        return new CInterfaceObj( elementType );

    case litealigndata::ELEMENT_Column:
        return new CColumnObject( elementType );

    case litealigndata::ELEMENT_Parameter:
        return new CParameterObject( elementType );

    case litealigndata::ELEMENT_SubColumn:
        return new subColumnObject( elementType );

    case litealigndata::ELEMENT_DimensionalFile:
        return new DimensionalFile( elementType );

    case litealigndata::ELEMENT_FileColumn:
        return new DimensionFileColumn( elementType );

    case litealigndata::ELEMENT_Function:
        return new FunctionObject( elementType );

    default:
        return new CAlignDataObject( elementType );
    }
}

bool CAlignDataObject::processOjbect( CAlignDataObject* pRoot )
{
    return true;
}

void CAlignDataObject::SetElementValue( DOMNode* pNode )
{
    switch ( this->m_elementType )
    {
    case litealigndata::ELEMENT_Parameter:
    case litealigndata::ELEMENT_FuncParameter:
    {
        if ( pNode->getFirstChild()
             && pNode->getFirstChild()->getNodeType() == DOMNode::TEXT_NODE )
        {
            StrX value( pNode->getFirstChild()->getNodeValue() );
            this->m_strValue = value;
        }
    }

    default:
        return;
    }
}

void CInterfaceObj::ModifyKeysTableIdxMap()
{

}

void CInterfaceObj::ProduceExtractedSeedData()
{
    for(auto tblMap : m_mapLitealigndataObject)
    {
        CTableObject* tblObj = dynamic_cast<CTableObject*>(tblMap.second);
        tblObj->ProduceExtractedSeedData();
    }

    ModifyKeysTableIdxMap();

    for(auto tblMap : m_mapLitealigndataObject)
    {
        CTableObject* tblObj = dynamic_cast<CTableObject*>(tblMap.second);
        tblObj->WriteExtractedSeedData();
    }

}

void CInterfaceObj::Settablefinished( uint8_t tableId )
{
    this->m_bfullTable[tableId] = true;
}

bool CInterfaceObj::getTableFinished( uint8_t tableId )
{
    return this->m_bfullTable[tableId];
}

bool CInterfaceObj::checkalltheTablefull()
{
    GetNetObjectList::iterator it;
    
    for ( it = m_mapLitealigndataObject.begin(); it != m_mapLitealigndataObject.end(); ++it )
    {
        CTableObject* pObject = static_cast<CTableObject*>( it->second );
        
        std::string get_table_id;
        pObject->findAttriValue( litealigndata::ATTR_tableid, get_table_id );

        if ( m_bfullTable[atoi(get_table_id.c_str())] == false )
            return false;
    }

    return true;
}
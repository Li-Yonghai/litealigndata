/*
 * =============================================================================
 *
 *       Filename:  CXmlParser.cc
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
#include "ace/Log_Msg.h"
#include "sub/parser/XmlParser.h"
#include "struct/TableObject.h"
#include "struct/ColumnObject.h"
#include "file/Example/Example.h"
#include "xercesc/util/OutOfMemoryException.hpp"
#include "xercesc/framework/LocalFileFormatTarget.hpp"

#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif

#include <queue>

using namespace std;

XercesDOMParser::ValSchemes CXmlParser::gValScheme = AbstractDOMParser::Val_Auto;
bool CXmlParser::gDoNamespaces = false;
bool CXmlParser::gDoSchema = false;
bool CXmlParser::gSchemaFullChecking = false;
bool CXmlParser::gDoCreate = false;

CXmlParser::CXmlParser( const char* pXmlFile, const char* outPutfile ) :
	m_offset(0), m_fSawErrors( false ), m_pLitealigndataObjRoot( NULL ),XML_expand_Flag(false)
{
	strcpy( m_xmlFile, pXmlFile );

	strcpy( m_outPutfilePath, outPutfile );

	m_Interfacelist.clear();
	stack_init(&m_stack);

}

uint16_t CXmlParser::GetColumnOffset( std::string& tableid, uint16_t colidx, CAlignDataObject* pLitealigndataObject )
{
	if ( colidx == 0 )
		return 0xFFFE;

	if ( pLitealigndataObject != NULL )
	{
		std::string tableType;

		if ( !pLitealigndataObject->findAttriValue( litealigndata::ATTR_TableType, tableType ) )
		{
			ACE_DEBUG((LM_ERROR, "XmlParser::GetColumnOffset(): current table object %s can not find the tableType attribute.\n",
						pLitealigndataObject->getObjectName().c_str()));
			return 0xFFFE;
		}

		return 0;

	}

	return 0xFFFE;
}

litealigndata::autoType CXmlParser::GetColumnType( std::string& tableid, uint16_t colIdx )
{
	return litealigndata::AUTO_Invalid;
}

uint8_t CXmlParser::GetColumnLen( std::string& tableid, uint16_t colIdx )
{

	return litealigndata::AUTO_Invalid;//m_AllTableDefine[atoi(tableid.c_str())].tableinfo[colIdx - 1].len;
}

uint32_t CXmlParser::GetColumnIdx( std::string& tableid, const std::string& colName )
{
	uint32_t notFound = 0xffffffff;

	int index_column = 0; 

	return notFound;
}

bool CXmlParser::Initialize()
{
	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch ( const XMLException& toCatch )
	{
		ACE_DEBUG((LM_ERROR, "Error during Xerces-c Initialization.\n"));
		ACE_DEBUG((LM_ERROR, "  Exception message:%s.\n", toCatch.getMessage()));
		return false;
	}

	static const XMLCh gLS[] =
	{ chLatin_L, chLatin_S, chNull};
	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation( gLS );

	DOMLSParser* parser = ( ( DOMImplementationLS* )impl )->createLSParser( DOMImplementationLS::MODE_SYNCHRONOUS, 0 );
	DOMConfiguration* config = parser->getDomConfig();

	config->setParameter( XMLUni::fgDOMNamespaces, gDoNamespaces );

	if ( gValScheme == AbstractDOMParser::Val_Auto )
		config->setParameter( XMLUni::fgDOMValidateIfSchema, true );

	config->setParameter( XMLUni::fgXercesSchema, gDoSchema );
	config->setParameter( XMLUni::fgXercesHandleMultipleImports, true );
	config->setParameter( XMLUni::fgXercesSchemaFullChecking, gSchemaFullChecking );

	config->setParameter( XMLUni::fgDOMDatatypeNormalization, true );
	config->setParameter( XMLUni::fgDOMErrorHandler, this );
	bool errorsOccured = false;
	this->resetErrors();
	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = 0;

	try
	{
		parser->resetDocumentPool();
		doc = parser->parseURI( m_xmlFile );
	}
	catch ( const XMLException& toCatch )
	{
		ACE_DEBUG((LM_ERROR, "\nError during parsing: '%s'\n", m_xmlFile));
		ACE_DEBUG((LM_ERROR, "%IException message is: %s.\n", toCatch.getMessage()));
		errorsOccured = true;
	}
	catch ( const DOMException& toCatch )
	{
		const unsigned int maxChars = 2047;
		XMLCh errText[maxChars + 1];

		ACE_DEBUG((LM_ERROR, "\nDOM Error during parsing: '%s'\n", m_xmlFile));
		ACE_DEBUG((LM_ERROR, "DOMException code is:  %u.\n", toCatch.code));

		if ( DOMImplementation::loadDOMExceptionMsg( toCatch.code, errText, maxChars ) )
			ACE_DEBUG((LM_ERROR, "Message is: %s.\n", errText));

		errorsOccured = true;
	}
	catch ( ... )
	{
		ACE_DEBUG((LM_ERROR, "\nUnexpected exception during parsing: '%s'.\n", m_xmlFile));
		errorsOccured = true;
	}

	if ( !errorsOccured && !getSawErrors() )
	{
		DOMNode* docNode = ( DOMNode* )doc->getDocumentElement();
		if (!Random::createInstance(32))
		{
			ACE_DEBUG((LM_ERROR, "Cannot initialize Random generator, abort..., Maximum support 32 slave threads\n"));
			exit(0);
		}

		if ( !travelXmlElement( docNode, NULL ) )
			return false;
        
		if ( m_pLitealigndataObjRoot == NULL )
			return false;

		SetAllItemOffset();

		if ( ! ProcessObject( m_pLitealigndataObjRoot ) )
			return false;

		if ( !LoadDimensionFiles() )
			return false;
	}

	parser->release();
	XMLPlatformUtils::Terminate();
	return true;
}
bool CXmlParser::processItem()
{
    for ( auto it = m_Interfacelist.begin(); it != m_Interfacelist.end(); ++it )
    {
        //CInterfaceObj* obj = static_cast<CInterfaceObj*>( *it );
		std::list<CAlignDataObject*> tablelist = (*it)->getLitealigndataObjectList();
		for(auto it1 : tablelist)
		{
			((CTableObject*)it1)->processItem();
			std::list<CAlignDataObject*> columnlist = (it1)->getLitealigndataObjectList();
			for(auto it2 : columnlist)
				((CColumnObject*)it2)->processItem();

		}
        
    }
	return true;

}
int CXmlParser::find_xml_node_attrbt(DOMNode* pNode, litealigndata::AttrTag const attrbt, std::string& attrbt_name, std::string& attrbt_value)
{

	DOMNamedNodeMap* pAttributes = pNode->getAttributes();
	const XMLSize_t nSize = pAttributes->getLength();
	for ( XMLSize_t i = 0; i < nSize; ++i )
	{
		DOMAttr* pAttributeNode = ( DOMAttr* ) pAttributes->item( i );

		// get attribute name
		StrX attrName( pAttributeNode->getName() );

		litealigndata::AttrTag tag = ( litealigndata::AttrTag ) m_AlignDataTypeDef.FindRelativeTag( litealigndata::AttrTagString, litealigndata::ATTR_Max, attrName );

		StrX value( pAttributeNode->getValue() );

		if (attrbt == tag)
		{
			attrbt_name  = (const char*)attrName;
			attrbt_value = (const char*)value;
			break;
		}
	}

	return 0;
}


int CXmlParser::getLoadedCsvColumn(std::string& talbe_column_input, DimensionfileDatas const *& pCsvFileData, uint32_t& csv_column_loaded_Idx ,uint32_t& row_quantity)
{
	CAlignDataObject*    pDimensionFile = NULL;    
	CAlignDataObject*    pDimensionColumn = NULL;
	std::string donot_care;

	anyzeDepDimensionInfo(donot_care, talbe_column_input, pDimensionFile, pDimensionColumn);

	pCsvFileData = findDimensionFileDatas( pDimensionFile->getObjectName() );
	if ( pCsvFileData == NULL || pCsvFileData->empty() )
	{
		ACE_DEBUG((LM_ERROR, "can not find the the proper dimension Data (%s).\n",
					pDimensionFile->getObjectName().c_str()));
		return false;
	}


	row_quantity = pCsvFileData->size();
	csv_column_loaded_Idx  = dynamic_cast<DimensionFileColumn*>(pDimensionColumn)->getMappingidx() - 1; 
	return 0;
}


int CXmlParser::getCsvDataColumn  (DimensionfileDatas const * pDimensionfileDataTable, unsigned DimensionfileDataColumnIndex, unsigned rowIdex, baseType*& pPercentValue)
{
	pPercentValue = NULL;
	const CXmlParser::Dimensioncolvalues* pRowdata = ( *pDimensionfileDataTable )[rowIdex];
	pPercentValue = (*pRowdata)[DimensionfileDataColumnIndex];
	return 0;
}

int CXmlParser::getColumnCharArray(DimensionfileDatas const* pDimensionfileDataTabl, unsigned csv_loaded_colmn_Idx, unsigned rowIdex, std::string& pacVal )
{
	const CharArray* pacValue = NULL;
	{
		baseType*   pColumnValue = NULL;
		getCsvDataColumn(pDimensionfileDataTabl, csv_loaded_colmn_Idx, rowIdex, pColumnValue);
		assert(0!=pColumnValue);
		pacValue = dynamic_cast<CharArray*>(pColumnValue);
	}
	assert(0!=pacValue);
	pacVal = pacValue->getValue(); 
	return 0;
}

bool CXmlParser::handleError( const DOMError& domError )
{
	m_fSawErrors = true;

	if ( domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING )
		ACE_DEBUG((LM_ERROR, "\nWarning at file "));
	else if ( domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR )
		ACE_DEBUG((LM_ERROR, "\nError at file "));
	else
		ACE_DEBUG((LM_ERROR, "\nFatal Error at file "));

	ACE_DEBUG((LM_ERROR, "%s, line %u, char %u\n  Message: %s\n",
				domError.getLocation()->getURI(),
				domError.getLocation()->getLineNumber(),
				domError.getLocation()->getColumnNumber(),
				domError.getMessage()));

	return true;
}

void CXmlParser::resetErrors()
{
	m_fSawErrors = false;
}

bool CXmlParser::travelXmlElement( DOMNode* node, CAlignDataObject* pParent )
{
	DOMNode* child = NULL;
	DOMNode* sibling = NULL;
	bool ret = true;

	if ( node )
	{
		CAlignDataObject* pObj = NULL;

		if ( node->getNodeType() == DOMNode::ELEMENT_NODE )
		{
			pObj = CAlignDataObjectFactory::Instance().createalignDataObject( node );

			if ( pObj == NULL )
			{
				ACE_DEBUG((LM_ERROR, "create the object failed!\n"));
				return false;
			}

			if ( pObj->getElementType() == litealigndata::ELEMENT_AligndataModel )
			{
				m_pLitealigndataObjRoot = pObj;
			}

			pObj->setParser( this );

			if ( pObj->getElementType() == litealigndata::ELEMENT_Invalid )
			{
				ACE_DEBUG((LM_ERROR, "current element nodename = %s is Element_Invalid.\n", node->getNodeName()));
				return false;
			}

			if ( !pObj->Initialize( node, pParent ) )
			{
				ACE_DEBUG((LM_ERROR, "Current node %s--%s intial failed!\n",
							pObj->getObjectName().c_str(), litealigndata::elementTagString[pObj->getElementType()]));
				return false;
			}

			addInterfaceInList( pObj );

			if ( !this->addlitealigndataObjectInMapper( pObj->getObjectName(), pObj ) )
			{
				ACE_DEBUG((LM_WARNING, "The ngbaObject already exist, can not add the duplicate Dimension Object %s\n", 
							pObj->getObjectName().c_str()));
				return false;
			}

			if ( pParent != NULL )
				ACE_DEBUG((LM_DEBUG, "travelXmlElement Parent--%s.\n", pParent->getObjectName().c_str()));
			if ( pObj != NULL )
				ACE_DEBUG((LM_DEBUG, "travelXmlElement CurrentNode--%s.\n", pObj->getObjectName().c_str()));
			ACE_DEBUG((LM_DEBUG, "----------------------------------------------\n"));
		}
		
		if ( ( child = node->getFirstChild() ) != NULL )
		{
			if ( !CXmlParser::travelXmlElement( child, pObj ) )
			{
				ACE_DEBUG((LM_ERROR, "Current node initials child node failed Parents is %s--%s.\n",
							pObj->getObjectName().c_str(), litealigndata::elementTagString[pObj->getElementType()]));
				return false;
			}
		}

		if ( ( sibling = node->getNextSibling() ) != NULL )
		{
			if ( !CXmlParser::travelXmlElement( sibling, pParent ) )
			{
				ACE_DEBUG((LM_ERROR, "Current node initials silbiling node failed the parents is %s -- %s.\n",
							pParent->getObjectName().c_str(),
							litealigndata::elementTagString[pParent->getElementType()] ));
				return false;
			}
		}

	}

	return ret;
}


CAlignDataObject* CXmlParser::GetTheSpecialObject( CAlignDataObject* pChild, litealigndata::elementTag tag )
{
	CAlignDataObject* pParent = pChild->getParent();

	while ( pParent )
	{
		if ( pParent->getElementType() == tag )
			return pParent;

		pParent = pParent->getParent();
	}

	return NULL;
}

bool CXmlParser::ProcessObject( CAlignDataObject* obj )
{
	if ( obj )
	{
		//cout << "start process the objectName        " << obj->getObjectName().c_str() << endl;
		ACE_DEBUG((LM_DEBUG, "start process the objectName %s\n", obj->getObjectName().c_str()));
		if ( obj->getParent() )
		{
			//cout << "parentName         " <<  obj->getParent()->getObjectName().c_str() << endl;
			ACE_DEBUG((LM_DEBUG, "parentName %s\n", obj->getParent()->getObjectName().c_str()));
		}
			
		ACE_DEBUG((LM_DEBUG, "-----------------------------------------\n"));

		if ( !obj->processOjbect( this->m_pLitealigndataObjRoot ) )
		{
			cout << "The object name " << "process object failed!   " << obj->getObjectName().c_str() << endl;
			ACE_DEBUG((LM_ERROR, "The object name %s process object failed!\n", obj->getObjectName().c_str()));
			return false;
		}

		CAlignDataObject::LitealigndataObjectList& childList = obj->getLitealigndataObjectList();
		CAlignDataObject::LitealigndataObjectList::iterator it;

		for ( it = childList.begin(); it != childList.end(); ++it )
		{
			if ( !CXmlParser::ProcessObject( *it ) )
			{
				cout << "ERROR Current object name  " << ( *it )->getObjectName().c_str() << endl;
				ACE_DEBUG((LM_ERROR, "Current object name %s process object failed!\n", 
							( *it )->getObjectName().c_str()));
				return false;
			}
		}

	}

	return true;
}

void CXmlParser::addInterfaceInList( CAlignDataObject* pLitealigndataObject )
{
	switch ( pLitealigndataObject->getElementType() )
	{
		case litealigndata::ELEMENT_Interface:
			{
				m_Interfacelist.push_back( pLitealigndataObject );
			}

		default:
			return;
	}
}

void CXmlParser::ProduceExtractedSeedData()
{
	for(auto itf : m_Interfacelist)
	{
		CInterfaceObj* itfObj = dynamic_cast<CInterfaceObj*>(itf);
		itfObj->ProduceExtractedSeedData();
	}
}

void CXmlParser::ProduceData(bufferevent *buffev)
{

	listInterface::const_iterator it;

	std::vector<std::thread> vt;
	for ( it = m_Interfacelist.begin(); it != m_Interfacelist.end(); ++it )
	{
		(dynamic_cast<CInterfaceObj*>(*it))->setbufferevent(buffev);
		vt.emplace_back( &CInterfaceObj::StartProduceData, dynamic_cast<CInterfaceObj*>(*it) );
		usleep(100);
	}

	for(auto &t: vt)
	{
		t.join();
	}


	ACE_DEBUG((LM_INFO, "All the interfaces' thread tasks done, exit! \n"));

}

bool CXmlParser::addlitealigndataObjectInMapper( std::string str, CAlignDataObject* pLitealigndataObject )
{
	switch ( pLitealigndataObject->getElementType() )
	{
		case litealigndata::ELEMENT_DimensionalFile:
			{
				std::pair<MapDimensionFile::iterator, bool> ret;

				ret = m_DimensionFileMap.insert( std::make_pair( str, pLitealigndataObject ) );

				return ret.second;
			}

		case litealigndata::ELEMENT_Function:
			{
				std::pair<MapFuncion::iterator, bool> ret;

				ret = m_FunctionMap.insert( std::make_pair( str, pLitealigndataObject ) );

				return ret.second;
			}

		default:
			return true;
	}
}

CAlignDataObject* CXmlParser::findlitealigndataObjectFileInMapper( const std::string& objname, litealigndata::elementTag tag )
{

	switch ( tag )
	{
		case litealigndata::ELEMENT_DimensionalFile:
			{
				MapDimensionFile::const_iterator it = m_DimensionFileMap.find( objname );

				if ( it != m_DimensionFileMap.end() )
					return it->second;

				break;
			}

		case litealigndata::ELEMENT_Function:
			{
				MapFuncion::const_iterator it = m_FunctionMap.find( objname );

				if ( it != m_DimensionFileMap.end() )
					return it->second;

				break;    
			}

		default:
			return NULL;
	}

	return NULL;
}

uint8_t CXmlParser::addDimensionInfo( CAlignDataObject* dimensionFile, CAlignDataObject* dimensionColumn )
{
	CXmlParser::DimensionLoadInfo::iterator it = m_DimensionInfoMap.find( dimensionFile->getObjectName() );
	uint8_t pos = 0;

	if ( it != m_DimensionInfoMap.end() )
	{
		//compare the column info.
		ColumnArray::const_iterator arrayIt;

		for ( arrayIt = it->second.m_CollumnArray.begin(); arrayIt != it->second.m_CollumnArray.end(); ++arrayIt )
		{
			++pos;

			if ( ( ( DimensionFileColumn* ) dimensionColumn )->getColidx() == ( ( DimensionFileColumn* )( *arrayIt ) )->getColidx() )
			{
				( ( DimensionFileColumn* ) dimensionColumn )->setMappingidx( pos );
				return pos;
			}
		}

		it->second.m_CollumnArray.push_back( dimensionColumn );
		( ( DimensionFileColumn* ) dimensionColumn )->setMappingidx( it->second.m_CollumnArray.size() );
		return it->second.m_CollumnArray.size();
	}

	LoadDimensionInfo info;

	info.hashKey = 0;
	info.m_pDimensionFile = dimensionFile;
	info.m_CollumnArray.push_back( dimensionColumn );
	m_DimensionInfoMap.insert( std::make_pair( dimensionFile->getObjectName(), info ) );

	( ( DimensionFileColumn* ) dimensionColumn )->setMappingidx( info.m_CollumnArray.size() ); //set the mappingIdx.
	return info.m_CollumnArray.size();
}

void CXmlParser::addDimensionKeyInfo( LoadDimensionInfo& info )
{
	char msz[256] =
	{ 0 };

	sprintf( msz, "%s", info.m_pDimensionFile->getObjectName().c_str() );

	ColumnArray::const_iterator arrayIt;

	for ( arrayIt = info.m_CollumnArray.begin(); arrayIt != info.m_CollumnArray.end(); ++arrayIt )
	{
		sprintf( msz + strlen( msz ), "%u", ( ( DimensionFileColumn* )( *arrayIt ) )->getColidx() );
	}

	ACE_DEBUG((LM_DEBUG, "current hash string = %s \n", msz));

	//info.hashKey = m_AlignDataTypeDef.RSHash( msz );

	CXmlParser::mappingKeyInfo::iterator it = m_KeyInfoMap.find( info.hashKey );

	if ( it != m_KeyInfoMap.end() )
		return;

	m_KeyInfoMap.insert( std::make_pair( info.hashKey, info ) );

	return;
}

const LoadDimensionInfo* CXmlParser::findDimensionKeyInfo( uint32_t hashKey )
{
	CXmlParser::mappingKeyInfo::const_iterator it = m_KeyInfoMap.find( hashKey );

	if ( it != m_KeyInfoMap.end() )
		return &( it->second );

	return NULL;
}

std::string CXmlParser::parseCsvString(std::string& line)
{
	if ( !line.empty() )
	{
		line.erase( 0, line.find_first_not_of(" ") );  
		line.erase(line.find_last_not_of(" ") + 1); 
		if( line.length() > 1 )
		{
			if( line.at(0) == '"' )
			{
				std::size_t found = line.find('"', 1);
				if (found != std::string::npos && found > 1)
				{
					std::string strField = line.substr(1, found-1);
					std::string subStr = line.substr(found+1);
					line = subStr;
					return strField;
				}
			}
			else
			{
				std::size_t found = line.find(',');
				if (found != std::string::npos)
				{
					std::string strField = line.substr(0, found);
					std::string subStr = line.substr(found+1);
					line = subStr;
					return strField;
				}
			}
		}
	}
	return std::string("");
}

bool CXmlParser::LoadDimensionFiles()
{
	CXmlParser::DimensionLoadInfo::iterator it = m_DimensionInfoMap.begin();

	while ( it != m_DimensionInfoMap.end() )
	{
		bool (CXmlParser::*pFunc)( const LoadDimensionInfo& ) = NULL;

		pFunc = &CXmlParser::LoadDimensionFile;

		if ( !(this->*pFunc)( it->second ) )
			return false;

		++it;
	}

	return true;
}

bool CXmlParser::LoadDimensionFile( const LoadDimensionInfo& info )
{
	char szFilePath[256] = { 0 };

	DimensionalFile* DimensionFile = ( DimensionalFile* ) info.m_pDimensionFile;

	if ( DimensionFile == NULL )
		return false;

	sprintf( szFilePath, "%s%s.csv", DimensionFile->getFilePath(), DimensionFile->getObjectName().c_str() );

	FILE* fp = fopen( szFilePath, "r" );

	if ( fp == NULL )
	{
		ACE_DEBUG((LM_ERROR, "current dimensionFile does not exist: %s.\n", szFilePath));
		return false;
	}

	uint32_t rowCount = m_AlignDataTypeDef.GetTotalLineCount( fp );

	ACE_DEBUG((LM_INFO, "start loading DimensionFile: %s and count = %u.\n", szFilePath, rowCount));


	char buffer[1024] = { 0 };

	uint8_t colnum = info.m_CollumnArray.size();

	char* line;
	char* record;

	DimensionfileDatas* table = new DimensionfileDatas();
	table->reserve( rowCount );


	while ( ( line = fgets( buffer, sizeof( buffer ), fp ) ) != NULL )
	{
		bool isQuoteExist = false;
		std::string lineStr = line;
		std::size_t found = lineStr.find('"');
		if(found != std::string::npos)
		{
			isQuoteExist = true;
		}

		if( isQuoteExist )
		{
			std::string ret = parseCsvString(lineStr);
			if(ret.length() > 0)
			{
				record = (char*)(ret.c_str());
			}
			else
			{
				record = NULL;
			}
		}
		else
		{
			record = strtok( line, "," );
		}

		uint8_t colidx = 0;  //calculate the col number;

		Dimensioncolvalues* rowvalue = new Dimensioncolvalues( colnum, NULL );

		//get ColValue.
		while ( record != NULL )
		{
			colidx++;

			uint8_t pos = 0;

			DimensionFileColumn* pobject = ( DimensionFileColumn* ) GetColObject( info.m_CollumnArray, colidx, pos );

			if ( pobject == NULL )
			{
				if( isQuoteExist )
				{
					std::string ret = parseCsvString(lineStr);
					if(ret.length() > 0)
					{
						record = (char*)(ret.c_str());
					}
					else
					{
						record = NULL;
					}
				}
				else
				{
					record = strtok( NULL, "," );
				}
				continue;
			}

			if ( pobject->getType() == litealigndata::AUTO_UINT64 || pobject->getType() == litealigndata::AUTO_UINT32 || 
				pobject->getType() == litealigndata::AUTO_UINT16 || pobject->getType() == litealigndata::AUTO_UINT8 || 
				pobject->getType() == litealigndata::AUTO_IMSIBCD || pobject->getType() == litealigndata::AUTO_IPV4 || 
				pobject->getType() == litealigndata::AUTO_IPV4V6 || pobject->getType() == litealigndata::AUTO_IMSI_TBCD || 
				pobject->getType() == litealigndata::AUTO_IMEISV_BCD || pobject->getType() == litealigndata::AUTO_BCD_2Bytes)
			{
				baseType* value = new numberType( pobject->getType() );

				( ( numberType* ) value )->AsciiToDec( record );

				( *rowvalue )[pos] = value;
			}
			else if((pobject->getType() == litealigndata::AUTO_ByteArray) 
					||(pobject->getType() == litealigndata::AUTO_IPV4_NET) ||(pobject->getType() == litealigndata::AUTO_IPV6_NET)
					||(pobject->getType() == litealigndata::AUTO_STRING_NET))
			{
				baseType* value = new CharArray( pobject->getType() );

				( ( CharArray* ) value )->setValue( record);

				( *rowvalue )[pos] = value;
			}
			if( isQuoteExist )
			{
				std::string ret = parseCsvString(lineStr);
				if(ret.length() > 0)
				{
					record = (char*)(ret.c_str());
				}
				else
				{
					record = NULL;
				}
			}
			else
			{
				record = strtok( NULL, "," );
			}
		}

		table->push_back( rowvalue );
	}

	m_tableData.insert( std::make_pair( DimensionFile->getObjectName().c_str(), table ) );

	ACE_DEBUG((LM_INFO, "Dimension file: %s load %u records into memory.\n", 
				szFilePath, table->size()));
	return true;
}


bool CXmlParser::LoadDimensionFileArbor( const LoadDimensionInfo& info )
{
	char szFilePath[256] = { 0 };

	DimensionalFile* DimensionFile = ( DimensionalFile* ) info.m_pDimensionFile;

	if ( DimensionFile == NULL )
		return false;

	sprintf( szFilePath, "%s%s.csv", DimensionFile->getFilePath(), DimensionFile->getObjectName().c_str() );

	FILE* fp = fopen( szFilePath, "r" );

	if ( fp == NULL )
	{
		ACE_DEBUG((LM_ERROR, "current dimensionFile does not exist :%s.\n", szFilePath));
		return false;
	}

	uint32_t rowCount = m_AlignDataTypeDef.GetTotalLineCount( fp );

	ACE_DEBUG((LM_INFO, "start loading DimensionFile: %s and count = %u.\n", szFilePath, rowCount));


	char buffer[1024] = { 0 };

	uint8_t colnum = info.m_CollumnArray.size();

	char* line;
	char* record;

	DimensionfileDatas* table = new DimensionfileDatas();
	table->reserve( rowCount );


	while ( ( line = fgets( buffer, sizeof( buffer ), fp ) ) != NULL )
	{
		bool isQuoteExist = false;
		std::string lineStr = line;
		{
			record = strtok( line, "," );
		}

		uint8_t colidx = 0;  //calculate the col number;

		Dimensioncolvalues* rowvalue = new Dimensioncolvalues( colnum, NULL );

		//get ColValue.
		while ( record != NULL )
		{
			char const * recordget = NULL;

			{//  a,"xx,xx",c,d, take care of "xx,xx"
				bool has_quete = strchr(record,'"');
				if(false == has_quete)
				{
					recordget = record;
					//cout<<" "<<recordget<<endl;
				}
				else
				{    
					std::string merge;
					merge += record;
					merge +=",";

					while(1)
					{
						record = strtok(NULL,",");
						{
							bool has_quete = strchr(record,'"');
							if(false == has_quete){          
								merge += record ;
								merge += ",";
							}else{          
								merge += record;
								break;
							}
						}          
					}

					recordget = merge.c_str();
					//cout<<" "<<recordget<<endl;
				}
			}

			{
				colidx++;

				uint8_t pos = 0;

				DimensionFileColumn* pobject = ( DimensionFileColumn* ) GetColObject( info.m_CollumnArray, colidx, pos );

				if ( pobject != NULL )
				{
					if ( pobject->getType() == litealigndata::AUTO_UINT64 || pobject->getType() == litealigndata::AUTO_UINT32 || pobject->getType() == litealigndata::AUTO_UINT16
							|| pobject->getType() == litealigndata::AUTO_UINT8 || pobject->getType() == litealigndata::AUTO_IMSIBCD || pobject->getType() == litealigndata::AUTO_IPV4
							|| pobject->getType() == litealigndata::AUTO_IPV4V6 || pobject->getType() == litealigndata::AUTO_IMSI_TBCD || pobject->getType() == litealigndata::AUTO_IMEISV_BCD
							|| pobject->getType() == litealigndata::AUTO_BCD_2Bytes)
					{
						baseType* value = new numberType( pobject->getType() );

						( ( numberType* ) value )->AsciiToDec( recordget );

						( *rowvalue )[pos] = value;
					}
					else if((pobject->getType() == litealigndata::AUTO_ByteArray))
					{
						baseType* value = new CharArray( pobject->getType() );

						( ( CharArray* ) value )->setValue( recordget);

						( *rowvalue )[pos] = value;
					}
				}
			}

			record = strtok(NULL,",");
		}

		table->push_back( rowvalue );
	}

	m_tableData.insert( std::make_pair( DimensionFile->getObjectName().c_str(), table ) );

	ACE_DEBUG((LM_INFO, "Dimension file: %s load %u records into memory.\n", 
				szFilePath, table->size()));
	return true;
}


const CXmlParser::DimensionfileDatas* CXmlParser::findDimensionFileDatas( const std::string& filename ) const
{
	mapDimensionfileDatas::const_iterator it = m_tableData.find( filename );

	if ( it != m_tableData.end() )
	{
		return it->second;
	}

	return NULL;
}

const CXmlParser::DimensionfileDatas* CXmlParser::findDimensionFileDatasforIdx( const std::string& filename, uint32_t colHashKey ) const
{
	doublehashIdx temp;

	//temp.fileIdx = m_AlignDataTypeDef.RSHash( ( char* ) filename.c_str() );
	temp.colarrayIdx = colHashKey;

	dimensionfilemapping::const_iterator Idx = m_idxtableData.find( temp );

	if ( Idx != m_idxtableData.end() )
	{
		return Idx->second;
	}

	return NULL;
}


bool CXmlParser::SerializeDom( DOMNode* node , char const * pChar)
{
	XMLCh tempStr[3] = {chLatin_L, chLatin_S, chNull};
	DOMImplementation* impl = DOMImplementationRegistry::getDOMImplementation( tempStr );
	DOMLSSerializer* theSerializer = ( ( DOMImplementationLS* )impl )->createLSSerializer();

	DOMLSOutput* theOutputDesc = ( ( DOMImplementationLS* )impl )->createLSOutput();

	if ( theSerializer->getDomConfig()->canSetParameter( XMLUni::fgDOMWRTDiscardDefaultContent, true ) )
		theSerializer->getDomConfig()->setParameter( XMLUni::fgDOMWRTDiscardDefaultContent, true );

	if ( theSerializer->getDomConfig()->canSetParameter( XMLUni::fgDOMWRTFormatPrettyPrint, true ) )
		theSerializer->getDomConfig()->setParameter( XMLUni::fgDOMWRTFormatPrettyPrint, true );

	theSerializer->getDomConfig()->setParameter( XMLUni::fgDOMErrorHandler, this );

	XMLFormatTarget* myFormTarget;

	char prefix[200]={0};
	strcat(prefix,pChar);
	strcat(prefix,m_xmlFile);

	myFormTarget = new LocalFileFormatTarget( prefix );

	theOutputDesc->setByteStream( myFormTarget );

	try
	{
		theSerializer->write( node, theOutputDesc );
	}
	catch ( const XMLException& toCatch )
	{
		char* message = XMLString::transcode( toCatch.getMessage() );
		ACE_DEBUG((LM_INFO, "Exception message is: %s.\n", message));
		XMLString::release( &message );
		return false;
	}
	catch ( const DOMException& toCatch )
	{
		char* message = XMLString::transcode( toCatch.msg );
		ACE_DEBUG((LM_INFO, "Exception message is: %s.\n", message));
		XMLString::release( &message );
		return false;
	}
	catch ( ... )
	{
		ACE_DEBUG((LM_ERROR, "Unexpected Exception \n"));
		return false;
	}

	theOutputDesc->release();
	theSerializer->release();
	delete myFormTarget;
	return true;

}

bool CXmlParser::CreateTableMapping( mapAligndatatablelist& maptableheadlist, const SeedFileInfo& seedFileInfo,bool isSeedFromDir  )
{
	//get the HeaderInfomation.
	maptableheadlist.clear();

	ALIGNDATA_TABLE * pTableHeader;
	uint32_t ifn = 0;
	const SeedFile *seedfileinfo = &(seedFileInfo.seedfile);

	for ( uint32_t i = 0; i < seedFileInfo.filenumber; ++i )
	{
		seedInfo* pTableInfo = new seedInfo( new ( ALIGNDATA_TABLE ), seedfileinfo->filePath[i], seedfileinfo->fileType[i], seedfileinfo->jsonFile[i], seedfileinfo->tablename[i]);
		pTableHeader = pTableInfo->m_pHeader;
		
		if ( !isSeedFromDir )
		{
			uint32_t maxItf = seedFileInfo.initItf + seedFileInfo.itfNum;
			for ( ifn = seedFileInfo.initItf; ifn < maxItf; ++ifn )
			{
				mapAligndatatablelist::iterator it = maptableheadlist.find( ifn );

				if ( it != maptableheadlist.end() )
				{
					it->second.push_back( pTableInfo );
				}
				else
				{
					astTablelist asttablearray;
					asttablearray.push_back( pTableInfo );
					maptableheadlist.insert( std::make_pair( ifn, asttablearray ) );
				}
			}
		}
	}

	return true;
}

void CXmlParser::clearMaptable( mapAligndatatablelist& maptableheadlist )
{
	mapAligndatatablelist::iterator it;

	for ( it = maptableheadlist.begin(); it != maptableheadlist.end(); ++it )
	{
		astTablelist::iterator listItor;

		for ( listItor =  it->second.begin(); listItor != it->second.end(); ++listItor )
		{
			if ( ( *listItor ) != NULL )
			{
				delete ( *listItor );
				*listItor = NULL;
			}
		}
	}
}

void CXmlParser::CreateColumnElementNode( seedInfo* info, SubConfigInfo& subConfig, uint32_t itfNum, DOMDocument* doc, DOMElement* parent )
{

}

void CXmlParser::CreateParameterElementNode( SubConfigInfo& subConfig, uint32_t itfNum, DOMDocument* doc, DOMElement* parent )
{
	StrX atrrName( parent->getAttribute( X( "genFuncName" ) ) );
	if ( strcasecmp(atrrName, "accumulate") != 0 )
	{
		return;
	}

	std::string paramList[][2] = { {"prefix", "0"}, {"init", "100000001"}, {"step", "1"}, {"lowrange", "1"}, {"highrange", "1"}, {"imsitype", "1"}, {"percent", "100"}};
	std::string itfStr = to_string(itfNum);
	auto itfLen = itfStr.size();
	std::string padding;
	if( itfLen < 6 )
	{
		padding.assign(6-itfLen, '0'); 
	}
	paramList[0][1] = itfStr + padding;//prefix = itfNum + N 0(total 6)
	paramList[3][1] = paramList[0][1] + paramList[1][1];//lowrange = prefix+init

	char * pEnd;
	uint64_t highNum = strtoull(paramList[3][1].c_str(), &pEnd, 10) + subConfig.imsiNum - 1;
	paramList[4][1] = to_string(highNum); //highrange = lowrange + itfNum - 1

	paramList[6][1] = to_string(subConfig.percent); 

	uint32_t paramNum = sizeof(paramList) / sizeof(paramList[0]);
	for(uint32_t i = 0; i < paramNum; i++)
	{
		DOMElement* parameterElem = CreateParameterNode( doc, paramList[i][0].c_str(), paramList[i][1].c_str() );
		parent->appendChild( parameterElem );
	}

	return;
}

DOMElement* CXmlParser::CreateParameterNode( DOMDocument* doc, const char* attrValue, const char* testValue )
{
	DOMElement* ParameterElem = doc->createElement( X( "parameter" ) );
	ParameterElem->setAttribute( X( "name" ), X( attrValue ) );

	DOMText*  paraText = doc->createTextNode( X( testValue ) );
	ParameterElem->appendChild( paraText );

	return ParameterElem;
}

CAlignDataObject* CXmlParser::GetColObject( const ColumnArray& colarray, uint8_t idx, uint8_t& pos )
{
	ColumnArray::const_iterator it;

	pos = 0;

	for ( it = colarray.begin(); it != colarray.end(); ++it )
	{
		if ( ( ( DimensionFileColumn* )( *it ) )->getColidx() == idx )
		{
			return ( *it );
		}

		pos++;
	}

	return NULL;
}

bool CXmlParser::CreateIdxForFiles()
{

	CXmlParser::mappingKeyInfo::const_iterator it;

	for ( it = m_KeyInfoMap.begin(); it != m_KeyInfoMap.end(); ++it )
	{
		std::string filename = it->second.m_pDimensionFile->getObjectName();

		const DimensionfileDatas* pfileData = findDimensionFileDatas( filename );

		if ( pfileData == NULL )
		{
			ACE_DEBUG((LM_ERROR, "can not find the dimensionFile:%s's Data,please check whether the data has been Loaded!.\n",
						filename.c_str()));
			return false;
		}

		ACE_DEBUG((LM_INFO, "starting creating the idx for dimensionfile %s\n", filename.c_str()));

		ACE_DEBUG((LM_INFO, "idx [ "));
		ColumnArray::const_iterator coutit;

		for ( coutit = it->second.m_CollumnArray.begin(); coutit != it->second.m_CollumnArray.end(); ++coutit )
		{
			ACE_DEBUG((LM_INFO, "%s ", ( *coutit )->getObjectName().c_str()));
		}

		ACE_DEBUG((LM_INFO, " ]"));

		DimensionfileDatas::const_iterator rowit;

		for ( rowit = pfileData->begin(); rowit != pfileData->end(); ++rowit )
		{
			uint32_t hashKey2 = createHashKey( *rowit, it->second.m_CollumnArray );
			uint32_t hashKey1 = 0;//m_AlignDataTypeDef.RSHash( ( char* ) filename.c_str() );

			doublehashIdx temphash;
			temphash.colarrayIdx = hashKey2;
			temphash.fileIdx = hashKey1;

			dimensionfilemapping::const_iterator idxIt = m_idxtableData.find( temphash );

			if ( idxIt != m_idxtableData.end() )
			{
				idxIt->second->push_back( ( *rowit ) );
			}
			else
			{
				DimensionfileDatas* pRows = new DimensionfileDatas();
				pRows->push_back( *rowit );
				m_idxtableData.insert( std::make_pair( temphash, pRows ) );
			}
		}
	}

	return true;
}

uint32_t CXmlParser::createHashKey( const Dimensioncolvalues* rowdata, const ColumnArray& keyCollumn )
{
	ColumnArray::const_iterator colit;
	char hashstring[256] =
	{ 0 };

	for ( colit = keyCollumn.begin(); colit != keyCollumn.end(); ++colit )
	{
		uint8_t mappingidx = ( ( DimensionFileColumn* )( *colit ) )->getMappingidx();

		baseType* value = ( *rowdata )[mappingidx - 1];

		char strvalue[64] =
		{ 0 };

		sprintf( hashstring + strlen( hashstring ), "%s", DimensionFileColumn::GetColvaluestringforhash( value, strvalue ) );
	}


	ACE_DEBUG((LM_DEBUG, "Current hash string is %s.\n", hashstring));

	return 1;//m_AlignDataTypeDef.RSHash( hashstring );
}

bool CXmlParser::anyzeDepDimensionInfo( const std::string& parameterName, const std::string& ParameterValue, CAlignDataObject*& pDimentsionFile,
		CAlignDataObject*& pDimensionColumn )
{

	std::string strTableName, strColName;
	uint32_t rowNum;

	m_AlignDataTypeDef.ParserTableInfoFromString( ParameterValue.c_str(), ParameterValue.length(), strTableName, rowNum, strColName );

	if ( !strTableName.empty() && !strColName.empty() )
	{
		pDimentsionFile = findlitealigndataObjectFileInMapper( strTableName, litealigndata::ELEMENT_DimensionalFile );

		if ( pDimentsionFile == NULL )
		{
			ACE_DEBUG((LM_ERROR, "current depended dimensionObject: %s does not exists, check the xml's configuration of CParameterObject = %s\n",
						strTableName.c_str(), parameterName.c_str()));
			return false;
		}

		pDimensionColumn = pDimentsionFile->findChildObject( strColName );

		if ( pDimensionColumn == NULL )
		{
			ACE_DEBUG((LM_ERROR, "current depended dimensionColumn: %s does not exists, check the xml's configuration of CParameterObject = %s.\n",
						strColName.c_str(), parameterName.c_str()));
			return false;
		}

		return true;
	}

	ACE_DEBUG((LM_ERROR, "can not find relative table name or column name for parameterName:%s and value:%s.\n",
				parameterName.c_str(), ParameterValue.c_str()));
	return true;
}

bool CXmlParser::setTableDefineInfo( uint16_t table_id, TableDefine* p_tableDef, uint16_t entryNum)
{
	if ( table_id >= 256 )
		return false;

	// m_AllTableDefine[table_id].tableinfo = const_cast<ptrTabDef>(p_tableDef);
	// m_AllTableDefine[table_id].body_entry_num = entryNum;
	// m_AllTableDefine[table_id].local_key_pos = 0xFFFE;

	return true;
}

bool CXmlParser::InitialCorrelation(bool isExtractCorrelation, bool isSipPrackDrop)
{
	return true;
}

CAlignDataObject* CXmlParser::getCInterfaceObject(std::string& strInfname)
{
	listInterface::iterator it;

	for ( it = m_Interfacelist.begin(); it != m_Interfacelist.end(); ++it )
	{
		std::string strName = ( ( CInterfaceObj* )( *it ) )->getObjectName();
		if (  !strName.compare(strInfname) )
		{
			return (*it);
		}

	}

	return NULL;

}

bool CXmlParser::parse_node(listInterface::iterator it, uint32_t& columnNo)
{
	litealigndata::elementTag tag = (*it)->getElementType();
	std::string ObjectName = (*it)->getObjectName();
    
    // 处理Column节点（基础类型）
    if (litealigndata::ELEMENT_Column == tag) 
	{
		//m_itemVector->push_back(*it);
		std::string type;
		if ( !(*it)->findAttriValue( litealigndata::ATTR_Type, type ) )
		{
			return false;
		}

		uint32_t len = litealigndata::findNametypeLen((char*)type.c_str());
		(*it)->setOffset(m_offset);
		(*it)->setLen(len);
		m_offset += len;
		
		// 更新嵌套的结构体/联合体长度
		if (!stack_is_empty(&m_stack)) 
		{
			NestedElement* current = stack_top(&m_stack);
			if (strcmp(current->type, "struct") == 0)
			{
				current->total_length += len; // 结构体累加
			}
			else if (strcmp(current->type, "union") == 0)
			{
				// 联合体暂存所有成员长度
				dynamic_array_add(&current->member_lengths, len);
				current->total_length = dynamic_array_max(&current->member_lengths);
			}
		}
		columnNo++;
		return true;
    }

   // 处理Struct节点（结构体）
	else if(litealigndata::ELEMENT_Struct == tag)
	{
      	// 初始化结构体元素
    	NestedElement struct_elem = {0};
    	strncpy(struct_elem.name, ObjectName.c_str(), sizeof(struct_elem.name) - 1);
    	strncpy(struct_elem.type, "struct", sizeof(struct_elem.type) - 1);
    	struct_elem.total_length = 0;
    	dynamic_array_init(&struct_elem.member_lengths);
      
    	// 压栈
    	if (stack_push(&m_stack, struct_elem) != 0)
		{
        	dynamic_array_free(&struct_elem.member_lengths);
        	return false;
    	}
		m_strcut_offset = m_offset;
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
		for ( listInterface::iterator  subStruct = obj.begin(); subStruct != obj.end(); ++(subStruct) )
		{
        	parse_node(subStruct, columnNo);
    	}
		if((*it)->getObjectName() == "BodyData")
		{
			(*it)->setColumnNo(columnNo);
		}
		

		// 弹栈并输出结构体信息
		NestedElement popped = stack_pop(&m_stack);
		//printf("name=%s,len=%zu,type=struct\n", popped.name, popped.total_length);
		(*it)->setOffset(m_strcut_offset);
		(*it)->setLen(popped.total_length);
		m_offset = m_strcut_offset + popped.total_length;
		dynamic_array_free(&popped.member_lengths);
      
		// 更新父节点长度
		if (!stack_is_empty(&m_stack)) 
		{
			NestedElement* parent = stack_top(&m_stack);
			if (strcmp(parent->type, "struct") == 0) 
			{
				parent->total_length += popped.total_length;
			}
			else if (strcmp(parent->type, "union") == 0) 
			{
				dynamic_array_add(&parent->member_lengths, popped.total_length);
				parent->total_length = dynamic_array_max(&parent->member_lengths);
			}
		}
        return true;
	}
   // 处理Union节点（联合体）
    else if (litealigndata::ELEMENT_Union == tag) 
	{
       // 初始化联合体元素
       NestedElement union_elem = {0};
       strncpy(union_elem.name, ObjectName.c_str(), sizeof(union_elem.name) - 1);
       strncpy(union_elem.type, "union", sizeof(union_elem.type) - 1);
       union_elem.total_length = 0;
       dynamic_array_init(&union_elem.member_lengths);
       
       // 压栈
       if (stack_push(&m_stack, union_elem) != 0)
	   {
           dynamic_array_free(&union_elem.member_lengths);
           return false;
       }
       m_union_offset = m_offset;
       // 递归解析子节点
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
		for ( listInterface::iterator  subUnion = obj.begin(); subUnion != obj.end(); ++(subUnion) )
		{
        	parse_node(subUnion, columnNo);
    	}
       
       // 弹栈并输出联合体信息
       NestedElement popped = stack_pop(&m_stack);
       //printf("name=%s,len=%zu,type=union\n", popped.name, popped.total_length);
	   (*it)->setOffset(m_union_offset);
	   (*it)->setLen(popped.total_length);
		m_offset = m_union_offset + popped.total_length;
        dynamic_array_free(&popped.member_lengths);
       
       // 更新父节点长度
       if (!stack_is_empty(&m_stack)) 
	   {
           NestedElement* parent = stack_top(&m_stack);
           if (strcmp(parent->type, "struct") == 0)
		   {
               parent->total_length += popped.total_length;
           }
		   else if (strcmp(parent->type, "union") == 0)
		   {
               dynamic_array_add(&parent->member_lengths, popped.total_length);
               parent->total_length = dynamic_array_max(&parent->member_lengths);
           }
       }
       return true;
    }
	else if(litealigndata::ELEMENT_Head == tag)
	{
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
		for ( listInterface::iterator  head = obj.begin(); head != obj.end(); ++(head) )
		{
        	parse_node(head, columnNo);
    	}
		(*it)->setOffset(m_offset);

	}
	else if(litealigndata::ELEMENT_Body == tag)
	{
		m_offset = 0;
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
		for ( listInterface::iterator  body = obj.begin(); body != obj.end(); ++(body) )
		{
        	parse_node(body, columnNo);
    	}
		(*it)->setOffset(m_offset);
	}
    else if(litealigndata::ELEMENT_Interface == tag)
    {
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
		for ( listInterface::iterator  table = obj.begin(); table != obj.end(); ++(table) )
		{
        	parse_node(table, columnNo);
    	}
		m_offset = 0;
    }
    else if(litealigndata::ELEMENT_Table == tag)
    {
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
		for ( listInterface::iterator  headbody = obj.begin(); headbody != obj.end(); ++(headbody) )
		{
        	parse_node(headbody, columnNo);
    	}
		m_offset = 0;
		(*it)->Initialize();
		(*it)->processItem();
    }
    else
    {
        //printf("Not Support ObjectName name=%s\n", ObjectName.c_str());
		return true;
    }
	return true;
}

void CXmlParser::SetAllItemOffset()
{
	uint32_t columnNo = 0;
    stack_init(&m_stack);
    for ( listInterface::iterator  it = m_Interfacelist.begin(); it != m_Interfacelist.end(); ++(it) )
	{
        parse_node(it, columnNo);

    }

}
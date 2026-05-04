/*
 * =============================================================================
 *
 *       Filename:  CTableObject.cc
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
#include <iomanip>
#include <atomic>
#include <vector>
#include <future>
#include <memory>
#include <atomic>
#include <thread>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "com/Comm.h"
#include "log/LogHandler.h"
#include "conf/AlignConfig.h"
#include "sub/parser/XmlParser.h"
#include <xercesc/dom/DOM.hpp>
#include "fun/FunctionObject.h"
#include "struct/TableObject.h"
#include "boost/lockfree/queue.hpp"
#include "xercesc/dom/DOMAttr.hpp"

using namespace std;

static const uint32_t       BIA_TX_ENCODESZ = 262140; // max encoded size in bytes
static const char             LENGTH_MSGHEADER = 4;
const uint32_t       ENTRY_BUFFER_SIZE = BIA_TX_ENCODESZ + LENGTH_MSGHEADER; //256kB
const uint32_t       NTC_ENTRY_BUFFER_SIZE = 2048; //2kB

#define isTxtTable(tableid)\
	(\
	 (|(EXAMPLE_TABLE_ID == tableid)\
	 )\
	)
CTableObject::CTableObject( litealigndata::elementTag tag ) :
	CAlignDataObject( tag ), m_status( NotReady ), m_rownum( MAX_UINT32 ), m_curFilledCount( 0 ), m_nextFilledPosition( 0 ), m_createdTableNum( 0 ), m_pData( NULL ), m_fp( NULL ), m_seedfilenum( 0 ), m_seedfilesize( 0 ), m_seedfileReadlineOrWhole(false), m_templateId(0), m_isRandomOrder(false),m_BaseFileFormat(NULL),
	thread_index(0), m_pTableDef(NULL), m_tableDefNum(0), pExtractedData( NULL ),RowsPerTimeInterval(0),nextTimeEdge(0),
	rowsAccumulated_interval(0),rowsAccumulated(0), totalArborRowsAccumulated(0), bytesAccumulated(0), lastBytesAccumulated(0), MessagesAccumulated(0), 
	lastMessagesAccumulated(0), timeTableBorn(0),m_total_size(0),m_fd(0),interval_stats("I_"),lastSmsPayloadEnd(0)
{
	RAND_GEN = Random::getIns();

}

CTableObject::CTableObject( const CTableObject& rhs) :
	CAlignDataObject( rhs ), m_status( NotReady ), m_tableHeader( rhs.m_tableHeader ), m_rownum( rhs.m_rownum ), m_curFilledCount( 0 ), m_nextFilledPosition( 0 ),m_interval(rhs.m_interval), m_createdTableNum(
			0 ), m_pData( NULL ), m_fp( rhs.m_fp ), m_seedfilenum( rhs.m_seedfilenum), m_seedfilesize( rhs.m_seedfilesize), m_seedfileReadlineOrWhole( rhs.m_seedfileReadlineOrWhole), 
	m_tableType( rhs.m_tableType ), m_templateId(rhs.m_templateId), m_isRandomOrder(rhs.m_isRandomOrder), thread_index(rhs.thread_index),
	m_pTableDef(rhs.m_pTableDef), m_tableDefNum(rhs.m_tableDefNum), RAND_GEN(rhs.RAND_GEN),
	pExtractedData( rhs.pExtractedData ),RowsPerTimeInterval(rhs.RowsPerTimeInterval),nextTimeEdge(rhs.nextTimeEdge)
	,rowsAccumulated_interval(0),rowsAccumulated(0), totalArborRowsAccumulated(rhs.totalArborRowsAccumulated), bytesAccumulated(0), lastBytesAccumulated(rhs.lastBytesAccumulated), 
	MessagesAccumulated(0), lastMessagesAccumulated(rhs.lastMessagesAccumulated), timeTableBorn(rhs.timeTableBorn),interval_stats(rhs.interval_stats),lastSmsPayloadEnd(rhs.lastSmsPayloadEnd)
{
	m_pParent =rhs.m_pParent;
}

CTableObject& CTableObject::operator=( const CTableObject& rhs)
{
	if ( &rhs == this )
		return *this;

	CAlignDataObject::operator= (rhs);
	m_status = NotReady;
	m_tableHeader = rhs.m_tableHeader;
	m_rownum = rhs.m_rownum;
	m_curFilledCount = 0;
	m_nextFilledPosition = 0;
	m_createdTableNum =  0;
	m_pData =  NULL;
	m_fp = rhs.m_fp;
	m_seedfilenum = rhs.m_seedfilenum;
	m_seedfilesize = rhs.m_seedfilesize;
	m_seedfileReadlineOrWhole = rhs.m_seedfileReadlineOrWhole;
	m_tableType = rhs.m_tableType;
	m_templateId = rhs.m_templateId;
	m_pParent =rhs.m_pParent;
	m_isRandomOrder = rhs.m_isRandomOrder;

	return *this;
}

uint64_t CTableObject::getFileSize()
{
	uint64_t total_size;

	total_size = (uint64_t)m_num_entries * m_tableHeader.entry_size;

	return total_size;
}

bool CTableObject::Init()
{
	return true;
}

bool CTableObject::Initialize( DOMNode* pNode, CAlignDataObject* pParent )
{
	//adapt_XML_parameter(pNode);

	if ( !CAlignDataObject::Initialize( pNode, pParent ) )
		return false;
	CInterfaceObj* InterfaceObj = ( CInterfaceObj* ) getParent();

	if ( !InterfaceObj )
	{
		ACE_DEBUG((LM_ERROR, "the Parents object is NULL, the table Object is %s.\n",
					this->getObjectName().c_str()));
		return false;
	}

	m_pinterface = InterfaceObj;
	InterfaceObj->addObjectInMapper( this->getObjectName(), this );

	std::string strInterval;

	if ( !findAttriValue( litealigndata::ATTR_percentage_period_rows, strInterval )
			|| !m_AlignDataTypeDef.isAlldigits( strInterval.c_str(), strInterval.length() ) )
	{
	}
	else
	{
		//percentage_period_rows = atoi( strInterval.c_str() );        
	}

	if ( !findAttriValue( litealigndata::ATTR_Interval, strInterval )
			|| !m_AlignDataTypeDef.isAlldigits( strInterval.c_str(), strInterval.length() ) )
	{
		ACE_DEBUG((LM_ERROR, "current table object %s can not find the interval attribute.\n",
					this->getObjectName().c_str()));
		return false;
	}

	m_interval = atoi( strInterval.c_str() );
	InterfaceObj->setInterval (m_interval);

	return true;
}
bool CTableObject::processItem()
{
	std::string seedfile;

	if ( !findAttriValue( litealigndata::ATTR_SeedFile, seedfile ) )
	{
		ACE_DEBUG((LM_ERROR, "current table object %s can not find the SeedFile attribute.\n",
					this->getObjectName().c_str()));
		return false;
	}

	if ( !findAttriValue( litealigndata::ATTR_TableType, m_tableType ) )
	{
		ACE_DEBUG((LM_ERROR, "current table object %s can not find the tableType attribute.\n",
					this->getObjectName().c_str()));
		return false;
	}

	uint32_t get_table_id = 0;
	std::string strTableId;

	if ( !findAttriValue( litealigndata::ATTR_tableid, strTableId ) )
	{
		ACE_DEBUG((LM_ERROR, "current table object %s can not find the table-id attribute.\n", 
				seedfile.c_str()));
	return false;
	}

	get_table_id = ( uint8_t ) atoi( strTableId.c_str() );

	if ( !seedfile.empty() )
	{
		if (( m_tableType.compare( "example" ) == 0 ) ||
		( m_tableType.compare( "example1" ) == 0 ) )
		{
			if(!m_BaseFileFormat)
				m_BaseFileFormat = new CExampledata();
		}
		else if (( m_tableType.compare( "Ctrldata_keys" ) == 0 )  ||
				( m_tableType.compare( "Ctrldata_stats" ) == 0 ))
		{
			if(!m_BaseFileFormat)
				m_BaseFileFormat = new CCtrldata();
		}
		else
		{
			ACE_DEBUG((LM_ERROR, "current seedfile %s has wrong table type %s.\n", 
						seedfile.c_str(), m_tableType.c_str()));
			return false;
		}

		memset( &m_tableHeader, 0, sizeof( m_tableHeader ) );

		//m_BaseFileFormat->fill_tableHeader(m_tableType.c_str(), seedfile.c_str(), &m_tableHeader, getParser()->getTableTypeIdMap(), getParser(), get_table_id);
		
		m_num_entries = countFileLine(seedfile.c_str());
		CAlignDataObject* Body = findChildObject("Body");
		//CAlignDataObject* BodyData =  Body->findChildObject("BodyData");
		m_entry_size = Body->getOffset();

		//uint16_t m_entry_size = getParser();
		if ( !m_BaseFileFormat->ReadData( seedfile, getParser(), this ) )
		{
			ACE_DEBUG((LM_ERROR, "parse_call_proto seedfile failed: %s.\n", seedfile.c_str()));
			return false;
		}
		SetAlignseedfilenum();
	}

	std::string strFileNum;

	if ( !findAttriValue( litealigndata::ATTR_FileNum, strFileNum ) )
	{
		ACE_DEBUG((LM_ERROR, "current table object %s can not find the FileNum attribute.\n", 
					this->getObjectName().c_str()));
		return false;
	}

	m_toCreateTableNum = atoi( strFileNum.c_str() );
	return true;
}

CTableObject::~CTableObject() 
{
	if(m_pData)
	{
		munmap((char*)m_pData, m_total_size);
	}

    if(m_BaseFileFormat)
	{
		delete m_BaseFileFormat;
	}
	for(auto children : m_Children)
	{
		CColumnObject* obj = static_cast<CColumnObject*>(children);
		delete obj;
	}

}

bool CTableObject::processOjbect( CAlignDataObject* root )
{
	ACE_DEBUG((LM_DEBUG, "Current Get the Entry Size: %u.\n", m_tableHeader.entry_size));

	const CAlignDataObject::mapAttribute attrList = this->getAttributes();
	CAlignDataObject::mapAttribute::const_iterator it;

	std::string strTableName;
	std::string strColName;
	uint32_t RowNumber = 0;

	for ( it = attrList.begin(); it != attrList.end(); ++it )
	{
		m_AlignDataTypeDef.ParserTableInfoFromString( it->second.c_str(), it->second.length(),
				strTableName, RowNumber, strColName );

		if ( strTableName.empty() || strColName.empty() )
			continue;

		CAlignDataObject* pTableObj = m_pParent->findChildObject( strTableName );

		CTableObject* pTable = dynamic_cast<CTableObject*>( pTableObj );

		if ( pTable == NULL )
		{
			ACE_DEBUG((LM_ERROR, "Current Depended table = %s for attribute is %s.\n", 
						strTableName.c_str(), it->second.c_str()));
			return false;
		}

		CAlignDataObject* pColumnObj = pTable->findColumnObject( strColName );

		if ( pColumnObj == NULL )
		{
			ACE_DEBUG((LM_ERROR, "Current Depended column info = %s for attribute is %s.\n", 
						strColName.c_str(), it->second.c_str()));
			return false;
		}

		DependencyInfo info;
		info.m_pColumnInfo = pColumnObj;
		info.m_pTableInfo = pTableObj;
		info.m_rowNum = RowNumber;

		this->addAttrDependency( it->first, info );

		this->m_depTables.push_back( info );

		pTable->addDependedtable( this );
	}

	if ( !calculateRowNum() )
	{
		ACE_DEBUG((LM_ERROR, "calculateRowNum failed for table object %s.\n", this->getObjectName().c_str()));
		return false;
	}

	//uint64_t total_size = 0L;
	{
		m_total_size = (uint64_t)m_rownum * m_entry_size;
	}

	m_pData = (uint8_t*)mmap(NULL, m_total_size, 
				PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS,-1,0);
	
	if ( m_pData == NULL )
	{
		ACE_DEBUG((LM_ERROR, "can not allocate %Q size memory for table %s.\n", 
					m_total_size, this->getObjectName().c_str()));
		return false;
	}

	ACE_DEBUG((LM_DEBUG, "allocate memory for %u records of table %s, the total memory size is %Q.\n", 
				m_rownum, getObjectName().c_str(), m_total_size));
	
	memset( m_pData, 0, m_total_size );
	

	uint64_t curtime = time( NULL );
	{
		m_InitialTime = m_AlignDataTypeDef.calc_align( curtime, m_interval * 60 );
	}
	CInterfaceObj* InterfaceObj = ( CInterfaceObj* ) this->getParent();
	InterfaceObj->setInitialTime(m_InitialTime) ;
	timeTableBorn = curtime-1;          //time-1 avoid run too fast which making devie / time(0) core down.
	interval_stats.time_Pre =curtime-1; //time-1 avoid run too fast which making devie / time(0) core down.

	uint64_t interval_sec = m_interval * 60;
	nextTimeEdge = (curtime/interval_sec + 2) * interval_sec;

	if((EXAMPLE_TABLE_ID == m_tableHeader.table_id))
	{
		std::string seedfile;
		{
			//read the seed file's header.            
			if ( !findAttriValue( litealigndata::ATTR_SeedFile, seedfile ) )
				assert(0);
		}    
		readExampleFile.open(seedfile, ios::in);        
	}
	return true;
}

bool CTableObject::streamTableData(bufferevent *buffev)
{
	uint8_t* pDrTable = m_pData;
	CInterfaceObj* InterfaceObj = ( CInterfaceObj* ) getParent();
	CAlignDataObject* Body = findChildObject("Body");
	CAlignDataObject* BodyData =  Body->findChildObject("BodyData");

	for ( uint32_t rownum = 0; rownum < this->m_rownum; ++rownum )
	{
		if ( !ProducedRowData( this ) )
			return false;
	}
	//tabledef* AllTableDefine = getParser()->GetAllTableDefine();
	CAlignDataObject::LitealigndataObjectList& body_childList = BodyData->getLitealigndataObjectList();
	m_BaseFileFormat->TableGeneratedStreamData(this, pDrTable, Body, buffev);

	InterfaceObj->setInitialTime( m_InitialTime );
	InterfaceObj->setInterval(m_interval);
	
	m_InitialTime += m_interval * 60;

	finishtoCreateTableNum();	
	return true;
}

bool CTableObject::createTableData()
{
	CInterfaceObj* InterfaceObj = ( CInterfaceObj* ) getParent();
	if ( InterfaceObj == NULL )
	{
		ACE_DEBUG((LM_ERROR, "the parent node is null when table (%s) creating  Data!\n",
					getObjectName().c_str()));
		return false;
	}

	ACE_DEBUG((LM_DEBUG, "\nstart creating data for table = %s\n", this->m_ObjectName.c_str()));

	CAlignDataObject* Body = findChildObject("Body");
	CAlignDataObject* BodyData =  Body->findChildObject("BodyData");

	for ( uint32_t rownum = 0; rownum < this->m_rownum; ++rownum )
	{
		if ( !ProducedRowData( this ) )
			return false;
		//createData();
	}
	std::string FileName = { 0 };
    m_BaseFileFormat->GetFilename(this, FileName);

	uint8_t* pDrTable = m_pData;

	//tabledef* AllTableDefine = getParser()->GetAllTableDefine();
	CAlignDataObject::LitealigndataObjectList& body_childList = BodyData->getLitealigndataObjectList();
    m_BaseFileFormat->TableGeneratedData(this, FileName, pDrTable, Body);

	InterfaceObj->setInitialTime( m_InitialTime );
	InterfaceObj->setInterval(m_interval);
	
	m_InitialTime += m_interval * 60;   

	finishtoCreateTableNum();
	
	return true;
}
void CTableObject::SendTableData(const char* ptr, uint64_t total_len, bufferevent *buffev)
{
    struct timeval start;
    struct timeval end;
    gettimeofday( &start, NULL );

    time_t time_elapsed = 0;
    uint64_t bytes_sent = 0;
        
    uint64_t rate_limit = SEND_BIT_RATE;
    uint64_t bytes_can_send = 0;
    uint64_t to_send = 0;

    uint64_t start_time = start.tv_sec*1000000 + start.tv_usec;
    ACE_DEBUG((LM_DEBUG, "send bytes rate is 5Mb/s.\n"));
    printf("bytes_sent  %ld, total_len %ld\n", bytes_sent, total_len);
    while(bytes_sent < total_len)
    {
            //
        gettimeofday( &end, NULL );
        time_elapsed = end.tv_sec*1000000 + end.tv_usec - start_time;

        bytes_can_send = ((rate_limit) * time_elapsed) - bytes_sent;

        if(bytes_can_send <= 0)
        {
                //printf("%ld, %ld, %ld \n", rate_limit, time_elapsed, (rate_limit) * time_elapsed);
                //usleep(((-bytes_can_send) / rate_limit));
            usleep(100000);
                //gettimeofday( &start, NULL );
                //start_time = time(NULL);
            continue;
        }

        to_send = min(bytes_can_send, total_len - bytes_sent);
            
        bufferevent_flush(buffev, EV_WRITE, BEV_FLUSH);
        if(bufferevent_write(buffev, ptr, to_send) == -1)
        {
            continue;
        }
        bufferevent_flush(buffev, EV_WRITE, BEV_FLUSH);
        bytes_sent += to_send;
        ptr += to_send;
    };
    printf("send rate: %ld, %ld, %ld\n", bytes_sent, total_len, time_elapsed);
    ACE_DEBUG((LM_DEBUG, "send rate: %ld, %ld, %ld\n", bytes_sent, total_len, time_elapsed));
    finishtoCreateTableNum();
}
bool CTableObject::createData()
{

	m_curFilledCount++;
	{
		m_nextFilledPosition += m_entry_size;//m_num_entries;//m_tableHeader.entry_size;
	}

	return true;
}

//start to produce data.
bool CTableObject::notifyAlldepenedTable(bufferevent *buffev)
{
	//get the current status.
	TableStatus st = checkStatus();

	if ( st == Ready )
	{
		if(!CAlignConfig::GetInstance()->GetStream())
		{
			if ( createTableData() )
			{
				ACE_DEBUG((LM_DEBUG, "table :[%s] was Done on FileMode\n", \
							this->getObjectName().c_str()));
				m_status = Done;
			}
			else
				return false;
		}
		else
		{
			if(streamTableData(buffev))
			{
				ACE_DEBUG((LM_DEBUG, "table :[%s] was Done on StreamMode\n", \
					this->getObjectName().c_str()));
				m_status = Done;
			}
			else
				return false;
		}
	}

	if ( m_status == Done )
	{
		dependedlist::iterator it;

		for ( it = m_beDependedTables.begin(); it != m_beDependedTables.end();
				++it )
		{
			{
				CTableObject* pTable_Depend = *it;
				CInterfaceObj* const parent_1 = dynamic_cast<CInterfaceObj*>(pTable_Depend->getParent());
				CInterfaceObj* const parent_2 = dynamic_cast<CInterfaceObj*>(this->getParent());
				if(parent_1 != parent_2)
				{
					ACE_DEBUG((LM_ERROR, "Depend_Parent :[%s] and getParent :[%s] are not belong the \
						same interface\n", pTable_Depend->getObjectName().c_str(), \
						this->getObjectName().c_str()));
					continue;
				}
			}
			if ( !( *it )->notifyAlldepenedTable(buffev) )
			{
				ACE_DEBUG((LM_ERROR, "current_table :[%s] notifyAlldepenedTable failed\n", \
						this->getObjectName().c_str()));
				return false;
			}
		}
	}

	return true;

}

void CTableObject::addDependedtable( CTableObject* ptable )
{
	this->m_beDependedTables.push_back( ptable );
}

CAlignDataObject* CTableObject::findColumnObject( const std::string& colname )
{
	LitealigndataObjectList::const_iterator it;
	
	for(auto children : m_Children)
	{
		for ( it = (children->getLitealigndataObjectList()).begin(); 
			it != (children->getLitealigndataObjectList()).end(); ++it )
		{
			for(auto it1 : (*it)->getLitealigndataObjectList())
			{
				if ( ( it1 )->getElementType() == litealigndata::ELEMENT_Column
						&& ( it1 )->getObjectName() == colname )
					return ( it1 );
				//cout << "( *it )->getObjectName()  " << ( *it )->getObjectName() << endl;
			}

		}
	}
	
	//cout << "colname  " << colname << endl;

	return NULL;

}

void CTableObject::addAttrDependency( litealigndata::AttrTag tag, const DependencyInfo& pObj )
{
	this->m_AttrDepInfos.insert( std::make_pair( tag, pObj ) );
}

void CTableObject::addDependencyInfo( const DependencyInfo& info )
{
	m_depTables.push_back( info );
}

bool CTableObject::ProducedRowDataPercent( CAlignDataObject* parent )
{
	CAlignDataObject::LitealigndataObjectList& parent_childList = parent->getLitealigndataObjectList();

	CAlignDataObject::LitealigndataObjectList::iterator& myPercentit = parent->getPercentSelectIter(); 

	do
	{
		CAlignDataObject*& pChild = *myPercentit;

		if(pChild->PercentCount.rows_created >= pChild->PercentCount.rows_to_create){
			pChild->PercentCount.rows_created = 0;
			myPercentit++;
			if(parent_childList.end() == myPercentit)
				myPercentit = parent_childList.begin();
		}
		else
		{
			ProducedRowData(pChild);
			pChild->PercentCount.rows_created++;

			break;
		} 
	}while(true); //will not dead loop, because parent row number == sum of all child number

	if ( !parent->createData() ){
		return false;
	}

	return true;
}


bool CTableObject::ProducedRowData( CAlignDataObject* parent )
{    
	if(false == parent->hasPercentChild())
	{ 
		CAlignDataObject::LitealigndataObjectList& parent_childList = parent->getLitealigndataObjectList();    
		CAlignDataObject::LitealigndataObjectList::iterator it = parent_childList.begin();
		for ( ;it != parent_childList.end(); ++it )
		{
			//cout << (*it)->getObjectName() << endl;
			if ( !CTableObject::ProducedRowData( *it ) )
				return false;
		}

		if ( !parent->createData() )
			return false;

		return true;

	}
	else{
		return ProducedRowDataPercent(parent);
	}    
}

bool CTableObject::calculateRowNum()
{

	//recalculate the table's Row-number;
	FunctionObject* object = 
	( FunctionObject* )findAttributeObject( litealigndata::ATTR_RowNum );

	if ( object == NULL )
	{
		//get the row number
		std::string strRownum;

		if ( !findAttriValue( litealigndata::ATTR_RowNum, strRownum ) && 
		!findAttriValue( litealigndata::ATTR_RowNumPerFlow, strRownum ))
		{
			ACE_DEBUG((LM_ERROR, "current table object %s can not find the row-number attribute.\n",
						this->getObjectName().c_str()));
			return false;
		}

		//according to the strRownumber
		m_rownum = atol( strRownum.c_str() );

		// //get the file number.
		// if ( m_rownum > m_tableHeader.max_entries )
		// {
		// 	ACE_DEBUG((LM_WARNING, "the row_number %u is more than the max_entries of the table (%s), it will be set the max_entries = %u \n"
		// 				"user can edit ALIGNDATA file header of seedfile to make max_entries more larger to avoid error. And raise a Jira ticket!",
		// 				m_rownum, getObjectName().c_str(), m_tableHeader.max_entries));

		// 	m_rownum = m_tableHeader.max_entries;
		// }

		if ( m_rownum == 0 && m_seedfilenum != 0 )
			m_rownum = MAX_UINT32;
	}
	else
	{
		CBaseFunc* pfunc = object->getFunc();

		if ( pfunc == NULL )
			return false;

		if ( pfunc->getTag() == litealigndata::FUNC_REDEFINENUMBER )
			( ( RedefineRowFunc* )pfunc )->setFilenumber( m_toCreateTableNum );

		pfunc->getValue().clearData();

		if ( !pfunc->Calculate() )
			return false;

		m_rownum = pfunc->getValue().getnumber();

		// if ( m_rownum > m_tableHeader.max_entries )
		// {
		// 	ACE_DEBUG((LM_WARNING, "the row_number %u is more than the max_entries of the table (%s), it will be set the max_entries = %u \n"
		// 				"user can edit ALIGNDATA file header of seedfile to make max_entries more larger to avoid error. And raise a Jira ticket!",
		// 				m_rownum, getObjectName().c_str(), m_tableHeader.max_entries));

		// 	m_rownum = m_tableHeader.max_entries;
		// }
	}

	return true;
}

void CTableObject::PrintUsingOstream( std::ostream& out )
{
	out << "  m_seedfilenum: " << m_seedfilenum << std::endl;
	out << "  m_pData: " << static_cast<const void *>(m_pData) << std::endl;
}

void CTableObject::ProduceExtractedSeedData()
{
}

void CTableObject::WriteExtractedSeedData()
{

}

bool CTableObject::fillAlltheTableData()
{
	srand( time( 0 ) + rand() );

	//this->m_tableHeader.num_entries = m_rownum;
	//m_num_entries = m_rownum;
	//if ( m_fp != NULL && m_seedfilenum != 0 )
	if(m_seedfilenum != 0)
	{
		uint32_t rownum = m_seedfilenum;

		//if ( m_seedfilenum  > m_tableHeader.num_entries )
		if ( m_seedfilenum  > m_num_entries )
			rownum = m_num_entries;

		{
			memset( m_pData, 0, (uint64_t)m_rownum * m_entry_size );

            std::string ReadlineOrWhole;
			if(findAttriValue( litealigndata::ATTR_seedfileReadlineOrWhole, ReadlineOrWhole ))
			{
                if(ReadlineOrWhole == "true")
				{
					uint64_t rowcopyed = 0;
					while ( rowcopyed < m_rownum )
					{
						memcpy( m_pData + rowcopyed * m_entry_size, 
						m_BaseFileFormat->Getbody(), 1*m_entry_size );

						rowcopyed ++;
					}
				}
				else if(ReadlineOrWhole == "false")
				{
					uint64_t rowcopyed = 0;
					while ( rowcopyed < m_rownum )
					{
						memcpy( m_pData + rowcopyed * m_entry_size, 
						m_BaseFileFormat->Getbody(), (uint64_t)rownum * m_entry_size );
						rowcopyed += rownum;

						rownum = m_rownum - rowcopyed;

						if ( rownum > m_seedfilenum )
							rownum = m_seedfilenum;
					}
				}
				else
				{
					ACE_DEBUG((LM_ERROR, "current ReadlineOrWhole %s is not correct.\n", ReadlineOrWhole.c_str()));
				}
			}
		}
	}

	return true;
}

bool CTableObject::produceTableData( uint32_t thread_index )
{

    return true;
}

void* CTableObject::allocateMemoryForData()
{
	//m_tableHeader.table_enabled = 1;
	return NULL;
}

const char* DimensionalFile::FileTypeString[] =
{ "invalid", "CSV", "database" };

bool DimensionalFile::Initialize( DOMNode* pNode, CAlignDataObject* pParent )
{
	if ( !CAlignDataObject::Initialize( pNode, pParent ) )
		return false;

	//get the parth attribute.
	std::string filePath;

	if ( !findAttriValue( litealigndata::ATTR_Path, filePath ) )
	{
		ACE_DEBUG((LM_ERROR, "current DimensionFile object %s can not find the path attribute.\n",
					this->getObjectName().c_str()));
		return false;
	}

	strcpy( m_filePath, filePath.c_str() );

	//get fileType attribute
	std::string filetype;

	if ( !findAttriValue( litealigndata::ATTR_SourceType, filetype ) )
	{
		ACE_DEBUG((LM_ERROR, "current DimensionFile object %s can not find the sourceType attribute.\n",
					this->getObjectName().c_str()));
		return false;
	}

	m_fileType = ( DimentsionFileType ) m_AlignDataTypeDef.FindRelativeTag(
			DimensionalFile::FileTypeString, DATAFileType_MAX, filetype.c_str() );

	if ( m_fileType == INVALID_TYPE )
	{
		ACE_DEBUG((LM_ERROR, "current DimensionFile object %s can not find the supported sourceType for %s.\n",
					this->getObjectName().c_str(), filetype.c_str()));
		return false;
	}

	return true;
}
int CTableObject::adapt_XML_parameter(DOMNode* pNode)
{
	if ( pNode->hasAttributes() )
	{
		DOMNamedNodeMap* pAttributes = pNode->getAttributes();
		const XMLSize_t nSize = pAttributes->getLength();

		int TableId = 0;

		for ( XMLSize_t i = 0; i < nSize; ++i )
		{
			DOMAttr* pAttributeNode = ( DOMAttr* ) pAttributes->item( i );

			StrX attrName( pAttributeNode->getName() );

			litealigndata::AttrTag tag = ( litealigndata::AttrTag ) m_AlignDataTypeDef.FindRelativeTag( litealigndata::AttrTagString, litealigndata::ATTR_Max, attrName );

			StrX value( pAttributeNode->getValue() );

			if ( tag == litealigndata::ATTR_tableid )
			{
				TableId = atoi((const char*)value);
				break;
			}         
		}        

		if((EXAMPLE_TABLE_ID == TableId))
		{
			uint64_t RowsPerTimeInterval = 0;
			bool     RowsPerTimeInterval_exist = false;

			uint64_t FileNum = 0;
			bool     FileNum_exist = false;
			uint64_t adapted_FileNum = 0;

			uint64_t RowNum  = 0;
			bool     RowNum_exist = false;
			uint64_t adapted_RowNum  = 0;

			bool     exist_RowNumEachTimeRead = false;
			uint64_t RowNum_eachTimeRead  = 100*1000;  //default 100K

			for ( XMLSize_t i = 0; i < nSize; ++i )
			{
				DOMAttr* pAttributeNode = ( DOMAttr* ) pAttributes->item( i );

				StrX attrName( pAttributeNode->getName() );

				litealigndata::AttrTag tag = ( litealigndata::AttrTag ) m_AlignDataTypeDef.FindRelativeTag( litealigndata::AttrTagString, litealigndata::ATTR_Max, attrName );

				StrX value( pAttributeNode->getValue() );

				if ( tag == litealigndata::ATTR_RowsPerTimeInterval )
				{
					RowsPerTimeInterval_exist = true;
					RowsPerTimeInterval = atoi((const char*)value);
				}
				else if ( tag == litealigndata::ATTR_FileNum )
				{
					FileNum_exist = true;
					FileNum = atoi((const char*)value);
				}
				else if ( tag == litealigndata::ATTR_RowNum )
				{
					RowNum_exist = true;
					RowNum = atoi((const char*)value);
				}                
				else if ( tag == litealigndata::ATTR_row_quantity_each_time_read )
				{
					exist_RowNumEachTimeRead = true;
					RowNum_eachTimeRead = atoi((const char*)value);
					ACE_DEBUG((LM_INFO, "row_quantity_each_time_read %Q \n",RowNum_eachTimeRead));
				}            
			}

			if(RowsPerTimeInterval_exist)
				if(RowNum_eachTimeRead > RowsPerTimeInterval)
				{
					ACE_DEBUG((LM_INFO, "Litealigndata set RowNum_eachTimeRead to <= RowsPerTimeInterval \n"));
					RowNum_eachTimeRead = RowsPerTimeInterval;                    
				}

			adpt_row_and_file(RowNum, FileNum, RowNum_eachTimeRead, adapted_RowNum, adapted_FileNum);

			ACE_DEBUG((LM_DEBUG, " %Q \n",adapted_RowNum));

			if(FileNum_exist)
			{
				this->m_attributes.erase(litealigndata::ATTR_FileNum);
				this->m_attributes.insert( std::make_pair( litealigndata::ATTR_FileNum,to_string(adapted_FileNum).c_str()));
			}

			if(RowNum_exist)
			{
				this->m_attributes.erase(litealigndata::ATTR_RowNum);
				this->m_attributes.insert( std::make_pair( litealigndata::ATTR_RowNum, 
				to_string(adapted_RowNum).c_str()));
			} 
		}   
	}

	return 0;
}

	int CTableObject::adpt_row_and_file
(uint64_t RowNum, uint64_t FileNum, uint64_t& adapted_RowNum, uint64_t& adapted_FileNum)
{
	unsigned int max_row_num = 100*1000;

	uint64_t origin_row_num  = RowNum;
	uint64_t adapted_row_num = origin_row_num;

	if(max_row_num < origin_row_num)
	{    
		for(unsigned i = max_row_num ; 0 < i ; i-- )
		{
			unsigned int result = origin_row_num % i;
			if(0 == result)
			{ 
				adapted_row_num = i;    
				break;
			}      
		}
	}

	uint64_t adapted_file_num = 0;
	adapted_file_num = origin_row_num * FileNum /adapted_row_num;

	adapted_RowNum  = adapted_row_num;
	adapted_FileNum = adapted_file_num;

	return 0;
}


	int CTableObject::adpt_row_and_file
(uint64_t RowNum, uint64_t FileNum, uint64_t RowNum_eachTimeRead, 
 uint64_t& adapted_RowNum, uint64_t& adapted_FileNum)
{
	unsigned int max_row_num = RowNum_eachTimeRead;

	uint64_t origin_row_num  = RowNum;
	uint64_t adapted_row_num = origin_row_num;

	if(max_row_num < origin_row_num)
	{    
		for(unsigned i = max_row_num ; 0 < i ; i-- )
		{
			unsigned int result = origin_row_num % i;
			if(0 == result)
			{ 
				adapted_row_num = i;    
				break;
			}      
		}
	}

	uint64_t adapted_file_num = 0;
	adapted_file_num = origin_row_num * FileNum /adapted_row_num;

	adapted_RowNum  = adapted_row_num;
	adapted_FileNum = adapted_file_num;

	return 0;
}

#ifndef _TABLEOBJECT_H_
#define _TABLEOBJECT_H_

#include <time.h>
#include <fstream> 
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>
#include <unordered_map>
#include "com/Comm.h"
#include "file/Ctrldata/Ctrldata.h"
#include "ace/Log_Msg.h"
#include "ace/Log_Record.h"
#include "ace/Log_Priority.h"
#include "event2/event.h"
#include "fun/BaseFuncClass.h"
#include "com/AlignDataUtils.h"
#include "event2/bufferevent.h"
#include "file/Example/Example.h"
#include "file/Ctrldata/Ctrldata.h"
#include "struct/AlignDataObject.h"


class CColumnObject;

class CProducer_stats
{
public:
    CProducer_stats(std::string name):myName(name),time_Pre(0),rowsAccumulated_Pre(0)
        ,bytesAccumulated_Pre(0),MessagesAccumulated_Pre(0)
        {
        }
    std::string myName;
    uint64_t time_Pre;
    uint64_t rowsAccumulated_Pre;
    uint64_t bytesAccumulated_Pre;
    uint64_t MessagesAccumulated_Pre;    
};

class CTableObject : public CAlignDataObject
{
public:
    enum TableStatus
    {
        NotReady,
        Ready,
        Done
    };

    typedef std::list<DependencyInfo> tablelist;

    typedef std::list<CTableObject*> dependedlist;

    typedef std::map<litealigndata::AttrTag, DependencyInfo > mapperlist;
    typedef std::set<uint64_t> subscriber_id_set;

    CTableObject( litealigndata::elementTag tag );
    
    CTableObject( const CTableObject& rhs);
    
    CTableObject& operator=( const CTableObject& rhs); 

    bool Init();

    void SendTableData(const char* ptr, uint64_t total_len, bufferevent *buffev);
    virtual bool Initialize( DOMNode* pNode, CAlignDataObject* pParent );

    virtual bool processOjbect( CAlignDataObject* root );
    void SetAlignseedfilenum()
    {
        std::string strRownum;
        if ( !findAttriValue( litealigndata::ATTR_RowNum, strRownum ) )
        {
            ACE_DEBUG((LM_ERROR, "current table object %s can not find the row-number attribute.\n",
                this->getObjectName().c_str()));
            return;
        }
            
        if(m_num_entries > atol( strRownum.c_str()))
            m_num_entries =  atol( strRownum.c_str());
        m_seedfilenum = m_num_entries;

    };
    bool processItem();
    ~CTableObject();

    bool produceTableData( uint32_t thread_index );
    TableStatus getStatus() const
    {
        return m_status;
    }

    void setStatus( TableStatus status )
    {
        this->m_status = status;
    }

    TableStatus checkStatus()
    {

        if ( m_depTables.empty() )
        {
            if ( m_status == NotReady )
                m_status = Ready;

            return m_status;
        }

        tablelist::const_iterator it;

        for ( it = m_depTables.begin(); it != m_depTables.end(); ++it )
        {
            if ( ( ( CTableObject* )it->m_pTableInfo )->getStatus() != Done )
            {
                m_status = NotReady;
                return m_status;
            }
        }

        if ( m_status == NotReady )
            m_status = Ready;

        return m_status;
    }

    static const uint64_t nansec = 1000000000;

    static const uint64_t usec = 1000000;

    static const uint64_t thousands = 1000;

    static const uint32_t MAX_UINT32 = 0xFFFFFFFE;
    
    static const uint32_t  ALIGNDATA_LA_TYPE_QCI = 7; //the value is from nsprobe code loc_attr.h

    bool notifyAlldepenedTable(bufferevent *buffev);

    bool ProducedRowData( CAlignDataObject* obj );

    void addDependedtable( CTableObject* ptable );

    void addDependencyInfo( const DependencyInfo& info );

    CAlignDataObject* findColumnObject( const std::string& colnum );

    const ALIGNDATA_TABLE& getTableHeader() const
    {
        return m_tableHeader;
    }

   ALIGNDATA_TABLE& getTableHeader()
   {
      return m_tableHeader;
   }

    void setTableHeader( const ALIGNDATA_TABLE& tableHeader )
    {
      m_tableHeader = tableHeader;
    }

    static const uint16_t MAX_COL_NUM = 256;
    bool streamTableData(bufferevent *buffev);

    bool createTableData();

    void finishtoCreateTableNum()
    {
        CInterfaceObj* InterfaceObj = ( CInterfaceObj* ) getParent();
        m_createdTableNum++;

        if ( ( m_createdTableNum >= m_toCreateTableNum ) )
        {
            ACE_DEBUG((LM_INFO, "all the files ( %s: %s ) have been generated! \n",
                        InterfaceObj->getObjectName().c_str(),
                        getObjectName().c_str()));

            std::string get_table_id;
            findAttriValue( litealigndata::ATTR_tableid, get_table_id );
            InterfaceObj->Settablefinished( atoi(get_table_id.c_str()) );
            //InterfaceObj->Settablefinished( m_tableHeader.table_id );
        }
    };

    bool fillAlltheTableData();
    
    uint8_t* getData() const
    {
        return m_pData;
    }

    void setData(uint8_t* pData)
    {
        m_pData = pData;
    }

    void setCurFilledCount(uint32_t filledCount)
    {
        m_curFilledCount = filledCount;
    }

    uint32_t getCurFilledCount() const
    {
        return m_curFilledCount;
    }

    void setNextFilledPosition(uint64_t nextFilledPosition)
    {
        m_nextFilledPosition = nextFilledPosition;
    }

    uint64_t const & getNextFilledPosition() const
    {
        return m_nextFilledPosition;
    }

    const FILE* getFp() const
    {
        return m_fp;
    }

    const uint32_t getSeedFileNum() const
    {
        return m_seedfilenum;
    }
    const uint32_t getRowNum() const
    {
        return m_rownum;
    }


    uint32_t getToCreateTableNum() const
    {
        return m_toCreateTableNum;
    }

    void setToCreateTableNum( uint32_t toCreateTableNum )
    {
        m_toCreateTableNum = toCreateTableNum;
    }

    uint32_t getCreatedTableNum() const
    {
        return m_createdTableNum;
    }

    void setCreatedTableNum( uint32_t createdTableNum )
    {
        m_createdTableNum = createdTableNum;
    }

    uint64_t getInitialTime() const
    {
        return m_InitialTime;
    }

    void setInitialTime( uint64_t initialTime )
    {
        m_InitialTime = initialTime;
    }

    uint32_t getInterval() const
    {
        return m_interval;
    }

    void setInterval( uint32_t interval )
    {
        m_interval = interval;
    }

    uint32_t getRowsPerTimeInterval() const
    {
        return RowsPerTimeInterval;
    }

    std::string& getTableType()
    {
        return m_tableType;
    }

    uint16_t getTemplateId()
    {
        return m_templateId;
    }

    bool getIsRandom()
    {
        return m_isRandomOrder;
    }

    bool calculateRowNum();

    uint64_t getFileSize();

    void PrintUsingOstream( std::ostream& out );

    void WriteExtractedSeedData();

    void ProduceExtractedSeedData();

    uint32_t rand()
    {
        return RAND_GEN->rand((uint16_t)thread_index);
    }

    bool ProducedRowDataPercent( CAlignDataObject* parent );
    uint32_t getnum_entries()
    {
        return m_num_entries;
    }
    void setnum_entries(uint32_t num_entries)
    {
        m_num_entries = num_entries;
    }
    uint32_t getentry_size()
    {
        return m_entry_size;
    }
public:
    CAlignDataTypeDef m_AlignDataTypeDef;
    static const uint32_t MAX_DIM_ROW_NUM = 100000;
    std::multimap<uint32_t, uint32_t> keyTableIdxMap;//key: key_table_idx, value: row_idx, starting from 1
    std::multimap<uint32_t, uint32_t> newKeyTableIdxMap;//new keyTableIdxMap in extracted file
    std::unordered_map<uint64_t, uint32_t> dimItemCount;//key: fileIdx * 100000 + rowIdx, value: key count
    
protected:
    virtual bool createData();

    void addAttrDependency( litealigndata::AttrTag tag, const DependencyInfo& pObj );

    void* allocateMemoryForData();

    TableStatus m_status;

    tablelist m_depTables;  //the tables that this table depend on.

    dependedlist m_beDependedTables; //the tables that depend on this table.

    mapperlist m_AttrDepInfos; //the dependency info by the attribute.

    ALIGNDATA_TABLE m_tableHeader;

    uint16_t m_validColnum;

    uint32_t m_rownum;      //configure the rownumber;

    uint32_t m_curFilledCount;  //the count of the data that have been filled.

    uint64_t m_nextFilledPosition;  //the position of the data that will be filled in.

    uint64_t m_InitialTime;     //the time of creating File.

    uint32_t m_interval;   // minutes

    uint32_t m_createdTableNum;

    uint32_t m_toCreateTableNum;

    uint8_t* m_pData;             //storage the data to be written to the file
    FILE* m_fp;
    uint8_t* m_seedData;

    uint32_t m_seedfilenum; //row number in the seedfile
    uint64_t m_seedfilesize;
    bool m_seedfileReadlineOrWhole;

    bool m_bPersistence;          //whether need to make persistence.

    std::string m_tableType;


    CInterfaceObj* m_pinterface;

    uint16_t m_templateId;

    bool  m_isRandomOrder;
    CBaseFileFormat *m_BaseFileFormat;

private:
    uint32_t thread_index;
    TableDefine* m_pTableDef;
    uint16_t      m_tableDefNum;
    Random* RAND_GEN;
    uint8_t *pExtractedData;
    
    uint32_t RowsPerTimeInterval;
    uint64_t nextTimeEdge;
    uint32_t rowsAccumulated_interval;
    uint64_t rowsAccumulated;
    uint64_t totalArborRowsAccumulated;
    uint64_t bytesAccumulated;
    uint64_t lastBytesAccumulated;
    uint64_t MessagesAccumulated;
    uint64_t lastMessagesAccumulated;
    uint64_t timeTableBorn;
    uint64_t m_total_size;
    int32_t m_fd;
    std::string m_fileName;

    CProducer_stats interval_stats;

    ifstream readExampleFile; //argbor 300Gbye seedfile 
    int adapt_XML_parameter(DOMNode* pNode);
    int adpt_row_and_file(uint64_t RowNum, uint64_t FileNum, uint64_t& adapted_RowNum, uint64_t& adapted_FileNum);
    int adpt_row_and_file(uint64_t RowNum, uint64_t FileNum, uint64_t RowNum_eachTimeRead, 
                 uint64_t& adapted_RowNum, uint64_t& adapted_FileNum);

    uint64_t   lastSmsPayloadEnd;
    uint32_t m_num_entries;
    uint32_t m_entry_size;
};

class DimensionalFile: public CAlignDataObject
{
public:

    enum DimentsionFileType
    {
        INVALID_TYPE = 0,
        CSV_TYPE,
        DATABASE_TYPE,
        DATAFileType_MAX
    };

    static const char* FileTypeString[];

public:
    DimensionalFile( litealigndata::elementTag tag ): CAlignDataObject( tag ), m_fileType( INVALID_TYPE )
    {
        memset( m_filePath, 0, sizeof( m_filePath ) );
    }

    virtual bool Initialize( DOMNode* pNode, CAlignDataObject* pParent );

    const char* getFilePath() const
    {
        return m_filePath;
    }

    DimentsionFileType getFileType() const
    {
        return m_fileType;
    }

    void setFileType( DimentsionFileType fileType )
    {
        m_fileType = fileType;
    }

private:
    DimentsionFileType m_fileType;
    char m_filePath[256];

};

#endif /* m_TABLEOBJECT_H_ */

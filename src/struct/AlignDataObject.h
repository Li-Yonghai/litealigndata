#ifndef _ALIGNDATAOBJECT_H_
#define _ALIGNDATAOBJECT_H_

#include <map>
#include <set>
#include <list>
#include <mutex>
#include <vector>
#include <atomic>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <wchar.h>
#include <locale.h>
#include <vector>
#include <queue>
#include <pthread.h>
#include <thread>
#include <condition_variable>
#include "com/Comm.h"
#include "com/Comm.h"
#include "event2/bufferevent.h"
#include "com/AlignDataTypeDef.h"
#include "conf/AligndataConfig.h"
#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/dom/DOMDocument.hpp"
#include "xercesc/util/XMLString.hpp"

XERCES_CPP_NAMESPACE_USE

// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of XMLCh data to local code page for display.
// ---------------------------------------------------------------------------
class CBaseFunc;
class StrX
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    explicit StrX( const XMLCh* const toTranscode )
    {
        // Call the private transcoding method
        fLocalForm = XMLString::transcode( toTranscode );
    }

    ~StrX()
    {
        XMLString::release( &fLocalForm );
    }

    operator const char* () const
    {
        return fLocalForm;
    }

    operator char* () const
    {
        return fLocalForm;
    }

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const char* localForm() const
    {
        return fLocalForm;
    }

private:
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fLocalForm
    //      This is the local code page form of the string.
    // -----------------------------------------------------------------------
    char* fLocalForm;
};

inline ostream& operator<< ( ostream& target, const StrX& toDump )
{
    target << toDump.localForm();
    return target;
}

//XERCES_CPP_NAMESPACE_USE

// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of char* data to XMLCh data.
// ---------------------------------------------------------------------------
class XStr
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XStr( const char* const toTranscode )
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode( toTranscode );
    }

    ~XStr()
    {
        XMLString::release( &fUnicodeForm );
    }


    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh* unicodeForm() const
    {
        return fUnicodeForm;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh*   fUnicodeForm;
};

#define X(str) XStr(str).unicodeForm()


class CAlignDataObject;

struct DependencyInfo
{
    CAlignDataObject* m_pTableInfo;   //the table to be dependency with
    CAlignDataObject* m_pColumnInfo;  //the column to be dependency with
    int32_t m_rowNum;               //the rownum to be dependency with
    uint8_t m_percentage;      //the percentage that will pick from this dependency file
    uint32_t m_number;         //the item number that will pick from this dependency file
    uint32_t m_initRow;         // the initial row index that is generated randomly
    uint32_t m_endRow;         // the end row index, [m_initRow, m_endRow) includes m_number rows
    uint32_t m_lastnumber;   //// the last row index

    DependencyInfo() :
        m_pTableInfo( NULL ), m_pColumnInfo( NULL ), m_rowNum( 0 ), m_percentage( 0 ), m_number( 0 ), m_initRow( 0 ), m_endRow( 0 ), m_lastnumber( 0 )
    {

    }
    ~DependencyInfo()
    {
    }

};


typedef std::vector<CAlignDataObject*> ColumnArray;
struct LoadDimensionInfo
{
    uint32_t hashKey;
    CAlignDataObject* m_pDimensionFile;
    ColumnArray m_CollumnArray;
};



class CXmlParser;

/*
 *
 */

struct StatsContext
{
public:
    StatsContext(): packets( 0 ), bytes( 0 ) {};
    void resetSentPackets()
    {
        packets = 0;
    }
    const uint64_t getSentPackets() const
    {
        return packets;
    }
    void increaseSentPackets( uint64_t value = 1 )
    {
        packets += value;
    }
    void resetSentBytes()
    {
        bytes = 0;
    }
    const uint64_t getSentBytes() const
    {
        return bytes;
    }
    void increaseSentBytes( uint64_t value )
    {
        bytes += value;
    }

    void resetStats()
    {
        packets = 0;
        bytes = 0;
    }

    void increaseStats(uint64_t packets, uint64_t bytes)
    {
        this->packets += packets;
        this->bytes += bytes;
    }

    private:
        StatsContext(const StatsContext&) = delete;
        StatsContext& operator=(const StatsContext&) = delete;
        uint64_t packets;
        uint64_t bytes;
};

class CAlignDataObject
{

public:
    typedef std::list<CAlignDataObject*> LitealigndataObjectList;

    typedef std::map<litealigndata::AttrTag, std::string> mapAttribute;

    typedef std::map<litealigndata::AttrTag, CAlignDataObject*> mapLitealigndataObject;

public:
    CAlignDataObject(){};
    CAlignDataObject( litealigndata::elementTag tag );

    CAlignDataObject( const CAlignDataObject& rhs );

    virtual bool processItem()
    {
        return true;
    };
    CAlignDataObject& operator=( const CAlignDataObject& rhs );

    virtual bool Initialize( DOMNode* pNode, CAlignDataObject* pParent );

    virtual bool processOjbect( CAlignDataObject* pRoot );
    
    bool fun()
    {
        return true;
    };

    virtual bool createData()
    {
        return true;
    }

    virtual CBaseFunc* getFunc()
    {
        return NULL;
    }


    inline friend ostream& operator << ( ostream& target, const CAlignDataObject& obj )
    {
        target << "[" << litealigndata::elementTagString[obj.m_elementType] << "--"
               << obj.m_ObjectName;

        if ( !obj.m_strValue.empty() )
            target << "--" << obj.m_strValue;

        target << "]" << std::endl;
        return target;
    }

    virtual ~CAlignDataObject();

    virtual void Initialize();

    const mapAttribute& getAttributes() const
    {
        return m_attributes;
    }

    void setAttributes( const mapAttribute& attributes )
    {
        m_attributes = attributes;
    }

    bool findAttriValue( litealigndata::AttrTag tag, std::string& attrValue )
    {
        attrValue.clear();
        mapAttribute::iterator it = m_attributes.find( tag );

        if ( it != m_attributes.end() )
        {
            attrValue = it->second;
            return true;
        }

        return false;
    }

    CAlignDataObject* findAttributeObject( litealigndata::AttrTag tag )
    {

        mapLitealigndataObject::iterator it = m_attrLitealigndataObject.find( tag );

        if ( it != m_attrLitealigndataObject.end() )
        {
            return it->second;
        }

        return NULL;
    }

    CAlignDataObject* findChildObject( const std::string& objectName )
    {
        LitealigndataObjectList::const_iterator it;

        for ( it = this->m_Children.begin(); it != m_Children.end(); ++it )
        {
            if ( ( *it )->m_ObjectName == objectName )
                return ( *it );
        }

        return NULL;
    }

    litealigndata::elementTag getElementType() const
    {
        return m_elementType;
    }

    void setElementType( litealigndata::elementTag Tag )
    {
        m_elementType = Tag;
    }

    const LitealigndataObjectList& getLitealigndataObjectList() const
    {
        return m_Children;
    }
    virtual bool GetusedFlag()
    {
        return true;
    };
    virtual void SetusedFlag(bool usedFlag)
    {
        //m_usedFlag = usedFlag;
     };

    LitealigndataObjectList& getLitealigndataObjectList()
    {
        return m_Children;
    }

    void setLitealigndataObjectList( const LitealigndataObjectList& LitealigndataObjectList )
    {
        m_Children = LitealigndataObjectList;
    }
    const std::string& getunionFflag() const
    {
        return m_union_flag;
    }
    const std::string& getObjectName() const
    {
        return m_ObjectName;
    }

    void setObjectName( const std::string& objectName )
    {
        m_ObjectName = objectName;
    }

    const CAlignDataObject* getParent() const
    {
        return m_pParent;
    }

    CAlignDataObject* getParent()
    {
        return m_pParent;
    }

    void setParent( CAlignDataObject* pParent )
    {
        m_pParent = pParent;
    }

    const CXmlParser* getParser() const
    {
        return m_pParser;
    }
    int char_to_wchar(wchar_t *pDest, const char *pSrc)
    {
        int len = 0;
        int ret = 0;

        len = strlen(pSrc) + 1;

        if(len <= 1)
            return 0;

        ret = mbstowcs(pDest, pSrc, len);

        return ret;
    }

    int wchar_to_char(char *pDest, const wchar_t *pSrc)
    {
        int len = 0;
        int ret = 0;

        len = wcslen(pSrc) + 1;

        if(len <= 1)
            return 0;

        ret = wcstombs(pDest, pSrc, len*sizeof(wchar_t));

        return ret;
    }
    CXmlParser* getParser()
    {
        return m_pParser;
    }

    void setParser( CXmlParser* pParser )
    {
        m_pParser = pParser;
    }

    const std::string& getStrValue() const
    {
        return m_strValue;
    }

    void setStrValue( const std::string& strValue )
    {
        m_strValue = strValue;
    }
    const std::string& getstrColName() const
    {
        return m_strColName;
    }
    void setstrColName( const std::string& strColName)
    {
        m_strColName = strColName;
    }
    double getPercent()
    {
        return percentage;
    }

    bool isPercentSet()
    {
        return has_percent;
    }

    int& hasPercentChild()
    {
        return has_percent_child;
    }

    CAlignDataObject* getPercentSelect()
    {
        return (*percent_it);
    }

    CAlignDataObject::LitealigndataObjectList::iterator& getPercentSelectIter()
    {
        return percent_it;
    }

    int getNumRowsToCreate()
    {
        if (-1 != PercentCount.rows_to_create)
            return PercentCount.rows_to_create;
    
        return m_pParent->getNumRowsToCreate();
    }

    bool recurse_process( bool (CAlignDataObject::*pFunc)() );
    

    bool initPercent();
    bool clearPercent();

    struct percentCount
    {
      int rows_to_create;
      int rows_created;
      uint32_t row_volumn_for_1_period;
      const static uint32_t MAXIMUM = 10000000;
      percentCount():rows_to_create(-1),rows_created(0),row_volumn_for_1_period(0){};
    } PercentCount; 
    virtual void setLen(uint32_t len)
    {
        m_len = len;
    }
    virtual uint32_t getLen()
    {
        return m_len;
    }
    virtual void setOffset(uint32_t offset)
    {
        m_offset = offset;
    };
    virtual uint32_t getOffset()
    {
        return m_offset;
    };
    virtual uint32_t getcolumnNo()
    {
        return m_columnNo;
    }
    virtual void setColumnNo(uint32_t columnNo)
    {
        m_columnNo = columnNo;
    }
    virtual uint32_t GetallLen()
    {
        return 0;
    };
    virtual void SetallLen(uint32_t toalLen)
    {

    };
protected:

    std::string m_union_flag = "0xff";
    uint32_t m_len = 0;
    uint32_t m_offset;
    uint32_t m_columnNo = 0;
    std::string m_ObjectName; //get the attribute of "name" from this elementNode.

    std::string m_strValue;   //the element's value. if it have.
    std::string m_strColName;

    litealigndata::elementTag m_elementType;

    CAlignDataObject* m_pParent;

    CXmlParser* m_pParser;

    LitealigndataObjectList m_Children;

    mapAttribute m_attributes;    //all the attributes for the tag Object.

    mapLitealigndataObject m_attrLitealigndataObject; //if the attribute is a litealigndataObject

    double percentage;
    int has_percent;
    int has_percent_child;
    CAlignDataTypeDef m_AlignDataTypeDef;

private:
    void AddChildLitealigndataObject( CAlignDataObject* pNode );

    void AddAttribute( litealigndata::AttrTag tag, std::string name );

    void AddAttributeObject( litealigndata::AttrTag tag, CAlignDataObject* object );



    void SetElementValue( DOMNode* pNode );

    const std::string* GetAttributeValue( litealigndata::AttrTag tag )
    {
        mapAttribute::const_iterator it = m_attributes.find( tag );

        if ( it != m_attributes.end() )
            return &( it->second );

        return NULL;
    }

    CAlignDataObject* pPercent_selected;
protected:
    CAlignDataObject::LitealigndataObjectList::iterator percent_it;
};
class CAlignDataObjectFactory
{
public:
    static const CAlignDataObjectFactory& Instance()
    {
        static CAlignDataObjectFactory thefactory;
        return thefactory;
    }

    /***
     *
     */
    static CAlignDataObject* createalignDataObject( DOMNode* pNode );
    static CAlignDataObject* createalignDataObject( litealigndata::elementTag elementType );
    static CAlignDataObject* createalignDataObject( CAlignDataObject& obj );

private:
    CAlignDataObjectFactory()
    {
    }
    ;

    CAlignDataObjectFactory( const CAlignDataObjectFactory& );

    CAlignDataObjectFactory& operator = ( const CAlignDataObjectFactory& );

    ~CAlignDataObjectFactory()
    {
    };
    
};

struct Lz4TableInfo
{
    std::string ipAddress;
    int64_t     ifn;
    std::string tableName;
    uint64_t    timeStamp;
    uint32_t    rowNumber;
    std::string schema;
    std::string filename;    

    Lz4TableInfo(std::string ip, int64_t lz4Ifn, std::string name, uint64_t time, uint32_t rowNum, std::string sche, std::string lz4Name) :
        ipAddress( ip ), ifn( lz4Ifn ), tableName( name ), timeStamp( time ), rowNumber( rowNum ), schema( sche ), filename( lz4Name )
    {

    }
    ~Lz4TableInfo()
    {
    }

};

class CInterfaceObj : public CAlignDataObject
{
public:
    CInterfaceObj( litealigndata::elementTag tag );
    ~CInterfaceObj();
    void setbufferevent(bufferevent *buffev)
    {
        m_buffev = buffev;
    };
    bufferevent *getbufferevent()
    {
        return m_buffev;
    }


    typedef std::map<std::string, CAlignDataObject*> GetNetObjectList;

    bool addTableInMapper( std::string str, CAlignDataObject* pLitealigndataObject );

    bool addObjectInMapper( std::string str, CAlignDataObject* pLitealigndataObject );

    const CAlignDataObject* findLitealigndataObject( const std::string& objname ) const;

    CAlignDataObject* findLitealigndataObject( const std::string& objname );

    bool findTableObjects(int tableId, std::vector<CAlignDataObject*>& tableList);

    bool StartProduceData();

    void Settablefinished( uint8_t tableId );

    bool getTableFinished( uint8_t tableId );

    void initOriginTimePoint()
    {
        gettimeofday( &originPoint, NULL );
        lastPoint = originPoint;
    }

    std::shared_ptr<StatsContext> getStatsContext(uint32_t thread_index)
    {
        std::lock_guard<std::mutex> locker( stats_lock );
        return statsContextList[thread_index];
    }

    uint64_t getPacketsPerThread(uint32_t thread_index)
    {
        std::lock_guard<std::mutex> locker( stats_lock );
        return statsContextList[thread_index]->getSentPackets();
    }

    uint64_t getBytesPerThread(uint32_t thread_index)
    {
        std::lock_guard<std::mutex> locker( stats_lock );
        return statsContextList[thread_index]->getSentBytes();
    }

    void resetStatsPerThread(uint32_t thread_index)
    {
        std::lock_guard<std::mutex> locker( stats_lock );
        statsContextList[thread_index]->resetStats();
     }

    void increaseStatsPerThread(uint32_t thread_index, uint64_t packets, uint64_t bytes)
    {
        std::lock_guard<std::mutex> locker( stats_lock );
        statsContextList[thread_index]->increaseStats(packets, bytes);
     }

    void addStatsContext(std::shared_ptr<StatsContext>& sp_stats)
    {
        std::lock_guard<std::mutex> locker( stats_lock );
        statsContextList.push_back( sp_stats  );
    }

    double rate( timeval& end, timeval& start, uint64_t value, uint32_t scale = 1000 );

    virtual bool Initialize( DOMNode* pNode, CAlignDataObject* pParent );

    void ModifyKeysTableIdxMap();
    void ProduceExtractedSeedData();

    void setInitialTime( uint64_t initialTime )
    {
        m_initialTime = initialTime;
    }

    uint64_t getInitialTime()
    {
        return m_initialTime;
    }

    void setInterval( uint32_t interval )
    {
        m_interval = interval;
    }

    uint32_t getInterval()
    {
        return m_interval;
    }

    void setStatsFile();

    bool checkalltheTablefull();
        
    static const uint8_t MAX_TABLE_NUM = 255;

    GetNetObjectList const & getMapLitealigndataObject()
    {
        return m_mapLitealigndataObject;
    }

    uint32_t finished;

private:
    GetNetObjectList m_mapLitealigndataObject;
    bool m_bfullTable[MAX_TABLE_NUM];
    uint64_t m_initialTime;
    uint32_t m_interval;
    std::vector<std::shared_ptr<StatsContext>> statsContextList;
    std::mutex stats_lock;
    std::mutex m_subStatus_lock;
    timeval originPoint;
    timeval lastPoint;
    uint64_t   bytes_lastPoint;
    uint64_t   packets_lastPoint; 
    char m_statsFile[256];
    bool isToBeDuplicated;
    uint8_t duplicateIfnStart;
    uint8_t duplicateIfnNum;
    bufferevent *m_buffev;
            
};

#endif /* CAlignDataObject_H_ */

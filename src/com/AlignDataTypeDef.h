#ifndef _ALIGNDATATYPEDEF_H_
#define _ALIGNDATATYPEDEF_H_
#include <map>
#include <string>
#include <vector>
#include <cstdio>
#include <stdio.h>
#include <array>
#include <memory>
#include <errno.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <algorithm>
#include <strings.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include "ace/Log_Msg.h"
#include "com/Comm.h"

class autoValue
{
public:
    autoValue( uint8_t* data, uint32_t len ) :
        m_type( litealigndata::AUTO_Invalid ), m_data( data ), m_len( len ), isSubColumn( false )
    {

    }

    void reset()
    {
        m_type = litealigndata::AUTO_Invalid;
        m_len = 0;

        if ( !isSubColumn )
            m_data = NULL;
        else
            memset( m_inerData, 0, sizeof( m_inerData ) );
    }

    void clearData()
    {
        if ( !isSubColumn )
            memset( m_data, 0, m_len );
        else
            memset( m_inerData, 0, sizeof( m_inerData ) );
    }

    static const int maxvalue = 256;

    autoValue() :
        m_type( litealigndata::AUTO_Invalid ), m_data( NULL ), m_len( 0 ), isSubColumn( false )
    {
        memset( m_inerData, 0, sizeof( m_inerData ) );
    }


    uint8_t* getData() const
    {
        if ( isSubColumn )
            return ( uint8_t* )m_inerData;
        else
            return m_data;
    }

    void setData( uint8_t* data )
    {
        if ( !isSubColumn )
            m_data = data;
        else
        {
            if ( m_len <= maxColumnLen )
            {
                memcpy( m_inerData, data, m_len );
            }
        }
    }

    uint32_t getLen() const
    {
        return m_len;
    }

    void setLen( uint32_t len )
    {
        m_len = len;
    }

    litealigndata::autoType getType() const
    {
        return m_type;
    }

    static litealigndata::GenericType ConvertAutoTypeToGenericType( litealigndata::autoType type )
    {
        switch ( type )
        {
        case litealigndata::AUTO_UINT64:
        case litealigndata::AUTO_UINT32:
        case litealigndata::AUTO_IPV4:
        case litealigndata::AUTO_IPV4V6:
        case litealigndata::AUTO_UINT16:
        case litealigndata::AUTO_UINT8:
        case litealigndata::AUTO_INT64:
        case litealigndata::AUTO_INT32:
        case litealigndata::AUTO_INT16:
        case litealigndata::AUTO_INT8:
            return litealigndata::Generic_NUMBER;

        case litealigndata::AUTO_IPV6_RSA_CU:
        case litealigndata::AUTO_IPV6:
        case litealigndata::AUTO_IPV4_IPV6:       
            return litealigndata::Generic_IPV6;

        case litealigndata::AUTO_ByteArray:
        case litealigndata::AUTO_STRING:
        case litealigndata::AUTO_IPV4_NET:
        case litealigndata::AUTO_IPV6_NET:
        case litealigndata::AUTO_STRING_NET:

        case litealigndata::AUTO_IMSIBCD:
        case litealigndata::AUTO_BCD:
        case litealigndata::AUTO_IMSI_TBCD: 
        case litealigndata::AUTO_IMEISV_BCD:
        case litealigndata::AUTO_BCD_2Bytes:     
            return litealigndata::Generic_BCD;

        default:
            return litealigndata::Generic_Invalid;
        }
    }

    void setType( litealigndata::autoType type )
    {
        m_type = type;

        switch ( m_type )
        {
        case litealigndata::AUTO_UINT64:
        case litealigndata::AUTO_INT64:
        case litealigndata::AUTO_IMSIBCD:
        case litealigndata::AUTO_IMSI_TBCD: 
        case litealigndata::AUTO_IMEISV_BCD:  
        case litealigndata::AUTO_RealTime:
        case litealigndata::AUTO_RealTime_ms:
            m_len = sizeof( uint64_t );
            break;

        case litealigndata::AUTO_UINT32:
        case litealigndata::AUTO_INT32:
        case litealigndata::AUTO_IPV4:
        case litealigndata::AUTO_IPV4V6:
            m_len = sizeof( uint32_t );
            break;

        case litealigndata::AUTO_UINT16:
        case litealigndata::AUTO_INT16:
        case litealigndata::AUTO_BCD_2Bytes:    
            m_len = sizeof( uint16_t );
            break;

        case litealigndata::AUTO_UINT8:
        case litealigndata::AUTO_INT8:
            m_len = sizeof( uint8_t );
            break;

        case litealigndata::AUTO_IPV6_RSA_CU:
        case litealigndata::AUTO_IPV6:
        case litealigndata::AUTO_IPV4_IPV6:
            m_len = sizeof( litealigndata::_IPV4_IPV6_ADDR_ );
            break;
        case litealigndata::AUTO_IPV4_NET:
            m_len = sizeof(uint32_t);
            break;
        case litealigndata::AUTO_IPV6_NET:
            break;
        case litealigndata::AUTO_ByteArray:
            m_len = MAX_STR_LEN;
            break;
        default:
            break;
        }
    }

    void setIpV6Low( uint64_t num )
    {
        litealigndata::IPV4_IPV6_ADDR* pdata = ( litealigndata::IPV4_IPV6_ADDR* )getData();
        pdata->hl.low = num;
    }

    void setIpV6High( uint64_t num )
    {
        litealigndata::IPV4_IPV6_ADDR* pdata = ( litealigndata::IPV4_IPV6_ADDR* )getData();
        pdata->hl.high = num;
    }

    void setIpV6( uint64_t high, uint64_t low )
    {
        litealigndata::IPV4_IPV6_ADDR* pdata = ( litealigndata::IPV4_IPV6_ADDR* )getData();
        pdata->hl.high = high;
        pdata->hl.low = low;
    }

    const litealigndata::IPV4_IPV6_ADDR* getIpv6Value() const
    {
        return ( litealigndata::IPV4_IPV6_ADDR* )getData();
    }


    //do not support ipV6
    const char* GetHashString( char* hashString )
    {
        switch ( m_type )
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
        case litealigndata::AUTO_IPV4V6:
        case litealigndata::AUTO_IMSI_TBCD:
        case litealigndata::AUTO_IMEISV_BCD:    
            sprintf( hashString, "%lu", getnumber() );
            break;

        case litealigndata::AUTO_ByteArray:
        {
            sprintf( hashString, "%s", getData() );
            break;
        }

        case litealigndata::AUTO_IPV6_RSA_CU:
        case litealigndata::AUTO_IPV6:
        case litealigndata::AUTO_IPV4_IPV6:
        {
            break;
        }

        default:
            return hashString;
        }

        return hashString;
    }


    void AsciiToHex( const char* ascii );

    void AsciiToStr( const char* ascii );

    void AsciiToSpU8_4( const char* ascii );

    void AsciiToDec( const char* pNumber );

    void AsciiToByteArray( const char* ascii, size_t len, uint32_t is_Right_align = false );

    void IPv6ToSpIPv6( const litealigndata::IPV4_IPV6_ADDR* v6_addr );

    void setNumber( uint64_t number );

    void AsciiToNetStr( const char* str );

    uint64_t getnumber();

    std::string getString();
    std::string getToString();

    bool isIsSubColumn() const
    {
        return isSubColumn;
    }

    void setIsSubColumn( bool isSubColumn )
    {
        this->isSubColumn = isSubColumn;
    }

    static const uint8_t maxColumnLen = 64;

    static const unsigned long long magicNumber = 0x7FFFFFFFFFFFFFFF;    
    void setSpMac( const uint64_t value );

private:
    litealigndata::autoType m_type;
    uint8_t* m_data;
    uint8_t m_inerData[maxColumnLen];  //special for the subcolumn.
    uint32_t m_len;
    bool isSubColumn;
};

#if !defined(ULLONG_MAX)
    #define ULLONG_MAX 18446744073709551615ULL // == 2^64 - 1
#endif


struct TableCollumnOffsetDef
{
    uint32_t tableId;
    uint32_t MaxNum;
    uint32_t entrysize;
    uint32_t colIdx;
    litealigndata::autoType type;
    uint32_t columnLen;
};


struct TableDefine
{
    char szName[64];
    litealigndata::autoType type;
    uint16_t len;   //start offset
    uint16_t sublen;  //struct union 个数
    bool depenColumnFlag;  //被当前table 其它字段依赖
};

typedef TableDefine* ptrTabDef;

struct tabledef
{
    uint64_t   headLen;  //data head len
    uint16_t   head_entry_num;
    uint8_t    table_id;
    uint64_t   allLen;//tabledef len
    uint64_t   totalLen;//data body len
    uint16_t   body_entry_num;
    uint16_t   local_key_pos;
    ptrTabDef  head;
    ptrTabDef tableinfo;
};

char* strupr( char* str );

#define clearbit(x,len) ((x)&(~(1<<(len))) )


typedef std::vector<std::string> strArray;

class baseType
{
public:
    baseType( litealigndata::autoType type ):
        m_type( type )
    {
    }

    ~baseType()
    {
    }

    litealigndata::autoType getType() const
    {
        return m_type;
    }

    void setType( litealigndata::autoType type )
    {
        m_type = type;
    }

    virtual void justToMakeDynamicCastWork( )
    {
        ;
    }

protected:
    litealigndata::autoType m_type;
};


class numberType : public baseType
{
public:
    numberType( litealigndata::autoType type ) :
        baseType( type )
    {

    }

    void resetValue()
    {
        m_value = 0;
    }

    uint64_t getValue() const
    {
        return m_value;
    }

    void AsciiToDec(const char* ascii )
    {
        m_value = strtoull( ascii, NULL, 10 );
    }

    void AsciiToHex( const char* ascii )
    {
        m_value = strtoull( ascii, NULL, 16 );
    }

    void setImsiNumber( char* pImsi, uint8_t Len, bool isSwap, litealigndata::subIdType type )
    {
        m_value = 0;
    }
private:
    uint64_t m_value;
};

class IPtype : public baseType
{
public:
    IPtype( litealigndata::autoType type ) :
        baseType( type )
    {
        memset( &m_value, 0, sizeof( m_value ) );
    }

    void resetValue()
    {
        memset( &m_value, 0, sizeof( m_value ) );
    }

    litealigndata::IPV6_ADDR getValue()
    {
        return m_value;
    }
public:
    litealigndata::IPV6_ADDR getValue() const
    {
        return m_value;
    }
private:
    litealigndata::IPV6_ADDR m_value;
};

class CharArray : public baseType
{
public:
    CharArray( litealigndata::autoType type ) :
        baseType( type )
    {
        memset( m_value, 0, sizeof( m_value ) );
    }

    char const * getValue() const
    {
        return m_value;
    }

    void resetValue()
    {
        memset( m_value, 0, sizeof( m_value ) );
    }

    void setValue( char const * ivalue )
    {
        snprintf( m_value, sizeof( m_value ), "%s", ivalue );
    }
private:
    char m_value[256];
};


class Time_Stamp
{
public:
    static Time_Stamp * GetInstance()
    {   // thread_local is needed for multi-thread
        thread_local  static Time_Stamp *m_pInstance = NULL;
        if(m_pInstance == NULL) //is first time call?
        {
            m_pInstance = new Time_Stamp();
        }
        return m_pInstance;
    }
    int32_t setSeedFileTime(uint32_t second,uint32_t usec)
    {
        seed_file_time.tv_sec =  second;
        seed_file_time.tv_usec = usec;
        return 0;
    }
    
    void show()
    {
      std::cout<<"seed_file_time "<<seed_file_time.tv_sec<<"s "<< seed_file_time.tv_usec<<std::endl;
      std::cout<<"current_time   "<<  current_time.tv_sec<<"s "  << current_time.tv_usec<<std::endl;
    }
    
    int32_t gettimeofday_master(struct timeval*tv, struct timezone *tz)
    {
        return_value = gettimeofday( tv, tz );
        current_time = *tv;
        return return_value;
    };
    
    int32_t gettimeofday_slave(struct timeval*tv)
    {
        *tv = current_time;
        return return_value;
    };

    int32_t getUpdateNewTime(int64_t seed_sec,int64_t seed_usec,uint32_t& update_sec,uint32_t& update_usec)
    {
       #define U_SEC (1000*1000)
    
       int64_t seedtimeSum = seed_sec*U_SEC+seed_usec;
       int64_t seed_file_timeSum = seed_file_time.tv_sec*U_SEC+seed_file_time.tv_usec;
       int64_t current_timeSum = current_time.tv_sec*U_SEC+current_time.tv_usec;
       auto offset = seedtimeSum - seed_file_timeSum;
       int64_t update_timeSum = offset + current_timeSum;

       update_sec  =  update_timeSum/U_SEC;
       update_usec =  update_timeSum%U_SEC;
       
       return 0;
    };

    int32_t setCurrentTime(uint32_t sec, uint32_t Usec)
    {
        current_time.tv_sec = sec;
        current_time.tv_usec= Usec;  
        return 0;
    };

    
private:
    Time_Stamp() {
       seed_file_time.tv_sec =0;
       seed_file_time.tv_usec=0;
       current_time.tv_sec =0;
       current_time.tv_usec=0;       
    };
    struct timeval seed_file_time;
    struct timeval current_time;
    int32_t return_value;
};

void get_time_diff( uint32_t start_sec, uint32_t start_usec, uint32_t end_sec,
                    uint32_t end_usec, uint32_t& diff_sec, uint32_t& diff_usec );

void recursion_scan_dir_file( std::string& completeDir, std::vector<std::string>& vFiles );

class CAlignDataTypeDef
{
    public:
        #define NIBBLE_SWAP(i) ((0x00F0 & ((i) << 4)) | (0x000F & ((i) >> 4)))
        CAlignDataTypeDef(){};
        ~CAlignDataTypeDef(){};
        void get_time_diff( uint32_t start_sec, uint32_t start_usec, uint32_t end_sec,
                            uint32_t end_usec, uint32_t& diff_sec, uint32_t& diff_usec );
        void add_time_diff( uint32_t start_sec, uint32_t start_usec, uint32_t diff_sec,
                            uint32_t diff_usec, uint32_t& end_sec, uint32_t& end_usec );
        int FindRelativeTag( const char* pstringArray[], uint32_t size, const char* pstr );
        void ParserTableInfoFromString( const char* ParseredStr, uint32_t strLen, std::string& strTable,
                                        uint32_t& Row, std::string& strCol );
        bool isAlldigits( const char* pchar, int Len );
        uint64_t swap64Bits( uint64_t i );
        const TableCollumnOffsetDef* FindTableInfo( const TableCollumnOffsetDef* defarray, 
                                                uint32_t entry_size, uint32_t tableId );
        const TableCollumnOffsetDef* FindColumnInfo( const TableCollumnOffsetDef* defarray, 
                                uint32_t entry_size, uint32_t tableId, uint32_t columnidx );
        uint64_t randnumer( uint64_t low, uint64_t high );
        uint8_t NibbleSwap( uint8_t i );
        void TimeUsecToString( uint64_t time, char* str, uint32_t strLen );
        void splitValue( const char* defaultValue, strArray& strValueList );
        uint8_t parseStrToArray( const std::string& str, uint16_t v6_addr[] );
        std::string Trim( std::string& str );
        char* trim( char* psz );
        uint64_t calc_align( uint64_t n, uint64_t align );
        //uint32_t RSHash( char* str );
        uint32_t JSHash( char* str );
        uint32_t BKDRHash(char *str);
        uint32_t GetTotalLineCount( FILE* fp );
        char* strupr( char* str );
        std::string& str_trim( std::string& s );
        char* setNewLogFile(std::string& xmlfile, char* curLog);

};

#endif /* ALIGNDATATYPEDEF_H_ */

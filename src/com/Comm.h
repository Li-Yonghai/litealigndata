#ifndef _COMM_H_
#define _COMM_H_
#include <fcntl.h>
#include <stdio.h>
#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <float.h>
#include <limits.h>
#include <stdint.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/types.h>

namespace litealigndata
{
    #define MAX_PATH 256

    #define MAX_RATE 0.2

    #define MAX_UINT64 0XFFFFFFFFFFFFFFFE

    #define EXAMPLE_TABLE_ID  0
    #define EXAMPLE_TABLE_ID_1 1
    #define CTRLDATA_KEYS_TABLE_ID  2
    #define CTRLDATA_STATS_TABLE_ID  3
    #define UNDEF_TABLE_ID       255
    #define SEED_FILE_PATH_LEN     512


    enum autoType
    {
        AUTO_Invalid = 0,
        AUTO_HEAD,
        AUTO_BODY,
        AUTO_STRUCT,
        AUTO_UNION,
        AUTO_UINT64,
        AUTO_INT64,
        AUTO_UINT32,
        AUTO_INT32,
        AUTO_UINT16,
        AUTO_INT16,
        AUTO_UINT8,
        AUTO_INT8,
        AUTO_IPV4,
        AUTO_IPV6,
        AUTO_IPV4V6,
        AUTO_DateTime,
        AUTO_ByteArray,
        AUTO_STRING,
        AUTO_IMSIBCD,
        AUTO_BCD,
        AUTO_TIMEVAL,
        AUTO_SP_U8_4,
        AUTO_SP_IPV6,
        AUTO_IMSI_TBCD,
        AUTO_IMEISV_BCD,
        AUTO_RealTime,
        AUTO_BCD_2Bytes,
        AUTO_IPV6_RSA_CU,
        AUTO_Metric_List,
        AUTO_RealTime_ms,
        AUTO_IPV4_IPV6,
        AUTO_SharePointer_MAC_6Byte,
        AUTO_IPV4_NET,
        AUTO_IPV6_NET,
        AUTO_STRING_NET,
        AUTO_MAX
    };


    enum GenericType
    {
        Generic_Invalid = 0,
        Generic_NUMBER,
        Generic_BYTEARRAY,
        Generic_IPV6,
        Generic_BCD,
        Generic_MAX
    };

    enum subIdType
    {
        Invalid_type = 0,
        ALIGNDATA_SUB_ID_TYPE_MSISDN = 1,
        ALIGNDATA_SUB_ID_TYPE_IMSI,
        ALIGNDATA_SUB_ID_TYPE_IMSI_OR_MSISDN,
        ALIGNDATA_SUB_ID_TYPE_IPv4_ADDR,
        ALIGNDATA_SUB_ID_TYPE_IPv6_ADDR_INDEX,
        ALIGNDATA_SUB_ID_TYPE_MSISDN_WITH_PLUS,
        occupy_num_other_wise_MAC_is_7,
        ALIGNDATA_SUB_ID_TYPE_MAC,
    };

    enum AttrTag
    {
        ATTR_Invalid = 0,
        ATTR_UNION_Flag,
        ATTR_Name,
        ATTR_Interval,
        ATTR_FileNum,
        ATTR_RowNum,
        ATTR_RowNumPerFlow,
        ATTR_Type,
        ATTR_GenFuncName,
        ATTR_SourceType,
        ATTR_Len,
        ATTR_LowRange,
        ATTR_HighRange,
        ATTR_Prefix,
        ATTR_tableid,
        ATTR_localkey,
        ATTR_LocalKeyType,
        ATTR_LocalKeyBit,
        ATTR_ColIdx,
        ATTR_csvheadFlag,
        ATTR_CsvFlag,
        ATTR_BitLen,
        ATTR_Path,
        ATTR_SeedFile,
        ATTR_TableName,
        ATTR_Confile,//jsonFile
        ATTR_seedfileReadlineOrWhole,
        ATTR_isStream,
        ATTR_TableType,
        ATTR_MetricsNum,
        ATTR_RowsPerTimeInterval,
        ATTR_Percentage,
        ATTR_offset,
        ATTR_misc,
        ATTR_percentage_period_rows,
        ATTR_duplicateIfnStart,
        ATTR_duplicateIfnNum,
        ATTR_row_quantity_each_time_read,
        ATTR_TemplateId,
        ATTR_ColValue,
        ATTR_isRandomOrder,
        ATTR_Max
    };

    enum elementTag
    {
        ELEMENT_Invalid = 0,
        ELEMENT_AligndataModel,
        ELEMENT_Interface,
        ELEMENT_Table,
        ELEMENT_Head,
        ELEMENT_Body,
        ELEMENT_Struct,
        ELEMENT_Union,
        ELEMENT_Column,
        ELEMENT_Bit,
        ELEMENT_Parameter,
        ELEMENT_SubColumn,
        ELEMENT_Subbit,
        ELEMENT_DimensionalFile,
        ELEMENT_FileColumn,
        ELEMENT_Function,
        ELEMENT_FuncParameter,
        ELEMENT_Max
    };
    enum funcTag
    {
        FUNC_Invalid = 0,
        FUNC_Acculate,
        FUNC_Random,
        FUNC_FileReader,
        FUNC_FIX,
        FUNC_Merge,
        FUNC_NONE,
        FUNC_REDEFINENUMBER,
        FUNC_Correlation,
        FUNC_UniqueRandom,
        FUNC_MAX
    };

    enum parameterTag
    {
        PARA_Invalid = 0,
        PARA_Prefix,
        PARA_Init,
        PARA_Step,
        PARA_LowRange,
        PARA_HighRange,
        PARA_SourceType,
        PARA_Value,
        PARA_IMSIType,
        PARA_DepColumn,
        PARA_DepKey,
        PARA_TotNumber,
        PARA_FileNumber,
        PARA_Percent,
        PARA_RowsInBatch,    
        PARA_PercentInBatch,
        PARA_Reset,
        PARA_MaxNum, 
        PARA_SubscriberId_Type,
        PARA_SubscriberId_HiRange,
        PARA_SubscriberId_LowRange,
        PARA_SessionId_HiRange,
        PARA_SessionId_LowRange,
        PARA_IPV4_IPV6,
        PARA_IP_HiRange,
        PARA_IP_LowRange,
        PARA_Port_HiRange,
        PARA_Port_LowRange,
        PARA_Effect_On_Interface,
        PARA_sip_child_app,
        PARA_byte_array_right_align,
        PARA_DuplicatedUnit,
        PARA_MAX
    };

    enum AligndataLATag
    {
        ALIGNDATA_LA_TYPE_NONE  = 0,
        ALIGNDATA_LA_TYPE_CLIENT_SITE,
        ALIGNDATA_LA_TYPE_SERVER_SITE,
        ALIGNDATA_LA_TYPE_RAT_TYPE,
        ALIGNDATA_LA_TYPE_APN,
        ALIGNDATA_LA_TYPE_CLIENT_ORIGIN_HOST,
        ALIGNDATA_LA_TYPE_SERVER_ORIGIN_HOST,
        ALIGNDATA_LA_TYPE_QCI,
        ALIGNDATA_LA_TYPE_CMTS,
        ALIGNDATA_LA_TYPE_HANDSET_GROUP,
        ALIGNDATA_LA_TYPE_VLAN,
        ALIGNDATA_LA_TYPE_VRF_GROUP,
        ALIGNDATA_LA_TYPE_VRF_ADMIN,
        ALIGNDATA_LA_TYPE_VRF_ASSIGN,
        ALIGNDATA_LA_TYPE_CELL_GROUP,
        ALIGNDATA_LA_TYPE_CELL_AREA,
        ALIGNDATA_LA_TYPE_TAC, 
        ALIGNDATA_LA_TYPE_BSID,
        ALIGNDATA_LA_TYPE_NETFLOW_ADDR,
        ALIGNDATA_LA_TYPE_SFLOW_ADDR,
        ALIGNDATA_LA_TYPE_ROUTER_IFN_IN,
        ALIGNDATA_LA_TYPE_ROUTER_IFN_OUT,
        ALIGNDATA_LA_TYPE_CELL_ID,
        ALIGNDATA_LA_TYPE_OPC,
        ALIGNDATA_LA_TYPE_DPC, 
        ALIGNDATA_LA_TYPE_ALIGNDATAP_CLIENT_SITE,
        ALIGNDATA_LA_TYPE_ALIGNDATAP_SERVER_SITE,
        ALIGNDATA_LA_TYPE_ALIGNDATAP_LA_RA,
        ALIGNDATA_LA_TYPE_SOURCE_ASN,
        ALIGNDATA_LA_TYPE_DEST_ASN,
        ALIGNDATA_LA_TYPE_VXLAN_VID,
        ALIGNDATA_LA_TYPE_MCC_MNC,
        ALIGNDATA_LA_TYPE_CLIENT_ORIGIN_REALM,
        ALIGNDATA_LA_TYPE_SERVER_ORIGIN_REALM,
        ALIGNDATA_LA_TYPE_SITE,
        ALIGNDATA_LA_TYPE_VRF,
        ALIGNDATA_LA_TYPE_SGT,
        ALIGNDATA_LA_TYPE_MICRO_SEGMENT,
        ALIGNDATA_LA_TYPE_PFS_PORT_ID,
        ALIGNDATA_LA_TYPE_MAX
    };

    enum LitealigndataDataType
    {
        DATA_Invalid = 0,
        DATA_NUMBER,
        DATA_BCD,
        DATA_ASCII,
        DATA_BYTE
    };

    enum SysLogDebugLevel
    {
        LOG_LEVEL_None = 0,
        LOG_LEVEL_Low,
        LOG_LEVEL_Medium,
        LOG_LEVEL_High,
        LOG_LEVEL_Detail_High,
        LOG_LEVEL_Very_Detail_High,
        LOG_LEVEL_Max
    };
    //;SysLogDebugLevel Debug

    #define ALIGNDATA_KEYS_0 0 // Used to index the la-type in flow_info, rt_data and socket
    #define ALIGNDATA_KEYS_1 1
    #define ALIGNDATA_KEYS_2 2
    #define ALIGNDATA_KEYS_3 3
    #define ALIGNDATA_KEYS_4 4
    #define ALIGNDATA_KEYS_5 5
    #define ALIGNDATA_KEYS_6 6
    #define ALIGNDATA_KEYS_7 7
    #define ALIGNDATA_KEYS_8 8
    #define ALIGNDATA_KEYS_9 9
    #define ALIGNDATA_KEYS_10 10
    #define ALIGNDATA_KEYS_11 11
    #define ALIGNDATA_KEYS_12 12
    #define ALIGNDATA_KEYS_MAX  ALIGNDATA_KEYS_12

    #define ALIGNDATA_SUB_STATS_KEY 0
    #define ALIGNDATA_SUB_STATS_TLV_KEY 1

    #define IPV6_ADDR_STR_LEN 46

    #define IPV6_ADDR_LEN   16
    #define IPV6_ADDR_WORDS  4
    #define IPV6_ADDR_N64    2


    typedef uint32_t tAPPLICATION_ID;
    typedef uint32_t tMSG_ID;

    typedef uint32_t tIPV4_ADDR;
    typedef uint32_t tALIGNDATA_KEY;
    typedef uint8_t tQOS;

   const static char* autoTypeString[] =
    {
        "AUTO_Invalid",
        "AUTO_Head",
        "AUTO_Body",
        "AUTO_Struct",
        "AUTO_Union",
        "AUTO_UINT64",
        "AUTO_INT64",
        "AUTO_UINT32",
        "AUTO_INT32",
        "AUTO_UINT16",
        "AUTO_INT16",
        "AUTO_UINT8",
        "AUTO_INT8",
        "AUTO_IPV4",
        "AUTO_IPV6",
        "AUTO_IPV4V6",
        "AUTO_DateTime",
        "AUTO_ByteArray",
        "AUTO_STRING",
        "AUTO_IMSIBCD",
        "AUTO_BCD",
        "AUTO_TIMEVAL",
        "AUTO_SP_U8_4",
        "AUTO_SP_IPV6",
        "AUTO_IMSI_TBCD",
        "AUTO_IMEISV_BCD",
        "AUTO_RealTime",
        "AUTO_BCD_2Bytes",
        "AUTO_IPV6_RSA_CU",
        "AUTO_Metric_List",
        "AUTO_RealTime_ms",
        "AUTO_IPV4_IPV6",
        "AUTO_SharePointer_MAC_6Byte",
        "AUTO_IPV4_NET",
        "AUTO_IPV6_NET",    
        "AUTO_STRING_NET",
    };

    const static char* AttrTagString[] =
    {
        "invalid",
        "union_flag",
        "name",
        "interval",
        "fileNum",
        "rowNum",
        "rowNumPerFlow",
        "type",
        "genFuncName",
        "sourceType",
        "len",
        "LowRange",
        "HighRange",
        "Prefix",
        "TableId",
        "localkey",
        "localkeytype",
        "localkeybit",
        "ColIdx",
        "csvheadFlag",
        "csvFlag",
        "bitlen",
        "path",
        "seedfile",
        "TableName",
        "Confile", //jsonFile
        "seedfileReadlineOrWhole",
        "isStream",
        "tableType",
        "metricsNum",
        "RowsPerTimeInterval",
        "percentage",
        "offset",
        "misc",
        "percentage_period_rows",
        "duplicateIfnStart",
        "duplicateIfnNum",
        "row_quantity_each_time_read",
        "templateid",    
        "colValue",
        "isRandomOrder"
    };

    const static char* elementTagString[] =
    {
        "INVALID",
        "aligndataMODEL",
        "INTERFACE",
        "TABLE",
        "Head",
        "Body",
        "Struct",
        "Union",
        "COLUMN",
		"Bit",
        "PARAMETER",
        "SUBCOLUMN",
		"Subbit",
        "DimensionalFile",
        "FileColumn",
        "Function",
        "FuncParameter"
    };

    const static char* funcTagString[] =
    {
        "invalid",
        "accumulate",
        "random",
        "fileReader",
        "fix",
        "merge",
        "None",
        "RedefineRowNumber",
        "Correlation",
        "UniqueRandom"
    };

    const static char* DataTypeString[] =
    {
        "INVALID",
        "NUMBER",
        "BCD",
        "ASCII",
        "BYTE"
    };


    const static char* ParaTypeString[] =
    {
        "invalid",
        "prefix",
        "init",
        "step",
        "lowrange",
        "highrange",
        "SourceType",
        "value",
        "imsitype",
        "depColumn",
        "depKey",
        "totalnumber",
        "filenumber",
        "percent",
        "rowsInBatch",
        "percentInBatch",
        "reset",
        "maxnum",
        "subscriberid_type",
        "subscriberid_highrange",
        "subscriberid_lowrange",
        "sessionid_highrange",
        "sessionid_lowrange",
        "is_ipv4",
        "ip_highrange",
        "ip_lowrange",
        "port_highrange",
        "port_lowrange",
        "Effect_On_Interface",
        "sip_child_app",
        "byte_array_right_align",
        "duplicatedunit"
    };
    union _IPV4_IPV6_ADDR_
    {
        uint32_t v4_addr[4];
        struct
        {
            uint64_t high;
            uint64_t low;
        } hl;
    };
    //#include "com/AlignDataTypeDef.h"
    //1Mb/s
    #define SEND_BIT_RATE (uint64_t)(1*1024*1024/1000000)
    #define OFFSET(TYPE,MEMBER) ( (uint64_t) (&(( (TYPE *)0)->MEMBER) ))

    typedef union _IPV4_IPV6_ADDR_ IPV4_IPV6_ADDR;
    typedef union _IPV4_IPV6_ADDR_ IPV6_ADDR;
    #define ONE_MILLION (1000000)
    typedef struct _NametypeEnDeFun
    {
        char name[32];
        autoType type;
        uint16_t len;
        char *encodeFun;
        char *decodeFun;
    }NametypeEnDeFun;

    NametypeEnDeFun* findNametype(char* name);
    autoType findNameautotype(char* name);
    uint16_t findNamelen(autoType type);
    uint16_t findNametypeLen(char* name);

};

typedef struct
{
    uint8_t  table_id;
    uint16_t entry_size;
    uint32_t ifn;
    uint32_t num_entries;
    uint8_t  reserved4[9];
}  ALIGNDATA_TABLE;

// 最大栈深度（适配多层嵌套）
#define MAX_STACK_DEPTH 100
// 动态数组初始大小
#define INIT_ARRAY_SIZE 8

// 类型映射结构体
typedef struct {
    const char* xml_type;
    const char* c_type;
    size_t size;
} TypeMap;

// 动态数组（存储联合体成员长度）
typedef struct {
    size_t* data;
    int size;
    int capacity;
} DynamicArray;

// 嵌套元素信息（栈元素）
typedef struct {
    char name[64];       // 结构体/联合体名称
    char type[16];       // 类型（struct/union）
    size_t total_length; // 结构体：累加长度；联合体：最大长度
    DynamicArray member_lengths; // 联合体成员长度
} NestedElement;


// 栈结构体（数组模拟）
typedef struct {
    NestedElement data[MAX_STACK_DEPTH];
    int top; // 栈顶指针（-1为空）
} Offset_Stack;

// ==================== 工具函数 ====================
// 初始化动态数组
void dynamic_array_init(DynamicArray* arr);

// 动态数组扩容
void dynamic_array_resize(DynamicArray* arr);

// 动态数组添加元素
void dynamic_array_add(DynamicArray* arr, size_t value);

// 释放动态数组内存
void dynamic_array_free(DynamicArray* arr);

// 找动态数组最大值
size_t dynamic_array_max(DynamicArray* arr);

// 初始化栈
void stack_init(Offset_Stack* stack);

// 判断栈是否为空
int stack_is_empty(Offset_Stack* stack);

// 判断栈是否满
int stack_is_full(Offset_Stack* stack);

// 压栈
int stack_push(Offset_Stack* stack, NestedElement elem);

// 弹栈
NestedElement stack_pop(Offset_Stack* stack);

// 获取栈顶元素
NestedElement* stack_top(Offset_Stack* stack);

// 根据XML类型查找映射信息
TypeMap* find_type_map(const char* xml_type);


#define MAX_STR_LEN 256
#define MAX_EXAMPLE_TABLE     150000000 

template<typename Element>
class MapperStackIterator : public std::iterator<std::random_access_iterator_tag, Element>
{
public:
    typedef Element value_type;

public:
    MapperStackIterator() :
        offset( 0 )
    {
    }

    MapperStackIterator( const value_type* ptr )
    {
        offset = ptr - m_start;
    }

    explicit MapperStackIterator( uint32_t offsetIn ) :
        offset( offsetIn )
    {
    }

    value_type& operator * () const
    {
        return *( m_start + offset );
    }

    value_type* operator -> () const
    {
        return ( m_start + offset );
    }

    bool operator == ( const MapperStackIterator& iter ) const
    {
        return offset == iter.offset;
    }

    bool operator != ( const MapperStackIterator& iter ) const
    {
        return ( offset != iter.offset );
    }

    bool operator > ( const MapperStackIterator& iter ) const
    {
        return ( offset > iter.offset );
    }

    bool operator >= ( const MapperStackIterator& iter ) const
    {
        return ( offset >= iter.offset );
    }

    bool operator < ( const MapperStackIterator& iter ) const
    {
        return ( offset < iter.offset );
    }

    bool operator <= ( const MapperStackIterator& iter ) const
    {
        return ( offset <= iter.offset );
    }

    MapperStackIterator operator + ( const uint32_t n ) const
    {
        return MapperStackIterator( offset + n );
    }

    MapperStackIterator operator - ( const uint32_t n ) const
    {
        return MapperStackIterator( offset - n );
    }

    uint32_t operator + ( const MapperStackIterator iter ) const
    {
        return offset + iter.offset;
    }

    uint32_t operator - ( const MapperStackIterator iter ) const
    {
        return offset - iter.offset;
    }

    MapperStackIterator& operator += ( const uint32_t n )
    {
        offset += n;
        return *this;
    }

    MapperStackIterator& operator -= ( const uint32_t n )
    {
        offset -= n;
        return *this;
    }

    MapperStackIterator& operator = ( const value_type* iter )
    {
        offset = iter - m_start;
        return *this;
    }

    MapperStackIterator& operator = ( const MapperStackIterator& iter )
    {
        offset = iter.offset;
        return *this;
    }

    MapperStackIterator& operator ++ ()
    {
        ++offset;
        return *this;
    }

    MapperStackIterator operator ++ ( int )
    {
        MapperStackIterator temp = *this;
        ++offset;

        return temp;
    }

    MapperStackIterator& operator -- ()
    {
        --offset;
        return *this;
    }

    MapperStackIterator operator -- ( int )
    {
        MapperStackIterator temp = *this;
        --offset;

        return temp;
    }

    static void setStartAddr( value_type* startAddrIn )
    {
        m_start = startAddrIn;
    }

    static value_type* getStartAddr()
    {
        return m_start;
    }

private:
    static value_type* m_start;
    uint32_t offset;
};

template<typename Element>
typename MapperStackIterator<Element>::value_type* MapperStackIterator<Element>::m_start = NULL;

template<typename Element>
class MmapStack
{
public:
    typedef MapperStackIterator<Element> iterator;
    typedef const MapperStackIterator<Element> const_iterator;

public:
    MmapStack( const std::string& mmapFile ) :
        m_mmapFile( mmapFile ), m_recordsNum( 0 ), m_fileHeader( NULL ), mmapFd( -1 )
    {
        //this->m_headerSize = sizeof( ALIGNDATA_TABLE );
    }

    ~MmapStack()
    {
        stopMappingFile();
        closeMappingFile();
    }

    bool OpenMappingFile();

    iterator begin() const
    {
        return m_Start;
    }

    iterator end() const
    {
        return m_End;
    }

    const ALIGNDATA_TABLE* getFileHeader() const
    {
        return m_fileHeader;
    }

    ALIGNDATA_TABLE* getFileHeader()
    {
        return m_fileHeader;
    }

    uint32_t getRecordsNum() const
    {
        return m_recordsNum;
    }

    void setRecordsNum( uint32_t recordsNum )
    {
        m_recordsNum = recordsNum;
    }

private:
    void stopMappingFile();

    void closeMappingFile();

    bool writeMmapHeaer()
    {
        ALIGNDATA_TABLE tmpHeader;
        uint32_t writeSize = write( mmapFd, reinterpret_cast<char*>( &tmpHeader ), sizeof( ALIGNDATA_TABLE ) );

        if ( writeSize != sizeof( ALIGNDATA_TABLE ) )
        {
            return false;
        }

        return true;
    }

    bool writebody()
    {
        Element* element = new Element[m_recordsNum];

        uint64_t toWriteSize = m_recordsNum * sizeof( Element );

        memset( element, 0, toWriteSize );

        uint64_t beWrited = write( mmapFd, element, toWriteSize );

        delete[] element;

        if ( beWrited != toWriteSize )
        {
            return false;
        }

        return true;
    }

private:
    std::string m_mmapFile;
    uint32_t m_headerSize;
    uint32_t m_recordsNum;
    ALIGNDATA_TABLE* m_fileHeader;
    int32_t mmapFd;
    uint64_t mappingSize;

    iterator m_Start;
    iterator m_End;
};

template<typename Element>
bool MmapStack<Element>::OpenMappingFile()
{

    mmapFd = open( m_mmapFile.c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRWXU | S_IXUSR );

    if ( -1 == mmapFd )
    {
        return false;
    }

    mappingSize = this->m_headerSize + this->m_recordsNum * sizeof( Element );

    void* mem = mmap( NULL, mappingSize, PROT_READ | PROT_WRITE, MAP_SHARED, mmapFd, 0 );

    if ( MAP_FAILED == mem )
    {
        return false;
    }

    m_fileHeader = reinterpret_cast<ALIGNDATA_TABLE*>( mem );

    MmapStack::iterator::setStartAddr( reinterpret_cast<Element*>( m_fileHeader + 1 ) );

    this->m_Start = MmapStack::iterator::getStartAddr();

    this->m_End = this->m_Start + m_recordsNum;

    return true;
}

template<typename Element>
void MmapStack<Element>::stopMappingFile()
{
    if ( m_fileHeader != NULL )
    {
        if ( -1 == munmap( reinterpret_cast<uint8_t*>( m_fileHeader ), mappingSize ) )
        {
            fprintf( stderr, "close error with msg is: %s\n", strerror( errno ) );
        }

        m_fileHeader = NULL;
    }
}

template<typename Element>
void MmapStack<Element>::closeMappingFile()
{
    if ( mmapFd != -1 )
    {
        close( mmapFd );
        mmapFd = -1;
    }
}

class WriteFile
{
public:
    WriteFile( const std::string& fileName ) :
        m_pfile( NULL ), m_fileName( fileName )
    {
        this->m_headerSize = sizeof( ALIGNDATA_TABLE );
    }

    WriteFile(void) :
        m_pfile( NULL ), m_fileName("")
    {
        this->m_headerSize = sizeof( ALIGNDATA_TABLE );
    }

    ~WriteFile()
    {
        if ( m_pfile != NULL )
            fclose( m_pfile );
    }

    bool openfile(const char* mode = "wb")
    {
    
        this->m_pfile = fopen( m_fileName.c_str(), mode );

        if ( m_pfile == NULL )
        {
	        char cwdCur[MAX_PATH] = {0};
	        char *ch=getcwd( cwdCur, MAX_PATH );
     
            int errNum = errno;
            return false;
        }

        return true;
    }

    void closefile()
    {
        if ( m_pfile != NULL )
            fclose( m_pfile );

        m_pfile = NULL;
    }

    uint32_t getRecordsNum() const
    {
        return m_recordsNum;
    }

    void setRecordsNum( uint32_t recordsNum )
    {
        m_recordsNum = recordsNum;
    }

    template<typename Element>
    bool
    writeTable( const ALIGNDATA_TABLE* fileHeader, const Element* body, int nCount )
    {
        bool ret = writeHeader( fileHeader );

        if ( ret == false )
            return false;

        return writeBody( body, nCount );
    }

    bool writeTable( const ALIGNDATA_TABLE* fileHeader, uint8_t* body, uint64_t nCount )
    {
        if ( !writeHeader( fileHeader ) )
            return false;

        return writebody( body, nCount );
    }


    void setFileName(const std::string fileName )
    {
        m_fileName = fileName;
    }

    bool writeHeader( const ALIGNDATA_TABLE* fileHeader )
    {
        if ( fileHeader == NULL )
            return true;

        uint32_t size = fwrite( fileHeader, sizeof( ALIGNDATA_TABLE ), 1, m_pfile );

        if ( size != 1 )
        {
            return false;
        }

        return true;
    }

    bool reWriteHeader( const ALIGNDATA_TABLE* fileHeader )
    {
        if ( fileHeader == NULL )
            return true;

        fseek(m_pfile,0,SEEK_SET);

        uint32_t size = fwrite( fileHeader, sizeof( ALIGNDATA_TABLE ), 1, m_pfile );

        if ( size != 1 )
        {
            return false;
        }

        return true;
    }


    template<typename Element>
    bool
    writeBody( const Element* body, uint32_t ncount )
    {
        uint32_t size = fwrite( body, sizeof( Element ), ncount, m_pfile );

        if ( size != ncount )
        {
            return false;
        }

        return true;
    }

    bool writebody( uint8_t* body, uint64_t nCount )
    {
        uint64_t size = 0;

        while ( nCount > 0 )
        {
            size = fwrite( body, sizeof( uint8_t ), nCount, m_pfile );

            if ( size <= 0 )
            {
                return false;
            }

            body += size;
            nCount -= size;
        }

        return true;
    }

private:
    FILE* m_pfile;

    std::string m_fileName;

    uint32_t m_headerSize;

    uint32_t m_recordsNum;
};

#endif

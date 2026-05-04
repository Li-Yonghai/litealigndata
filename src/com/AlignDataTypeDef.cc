/*
 * =============================================================================
 *
 *       Filename:  alignDataTypeDef.cc
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

#include "AlignDataTypeDef.h"

void CAlignDataTypeDef::get_time_diff( uint32_t start_sec, uint32_t start_usec, uint32_t end_sec,
                    uint32_t end_usec, uint32_t& diff_sec, uint32_t& diff_usec )
{
    uint64_t start = ( uint64_t )start_sec * ONE_MILLION + start_usec;
    uint64_t end = ( uint64_t )end_sec * ONE_MILLION + end_usec;

    diff_sec = 0;
    diff_usec = 0;

    if ( start >= end )
        return;

    uint64_t diff = end - start;
    diff_sec = diff / ONE_MILLION;
    diff_usec = diff % ONE_MILLION;
}

void CAlignDataTypeDef::add_time_diff( uint32_t start_sec, uint32_t start_usec, uint32_t diff_sec,
                    uint32_t diff_usec, uint32_t& end_sec, uint32_t& end_usec )
{
    uint64_t start = ( uint64_t )start_sec * ONE_MILLION + start_usec;
    uint64_t diff = ( uint64_t )diff_sec * ONE_MILLION + diff_usec;
    uint64_t end = start + diff;
    end_sec = end / ONE_MILLION;
    end_usec = end % ONE_MILLION;
}


int CAlignDataTypeDef::FindRelativeTag( const char* pstringArray[], uint32_t size, const char* pstr )
{
    for ( uint32_t i = 1; i < size; ++i )
    {
        if ( strcasecmp( pstringArray[i], pstr ) == 0 )
            return i;
    }

    return 0;
}


void CAlignDataTypeDef::ParserTableInfoFromString( const char* ParseredStr, uint32_t strLen, std::string& strTable,
                                uint32_t& Row, std::string& strCol )
{
    if ( ParseredStr == NULL || strLen == 0 )
        return;

    ParseredStr = trim( ( char* )ParseredStr );

    const char* pFirLeftSqbr = strchr( ParseredStr, '[' );

    if ( pFirLeftSqbr == NULL )
        return;

    const char* pFirRigSqbr = strchr( pFirLeftSqbr, ']' );

    if ( pFirRigSqbr == NULL )
        return;

    uint32_t matchLen = static_cast<uint32_t>( pFirRigSqbr - ParseredStr ) + 1; //
    strCol.clear();
    strTable.clear();

    std::string strRow;
    strRow.clear();


    uint32_t TableLen = static_cast<uint32_t>( pFirLeftSqbr - ParseredStr );
    strTable.insert( 0, ParseredStr, TableLen );

    uint32_t rowLen = static_cast<uint32_t>( pFirRigSqbr - pFirLeftSqbr ) - 1; //

    //if only matches one pare of "[]". it is a colName.
    if ( matchLen == strLen )
    {
        strCol.insert( 0, pFirLeftSqbr + 1, rowLen );

        ACE_DEBUG((LM_DEBUG, "parser info: tableName = %s and colName = %s.\n", strTable.c_str(), strCol.c_str()));

        return;
    }

    if ( matchLen < strLen )
    {
        const char* pSecLeftSqbr = strchr( pFirRigSqbr, '[' );

        if ( pSecLeftSqbr == NULL )
            return;

        const char* pSecRigSqbr = strchr( pSecLeftSqbr, ']' );

        if ( pSecRigSqbr == NULL )
            return;

        uint32_t colLen = static_cast<uint32_t>( pSecRigSqbr - pSecLeftSqbr ) - 1; //
        strRow.insert( 0, pFirLeftSqbr + 1, rowLen );
        strCol.insert( 0, pSecLeftSqbr + 1, colLen );

        Row = atoi( strRow.c_str() );

        ACE_DEBUG((LM_DEBUG, "parser info: tableName = %s, rowNum = %d and colName = %s.\n", 
                   strTable.c_str(), Row, strCol.c_str() ));
    }

    return;
}

bool CAlignDataTypeDef::isAlldigits( const char* pchar, int Len )
{
    for ( int i = 0; i < Len; ++i )
    {
        if ( !isdigit( pchar[i] ) )
            return false;
    }

    return true;
}

uint64_t CAlignDataTypeDef::swap64Bits( uint64_t i )
{
    return ( ( i & 0xff00000000000000ULL ) >> 56 ) |
           ( ( i & 0x00ff000000000000ULL ) >> 40 ) |
           ( ( i & 0x0000ff0000000000ULL ) >> 24 ) |
           ( ( i & 0x000000ff00000000ULL ) >>  8 ) |
           ( ( i & 0x00000000ff000000ULL ) <<  8 ) |
           ( ( i & 0x0000000000ff0000ULL ) << 24 ) |
           ( ( i & 0x000000000000ff00ULL ) << 40 ) |
           ( ( i & 0x00000000000000ffULL ) << 56 );
}


const TableCollumnOffsetDef* CAlignDataTypeDef::FindTableInfo( 
            const TableCollumnOffsetDef* defarray, uint32_t entry_size, uint32_t tableId )
{
    for ( uint32_t i = 0; i < entry_size; ++i )
    {
        if ( tableId == defarray[i].tableId )
            return &defarray[i];
    }

    return NULL;
}

const TableCollumnOffsetDef* CAlignDataTypeDef::FindColumnInfo( 
    const TableCollumnOffsetDef* defarray, uint32_t entry_size, uint32_t tableId, 
    uint32_t columnidx )
{
    for ( uint32_t i = 0; i < entry_size; ++i )
    {
        if ( tableId == defarray[i].tableId  && columnidx == defarray[i].colIdx )
            return &defarray[i];
    }

    return NULL;
}

uint64_t CAlignDataTypeDef::randnumer( uint64_t low, uint64_t high )
{
    if ( high == MAX_UINT64 )
        high = RAND_MAX;

    if ( high == 0 )
        return 0;

    if ( low > high )
        return 0;

    uint64_t max = high - low + 1;

    return rand() % max + low;
}


uint8_t CAlignDataTypeDef::NibbleSwap( uint8_t i )
{
    return ( ( 0x00F0 & ( i << 4 ) ) | ( 0x000F & ( i >> 4 ) ) );
}

void CAlignDataTypeDef::TimeUsecToString( uint64_t time, char* str, uint32_t strLen )
{
    time_t timeSec = ( time_t )( time / 1000000 );
    uint32_t timeMsec = ( uint32_t )( time - ( uint32_t )timeSec * 1000000 ) / 1000;
    struct tm* ptm = localtime( &timeSec );
    char tmpStr[40];
    strftime( tmpStr, sizeof( tmpStr ), "%Y-%m-%d %H:%M:%S", ptm );
    snprintf( str, strLen, "%s.%03u", tmpStr, timeMsec );
}

void CAlignDataTypeDef::splitValue( const char* defaultValue, strArray& strValueList )
{
    char* pToken = NULL;
    char* pSave = NULL;
    const char* pDelimiter = ",";

    pToken = strtok_r( ( char* ) defaultValue, pDelimiter, &pSave );

    while ( pToken )
    {
        strValueList.push_back( pToken );
        pToken = strtok_r( NULL, pDelimiter, &pSave );
    }
}
std::string CAlignDataTypeDef::Trim( std::string& str )
{
    str.erase( 0, str.find_first_not_of( " \t\r\n" ) );
    str.erase( str.find_last_not_of( " \t\r\n" ) + 1 );
    return str;
}
uint64_t CAlignDataTypeDef::calc_align( uint64_t n, uint64_t align )
{
    if ( n / align * align == n )
        return n;

    return ( n / align + 1 ) * align;
}
char* CAlignDataTypeDef::trim( char* psz )
{
    char* left = psz;

    char* right = left + strlen( psz ) - 1;

    while ( *left == ' ' || *left == '\t' || *left == '\r' || *left == '\n' )
    {
        left++;
    }

    while ( *right == ' ' || *right == '\t' || *right == '\r' || *right == '\n' )
    {
        *right = '\0';
        right--;
    }

    return left;
}

uint32_t CAlignDataTypeDef::GetTotalLineCount( FILE* fp )
{
    uint32_t i = 0;
    char strLine[1024];
    fseek( fp, 0, SEEK_SET );

    while ( fgets( strLine, 1024, fp ) )
        i++;

    fseek( fp, 0, SEEK_SET );
    return i;
}

char* CAlignDataTypeDef::strupr( char* str )
{
    char* ptr = str;

    while ( *ptr != '\0' )
    {
        if ( islower( *ptr ) )
            *ptr = toupper( *ptr );

        ptr++;
    }

    return str;
}

std::string& CAlignDataTypeDef::str_trim( std::string& s )
{
    if ( s.empty() )
    {
        return s;
    }

    s.erase( 0, s.find_first_not_of( " " ) );
    s.erase( s.find_last_not_of( " " ) + 1 );
    return s;
}

// set new ACE log file for each thread
char* CAlignDataTypeDef::setNewLogFile(std::string& xmlfile, char* curLog)
{
    std::string logPath = "./output/log/";
    auto n = xmlfile.rfind('/');
    std::string templateName = xmlfile.substr(n+1);
    auto x = templateName.rfind('.');
    std::string traffic = templateName.substr(0,x);
    time_t cur_time;
    time( &cur_time );
    tm time = *gmtime(&cur_time);
    sprintf( curLog, "%ssys_%02d-%02d-%d_%s_pid%d.log", logPath.c_str(), 
        time.tm_mon +1, time.tm_mday, time.tm_year + 1900,  
        traffic.c_str(), getpid());

    ACE_OSTREAM_TYPE *outStream = new std::ofstream(curLog, std::ios::out | std::ios::app);
    ACE_LOG_MSG->msg_ostream (outStream, 1);
    //if(!isVerbose)
    ACE_LOG_MSG->clr_flags (ACE_Log_Msg::STDERR);
    ACE_LOG_MSG->set_flags (ACE_Log_Msg::OSTREAM);

    return curLog;
}

uint8_t CAlignDataTypeDef::parseStrToArray( const std::string& str, uint16_t v6_addr[] )
{
    char* pToken = NULL;
    char* pSave = NULL;
    const char* pDelimiter = ":";
    uint32_t i = 0;

    if ( str.empty() )
        return 0;

    pToken = strtok_r( ( char* )str.c_str(), pDelimiter, &pSave );

    //while ( pToken && i < Litealigndata_IPV6_SIZE )
    {
        v6_addr[i] = ( uint16_t )strtoul( pToken, NULL, 16 );
        pToken = strtok_r( NULL, pDelimiter, &pSave );
        i++;
    }

    return i;
}

void autoValue::setSpMac( const uint64_t value )
{
    
}

void autoValue::AsciiToSpU8_4( const char* ascii )
{
}

void autoValue::AsciiToStr( const char* ascii )
{
    std::string* pStr = ( std::string* )getData();
    *pStr = ascii;
}

void autoValue::AsciiToNetStr( const char* str )
{
    m_len = strlen( str);
    char* pTempData = (char*)getData();
    memcpy( pTempData, str, m_len );
}
void autoValue::AsciiToByteArray( const char* ascii, size_t len, uint32_t is_Right_align )
{
    if ( len > 255 )
        len = 255;

    if(false == is_Right_align)
    {
        char* pStr = ( char* )getData();
        memcpy( pStr, ascii, len );
        *( pStr + len ) = '\0';
    }
    else
    {
       uint32_t data_length = getLen();
       char* pHead = ( char* )getData();
       memset(pHead,0,data_length);

       if(len > data_length)
         len = data_length;
       
       uint32_t offset = data_length - len;
       memcpy( pHead + offset, ascii, len );
    }
}
void autoValue::AsciiToDec( const char* ascii )
{
    uint64_t number = 0;
    bool isNullSting = !strcmp(ascii,"");

    if(isNullSting){
        number = magicNumber; // 
    }else{
        number = strtoull( ascii, NULL, 10 );
    }       
    
    setNumber( number );
}

void autoValue::AsciiToHex( const char* ascii )
{
    uint64_t number = strtoull( ascii, NULL, 16 );
    setNumber( number );
}

void autoValue::IPv6ToSpIPv6( const litealigndata::IPV4_IPV6_ADDR* addr )
{
}

void autoValue::setNumber( uint64_t number )
{
    uint8_t* pData = NULL;
    std::string str;

    if ( isSubColumn )
        pData = ( uint8_t* )m_inerData;
    else
        pData = m_data;


    switch ( this->m_type )
    {
    case litealigndata::AUTO_UINT64:
    case litealigndata::AUTO_IMSIBCD:
    case litealigndata::AUTO_BCD:
    case litealigndata::AUTO_IMSI_TBCD:
    case litealigndata::AUTO_IMEISV_BCD:
    case litealigndata::AUTO_RealTime:
    case litealigndata::AUTO_RealTime_ms:
        *( ( uint64_t* )pData ) = number;        
        break;

    case litealigndata::AUTO_INT64:
        *( ( int64_t* )pData ) = number;
        break;
        
    case litealigndata::AUTO_UINT32:
    case litealigndata::AUTO_IPV4:
    case litealigndata::AUTO_IPV4V6:
        if ( number >= 1ULL << 32 )
            number -= 1ULL << 32;

        *( ( uint32_t* )pData ) = number;
        break;

    case litealigndata::AUTO_INT32:
        *( ( int32_t* )pData ) = ( int32_t )number;
        break;

    case litealigndata::AUTO_UINT16:
    case litealigndata::AUTO_BCD_2Bytes:    
        if ( number >= 1 << 16 )
            number -=  1 << 16;

        *( ( uint16_t* )pData ) = number;
        break;

    case litealigndata::AUTO_INT16:
        *( ( int16_t* )pData ) = ( int16_t )number;
        break;

    case litealigndata::AUTO_UINT8:
        if ( number >= 1 << 8 )
            number -=  1 << 8;

        *( ( uint8_t* )pData ) = number;
        break;

    case litealigndata::AUTO_INT8:
        *( ( int8_t* )pData ) = ( int8_t )number;
        break;

    case litealigndata::AUTO_ByteArray:
        str = std::to_string( number );
        memcpy( pData, str.c_str(), str.size() );
        pData[str.size()] = '\0';
        break;

    case litealigndata::AUTO_SharePointer_MAC_6Byte:
        setSpMac(number);
        break;
    default:
        break;
    }
}

uint64_t autoValue::getnumber()
{
    uint8_t* pData = NULL;

    if ( isSubColumn )
        pData = ( uint8_t* )m_inerData;
    else
        pData = m_data;

    switch ( m_type )
    {
    case litealigndata::AUTO_UINT64:
    case litealigndata::AUTO_IMSIBCD:
    case litealigndata::AUTO_BCD:
    case litealigndata::AUTO_IMSI_TBCD:
    case litealigndata::AUTO_IMEISV_BCD:   
    case litealigndata::AUTO_RealTime:
    case litealigndata::AUTO_RealTime_ms:
        return *( ( uint64_t* )pData );

    case litealigndata::AUTO_INT64:
        return *( ( int64_t* )pData );

    case litealigndata::AUTO_UINT32:
    case litealigndata::AUTO_IPV4:
    case litealigndata::AUTO_IPV4V6:     
        return *( ( uint32_t* )pData );

    case litealigndata::AUTO_INT32:
        return *( ( int32_t* )pData );

    case litealigndata::AUTO_UINT16:
    case litealigndata::AUTO_BCD_2Bytes:    
        return *( ( uint16_t* )pData );

    case litealigndata::AUTO_INT16:
        return *( ( int16_t* )pData );

    case litealigndata::AUTO_UINT8:
        return *( ( uint8_t* )pData );

    case litealigndata::AUTO_INT8:
        return *( ( int8_t* )pData );

    default:
        return 0;
    }
}

std::string autoValue::getString()
{
    uint8_t* pData = NULL;

    if ( isSubColumn )
        pData = ( uint8_t* )m_inerData;
    else
        pData = m_data;

    std::string str = ( char* )pData;

    //printf("pData: %s\n",(char*)pData);
    return str;
}

std::string autoValue::getToString()
{
//    stringstream toString;
    std::string resultStream;

    litealigndata::GenericType type = ConvertAutoTypeToGenericType(m_type);

    switch(type)
    {
        case litealigndata::Generic_NUMBER:
        {
          uint64_t tmpValue = getnumber();
          break;
        }
        case litealigndata::Generic_IPV6:
        {
          break;
        }
        case litealigndata::Generic_BYTEARRAY:
        {
          resultStream = getString();
          break;
        }
        case litealigndata::Generic_BCD:
        {
          break;
        }
        default:
        {
          break;
        } 
    }

    return resultStream;
}


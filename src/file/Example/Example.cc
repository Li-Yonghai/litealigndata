/*
 * =============================================================================
 *
 *       Filename:  example.cc
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
#include "file/Example/Example.h"
#include "struct/TableObject.h"


using namespace std;
//using namespace boost::posix_time;
//using namespace boost::gregorian;

#define MICROSECONDS_PER_SECOND (1000000)

const char mon_name[][4] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };


void CExampledata::decode_time_string(std::string& time, struct time_stamp_t *pTimeStamp)
{
}

shared_ptr<char> CExampledata::encode_time_string(time_stamp_t &timeStamp)
{
    shared_ptr<char>buffer( new char[512], [] (char *p) { delete [] p;} );
    sprintf(buffer.get(), "%04d-%s-%02d %02d:%02d:%02d.%06ld", (timeStamp.tm_time.tm_year+1900), mon_name[timeStamp.tm_time.tm_mon], 
        timeStamp.tm_time.tm_mday, timeStamp.tm_time.tm_hour, timeStamp.tm_time.tm_min, timeStamp.tm_time.tm_sec,
        timeStamp.us);
    return buffer;
}

CExampledata::exampleType CExampledata::example_get_exampleType(uint8_t table_id)
{
    switch ( table_id )
    {
        case EXAMPLE_TABLE_ID:
            return EXAMPLE_FEED_TABLE_TYPE;
        case EXAMPLE_TABLE_ID_1:
            return EXAMPLE_FEED_TABLE1_TYPE;
        default:
            return EXAMPLETYPE_MAX;
    }
}
void CExampledata::EntryColumn(uint8_t*& pData, listInterface::iterator it, std::string& lineStr, uint32_t index,
        std::string::size_type& n, std::string::size_type& m)
{
	litealigndata::elementTag tag = (*it)->getElementType();
	std::string ObjectName = (*it)->getObjectName();
    std::string str_type;
    //uint8_t* pDataStart = pData;
    const char* pToken = NULL;
    //uint8_t* pDataStart = pData;
	if (litealigndata::ELEMENT_Column == tag)
	{
        (*it)->findAttriValue( litealigndata::ATTR_Type, str_type );
        litealigndata::autoType auto_type = 
            litealigndata::findNameautotype((char*)(str_type.c_str()));
        n = lineStr.find(',', m);
        if( n == std::string::npos)
        {
            n = lineStr.length();
        }
        auto str = lineStr.substr(m, n-m);
        m = n+1;
        pToken = str.c_str();
        
        if( auto_type == litealigndata::AUTO_UINT64 ||
        auto_type == litealigndata::AUTO_RealTime ||
        auto_type == litealigndata::AUTO_RealTime_ms)
        {
           // cout << atoll(pToken) << endl;
            *(int64_t *)(pData) = atoll(pToken);
            (pData) += sizeof(int64_t);

        }
        else if(auto_type == litealigndata::AUTO_STRING ||
        auto_type == litealigndata::AUTO_ByteArray)
        {
            
        
            memcpy((pData), pToken, strlen(pToken));
            (pData)[strlen(pToken)] = '\0';
            //cout << pData << endl;
            pData += sizeof(char) * MAX_STR_LEN;
        }
		if(!strcasecmp ((*it)->getunionFflag().c_str(), "true"))
		{
            EntryColumn((pData), std::next(it), lineStr, atoll(pToken), n, m);
		
		}
	}
	else if(litealigndata::ELEMENT_Struct == tag)
	{
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
		for ( listInterface::iterator  it = obj.begin(); it != obj.end(); ++(it) )
		{
            //cout << pData << endl;
            EntryColumn((pData), it, lineStr, 0, n, m);
            //cout << pData << endl;
		}
	
	}
	else if (litealigndata::ELEMENT_Union == tag)
	{
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
        listInterface::iterator  it = obj.begin();
        uint16_t i = 0;
		for ( ; it != obj.end(); ++(it) )
		{
			if(i == index)
            {
                break;
            }
            i++;
		}
        EntryColumn((pData), it, lineStr, 0, n, m);
	}
	else
	{
	
	
	}
	
}
void CExampledata::example_parse_one_line( std::string& lineStr, uint8_t*& pData, 
        uint8_t table_id, CXmlParser* xml, CTableObject* table)
{
    //cout << "example_parse_one_line:     ###"<< lineStr <<"###" << endl;
    exampleType type = example_get_exampleType( table_id );
    if(type == EXAMPLETYPE_MAX)
    {
        ACE_DEBUG((LM_ERROR, "example_parse_one_line: wrong table_id for example: %u.\n", table_id));
        return;
    }
    
    uint8_t* pDataStart = pData;
    std::string::size_type n = 0, m = 0;

	CAlignDataObject* Body = table->findChildObject("Body");
	CAlignDataObject* BodyData =  Body->findChildObject("BodyData");

    std::list<CAlignDataObject*> obj = (Body)->getLitealigndataObjectList();
    for ( listInterface::iterator  it = obj.begin(); it != obj.end(); ++(it) )
    {
        EntryColumn(pData, it, lineStr, 0, n, m);
            
    }

	uint32_t m_entry_size = BodyData->getLen();//BodyData->getOffset();
    if ( pData - pDataStart != m_entry_size )
    {
        ACE_DEBUG((LM_ERROR, "wrong data length: %d.\n", (pData - pDataStart)));
    }

}

bool CExampledata::ReadData( std::string& fileName, CXmlParser* xml,
                CTableObject* table)
{
    ifstream readFile;
    
    readFile.open(fileName, ios::in);
    if ( readFile.fail() )
    {
        ACE_DEBUG((LM_ERROR, "current seedfile does not exist: %s.\n", fileName.c_str()));
        return false;
    }

    //uint64_t total_size = sizeof(uint8_t) * ((uint64_t)tableHeader.entry_size) * tableHeader.num_entries;
    uint64_t total_size = table->getnum_entries() * table->getentry_size();
    m_seedData = (uint8_t*)malloc( total_size );
    memset( m_seedData, 0, total_size );
    
    uint8_t* itemData = m_seedData;
    string inbuf;
    uint32_t i = 0;
    uint32_t header_line_left = 0;

   // if((EXAMPLE_TABLE_ID == tableHeader.table_id))
    //    header_line_left = 0;//ARBOR_INSIGHT_HEADER_LINE;

    std::string  table_id ;
    if ( !table->findAttriValue( litealigndata::ATTR_tableid, table_id ) )
    {
        return false;
    }   
    while ( !readFile.eof() )  
    {
        getline(readFile, inbuf);
        if( inbuf.empty() )
            break;
        example_parse_one_line(inbuf, itemData, atoi(table_id.c_str()), xml, table);
        inbuf = "";
        i++;
    }
    
    if( i != table->getnum_entries() )
    {
        ACE_DEBUG((LM_WARNING, "entries number in the seedfile: %d, entries number in the name: %d.\n", 
                   i, table->getnum_entries()));
        //tableHeader.num_entries = i;
        return false;
    }
    Setbody(m_seedData);
    Sethead(m_seedData);
    return true;
}
void CExampledata::WriteColumn(uint8_t* pData, listInterface::iterator it, std::string& encodedStr, uint64_t value)
{
	litealigndata::elementTag tag = (*it)->getElementType();
	std::string ObjectName = (*it)->getObjectName();
    std::string str_type;
    
	if (litealigndata::ELEMENT_Column == tag)
	{
        char name[256]={'\0'};
        memset(name, '\0', 256);
        uint64_t value;
        char *p_dr_table = (char*)pData;
        std::string attrValue;
        (*it)->findAttriValue( litealigndata::ATTR_Type, attrValue );
        litealigndata::autoType type = 
        litealigndata::findNameautotype((char*)(attrValue.c_str()));

        //pData = (char*)p_dr_table + litealigndata::findNamelen(type);
        p_dr_table = (char*)pData + (*it)->getOffset();
        //cout << (*it)->getOffset() << endl;
        if( type == litealigndata::AUTO_UINT64 ||
        type == litealigndata::AUTO_RealTime ||
        type == litealigndata::AUTO_RealTime_ms)
        {
            memcpy((char*)&value, p_dr_table, sizeof(uint64_t));
            encodedStr += to_string(value);
            encodedStr += ',';

        }
        else if(type == litealigndata::AUTO_STRING ||
        type == litealigndata::AUTO_ByteArray)
        {
            memcpy(name, p_dr_table, 256);
            encodedStr += name;
            memset(name, '\0', 256);
            encodedStr += ',';
        }
        
		if(!strcasecmp ((*it)->getunionFflag().c_str(), "true"))
		{
            WriteColumn(pData, std::next(it), encodedStr, value);
		
		}
	}
	else if(litealigndata::ELEMENT_Struct == tag)
	{
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
		for ( listInterface::iterator  it = obj.begin(); it != obj.end(); ++(it) )
		{
			WriteColumn(pData, it, encodedStr, 0);
		}
	
	}
	else if (litealigndata::ELEMENT_Union == tag)
	{
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
        listInterface::iterator  it = obj.begin();
        uint16_t i = 0;
		for ( ; it != obj.end(); ++(it) )
		{
			if(i == value)
            {
                break;
            }
            i++;
		}
        WriteColumn(pData, it, encodedStr, 0);
	}
	else
	{
	
	
	}

}
uint16_t CExampledata::example_encode_data(CAlignDataObject* Body, uint8_t* p_dr_table, uint8_t  table_id, 
        string& encodedStr)
{
    std::string::size_type n, m;
    switch ( table_id )
    {
        case EXAMPLE_TABLE_ID:
        case EXAMPLE_TABLE_ID_1:
        {
            char name[256]={'\0'};
            memset(name, '\0', 256);
            uint64_t value;
            //char *pData = (char*)p_dr_table;
        std::list<CAlignDataObject*> obj = (Body)->getLitealigndataObjectList();
        for ( listInterface::iterator  it = obj.begin(); it != obj.end(); ++(it) )
            WriteColumn(p_dr_table, it, encodedStr, 0);
        encodedStr.replace(encodedStr.size() - 1, 1, "\n");
            break;                           
        }

        default:
            ACE_DEBUG((LM_ERROR, "unsupported table id for example: %d.\n", table_id));
            return 0;
    }
    
    uint16_t dataLen = encodedStr.length();
    return dataLen;
}
uint16_t CExampledata::example_encode_header( unsigned char* buffer_ptr, uint32_t buffer_size, ALIGNDATA_TABLE& tableHeader )
{
    exampleType type = example_get_exampleType( tableHeader.table_id );
    if( type == EXAMPLETYPE_MAX )
    {
        ACE_DEBUG((LM_ERROR, "wrong table id: %d.\n", tableHeader.table_id));
        exit(0);
    }  
    uint16_t headerLen = 0;
    {
        ACE_DEBUG((LM_ERROR, "header length too long: %d, buffer_size: %d.\n", headerLen, buffer_size));
    }  
    return headerLen;
}
uint64_t CExampledata::GetRandomFileLinesCount(std::string fileName, CXmlParser* xml, CTableObject* table_obj)
{
	uint8_t tableId;
    
	string cmd;
	cmd = string("cat  ") + fileName + "  |  wc -l";
	FILE* fp;
	char buffer[128];
    fp = popen(cmd.c_str(), "r");
	fgets(buffer, sizeof(buffer), fp);
	pclose(fp);
    std::string csvheadFlag;
    table_obj->findAttriValue( litealigndata::ATTR_csvheadFlag, csvheadFlag );

    if(csvheadFlag == "true")
    {
        return (atol( buffer )-1);
    }

	return atol( buffer );

}
void CExampledata::GetRandomLines(CTableObject* table, std::string filename, uint64_t expectLineCount,
            std::vector<std::string>& Lines, CXmlParser* xml)
{
    ifstream readFile;
    readFile.open(filename, ios::in);
    string inbuf;
    if ( readFile.fail() )
    {
        return;
    }
    uint64_t n = random()%expectLineCount;
    uint64_t m = 1;

    std::string csvheadFlag;
    table->findAttriValue( litealigndata::ATTR_csvheadFlag, csvheadFlag );
    if(csvheadFlag == "true")   
    {
        getline(readFile, inbuf);
        inbuf = "";
    } 
    uint64_t line = 1;
    while ( !readFile.eof() )  
    {
        getline(readFile, inbuf);
        if(line++ <= n)
        {
            inbuf = "";
            continue;
        }
    
        while(!readFile.eof())
        {
            Lines.push_back(inbuf);
            inbuf = "";
            getline(readFile, inbuf);

            if(m++ >= RandomMaxSize)
            {
                break;
            }
            
        }
        if(readFile.eof() || m >= RandomMaxSize)
        {
            break;
        }
        line = 1;
        readFile.seekg(0, std::ios::beg);
        if(csvheadFlag == "true")   
        {
            getline(readFile, inbuf);
            inbuf = "";
        }
        
    }
    readFile.close();
}
void CExampledata::GetFileLineItemValues(CColumnObject* release_column_obj, std::vector<std::string>& Lines, 
                std::map<std::string, litealigndata::autoType>& item, 
                unsigned int currentIndex, unsigned int totalsize)
{
    const char* pToken;
    std::string lineStr;
    std::string::size_type n = 0, m = 0;
    //printf("===============[%ld]\n", Lines.size());
    litealigndata::autoType type = (static_cast<CColumnObject*>(release_column_obj))->getType();
    while(!Lines.empty())
    {
        lineStr = Lines.back();
        Lines.pop_back();
        for(unsigned int i = 0; i < totalsize; i++)
        {
            n = lineStr.find(',', m);
            if( n == std::string::npos)
            {
                n = lineStr.length();
            }
            auto str = lineStr.substr(m, n-m);
            m = n+1;
            pToken = (char*)str.c_str();
            if(currentIndex == i)
            {
                if(type == litealigndata::AUTO_UINT64)
                {
                    //printf("litealigndata::AUTO_UINT64   %lx\n", atol(pToken));
                    item.insert(make_pair(pToken, litealigndata::AUTO_UINT64));
                }
                else if(type == litealigndata::AUTO_RealTime)
                {
                    //item.insert(make_pair(to_string(atoll(pToken)), AUTO_RealTime));
                    item.insert(make_pair(pToken, litealigndata::AUTO_RealTime));
                }
                else if(type == litealigndata::AUTO_STRING)
                {
                    item.insert(make_pair(pToken, litealigndata::AUTO_STRING));
                }
                else if(type == litealigndata::AUTO_ByteArray)
                {
                    item.insert(make_pair(pToken, litealigndata::AUTO_ByteArray));
                }
                else
                {
                }
                m = n = 0;
                break;
            }
        }
            
    }
    
}
bool  CExampledata::GetAllLines(std::vector<std::string>& Lines, std::string file)
{
    ifstream readFile;
    readFile.open(file, ios::in);
    string inbuf;
    if ( readFile.fail() )
    {
            return false;
    }
    while ( !readFile.eof() )  
    {                       
            getline(readFile, inbuf);
        Lines.push_back(inbuf);    
    }
    readFile.close();
	return true;
};
void CExampledata::WriteColumnName(listInterface::iterator it, std::string& encodedStr)
{
	litealigndata::elementTag tag = (*it)->getElementType();
	std::string ObjectName = (*it)->getObjectName();
    std::string str_type;
    
	if (litealigndata::ELEMENT_Column == tag)
	{
		encodedStr = encodedStr + (*it)->getObjectName() + ",";
		if(!strcasecmp ((*it)->getunionFflag().c_str(), "true"))
		{
            WriteColumnName(std::next(it), encodedStr);
		
		}
	}
	else if(litealigndata::ELEMENT_Struct == tag)
	{
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
		for ( listInterface::iterator  it = obj.begin(); it != obj.end(); ++(it) )
		{
			WriteColumnName(it, encodedStr);
		}
	
	}
	else if (litealigndata::ELEMENT_Union == tag)
	{
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
        listInterface::iterator  it = obj.begin();
        uint16_t i = 0;
		// for ( ; it != obj.end(); ++(it) )
		// {
		// 	if(i == index)
        //     {
        //         break;
        //     }
        //     i++;
		// }
        WriteColumnName(it, encodedStr);
	}
	else
	{
	
	
	}

}
void CExampledata::TableGeneratedData(CTableObject* table, std::string FileName, uint8_t* pDrTable,
        CAlignDataObject* Body)
{
	WriteFile bfile( FileName );
	bfile.openfile("a");
	uint16_t encode_buflen = 0;
	string encodedStr = "";
    std::string csvheadFlag;
    table->findAttriValue( litealigndata::ATTR_csvheadFlag, csvheadFlag );
    std::string get_table_id;
    table->findAttriValue( litealigndata::ATTR_tableid, get_table_id );
    if(csvheadFlag == "true")
    {
        std::list<CAlignDataObject*> obj = (Body)->getLitealigndataObjectList();
        for ( listInterface::iterator  it = obj.begin(); it != obj.end(); ++(it) )
        {
            WriteColumnName(it, encodedStr);
        }
        encodedStr.replace(encodedStr.size() - 1, 1, "\n");
        bfile.writeTable( NULL, encodedStr.c_str(), (uint64_t)encodedStr.length() );
    }
    encodedStr = "";
	for ( uint32_t i = 0; i < table->getRowNum(); i++ )
	{
		encode_buflen = example_encode_data(Body, pDrTable, atoi(get_table_id.c_str()), encodedStr);
		pDrTable += table->getentry_size();
		bfile.writeTable( NULL, encodedStr.c_str(), (uint64_t)encode_buflen );
        ACE_DEBUG((LM_DEBUG, "line value: [%s].\n", encodedStr));
		encodedStr = "";
		ACE_DEBUG((LM_DEBUG, "%d bytes are generated successfully.\n", encode_buflen));
	}
	bfile.closefile();
}
void CExampledata::TableGeneratedStreamData(CTableObject*table, uint8_t* pDrTable, 
	        CAlignDataObject* Body, bufferevent *buffev)
{
    const char* ptr;
    uint64_t total_len = 0;

    uint16_t encode_buflen = 0;
    string   encodedStr = "";
    std::string csvheadFlag;
    table->findAttriValue( litealigndata::ATTR_csvheadFlag, csvheadFlag );
    std::string get_table_id;
    table->findAttriValue( litealigndata::ATTR_tableid, get_table_id );
    std::string total_str = "";
    if(csvheadFlag == "true")
    {
        std::list<CAlignDataObject*> obj = (Body)->getLitealigndataObjectList();
        for ( listInterface::iterator  it = obj.begin(); it != obj.end(); ++(it) )
        {
            encodedStr = encodedStr + (*it)->getObjectName() + ",";
        }
        encodedStr.replace(encodedStr.size() - 1, 1, "\n");
        //cout << encodedStr << endl;
        total_str += encodedStr;
        encode_buflen = encodedStr.length();
        total_len += encode_buflen;
        encodedStr ="";
    }
    
    //如果优化，可以改成mmap代替string
    for ( uint32_t i = 0; i < table->getRowNum(); i++ )
    {
        encode_buflen = example_encode_data(Body, pDrTable, atoi(get_table_id.c_str()), encodedStr);
        pDrTable += table->getentry_size();
        total_str += encodedStr;
        total_len += encode_buflen;
        encodedStr = "";
        ACE_DEBUG((LM_DEBUG, "%d bytes are generated successfully.\n", encode_buflen));
    }
    ptr = total_str.c_str();
    ACE_DEBUG((LM_DEBUG, "line value: [%s].\n", total_str));
    table->SendTableData(ptr, total_len, buffev);
}
void CExampledata::GetFilename(CTableObject* table, std::string& FileName)
{
    char file[256] = {0};
    std::string strOutputPath = CAlignConfig::GetInstance()->GetOutputPath();
    CInterfaceObj* InterfaceObj = ( CInterfaceObj* ) (table->getParent());
    uint64_t InitialTime = table->getInitialTime() ;
	string tablename;
	table->findAttriValue( litealigndata::ATTR_TableName, tablename );
	snprintf( file, 256, "%s%s_%s_%lu.%s", strOutputPath.c_str(), 
	InterfaceObj->getObjectName().c_str(), tablename.c_str(),
		InitialTime, "csv" );
    FileName = file;
}
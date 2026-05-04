/*
 * =============================================================================
 *
 *       Filename:  Ctrl.cc
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
#include "file/Ctrldata/Ctrldata.h"
#include "struct/TableObject.h"

void CCtrldata::ReadCtrlFile(const char* filename, 
                CTableObject* table)
{
	FILE* fp = fopen( filename, "r" );
    CAlignDataObject* Head = table->findChildObject("Head");
    CAlignDataObject* HeadData =  Head->findChildObject("HeadData");
	if ( fp == NULL )
	{
		ACE_DEBUG((LM_ERROR, "current json does not exist: %s.\n", filename));
		return;
	}

	fseek( fp, 0, SEEK_END );
	long filelen = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	m_seedData=(uint8_t*)malloc( filelen + 1 );
	if ( m_seedData == NULL )
	{
		ACE_DEBUG((LM_ERROR, "malloc jsonBuf memory failed.\n"));
		return;
	}
	fread( m_seedData, 1, filelen, fp );
	fclose(fp);	

    uint16_t entry_size = 0;
    uint32_t num_entries = 0;
    std::list<CAlignDataObject*> obj = (HeadData)->getLitealigndataObjectList();
    for ( listInterface::iterator  it = obj.begin(); it != obj.end(); ++(it) )
    {
        uint16_t len = 0;
		std::string type;
		if ( (*it)->findAttriValue( litealigndata::ATTR_Type, type ) )
		{
			len = litealigndata::findNametypeLen((char*)type.c_str());
		}

        if(!strcmp((*it)->getObjectName().c_str(), "num_entries"))
        {
            memcpy((char*)&num_entries, m_seedData + (*it)->getOffset(), len);
        }  
    }
    table->setnum_entries(num_entries);
	//CAlignDataObject* Head = table->findChildObject("Head");
    Setbody(m_seedData + Head->getOffset());
    Sethead(m_seedData);
}
  
bool CCtrldata::ReadData( 
        std::string& fileName,  CXmlParser* xml,
        CTableObject* table)
{
    ReadCtrlFile(fileName.c_str(), table);

    return true;
}
uint64_t CCtrldata::GetRandomFileLinesCount(std::string fileName, CXmlParser* xml, 
            CTableObject* table_obj)
{
    FILE* fp = fopen( fileName.c_str(), "r" );
    if ( fp == NULL )
    {
        ACE_DEBUG((LM_ERROR, "current json does not exist: %s.\n", fileName.c_str()));
        return 0;
    }
	CAlignDataObject* Head = table_obj->findChildObject("Head");
	CAlignDataObject* Body = table_obj->findChildObject("Body");
    CAlignDataObject* HeadData =  Head->findChildObject("HeadData");
    std::list<CAlignDataObject*> obj = (HeadData)->getLitealigndataObjectList();
    uint8_t* seedData=(uint8_t*)malloc( Head->getOffset());

    memset(seedData, '\0', Head->getOffset());
    if ( seedData == NULL )
    {
        ACE_DEBUG((LM_ERROR, "malloc jsonBuf memory failed.\n"));
        return 0;
    }
    fread( seedData, 1, Head->getOffset(), fp );
    fclose(fp);

    uint32_t num_entries = 0;
    for ( listInterface::iterator  it = obj.begin(); it != obj.end(); ++(it) )
    {
        if(!strcmp((*it)->getObjectName().c_str(), "num_entries"))
        {
            uint16_t len = 0;
            std::string type;
            if ( (*it)->findAttriValue( litealigndata::ATTR_Type, type ) )
            {
                len = litealigndata::findNametypeLen((char*)type.c_str());
            }
            memcpy((char*)&num_entries, seedData + (*it)->getOffset(), len);
            break;
        }
    }
    free(seedData);
    return num_entries;
        
}
void CCtrldata::GetRandomLines(CTableObject* table, std::string filename, uint64_t expectLineCount,
            std::vector<std::string>& Lines, CXmlParser* xml)
{
    uint64_t n;
    uint64_t m = 0;
    CAlignDataObject* Head = table->findChildObject("Head");
    CAlignDataObject* HeadData =  Head->findChildObject("HeadData");

    FILE* fp = fopen( filename.c_str(), "r" );
    if ( fp == NULL )
    {
        ACE_DEBUG((LM_ERROR, "current json does not exist: %s.\n", filename));
        return;
    }
    fseek( fp, 0, SEEK_END );
    long filelen = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    uint8_t* seedData=(uint8_t*)malloc( filelen + 1 );
    if ( seedData == NULL )
    {
        ACE_DEBUG((LM_ERROR, "malloc jsonBuf memory failed.\n"));
        return;
    }
    fread( seedData, 1, filelen, fp );
    fclose(fp);

	//CAlignDataObject* Head = table->findChildObject("Head");
	CAlignDataObject* Body = table->findChildObject("Body");

    uint8_t* m_body = seedData + Head->getOffset();
    uint16_t entry_size = 0;
    std::list<CAlignDataObject*> obj = (HeadData)->getLitealigndataObjectList();
    for ( listInterface::iterator  it = obj.begin(); it != obj.end(); ++(it) )
    {
        //std::cout << (*it)->getObjectName() << std::endl;
		if(!strcmp((*it)->getObjectName().c_str(), "entry_size"))
		{
            uint16_t len = 0;
            std::string type;
            if ( (*it)->findAttriValue( litealigndata::ATTR_Type, type ) )
            {
                len = litealigndata::findNametypeLen((char*)type.c_str());
            }
            memcpy((char*)&entry_size, seedData + (*it)->getOffset(), len);
            break;
		}

    }                     
    while((m++) <= RandomMaxSize)
    {
        n = random()%expectLineCount;
        char buf[10240] = {'\0'};                       
        memcpy(buf, m_body + n*entry_size, entry_size);
        std::string binaryString(buf, buf + sizeof(buf));
        Lines.push_back(binaryString);
    }
    free(seedData);

}
void CCtrldata::GetFileLineItemValues(CColumnObject* release_column_obj, std::vector<std::string>& Lines, 
                std::map<std::string, litealigndata::autoType>& item, 
                unsigned int currentIndex, unsigned int totalsize)
{
    const char* pToken;
    std::string lineStr;
    uint16_t len = 0;
    std::string type;
    if ( release_column_obj->findAttriValue( litealigndata::ATTR_Type, type ) )
    {
        len = litealigndata::findNametypeLen((char*)type.c_str());
    }
    litealigndata::autoType auto_type = litealigndata::findNameautotype((char*)type.c_str());

    while(!Lines.empty())
    {
        lineStr = Lines.back();
        Lines.pop_back();
        char buf[1024] = {'\0'};
        const char* data = lineStr.c_str();
        
        memcpy(buf, data + release_column_obj->getOffset(), len);
        if( auto_type == litealigndata::AUTO_UINT64)
        {
            item.insert(std::make_pair(std::to_string(*(uint64_t*)buf), litealigndata::AUTO_UINT64));
        }
        else if(auto_type == litealigndata::AUTO_RealTime)
        {
            //item.insert(make_pair(to_string(atoll(pToken)), AUTO_RealTime));
            item.insert(std::make_pair(buf, litealigndata::AUTO_RealTime));
        }
        else if(auto_type == litealigndata::AUTO_STRING)
        {
            item.insert(std::make_pair(buf, litealigndata::AUTO_STRING));
        }
        else if(auto_type == litealigndata::AUTO_ByteArray)
        {
            item.insert(std::make_pair(buf, litealigndata::AUTO_ByteArray));
        }
        else if((auto_type == litealigndata::AUTO_UINT32))
        {
            item.insert(std::make_pair(std::to_string(*(uint32_t*)buf), litealigndata::AUTO_UINT64));
        }

    }
}
bool  CCtrldata::GetAllLines(std::vector<std::string>& Lines, std::string file)
{

    return true;
}
void CCtrldata::TableGeneratedData(CTableObject* table, std::string FileName, uint8_t* pDrTable,
        CAlignDataObject* Body)
{
	WriteFile bfile( FileName );
	bfile.openfile("a");

	CAlignDataObject* Head = table->findChildObject("Head");
    CAlignDataObject* HeadData =  Head->findChildObject("HeadData");

    uint32_t entry_size = table->getentry_size();
    uint32_t num_entries = table->getRowNum();
	std::list<CAlignDataObject*> obj = (HeadData)->getLitealigndataObjectList();
    for ( listInterface::iterator  it = obj.begin(); it != obj.end(); ++(it) )
    {    
        //std::cout << (*it)->getObjectName() << std::endl;
		if(!strcmp((*it)->getObjectName().c_str(), "num_entries"))
		{
            uint16_t len = 0;
            std::string type;
            if ( (*it)->findAttriValue( litealigndata::ATTR_Type, type ) )
            {
                len = litealigndata::findNametypeLen((char*)type.c_str());
            }
			memcpy(Gethead() + (*it)->getOffset(),
						(char*)(&(num_entries)), len);
		}
		if(!strcmp((*it)->getObjectName().c_str(), "entry_size"))
		{
            uint16_t len = 0;
            std::string type;
            if ( (*it)->findAttriValue( litealigndata::ATTR_Type, type ) )
            {
                len = litealigndata::findNametypeLen((char*)type.c_str());
            }
            
			memcpy(Gethead() + (*it)->getOffset(),
						(char*)(&(entry_size)), len);
		}

    }

	//CAlignDataObject* Head = table->findChildObject("Head");
	//CAlignDataObject* Body = table->findChildObject("Body");
	bfile.writeTable( NULL, Gethead(),  Head->getOffset());
	bfile.writeTable( NULL, pDrTable, Body->getOffset()*num_entries);
    ACE_DEBUG((LM_DEBUG, "Body->getOffset(): [%u], num_entries: [%u].\n",
    Body->getOffset(), num_entries));
	bfile.closefile();

}
void CCtrldata::TableGeneratedStreamData(CTableObject*table, uint8_t* pDrTable, 
	        CAlignDataObject* Body, bufferevent *buffev)
{
	CAlignDataObject* Head = table->findChildObject("Head");
    CAlignDataObject* HeadData =  Head->findChildObject("HeadData");
    uint32_t entry_size = table->getentry_size();
    uint32_t num_entries = table->getnum_entries();
    
	std::list<CAlignDataObject*> obj = (HeadData)->getLitealigndataObjectList();
    for ( listInterface::iterator  it = obj.begin(); it != obj.end(); ++(it) )
    {
		if(!strcmp((*it)->getObjectName().c_str(), "num_entries"))
		{
            uint16_t len = 0;
            std::string type;
            if ( (*it)->findAttriValue( litealigndata::ATTR_Type, type ) )
            {
                len = litealigndata::findNametypeLen((char*)type.c_str());
            }
			memcpy(Gethead() + (*it)->getOffset(),
						(char*)(&(num_entries)), len);
		}
		if(!strcmp((*it)->getObjectName().c_str(), "entry_size"))
		{
            uint16_t len = 0;
            std::string type;
            if ( (*it)->findAttriValue( litealigndata::ATTR_Type, type ) )
            {
                len = litealigndata::findNametypeLen((char*)type.c_str());
            }
            
			memcpy(Gethead() + (*it)->getOffset(),
						(char*)(&(entry_size)), len);
		}

    }

    table->SendTableData((const char*)Gethead(), 
                Head->getOffset()+(Body->getOffset())*num_entries, buffev);
    ACE_DEBUG((LM_DEBUG, "send bytes rate is 5Mb/s.\n"));
}
void CCtrldata::GetFilename(CTableObject* table, std::string& FileName)
{
    char file[256] = {0};
    std::string strOutputPath = CAlignConfig::GetInstance()->GetOutputPath();
    CInterfaceObj* InterfaceObj = ( CInterfaceObj* ) (table->getParent());
    uint64_t InitialTime = table->getInitialTime() ;
	std::string tablename;
	table->findAttriValue( litealigndata::ATTR_TableName, tablename );

	snprintf( file, 256, "%s%s_%s_%lu.%s", strOutputPath.c_str(), 
	InterfaceObj->getObjectName().c_str(), tablename.c_str(),
	InitialTime, table->getObjectName().c_str() );		
    FileName = file;
}
#ifndef _EXAMPLE_H
#define _EXAMPLE_H

#include <string>
#include <ace/Log_Msg.h>
#include <ace/Log_Record.h>
#include <ace/Log_Priority.h>
#include "file/BaseFileFormat.h"
class CTableObject;
class CXmlParser;

class CExampledata : public CBaseFileFormat
{
	public:
	CExampledata()
	{
	};
	virtual ~CExampledata()
	{
        if(m_seedData)
            free(m_seedData);

	};

	public:
	enum exampleType
	{
	    EXAMPLE_FEED_TABLE_TYPE, 
		EXAMPLE_FEED_TABLE1_TYPE,
	    EXAMPLETYPE_MAX
	};
	

	struct time_stamp_t
	{
	    struct tm tm_time;
	    time_t sec;
	    suseconds_t us;
	};
    virtual void TableGeneratedStreamData(CTableObject*table, uint8_t* pDrTable, 
	        CAlignDataObject* Body, bufferevent *buffev);
    void TableGeneratedData(CTableObject*table, std::string FileName, uint8_t* pDrTable,
			CAlignDataObject* Body);
	void WriteColumnName(listInterface::iterator it, std::string& encodedStr);
    uint64_t GetRandomFileLinesCount(std::string fileName, CXmlParser* xml, CTableObject* table_obj);
    void decode_time_string(std::string& time, struct time_stamp_t *pTimeStamp);
    exampleType example_get_exampleType(uint8_t table_id);	
	typedef std::unordered_map<std::string, int> Umap;
	std::shared_ptr<char> encode_time_string(time_stamp_t &timeStamp);
    virtual void GetFileLineItemValues(CColumnObject* release_column_obj, std::vector<std::string>& Lines, 
            std::map<std::string, litealigndata::autoType>& item, unsigned int currentIndex, unsigned int totalsize);
	//uint32_t get_ProtocolsSize(uint8_t table_id, CXmlParser* xml=NULL);
	bool ReadData( std::string& fileName, CXmlParser* xml,
			CTableObject* table);
	void GetRandomLines(CTableObject* table, std::string filename, uint64_t expectLineCount, std::vector<std::string>& Lines, CXmlParser* xml);
	//void fill_tableHeader(const char* filePath, ALIGNDATA_TABLE* pTableHeader, Umap& tableTypeIdMap, CXmlParser* xml, uint8_t alreadyKnowTableId);
	
	//bool e, uint8_t*& seedData,  ALIGNDATA_TABLE& tableHeader);
	
	bool GetAllLines(std::vector<std::string>& Lines, std::string file);
	uint16_t example_encode_data(CAlignDataObject* BodyData, uint8_t* p_dr_table, uint8_t  table_id, std::string& encodedStr);
	
	uint16_t example_encode_header( unsigned char* buffer_ptr, uint32_t buffer_size, ALIGNDATA_TABLE& tableHeader );
	void example_parse_one_line( std::string& lineStr, uint8_t*& pData, uint8_t table_id, 
			CXmlParser* xml, CTableObject* table );
    void GetFilename(CTableObject* table, std::string& FileName);
	void EntryColumn(uint8_t*& pData, listInterface::iterator it, std::string& lineStr, uint32_t index,
	std::string::size_type& n, std::string::size_type& m);
	void WriteColumn(uint8_t* pData, listInterface::iterator it, std::string& encodedStr, uint64_t value);
	private:
      	uint8_t* m_seedData;	

	};
	#endif

#ifndef _CTRLDATA_H
#define _CTRLDATA_H

#include <string>
#include <ace/Log_Msg.h>
#include <ace/Log_Record.h>
#include <ace/Log_Priority.h>
#include "file/BaseFileFormat.h"
class CTableObject;
class CXmlParser;

class CCtrldata : public CBaseFileFormat
{

public:
    CCtrldata()
    {
        m_seedData = NULL;
        m_TableDefine = NULL;
    };
	virtual ~CCtrldata()
	{
        if(m_seedData)
            free(m_seedData);

	};
    virtual void TableGeneratedStreamData(CTableObject*table, uint8_t* pDrTable, 
	        CAlignDataObject* Body, bufferevent *buffev);
    void TableGeneratedData(CTableObject*table, std::string FileName, uint8_t* pDrTable,
            CAlignDataObject* Body);
    void ReadCtrlFile(const char* filename, CTableObject* table);
    bool ReadData( std::string& fileName, CXmlParser* xml,
                CTableObject* table);
    uint64_t GetRandomFileLinesCount(std::string fileName, CXmlParser* xml, CTableObject* table_obj);
    virtual void GetFileLineItemValues(CColumnObject* release_column_obj, std::vector<std::string>& Lines, 
                std::map<std::string, litealigndata::autoType>& item, unsigned int currentIndex, unsigned int totalsize);
    virtual bool  GetAllLines(std::vector<std::string>& Lines, std::string file);
    void GetRandomLines(CTableObject* table, std::string filename, uint64_t expectLineCount,
                std::vector<std::string>& Lines, CXmlParser* xml);
    void GetFilename(CTableObject* table, std::string& FileName);
private:
    uint8_t* m_seedData;
    tabledef* m_TableDefine;

};
#endif

#ifndef _BASEFILEFORMAT_H
#define _BASEFILEFORMAT_H

#include <memory>
#include <iostream>
#include <unordered_map>
#include "com/Comm.h"
#include "event2/bufferevent.h"
#include "com/AlignDataTypeDef.h"
#include "struct/AlignDataObject.h"

class CTableObject;
class CXmlParser;
class CColumnObject;

#define RandomMaxSize 100
typedef std::list<CAlignDataObject*> listInterface;

class CBaseFileFormat
{
    public:
        typedef std::unordered_map<std::string, int> Umap;
        CBaseFileFormat(){};
        virtual ~CBaseFileFormat(){};
        uint32_t get_ProtocolsSize(uint8_t table_id, CXmlParser* xml);
        uint8_t* Getbody()
        {
            return m_body;
        };
        uint8_t* Gethead()
        {
            return m_head;
        };
        void Setbody(uint8_t* body)
        {
            m_body = body;
        };
        void Sethead(uint8_t* head)
        {
            m_head = head;
        }
        virtual void TableGeneratedStreamData(CTableObject*table, uint8_t* pDrTable, 
	        CAlignDataObject* Body, bufferevent *buffev);
        virtual void TableGeneratedData(CTableObject*table, std::string FileName, uint8_t* pDrTable,
                    CAlignDataObject* Body);
        void fill_tableHeader(const char* fileType, const char* filePath, 
        ALIGNDATA_TABLE* pTableHeader, Umap& tableTypeIdMap, CXmlParser* xml, uint8_t alreadyKnowTableId = 0xff);
        virtual uint64_t GetRandomFileLinesCount(std::string fileName, CXmlParser* xml, CTableObject* table_obj);
        virtual void GetRandomLines(CTableObject* table, std::string filename, uint64_t expectLineCount,
                std::vector<std::string>& Lines, CXmlParser* xml);
        virtual bool ReadData( std::string& fileName, CXmlParser* xml,
                    CTableObject* table);
        virtual void GetFileLineItemValues(CColumnObject* release_column_obj, std::vector<std::string>& Lines, 
                std::map<std::string, litealigndata::autoType>& item, unsigned int currentIndex, unsigned int totalsize);
        virtual bool  GetAllLines(std::vector<std::string>& Lines, std::string file);
        virtual uint16_t example_encode_data(tabledef* AllTableDefine, uint8_t* p_dr_table, ALIGNDATA_TABLE& tableHeader , std::string& encodedStr);
        virtual void GetFilename(CTableObject* table, std::string& FileName);
        void EntryColumn(uint8_t*& pData, listInterface::iterator it, std::string& lineStr, uint32_t index,
        std::string::size_type& n, std::string::size_type& m);
        void WriteColumn(uint8_t* pData, listInterface::iterator it, std::string& encodedStr, uint64_t value);
        void WriteColumnName(listInterface::iterator it, std::string& encodedStr);
    private:
        uint8_t* m_body;
        uint8_t* m_head;
};

#endif
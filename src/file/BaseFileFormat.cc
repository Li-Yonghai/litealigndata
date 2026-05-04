/*
 * =============================================================================
 *
 *       Filename:  BaseFileFormat.cc
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

#include "file/BaseFileFormat.h"
#include "com/AlignDataUtils.h"
#include "sub/parser/XmlParser.h"
#include "struct/TableObject.h"

uint32_t CBaseFileFormat::get_ProtocolsSize(uint8_t table_id, CXmlParser* xml)
{
    //switch ( table_id )
    //{
   //     case EXAMPLE_TABLE_ID:
             return xml->GetAllTableDefine()[table_id].totalLen;
            //return sizeof(alignData_Insight);
    //    default:
    //        return -1;
   // }
}

void CBaseFileFormat::fill_tableHeader(const char* fileType,
const char* filePath, ALIGNDATA_TABLE* pTableHeader, Umap& tableTypeIdMap, CXmlParser* xml, uint8_t alreadyKnowTableId)
{
    memset( pTableHeader, 0, sizeof( *pTableHeader ) );

    //if(0xff == alreadyKnowTableId)
    
    if(0xff == alreadyKnowTableId)
    {
        pTableHeader->table_id = tableTypeIdMap[fileType];
        ACE_DEBUG((LM_ERROR, "pTableHeader->table_id != alreadyKnowTableId is error.\n"));
        return;
    }
    if(0xff != alreadyKnowTableId)
    {
        pTableHeader->table_id = alreadyKnowTableId;
    }

    if((EXAMPLE_TABLE_ID == pTableHeader->table_id || EXAMPLE_TABLE_ID_1 == pTableHeader->table_id))
    {
        pTableHeader->num_entries = countFileLine(filePath);
    }
    ACE_DEBUG((LM_DEBUG, "table_id: %u, num_entries: %u.\n",
        pTableHeader->table_id,
        pTableHeader->num_entries));
    if(alreadyKnowTableId != 0xff)
        pTableHeader->entry_size = get_ProtocolsSize(pTableHeader->table_id, xml);
    //pTableHeader->max_entries = MAX_EXAMPLE_TABLE;
    pTableHeader->ifn = 1; //temp hardcode
   // return;
}
bool CBaseFileFormat::ReadData( std::string& fileName, CXmlParser* xml,
            CTableObject* table)
{
     return true;
}
uint16_t CBaseFileFormat::example_encode_data(tabledef* AllTableDefine, uint8_t* p_dr_table, ALIGNDATA_TABLE& tableHeader , std::string& encodedStr)
{
    return 0;
}
uint64_t CBaseFileFormat::GetRandomFileLinesCount(std::string fileName, CXmlParser* xml, CTableObject* table_obj)
{
    return 0;
}
void CBaseFileFormat::GetRandomLines(CTableObject* table, std::string filename, uint64_t expectLineCount,
                                    std::vector<std::string>& Lines, CXmlParser* xml)
{

}
void CBaseFileFormat::GetFileLineItemValues(CColumnObject* release_column_obj, std::vector<std::string>& Lines, 
            std::map<std::string, litealigndata::autoType>& item, unsigned int currentIndex, unsigned int totalsize)
{

}
bool  CBaseFileFormat::GetAllLines(std::vector<std::string>& Lines, std::string file)
{
    return true;
}
void CBaseFileFormat::TableGeneratedData(CTableObject*table, std::string FileName, uint8_t* pDrTable,
    CAlignDataObject* Body)
{

}
void CBaseFileFormat::TableGeneratedStreamData(CTableObject*table, uint8_t* pDrTable, 
	        CAlignDataObject* Body, bufferevent *buffev)
{


}
void CBaseFileFormat::GetFilename(CTableObject* table, std::string& FileName)
{

}
void CBaseFileFormat::EntryColumn(uint8_t*& pData, listInterface::iterator it, std::string& lineStr, uint32_t index,
std::string::size_type& n, std::string::size_type& m)
{
	
}
void CBaseFileFormat::WriteColumn(uint8_t* pData, listInterface::iterator it, std::string& encodedStr, uint64_t value)
{

}
void CBaseFileFormat::WriteColumnName(listInterface::iterator it, std::string& encodedStr)
{

}
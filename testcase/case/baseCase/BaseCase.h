#ifndef _BASECASE_H_
#define _BASECASE_H_

#include <string>
#include <stdio.h>
#include <spawn.h>
#include <dirent.h>
#include <stdlib.h>
#include <iostream>
#include <strings.h>
#include "ace/Log_Msg.h"
#include "gtest/gtest.h"
#include "log/LogHandler.h"
#include "conf/AlignConfig.h"
#include "sub/parser/XmlParser.h"
#include "struct/TableObject.h"
#include "com/AlignDataTypeDef.h"
#include "struct/AlignDataObject.h"
#include "BaseCaseFunAcculate.h"
#include "BaseCaseFunMerge.h"
#include "BaseCaseFunRandom.h"
#include "BaseCaseFun.h"


namespace testcase{

namespace CaseBaseCase{
    
        class CBaseCaseEnvironment : public testing::Environment
        {
            public:
                virtual void SetUp()
                {
                     std::cout << "CBaseCaseEnvironment SetUp" << std::endl;
                }

                virtual void TearDown()
                {
                     std::cout << "CBaseCaseEnvironment TearDown" << std::endl;
                }
        };
        class CBaseCase: public ::testing::TestWithParam<std::string>
        {

            public:
                CBaseCase();
                ~CBaseCase();

                static void SetUpTestCase();
                static void TearDownTestCase();
                virtual void SetUp() override;
                virtual void TearDown() override;
                void Cleanlitealigndata();
                ::testing::AssertionResult CheckConfFile();
                ::testing::AssertionResult CheckIniFile();
                ::testing::AssertionResult XmlToData();
                ::testing::AssertionResult Initialize();
                ::testing::AssertionResult CheckFileCount();
                ::testing::AssertionResult CheckFileLinesCount();
                ::testing::AssertionResult CheckFileItemValue();
                bool parse_node(std::string filename, uint8_t& column_num, uint64_t expectLineCount,  
                CTableObject* release_table_obj, listInterface::iterator it);
                bool getLineFromFile(CTableObject* table, std::string filename, uint64_t expectLineCount, 
                    std::vector<std::string>& Lines);
                void GetfileFormatInstance(std::string name);
                void GetBaseCaseFun(litealigndata::funcTag m_tag);
                uint64_t GetcmdContext(std::string cmd);
                void GetFilesCount(uint64_t& expectfileCount, uint64_t& actuallfileCount, std::string inter_name, 
                            CTableObject* table_obj, std::string& tablename);
                void GetFilesLineCount(CTableObject* table_obj, uint64_t& expectLineCount,
                    uint64_t& actuallLineCount, std::string inter_name);
                void GetRandomFileAndLinesCount( CTableObject* table_obj , uint64_t& expectLineCount, 
                std::string& filename, std::string inter_name);
                bool GetColumValueFromFile(CColumnObject* release_column_obj, std::vector<std::string>& Lines, 
                                            std::map<std::string, litealigndata::autoType>& item, 
                                            unsigned int currentIndex, unsigned int totalsize);
                bool CheckItemExpectValue(std::map<std::string, litealigndata::autoType>& item, 
                    CColumnObject* release_column_obj);
                inline void SetXmlparser()
                {
                    m_parser = new CXmlParser(xml.c_str());
                };
                inline CXmlParser* GetXmlparser()
                {
                    return m_parser;
                };
                inline void SetXmlparser_release()
                {
                    m_parser_release = new CXmlParser(xml_release.c_str());
                };
                inline CXmlParser* GetXmlparser_release()
                {
                    return m_parser_release;
                };             
            private:
                std::string xml;
                std::string xml_release;
                std::string ini;
                std::string outpath;
                CXmlParser* m_parser;
                CXmlParser* m_parser_release;
                CBaseFileFormat *m_fileFormat;
                CBaseCaseFun *m_CBaseCaseFun;
        };
};

}
#endif

/*
 * =============================================================================
 *
 *       Filename:  WorkThread.cc
 *
 *       Description: 
 *
 *       Version:  1.0
 *       Created:  04/01, 2024
 *       Revision:  V1
 *       Compiler:  g++
 *
 *       Author:  keen.lee 
 *       Company:  
 *
 * =============================================================================
 */
#include "BaseCase.h"


using namespace testcase::CaseBaseCase;

::testing::AssertionResult CBaseCase::CheckConfFile()
 {
    return ::testing::AssertionSuccess();
}

::testing::AssertionResult CBaseCase::CheckIniFile()
{
    return ::testing::AssertionSuccess();;
}

CBaseCase::CBaseCase()
{
    std::cout << "Enter CBaseCase()" << std::endl;
    m_parser = NULL;
    m_parser_release = NULL;
    m_fileFormat = NULL;

    std::cout << "Leave CBaseCase()" << std::endl;
}

CBaseCase::~CBaseCase()
{
    std::cout << "Enter ~CBaseCase()" << std::endl;

    if( m_parser )
    {
        delete m_parser;
        m_parser = NULL;
    }
    if( m_parser_release )
    {
        delete m_parser_release;
        m_parser_release = NULL;
    }
    if(m_CBaseCaseFun)
    {
        delete m_CBaseCaseFun;
    }
    litealigndata_destory(); 
    std::cout << "Leave ~CBaseCase()" << std::endl;
}

void CBaseCase::SetUpTestCase()
{
    std::cout << "Enter SetUpTestCase()" << std::endl;
    char cmd[128] = {'\0'};
    snprintf(cmd, sizeof(cmd), "rm -rf *output");
    CAlignConfig::GetInstance()->RunCmd(cmd); 
    std::cout << "Leave SetUpTestCase()" << std::endl;
}

void CBaseCase::TearDownTestCase()
{
    std::cout << "Enter TearDownTestCase()" << std::endl; 
    std::cout << "Leave TearDownTestCase()" << std::endl;
}

void CBaseCase::SetUp()
{
    std::cout << "Enter SetUp()" << std::endl;

    GetfileFormatInstance(GetParam());

    // Cleanlitealigndata();
    CAlignConfig::GetInstance()->Setlogpath("./" + GetParam() + "_" + "output/log/");
    //CAlignConfig::GetInstance()->SetOutputPath(std::string("./" + GetParam() + "_" + "output/litealigndata/"));
    CLogHandler::GetInstance()->Init(true);
   // xml = GetParam() + ".xml";
    xml_release = GetParam() + "/" + GetParam() + ".xml";
    //ini = GetParam() + "/" + GetParam() + ".ini";
    std::cout << "Leave SetUp()" << std::endl;
}

void CBaseCase::TearDown()
{
    std::cout << "Enter TearDown()" << std::endl;
    char cmd[128] = {'\0'};
   // snprintf(cmd, sizeof(cmd), "rm -rf *output");
   // CAlignConfig::GetInstance()->RunCmd(cmd); 
    std::cout << "Leave TearDown()" << std::endl;
}

::testing::AssertionResult CBaseCase::XmlToData()
{   
    std::cout << "Enter XmlToData()" << std::endl;
    char cmd[128] = {'\0'};
    snprintf(cmd, sizeof(cmd), "%s  %s", "ls", xml_release.c_str());
    //CAlignConfig::GetInstance()->RunCmd(cmd);
    int ret = system(cmd);
    if(ret)
        return (::testing::AssertionFailure()) << std::endl;

    memset(cmd, '\0', 128);
    snprintf(cmd, sizeof(cmd), "%s  %s > /dev/null 2>&1", ".././litealigndata -f", xml_release.c_str());
    if(!CAlignConfig::GetInstance()->RunCmd(cmd))
        return (::testing::AssertionFailure()) << std::endl;
    
    std::cout << "Leave XmlToData()" << std::endl;
    return ::testing::AssertionSuccess();
}
::testing::AssertionResult CBaseCase::Initialize()
{
    std::cout << "Enter Initialize()" << std::endl;
    SetXmlparser();
    SetXmlparser_release();
    if(GetXmlparser() == NULL || GetXmlparser_release() == NULL)
    {
        std::cout << "Leave Initialize()" << std::endl;
        return ::testing::AssertionFailure() << xml << "  and  " << xml_release << " Parser failed";
    }

    if(GetXmlparser()->Initialize() == false ||
           GetXmlparser_release()->Initialize() == false)
    {
       std::cout << "Leave Initialize()" << std::endl;
       return ::testing::AssertionFailure() <<  xml << "  and  " << xml_release << " Init failed";
    }
    
    std::cout << "Leave Initialize()" << std::endl;
    return ::testing::AssertionSuccess();
}

::testing::AssertionResult CBaseCase::CheckFileCount()
{
    std::cout << "Enter CheckFileCount()" << std::endl;
    CXmlParser::listInterface blist = GetXmlparser_release()->GetInterfacelist();
    CXmlParser::listInterface::const_iterator release_interface1 = blist.begin();

    uint64_t expectfileCount = 0;
    uint64_t actuallfileCount = 0;
    for(uint8_t num = 0; num < GetXmlparser_release()->GetInterfacelist().size(); num++)
    {
        CInterfaceObj* release_obj = static_cast<CInterfaceObj*>(*release_interface1);
        auto release_table = release_obj->getLitealigndataObjectList().begin();

        for(uint8_t table_num = 0; table_num < release_obj->getLitealigndataObjectList().size(); table_num++)
        {
            CTableObject* release_table_obj = static_cast<CTableObject*>(*release_table);
            std::string tablename = release_obj->getObjectName();
            GetFilesCount(expectfileCount, actuallfileCount, release_obj->getObjectName(),
                        release_table_obj, tablename);
           // printf("%lu, %lu\n", expectfileCount, actuallfileCount);
            if(expectfileCount != actuallfileCount)
                return (::testing::AssertionFailure()) <<  "expectfileCount : " << expectfileCount <<
                " actuallfileCount : " << actuallfileCount << " are different about  " << 
                "output/litealigndata/" << release_obj->getObjectName() <<  "_" << tablename << "*";

            release_table++;
        }
        release_interface1++;
    }
    std::cout << "Leave CheckFileCount()" << std::endl;
    return (::testing::AssertionSuccess());
}

::testing::AssertionResult CBaseCase::CheckFileLinesCount()
{
    std::cout << "Enter CheckFileLinesCount()" << std::endl;
    CXmlParser::listInterface blist = GetXmlparser_release()->GetInterfacelist();
    CXmlParser::listInterface::const_iterator release_interface1 = blist.begin();

    for(uint8_t num = 0; num < GetXmlparser_release()->GetInterfacelist().size(); num++)
    {
        CInterfaceObj* release_obj = static_cast<CInterfaceObj*>(*release_interface1);
        auto release_table = release_obj->getLitealigndataObjectList().begin();

        for(uint8_t table_num = 0; table_num < release_obj->getLitealigndataObjectList().size(); table_num++)
        {
            uint64_t expectLineCount = 0;
            uint64_t actuallLineCount = 0;
            CTableObject* release_table_obj = static_cast<CTableObject*>(*release_table);
            std::string tablename = release_obj->getObjectName();
            GetFilesLineCount(release_table_obj, expectLineCount, actuallLineCount, tablename);
            release_table++;
            
            //printf("%lu, %lu\n", expectLineCount, expectLineCount);
            if(expectLineCount != actuallLineCount)
            {
                return  (::testing::AssertionFailure() << "expectLineCount:" << expectLineCount << "  actuallLineCount:" << actuallLineCount << std::endl);
            }
        }
        release_interface1++;
    }
    std::cout << "Leave CheckFileLinesCount()" << std::endl;
    return ::testing::AssertionSuccess();
}
bool CBaseCase::parse_node(std::string filename, uint8_t& column_num, 
    uint64_t expectLineCount,  CTableObject* release_table_obj, listInterface::iterator it)
{
	litealigndata::elementTag tag = (*it)->getElementType();
	std::string ObjectName = (*it)->getObjectName();
    
    // 处理Column节点（基础类型）
    if (litealigndata::ELEMENT_Column == tag) 
	{
        std::vector<std::string> Lines;
        CColumnObject* release_column_obj = static_cast<CColumnObject*>(*it);
        //printf("[%s]\n", release_column_obj->getObjectName().c_str());
        std::string attrName;
        release_column_obj->findAttriValue( litealigndata::ATTR_GenFuncName, attrName); 
        //uint16_t colIdx = 0;
        if(!strcasecmp(attrName.c_str(), "None"))
        {
            if(release_column_obj->getLitealigndataObjectList().size() != 0UL)
            {
                return ::testing::AssertionFailure() << "Column num: " << 
                release_column_obj->getLitealigndataObjectList().size() << " is not None in " << 
                release_column_obj->getObjectName() << " in " << 
                release_table_obj->getObjectName() << " in " << (*it)->getObjectName() << 
                "  in  " << xml_release;
            }
        }
        else
        {
            //std::string tableid;
           // release_table_obj->findAttriValue( litealigndata::ATTR_tableid, tableid );
            GetBaseCaseFun(release_column_obj->getFunc()->getTag());
            getLineFromFile(release_table_obj, filename, expectLineCount, Lines);

            std::map<std::string, litealigndata::autoType> item;
            //printf("==============%ld\n", Lines.size());
            //GetColumValueFromFile(release_column_obj, Lines, item, column_num, bodyitemVector.size());
            GetColumValueFromFile(release_column_obj, Lines, item, column_num, release_column_obj->getParent()->getcolumnNo());
             if(!CheckItemExpectValue(item, release_column_obj))
                 return false;
        }
        column_num++;
    }

   // 处理Struct节点（结构体）
	else if(litealigndata::ELEMENT_Struct == tag)
	{
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
		for ( listInterface::iterator  subStruct = obj.begin(); subStruct != obj.end(); ++(subStruct) )
		{
        	parse_node(filename, column_num,expectLineCount, release_table_obj, subStruct);
    	}
        return true;
	}
   // 处理Union节点（联合体）
    else if (litealigndata::ELEMENT_Union == tag) 
	{
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
		for ( listInterface::iterator  subUnion = obj.begin(); subUnion != obj.end(); ++(subUnion) )
		{
            parse_node(filename, column_num,expectLineCount, release_table_obj, subUnion);
    	}
    }
	else if(litealigndata::ELEMENT_Head == tag)
	{
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
		for ( listInterface::iterator  head = obj.begin(); head != obj.end(); ++(head) )
		{
            parse_node(filename, column_num,expectLineCount, release_table_obj, head);
    	}

	}
	else if(litealigndata::ELEMENT_Body == tag)
	{
		std::list<CAlignDataObject*> obj = (*it)->getLitealigndataObjectList();
		for ( listInterface::iterator  body = obj.begin(); body != obj.end(); ++(body) )
		{
            parse_node(filename, column_num,expectLineCount, release_table_obj, body);
    	}
	}
    else
    {
        //printf("Not Support ObjectName name=%s\n", ObjectName.c_str());
		return true;
    }
	return true;
}
::testing::AssertionResult CBaseCase::CheckFileItemValue()
{
    std::cout << "Enter CheckFileItemValue()" << std::endl;
    CXmlParser::listInterface blist = GetXmlparser_release()->GetInterfacelist();
    ;
    //for(uint8_t num = 0; num < GetXmlparser_release()->GetInterfacelist().size(); num++)
    for ( CXmlParser::listInterface::const_iterator release_interface1 = blist.begin(); 
                release_interface1 != blist.end(); ++(release_interface1) )
    {
        CInterfaceObj* release_obj = static_cast<CInterfaceObj*>(*release_interface1);
        //for(uint8_t table_num = 0; table_num < release_obj->getLitealigndataObjectList().size(); table_num++)
        for ( auto release_table = release_obj->getLitealigndataObjectList().begin(); 
                release_table != release_obj->getLitealigndataObjectList().end(); ++(release_table) )
        {
            uint64_t expectLineCount = 0;
            std::string filename;
            CTableObject* release_table_obj = static_cast<CTableObject*>(*release_table);

            //std::vector<CAlignDataObject*>& bodyitemVector = release_table_obj->getm_bodyitemVector();

	        //CAlignDataObject* Body = release_table_obj->findChildObject("Body");
           // CAlignDataObject* BodyData =  Body->findChildObject("BodyData");
	       // std::list<CAlignDataObject*> obj = (BodyData)->getLitealigndataObjectList();            
            //auto release_column = bodyitemVector.begin();
            uint8_t column_num = 0;
            GetRandomFileAndLinesCount(release_table_obj , expectLineCount, filename, release_obj->getObjectName());
            
            // for(auto it : bodyitemVector)
            // {
            //     std::cout << "it->getObjectName()   " << it->getObjectName() << std::endl;
            // }
            //for(auto release_column : bodyitemVector)
            //for ( listInterface::iterator  release_column = obj.begin(); release_column != obj.end(); ++(release_column) )
            //{
            std::list<CAlignDataObject*> obj = (release_table_obj)->getLitealigndataObjectList();
            for ( listInterface::iterator  it = obj.begin(); it != obj.end(); ++(it) )
            {
                if(!parse_node(filename, column_num,expectLineCount, release_table_obj, it))
                    return ::testing::AssertionFailure();
            }
                //column_num++;
                //release_column++;
                //colIdx++;
                                    
           // }         
  
            //release_table++;
        }
        //release_interface1++;
    } 
    std::cout << "Leave CheckFileItemValue()" << std::endl;
    return ::testing::AssertionSuccess();
}

bool CBaseCase::GetColumValueFromFile(CColumnObject* release_column_obj, std::vector<std::string>& Lines, 
                            std::map<std::string, litealigndata::autoType>& item, 
                            unsigned int currentIndex, unsigned int totalsize)
{
    m_fileFormat->GetFileLineItemValues(release_column_obj, Lines, item, currentIndex, totalsize);

    return true;
}
bool CBaseCase::CheckItemExpectValue(std::map<std::string, litealigndata::autoType>& item, 
                CColumnObject* release_column_obj)
{

    if(!(m_CBaseCaseFun->MethodFun(item, release_column_obj, GetParam(), this->m_fileFormat)))
        return false;

    return true;
}
void CBaseCase::GetfileFormatInstance(std::string name)
{
    if(name ==  std::string("Example"))
    {
        m_fileFormat = new CExampledata();
    }
    else if (name == std::string("Ctrldata"))
    {
        m_fileFormat = new CCtrldata();
    }
    else
    {
        m_fileFormat = new CBaseFileFormat();
    }

};
void CBaseCase::GetBaseCaseFun(litealigndata::funcTag m_tag)
{
    switch(m_tag)
    {
        case litealigndata::FUNC_Acculate:
        {
            m_CBaseCaseFun = new CBaseCaseFunAcculate();
            break;
        }
        case litealigndata::FUNC_Random:
        {
            m_CBaseCaseFun = new CBaseCaseFunRandom();
            break;
        }
        case litealigndata::FUNC_Merge:
        {
            m_CBaseCaseFun = new CBaseCaseFunMerge();
            break;
        }
        default:
        {
            m_CBaseCaseFun = new CBaseCaseFun();
            break;
        }
    }
};
void CBaseCase::Cleanlitealigndata()
{
    char cmd[128] = {'\0'};
    snprintf(cmd, sizeof(cmd), "%s  > /dev/null 2>&1", "rm -rf output* *xml ");
    CAlignConfig::GetInstance()->RunCmd(cmd);
};
bool CBaseCase::getLineFromFile(CTableObject* table, std::string filename, uint64_t expectLineCount, 
    std::vector<std::string>& Lines)
{
    m_fileFormat->GetRandomLines(table, filename, expectLineCount, Lines, m_parser_release);
    return true;
};
uint64_t CBaseCase::GetcmdContext(std::string cmd)
{
    FILE* fp;
    char buffer[128];
    fp = popen(cmd.c_str(), "r");
    fgets(buffer, sizeof(buffer), fp);
    pclose(fp);
    return atol( buffer );
};
void CBaseCase::GetFilesCount(uint64_t& expectfileCount, uint64_t& actuallfileCount, std::string inter_name, 
            CTableObject* table_obj, std::string& tablename)
{
    std::string FileCount;
    table_obj->findAttriValue( litealigndata::ATTR_FileNum, FileCount );
    expectfileCount = atol( FileCount.c_str() );
    
    table_obj->findAttriValue( litealigndata::ATTR_TableName, tablename );
    std::string cmd = "ls  output/litealigndata/" + inter_name + "_" + tablename + "*" + "  | wc -l";
    actuallfileCount = GetcmdContext(cmd);
};
void CBaseCase::GetFilesLineCount(CTableObject* table_obj, uint64_t& expectLineCount,
    uint64_t& actuallLineCount, std::string inter_name)
{
    uint8_t  tableId = 0;
    std::string LineCount;
    table_obj->findAttriValue( litealigndata::ATTR_RowNum, LineCount );
    expectLineCount = atol( LineCount.c_str());
    std::string table_id;
    table_obj->findAttriValue( litealigndata::ATTR_tableid, table_id );
    tableId = atoi( table_id.c_str());
    FILE* fp;
    
    std::string tablename;
    table_obj->findAttriValue( litealigndata::ATTR_TableName, tablename);
    std::string cmd = "ls  output/litealigndata/" + inter_name + 
            "_" + tablename + "*" + "  | shuf | tail -n 1";
    char buffer[128] = {'\0'};
    fp = popen(cmd.c_str(), "r");
    fgets(buffer, sizeof(buffer), fp);
    pclose(fp);
    buffer[strlen(buffer)-1] = '\0';
    actuallLineCount = m_fileFormat->GetRandomFileLinesCount(buffer, m_parser_release, table_obj);  
    
};
void CBaseCase::GetRandomFileAndLinesCount( CTableObject* table_obj , uint64_t& expectLineCount, 
            std::string& filename, std::string inter_name)
{
    std::string tablename;
    char buffer[128] = {'\0'};
    table_obj->findAttriValue( litealigndata::ATTR_TableName, tablename );
    std::string cmd = "ls  output/litealigndata/" + inter_name + 
            "_" + tablename + "*" + "  | shuf | tail -n 1";
    FILE* fp;
    fp = popen(cmd.c_str(), "r");
    fgets(buffer, sizeof(buffer), fp);
    pclose(fp);
    buffer[strlen(buffer)-1] = '\0';
    filename = buffer;
    std::string LineCount;
    table_obj->findAttriValue( litealigndata::ATTR_RowNum, LineCount );
    expectLineCount = atol( LineCount.c_str() );
};
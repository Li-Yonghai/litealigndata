#ifndef _XMLPAESER_H_
#define _XMLPAESER_H_
#include <stack>
#include <limits.h>
#include <unordered_map>

#include "com/Comm.h"
#include "event2/bufferevent.h"
#include "struct/TableObject.h"
#include "struct/ColumnObject.h"
#include "struct/AlignDataObject.h"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/dom/DOMElement.hpp"
#include "xercesc/dom/DOMDocument.hpp"
#include "xercesc/dom/DOMErrorHandler.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOM.hpp"
#include "com/AlignDataTypeDef.h"

XERCES_CPP_NAMESPACE_USE


typedef char SeedFilePathArray[SEED_FILE_PATH_LEN];

#define MAX_SEEDFILE_NUM 255
#define MAX_TYPE_LEN (64)
#define MAX_NAME_LEN 64

struct SeedFile
{
    SeedFilePathArray filePath[MAX_SEEDFILE_NUM];
    SeedFilePathArray jsonFile[MAX_SEEDFILE_NUM];
    SeedFilePathArray tablename[MAX_SEEDFILE_NUM];
    char fileType[MAX_SEEDFILE_NUM][MAX_TYPE_LEN];
    unsigned int entryNum[MAX_SEEDFILE_NUM];
};

struct SubConfigInfo
{
    char genFuncName[MAX_NAME_LEN];
    uint32_t imsiNum;
    uint8_t percent;
    uint32_t subCtrlStatsInforType;
};

struct SeedFileInfo
{
    SeedFile seedfile;
    uint32_t filenumber;
    uint32_t initItf;
    uint32_t itfNum;
    SubConfigInfo subConfig;
};

class CXmlParser : public DOMErrorHandler
{
public:
    static XercesDOMParser::ValSchemes gValScheme;
    static bool gDoNamespaces;
    static bool gDoSchema;
    static bool gSchemaFullChecking;
    static bool gDoCreate;

    struct doublehashIdx
    {
        uint32_t fileIdx;
        uint32_t colarrayIdx;

        bool operator == ( const doublehashIdx& rht ) const
        {
            return ( fileIdx == rht.fileIdx && colarrayIdx == rht.colarrayIdx );
        }

        bool operator < ( const doublehashIdx& rht ) const
        {
            if ( fileIdx < rht.fileIdx )
                return true;
            else if ( fileIdx == rht.fileIdx )
            {
                if ( colarrayIdx < rht.colarrayIdx )
                    return true;
                else
                    return false;
            }
            else
                return false;

        }
    };

    typedef std::list<CAlignDataObject*> listInterface;

    typedef std::map<std::string, CAlignDataObject*> MapDimensionFile;

    typedef std::map<std::string, CAlignDataObject*> MapFuncion;

    typedef std::map<uint32_t, LoadDimensionInfo> mappingKeyInfo;

    typedef std::map<std::string, LoadDimensionInfo> DimensionLoadInfo;

    typedef std::vector<baseType* > Dimensioncolvalues;

    typedef std::vector<Dimensioncolvalues* > DimensionfileDatas;

    typedef std::map<std::string, DimensionfileDatas* > mapDimensionfileDatas;

    typedef std::map<doublehashIdx, DimensionfileDatas*> dimensionfilemapping;
    CBaseFileFormat m_BaseFileFormat;

    class seedInfo
    {
    public:
        seedInfo( ALIGNDATA_TABLE* pHeader, const char* name, const char* type,
        const char* json, const char* tablename) :
            m_pHeader( pHeader ), m_pszTableName( name ), m_tableType( type ), m_jsonFile(json), m_TableName(tablename)
        {
            memset( m_pHeader, 0, sizeof( ALIGNDATA_TABLE ) );
        }
        ~seedInfo()
        {
            if ( m_pHeader != NULL )
                delete m_pHeader;

            m_pHeader = NULL;
        }


    public:
        ALIGNDATA_TABLE* m_pHeader;
        const char* m_pszTableName;
        const char*  m_tableType;
        const char* m_jsonFile;
        const char* m_TableName;
    };

    typedef std::list<seedInfo*> astTablelist;

    typedef std::map<uint32_t, astTablelist> mapAligndatatablelist;

    typedef std::list<Lz4TableInfo*> Lz4TableList;
    
    typedef std::map<uint64_t, Lz4TableList> TimeLz4List;
    
    CXmlParser( const char* pXmlFile)
    {
        strcpy( m_xmlFile, pXmlFile );
    };
    CXmlParser( const char* pXmlFile, const char* outPutfile );
    ~CXmlParser()
    {
        //if(m_BaseFileFormat)
       // {
       //     free(m_BaseFileFormat);
      //  }
        delete m_pLitealigndataObjRoot;

        for ( auto it = m_Interfacelist.begin(); it != m_Interfacelist.end(); ++it )
        {
            CInterfaceObj* obj = static_cast<CInterfaceObj*>( *it );
            delete obj;
        }
    }

    bool Initialize();

    void ProduceExtractedSeedData();

    void ProduceData(bufferevent *buffev);

    bool getSawErrors() const;

    void resetErrors();

    virtual bool handleError( const DOMError& );

    static bool isDoCreate()
    {
        return gDoCreate;
    }

    static void setDoCreate( bool doCreate )
    {
        gDoCreate = doCreate;
    }

    static bool isDoNamespaces()
    {
        return gDoNamespaces;
    }

    static void setDoNamespaces( bool doNamespaces )
    {
        gDoNamespaces = doNamespaces;
    }

    static bool isDoSchema()
    {
        return gDoSchema;
    }

    static void setDoSchema( bool doSchema )
    {
        gDoSchema = doSchema;
    }

    static bool isSchemaFullChecking()
    {
        return gSchemaFullChecking;
    }

    static void setSchemaFullChecking( bool schemaFullChecking )
    {
        gSchemaFullChecking = schemaFullChecking;
    }

    static XercesDOMParser::ValSchemes getValScheme()
    {
        return gValScheme;
    }

    static void setValScheme( XercesDOMParser::ValSchemes valScheme )
    {
        gValScheme = valScheme;
    }

    const CAlignDataObject* getLitealigndataObjRoot() const
    {
        return m_pLitealigndataObjRoot;
    }

    void setLitealigndataObjRoot( CAlignDataObject* pLitealigndataObjRoot )
    {
        m_pLitealigndataObjRoot = pLitealigndataObjRoot;
    }

    const char* getXmlFile() const
    {
        return m_xmlFile;
    }

    void addInterfaceInList( CAlignDataObject* pLitealigndataObject );

    bool addlitealigndataObjectInMapper( std::string str, CAlignDataObject* pLitealigndataObject );

    CAlignDataObject* findlitealigndataObjectFileInMapper( const std::string& objname, litealigndata::elementTag tag );

    bool anyzeDepDimensionInfo(
        const std::string& parameterName,
        const std::string& ParameterValue,
        CAlignDataObject*& pDimentsionFile,
        CAlignDataObject*& pDimensionColumn );

    bool setTableDefineInfo( uint16_t table_id, TableDefine* p_tableDef, uint16_t entryNum);

    static CAlignDataObject* GetTheSpecialObject( CAlignDataObject* pChild, litealigndata::elementTag tag );

    uint8_t addDimensionInfo( CAlignDataObject* dimensionFile, CAlignDataObject* dimensionColumn );

    void addDimensionKeyInfo( LoadDimensionInfo& info );

    const LoadDimensionInfo* findDimensionKeyInfo( uint32_t hashKey );

    const DimensionfileDatas* findDimensionFileDatas( const std::string& filename ) const;


    const DimensionfileDatas* findDimensionFileDatasforIdx( const std::string& filename, uint32_t colHashKey ) const;

    const char* getOutPutfilePath() const
    {
        return m_outPutfilePath;
    }
    uint16_t GetColumnOffset( std::string& tableid, uint16_t colIdx, CAlignDataObject* pLitealigndataObject = NULL );

    litealigndata::autoType GetColumnType( std::string& tableid, uint16_t colIdx );

    uint8_t GetColumnLen( std::string& tableid, uint16_t colIdx );
    uint32_t GetColumnIdx( std::string& tableid, const std::string& colName );


    bool CreateTemplateXml( SeedFileInfo& seedFileInfo, bool isSeedFromDir );

    bool SerializeDom( DOMNode* node, char const* pChar = "" );

    bool InitialCorrelation(bool isExtractCorrelation = false, bool isSipPrackDrop = false);

    static int file_size(const char* filename)                
    {                     
        struct stat statbuf;                     
        int ret;                     
        ret = stat(filename,&statbuf);
        if(ret != 0) return -1;
        return statbuf.st_size;
    };

    listInterface const GetInterfacelist() const{
        return m_Interfacelist;
    };
    tabledef* GetAllTableDefine()
    {

        return NULL; //m_AllTableDefine;
    }
    Offset_Stack& getOffset_Stack()
    {
        return m_stack;
    }
    bool parse_node(listInterface::iterator it, uint32_t& columnNo);
    void SetAllItemOffset();
private:

    CXmlParser( const CXmlParser& );
    void operator= ( const CXmlParser& );

    bool travelXmlElement( DOMNode* n, CAlignDataObject* Parent );

    void clearMaptable( mapAligndatatablelist& maptableheadlist );

    bool CreateTableMapping( mapAligndatatablelist& maptableheadlist, const SeedFileInfo& seedFileInfo,bool isSeedFromDir  );

    void CreateColumnElementNode( seedInfo* info, SubConfigInfo& subConfig, uint32_t itfNum, DOMDocument* doc, DOMElement* parent );

    void CreateParameterElementNode( SubConfigInfo& subConfig, uint32_t itfNum, DOMDocument* doc, DOMElement* parent );

    DOMElement* CreateParameterNode( DOMDocument* doc, const char* attrValue, const char* testValue );


    bool ProcessObject( CAlignDataObject* root );

    bool LoadDimensionFiles();

    bool LoadDimensionFile( const LoadDimensionInfo& info );
    bool LoadDimensionFileArbor( const LoadDimensionInfo& info );

    std::string parseCsvString(std::string& line);

    uint32_t createHashKey( const Dimensioncolvalues* rowdata, const ColumnArray& keyCollumn );

    bool CreateIdxForFiles();

    CAlignDataObject* GetColObject( const ColumnArray& colarray, uint8_t idx, uint8_t& pos );
    CAlignDataObject* getCInterfaceObject(std::string& strInfname);
    bool processItem();
	int find_xml_node_attrbt(DOMNode* pNode, litealigndata::AttrTag const attrbt, std::string& attrbt_name, std::string& attrbt_value);
	int getLoadedCsvColumn(std::string& talbe_column_input, DimensionfileDatas const *& pCsvFileData, uint32_t& csv_column_loaded_Idx ,uint32_t& row_quantity);
	int getCsvDataColumn  (DimensionfileDatas const* pDimensionfileDataTable, unsigned DimensionfileDataColumnIndex, unsigned rowIdex, baseType*& pPercentValue);
	int getColumnCharArray(DimensionfileDatas const* pDimensionfileDataTabl, unsigned csv_loaded_colmn_Idx, unsigned rowIdex, std::string& pacVal );
    uint32_t m_offset = 0;
    uint32_t m_strcut_offset = 0;
    uint32_t m_union_offset = 0;
    std::vector<CAlignDataObject*> *m_itemVector = NULL;
    mapDimensionfileDatas m_tableData;

    dimensionfilemapping m_idxtableData;

    bool m_fSawErrors;
    char m_xmlFile[128];

    char m_outPutfilePath[128];
    CAlignDataObject* m_pLitealigndataObjRoot;

    listInterface m_Interfacelist;
    MapDimensionFile m_DimensionFileMap;

    MapFuncion m_FunctionMap;

    mappingKeyInfo m_KeyInfoMap;

    DimensionLoadInfo m_DimensionInfoMap;

    uint32_t m_corrlation_flag;
    TimeLz4List m_timeLz4TableList;
    int XML_expand_Flag;
    public:
    CAlignDataTypeDef m_AlignDataTypeDef;
    Offset_Stack m_stack;
};

inline bool CXmlParser::getSawErrors() const
{
    return m_fSawErrors;
}

#endif /* CXmlParser_H_ */

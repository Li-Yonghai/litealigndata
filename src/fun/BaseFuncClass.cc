/*
 * =============================================================================
 *
 *       Filename:  CBaseFuncClass.cc
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
#include "struct/TableObject.h"
#include "fun/FunctionObject.h"
#include "struct/ColumnObject.h"

using namespace std;

CBaseFunc::CBaseFunc( litealigndata::funcTag tag ) :
    m_tag( tag ), m_calTimes( 0 ),imsi_type( litealigndata::Invalid_type ), m_right_align(0)
{

}

CBaseFunc::CBaseFunc( const CBaseFunc& rhs ) :
    m_tag( rhs.m_tag ), m_value( rhs.m_value ), m_calTimes( rhs.m_calTimes ),imsi_type( rhs.imsi_type ), m_right_align( rhs.m_right_align )
{
        for ( auto it = rhs.m_parameterlist.begin(); it != rhs.m_parameterlist.end(); ++it )
        {
            CAlignDataObject* pObj = CAlignDataObjectFactory::Instance().createalignDataObject( **it );
            m_parameterlist.push_back(pObj);
        }

}

CBaseFunc& CBaseFunc::operator=( const CBaseFunc& rhs )
{
    if ( &rhs == this )
        return *this;

    m_tag= rhs.m_tag;
    m_value= rhs.m_value;
    m_calTimes= rhs.m_calTimes;
    imsi_type = rhs.imsi_type ;

    for ( auto it = rhs.m_parameterlist.begin(); it != rhs.m_parameterlist.end(); ++it )
    {
        CAlignDataObject* pObj = CAlignDataObjectFactory::Instance().createalignDataObject( **it );
        m_parameterlist.push_back(pObj);
    }

    return *this;
}


bool CBaseFunc::Initialize()
{
    if ( m_pColumn == NULL )
        return false;

    litealigndata::autoType type = litealigndata::AUTO_Invalid;

    if ( m_pColumn->getElementType() == litealigndata::ELEMENT_SubColumn ||  m_pColumn->getElementType() == litealigndata::ELEMENT_Function )
    {
        m_value.setIsSubColumn( true );

        std::string attrValue;

        if ( ! m_pColumn->findAttriValue( litealigndata::ATTR_Type, attrValue ) )
        {
            ACE_DEBUG((LM_ERROR, "BaseFunc %s Can not find the type attribute of the column %s\n", 
                litealigndata::funcTagString[m_tag], m_pColumn->getObjectName().c_str()));
            return false;
        }

        type = ( litealigndata::autoType )m_AlignDataTypeDef.FindRelativeTag( litealigndata::autoTypeString, litealigndata::AUTO_MAX, attrValue.c_str() );

        if ( type == litealigndata::AUTO_Invalid )
        {
            ACE_DEBUG((LM_ERROR, "BaseFunc %s can not find the relative data type for the value %s\n", 
                litealigndata::funcTagString[m_tag], attrValue.c_str()));
            return false;
        }

    }
    else if ( m_pColumn->getElementType() == litealigndata::ELEMENT_Column )
    {
        std::string attrValue;
        CXmlParser* pParser = m_pColumn->getParser();
        CTableObject* pObject = (( ( CColumnObject* )m_pColumn ))->getTable();
        std::string tableid;
        pObject->findAttriValue( litealigndata::ATTR_tableid, tableid );

        if ( m_pColumn->findAttriValue( litealigndata::ATTR_Type, attrValue ) )
        {
           //type = pParser->GetColumnType( tableid, ( ( CColumnObject* )m_pColumn )->getColidx() );
           type = litealigndata::findNameautotype((char*)(attrValue.c_str()));
        }
        // else
        // {
        //     type = ( litealigndata::autoType )(GetAlignDataTypeDef().FindRelativeTag( litealigndata::autoTypeString, litealigndata::AUTO_MAX, attrValue.c_str()) );
        // }

        // if ( type == litealigndata::AUTO_Invalid )
        // {
        // }

        // if ( type == litealigndata::AUTO_ByteArray )
        // {
        //     std::string tableid;
        //     pObject->findAttriValue( litealigndata::ATTR_tableid, tableid );
        //     m_value.setLen(litealigndata::findNametypeLen((char*)(attrValue.c_str())));
        //     //m_value.setLen( pParser->GetColumnLen( tableid, ( ( CColumnObject* )m_pColumn )->getColidx() ) );
        // }
    }

    m_value.setType( type );
    return true;
}


bool CBaseFunc::process()
{

    alignDataList::iterator it = this->m_parameterlist.begin();

    while ( it != this->m_parameterlist.end() )
    {
        std::string parameterName = ( *it )->getObjectName();

        litealigndata::parameterTag tag = 
        ( litealigndata::parameterTag ) GetAlignDataTypeDef().FindRelativeTag( litealigndata::ParaTypeString, litealigndata::PARA_MAX, parameterName.c_str() );

        std::string ParameterValue = ( *it )->getStrValue();

        if ( ParameterValue.empty() )
        {
            if(litealigndata::PARA_Value == tag)
            {
            }
            else
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is empty!\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }            
        }

        switch ( tag )
        {

            case litealigndata::PARA_IMSIType:
                if ( !GetAlignDataTypeDef().isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
                {
                    ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal!\n", parameterName.c_str(), ParameterValue.c_str()));
                    return false;
                }

                this->imsi_type = ( litealigndata::subIdType ) atoi( ParameterValue.c_str() );
                break;

            case litealigndata::PARA_byte_array_right_align:
                if ( !!GetAlignDataTypeDef().isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
                {
                    ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal!\n", parameterName.c_str(), ParameterValue.c_str()));
                    return false;
                }

                this->m_right_align = atoi( ParameterValue.c_str() );
                break;

            default:
                break;
        }

        ++it;
    }

    return true;
}


bool CBaseFunc::Calculate()
{
    if ( m_pColumn->getElementType() == litealigndata::ELEMENT_Column )
    {
        CColumnObject* pColumn = ( CColumnObject* ) m_pColumn;

        CTableObject* pTable = ( CTableObject* ) GetTableObject();

        if ( pColumn == NULL )
        {
            ACE_DEBUG((LM_ERROR, "can not get the column Object at CBaseFunc::Caculate\n"));
            return false;
        }

        if ( pTable == NULL )
        {
            ACE_DEBUG((LM_ERROR, "can not get the table Object at CBaseFunc::Calculate\n"));
            return false;
        }

        uint8_t* pAddr = pTable->getData() + pTable->getNextFilledPosition() + pColumn->getOffset();

        ACE_DEBUG((LM_DEBUG, "the table start Addr: %@, the Data start Addr: %@, the filledCount:%u, nextFilledPosition:%Q, the offset:%u.\n", 
                pTable->getData(), pAddr, pTable->getCurFilledCount(), pTable->getNextFilledPosition(), pColumn->getOffset()));
        m_value.setData( pAddr );

    }

    return true;

}


bool CBaseFunc::SetValue( const char* value, litealigndata::subIdType imsitype )
{
    char aligndataValue[17] = { 0 };

    switch ( m_value.getType() )
    {
    case litealigndata::AUTO_IMSIBCD:
        break;

    case litealigndata::AUTO_BCD:
        break;

    case litealigndata::AUTO_IPV6:
        break;

    case litealigndata::AUTO_IPV6_RSA_CU:
    case litealigndata::AUTO_IPV4_IPV6:
        break;  

    case litealigndata::AUTO_ByteArray:
        m_value.AsciiToByteArray( value, strlen( value ), m_right_align);
        break;

    case litealigndata::AUTO_STRING:
        m_value.AsciiToStr( value );
        break;

    case litealigndata::AUTO_SP_IPV6:
        break;

    case litealigndata::AUTO_SP_U8_4:
        break;

    case litealigndata::AUTO_IPV4:
        break;

    case litealigndata::AUTO_IMSI_TBCD:
        break;    

    case litealigndata::AUTO_IMEISV_BCD:
        break;

    case litealigndata::AUTO_BCD_2Bytes:
        {
            break;
        }

    case litealigndata::AUTO_IPV4_NET:
        {
            break;
        }

    case litealigndata::AUTO_IPV6_NET:
        {
            break;
        }

    case litealigndata::AUTO_STRING_NET:
        {
            break;
        }

    case litealigndata::AUTO_DateTime:
    default:
        m_value.AsciiToDec( ( char* )value );
        break;
    }

    return true;
}


//if get the number, invoke this function to set the value.
bool CBaseFunc::SetValue( uint64_t ivalue, litealigndata::subIdType SubIdType, bool forBcdMerge )
{
    litealigndata::autoType type = m_value.getType();

    if ( type == litealigndata::AUTO_IPV6_RSA_CU ||type == litealigndata::AUTO_IPV6 || 
    type == litealigndata::AUTO_SP_IPV6 || type == litealigndata::AUTO_STRING || 
    type == litealigndata::AUTO_SP_U8_4 || type == litealigndata::AUTO_IPV4_IPV6 )
    {
        ACE_DEBUG((LM_WARNING, "the specific value %Q is a number type, can not convert it to this type %s\n", 
                    ivalue, litealigndata::autoTypeString[type]));
        return false;
    }

    if( ivalue == 0 )
    {
        m_value.setNumber(0);
        return true;
    }

    char aligndataValue[17] = { 0 };

    if ( type == litealigndata::AUTO_IMSIBCD )
        snprintf( aligndataValue, 17, "%luF", ivalue );

    if ( type == litealigndata::AUTO_BCD )
        snprintf( aligndataValue, 17, "%lu", ivalue );

    switch ( type )
    {
    case litealigndata::AUTO_IMSIBCD:
    case litealigndata::AUTO_BCD:
        break;

    case litealigndata::AUTO_IMSI_TBCD:
        break;

    case litealigndata::AUTO_IPV6_RSA_CU:
    case litealigndata::AUTO_IPV6:
    case litealigndata::AUTO_IPV4_IPV6:
        break;

    case litealigndata::AUTO_IMEISV_BCD:

        break;  

    case litealigndata::AUTO_BCD_2Bytes:
        {
            break;   
        }
    default:
        m_value.setNumber( ivalue );
        break;
    }

    return true;
}

CAlignDataObject* CBaseFunc::GetTableObject()
{
    if ( m_pColumn->getElementType() == litealigndata::ELEMENT_Column || 
    m_pColumn->getElementType() == litealigndata::ELEMENT_SubColumn )
    {
        if ( ( ( CColumnObject* ) m_pColumn ) == NULL )
            return NULL;

        return ( ( CColumnObject* ) m_pColumn )->getTable();
    }

    return NULL;
}

uint8_t* CBaseFunc::GetRowData( uint32_t row )
{
    CTableObject* pTable = ( CTableObject* ) GetTableObject();

    if ( !pTable )
        return NULL;

    uint8_t* start = pTable->getData();

    if ( start == NULL )
        return NULL;

    return start += pTable->getTableHeader().entry_size * row;
}

uint16_t CBaseFunc::GetCurrentColIdx()
{
    if ( m_value.isIsSubColumn() )
        return 0;

    return ( ( CColumnObject* ) getColumn() )->getColidx();
}

CBaseFunc::~CBaseFunc()
{
}

CBaseFunc* FuncFactory::createBaseFuncObject( litealigndata::funcTag tag )
{
    switch ( tag )
    {
    case litealigndata::FUNC_Acculate:
        return new AcculateFunc( tag );

    case litealigndata::FUNC_Random:
        return new RandomFunc( tag );

    case litealigndata::FUNC_FileReader:
        return new fileReaderFunc( tag );

    case litealigndata::FUNC_Merge:
        return new MergeFunc( tag );

    case litealigndata::FUNC_REDEFINENUMBER:
        return new RedefineRowFunc( tag );

    case litealigndata::FUNC_UniqueRandom:
        return new UniqueRandomFunc( tag );

    default:
        return new CBaseFunc( tag );
    }
}

CBaseFunc* FuncFactory::createBaseFuncObject( CBaseFunc& func )
{
    switch ( func.getTag() )
    {
    case litealigndata::FUNC_Acculate:
        return new AcculateFunc( dynamic_cast<AcculateFunc&>(func) );

    case litealigndata::FUNC_Random:
        return new RandomFunc(  dynamic_cast<RandomFunc&>(func) );

    case litealigndata::FUNC_FileReader:
        return new fileReaderFunc(  dynamic_cast<fileReaderFunc&>(func) );

    case litealigndata::FUNC_Merge:
        return new MergeFunc(  dynamic_cast<MergeFunc&>(func) );

    case litealigndata::FUNC_REDEFINENUMBER:
        return new RedefineRowFunc(  dynamic_cast<RedefineRowFunc&>(func)  );
    case litealigndata::FUNC_UniqueRandom:
        return new UniqueRandomFunc(  dynamic_cast<UniqueRandomFunc&>(func) );

    default:
        return new CBaseFunc( func );
    }
}

void MergeFunc::addSubFunc( CBaseFunc* pFunc )
{
    this->m_Funclist.push_back( pFunc );
}

bool MergeFunc::process()
{
    ACE_DEBUG((LM_DEBUG, "start processing data by MergeFunc for column: %s.\n", m_pColumn->getObjectName().c_str()));

    if ( !CBaseFunc::process() )
        return false;

    return true;
}

bool MergeFunc::Calculate()
{
    ACE_DEBUG((LM_DEBUG, "start Caculate data by MergeFunc for column: %s.\n", m_pColumn->getObjectName().c_str()));

    if ( !CBaseFunc::Calculate() )
        return false;

    uint64_t value = 0;
    litealigndataFuncList::iterator it;
    if( m_value.getType() == litealigndata::AUTO_UINT64 )
    {
        uint8_t leftbit = 0;

        for ( it = m_Funclist.begin(); it != m_Funclist.end(); ++it )
        {
            uint64_t subfuncvalue = ( *it )->getValue().getnumber();
            value |= ( subfuncvalue << leftbit );
            subColumnObject* subColumn = dynamic_cast<subColumnObject*>( ( *it )->getColumn() );

            //printf("value_xxxx is 0x%lx,0x%lx,%u \n",value,subfuncvalue,leftbit);
            if ( subColumn == NULL )
            {
                ACE_DEBUG((LM_ERROR, "the MergeFunc can not get the subColumn object.\n"));
                return false;
            }

            leftbit += subColumn->getBitlen();
        }

        SetValue( value, imsi_type );

        ACE_DEBUG((LM_DEBUG, "MergeFunc Caculate data addr %@.\n current MergeFunc otherType value is @Q.\n", 
                   getValue().getData(), getValue().getnumber()));
    }
    else
    {

    }

    return true;
}

bool MergeFunc::addToString(std::string& toBeAdd, litealigndataFuncList::iterator itFuncToAdd)
{
    int is_obj_delimiter = false;
    CBaseFunc* pFuncSon = *itFuncToAdd;
    {        
        auto Colmn_have_func= pFuncSon->getColumn();
        subColumnObject* pSubColmn = dynamic_cast<subColumnObject*>(Colmn_have_func);
        is_obj_delimiter = pSubColmn->getIsDelimiter();
    }


    if(false == is_obj_delimiter)
    {
        toBeAdd += (pFuncSon->getValue()).getToString();
    }
    else
    {         
        litealigndataFuncList::iterator post_subColumn_func = itFuncToAdd;
        post_subColumn_func++;
        assert(m_Funclist.end() != post_subColumn_func);        

        int isPreString = false;
        {
            isPreString = toBeAdd.size();
        }

        int isPostString = false;
        {
            CBaseFunc* pFuncSonTemp = *post_subColumn_func;
            isPostString = pFuncSonTemp->getValue().getToString().empty();
        }

        if(isPreString && isPostString)
        {
            toBeAdd += pFuncSon->getValue().getToString();
        }
    }

    return true;
}

bool MergeFunc::addToString(std::string& toBeAdd, CBaseFunc* FuncToAdd)
{
    toBeAdd += FuncToAdd->getValue().getToString();
    return true;
}

void RandomFunc::AddDendencyInfo( DependencyInfo info )
{
    m_dependencylist.push_back( info );
}

void RandomFunc::AddDepDimensionInfo( DependencyInfo info )
{
    m_DepDimensionList.push_back( info );
}

void RandomFunc::parseDimensionInfo( const std::string& parameterValue, std::string& firstPara, DependencyInfo& info )
{
        typedef boost::tokenizer<boost::char_separator<char> >    tokenizer;
        boost::char_separator<char> sep(",");
        tokenizer tokens(parameterValue, sep);

        auto itor = tokens.begin();
        firstPara = *itor;

        if( ++itor == tokens.end() )
        {
            //std::cout << "RandomFunc::parseDimensionInfo(): only has one parameter." << std::endl;
            return;
        }

        std::string secondPara = *itor;
        auto n = secondPara.find('%');
        info.m_percentage = std::stoi(secondPara.substr(0, n));
        //std::cout << "RandomFunc::parseDimensionInfo(): percentage= " << (uint16_t)info.m_percentage << std::endl;

        if( ++itor == tokens.end() )
        {
            info.m_number = 0;
            //std::cout << "RandomFunc::parseDimensionInfo(): only has two parameters." << std::endl;
            return;
        }

        std::string thirdPara = *itor;
        info.m_number = std::stoi(thirdPara);
        //std::cout << "RandomFunc::parseDimensionInfo(): number= " << info.m_number << std::endl;
}

bool RandomFunc::fillDimensionInfo( dependencylist& depDimensionList )
{
    CXmlParser* parser = getColumn()->getParser();

    CTableObject* pTable = ( CTableObject* ) this->GetTableObject();
    if ( pTable == NULL )
    {
        ACE_DEBUG((LM_ERROR, "can not get the CTableObject for RandomFunc::fillDimensionInfo().\n"));
        return false;
    }

    for(auto& info : depDimensionList)
    {
        //set info.m_initRow for MultiFileRandom
        DimensionalFile* pDimTable = ( DimensionalFile* )info.m_pTableInfo;
        const CXmlParser::DimensionfileDatas* pDimensionfileData = parser->findDimensionFileDatas( pDimTable->getObjectName() );
        if ( pDimensionfileData == NULL || pDimensionfileData->empty() )
        {
            ACE_DEBUG((LM_ERROR, "can not find the the proper dimension Data (%s) for column %s in RandomFunc::fillDimensionInfo().\n",
                       pDimTable->getObjectName().c_str(), getColumn()->getObjectName().c_str()));
            return false;
        }

        uint32_t maxsize = pDimensionfileData->size();

        info.m_initRow = pTable->rand() % maxsize;

        if ( ( info.m_number == 0 ) || ( info.m_number > maxsize ) )
        {
            info.m_number = maxsize;
        }

        info.m_endRow = (info.m_initRow + info.m_number) % maxsize;
        info.m_lastnumber = info.m_initRow;

        CInterfaceObj* pItfObject = ( CInterfaceObj* ) pTable->getParent();

        ACE_DEBUG((LM_DEBUG, "Interface: %s, table: %s, dimension file: %s[%u,%u), total: %u.\n", 
                   pItfObject->getObjectName().c_str(),
                   pTable->getObjectName().c_str(),
                   pDimTable->getObjectName().c_str(),
                   info.m_initRow,
                   info.m_endRow,
                   info.m_number));    
    }
    return true;
}

bool RandomFunc::anyzeDepDimensionInfo( const std::string& parameterName, const std::string& ParameterValue, CAlignDataObject*& pDimentsionFile,
                                        CAlignDataObject*& pDimensionColumn )
{
    CAlignDataObject* pColumn = this->getColumn();

    if ( pColumn == NULL )
        return false;

    CXmlParser* parser = pColumn->getParser();

    if ( parser == NULL )
        return false;

    std::string strTableName, strColName;
    uint32_t rowNum;

    m_AlignDataTypeDef.ParserTableInfoFromString( ParameterValue.c_str(), ParameterValue.length(), strTableName, rowNum, strColName );

    if ( !strTableName.empty() && !strColName.empty() )
    {
        pDimentsionFile = parser->findlitealigndataObjectFileInMapper( strTableName, litealigndata::ELEMENT_DimensionalFile );

        if ( pDimentsionFile == NULL )
        {
            ACE_DEBUG((LM_ERROR, "current depended dimensionObject: %s does not exists, check the xml's configuration of CParameterObject = %s.\n",
                       strTableName.c_str(), parameterName.c_str()));
            return false;
        }

        pDimensionColumn = pDimentsionFile->findChildObject( strColName );

        if ( pDimensionColumn == NULL )
        {
            ACE_DEBUG((LM_ERROR, "current depended dimensionColumn: %s does not exists, check the xml's configuration of CParameterObject = %s.\n",
                       strColName.c_str(), parameterName.c_str()));
            return false;
        }

        return true;
    }

    ACE_DEBUG((LM_ERROR, "can not find relative table name or column name for parameterName: %s and value: %s.\n",
               parameterName.c_str(), ParameterValue.c_str()));
    return true;
}

const char* RandomFunc::RandomName[] =
{ "InvalidRandmon", "Table", "File", "DefaultValue", "NatureRandom", "mapping", "Less", "More", "Percent", "MultiFile", "PseudoRandom"};

bool RandomFunc::process()
{
    ACE_DEBUG((LM_DEBUG, "start process data by RandomFunc for column: %s.\n", m_pColumn->getObjectName().c_str()));
    //cout << "start process data by RandomFunc for column   " << m_pColumn->getObjectName().c_str() << endl;
    if ( !CBaseFunc::process() )
        return false;

    CAlignDataObject* pColumn = this->getColumn();

    if ( pColumn == NULL )
        return false;

    CXmlParser* parser = pColumn->getParser();

    if ( parser == NULL )
        return false;

    alignDataList::iterator it = this->m_parameterlist.begin();

    while ( it != this->m_parameterlist.end() )
    {
        std::string parameterName = ( *it )->getObjectName();

        litealigndata::parameterTag tag = ( litealigndata::parameterTag ) m_AlignDataTypeDef.FindRelativeTag( litealigndata::ParaTypeString, litealigndata::PARA_MAX, parameterName.c_str() );

        std::string ParameterValue = ( *it )->getStrValue();

        if ( ParameterValue.empty() )
        {
            if(litealigndata::PARA_Value == tag)
            {

            }
            else
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is empty!\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }            
        }

        CAlignDataObject* pDimensionFile = NULL;
        CAlignDataObject* pDimensionColumn = NULL;

        switch ( tag )
        {
        case litealigndata::PARA_Invalid:
            ACE_DEBUG((LM_ERROR, "the parameterName: %s have not been defined, please check the configuration.\n", parameterName.c_str()));
            return false;

        case litealigndata::PARA_SourceType:
            m_type = ( RandomFunc::RandomType ) m_AlignDataTypeDef.FindRelativeTag( RandomName, RandomMax, ParameterValue.c_str() );


            if ( m_type == RandomFunc::InvalidRandom )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s do not find valid RandomType for %s.\n", parameterName.c_str(), parameterName.c_str()));
                return false;
            }

            break;

        case litealigndata::PARA_Value:
            if ( m_type == RandomFunc::InvalidRandom )
            {
                ACE_DEBUG((LM_ERROR, "random function need to get the random type at first before get the parameter Value.\n"));
                return false;
            }

            if ( m_type == RandomFunc::DefalutRandom )
            {
                AdddefaultValue( ParameterValue.c_str(), ParameterValue.length() );
            }
            else if ( m_type == RandomFunc::FileRandom || m_type == RandomFunc::MappingRandom || m_type == RandomFunc::MultiFileRandom )
            {
                //storage DimensionInfo.
                DependencyInfo info;
                std::string firstPara;
                parseDimensionInfo( ParameterValue, firstPara, info);
                if ( !anyzeDepDimensionInfo( parameterName, firstPara, pDimensionFile, pDimensionColumn ) )
                    return false;

                if ( pDimensionFile != NULL && pDimensionColumn != NULL )
                {
                    m_mappingidx = parser->addDimensionInfo( pDimensionFile, pDimensionColumn ); //for loading DimensionData.
                    info.m_pTableInfo = pDimensionFile;
                    info.m_pColumnInfo = pDimensionColumn;
                    AddDepDimensionInfo( info );   //for calculating data.
                }
            }

            break;

        case litealigndata::PARA_LowRange:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "he parameterName: %s 's value %s is illegal.\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }

            this->m_lowRange = atol( ParameterValue.c_str() );
            break;

        case litealigndata::PARA_HighRange:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "he parameterName: %s 's value %s is illegal.\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }

            this->m_highRange = atol( ParameterValue.c_str() );
            break;

        case litealigndata::PARA_Percent:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "he parameterName: %s 's value %s is illegal.\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }
            this->m_percent = atol( ParameterValue.c_str() );
            break;

        case litealigndata::PARA_DepColumn:
            break;

        case litealigndata::PARA_DepKey:

            if ( m_type != RandomFunc::MappingRandom )
            {
                ACE_DEBUG((LM_ERROR, "the PARA_DepKey is only supported in the mapping mode, the paraname: %s and para value: %s.\n",
                           parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }

            if ( !anyzeDepDimensionInfo( parameterName, ParameterValue, pDimensionFile, pDimensionColumn ) )
                return false;

            if ( m_keyMappingInfo.m_pDimensionFile != NULL && m_keyMappingInfo.m_pDimensionFile != pDimensionFile )
            {
                ACE_DEBUG((LM_ERROR, "the DepKey are different tables in the same CColumnObject,it is not be supported! the paraName: %s and para value: %s.\n",
                           parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }

            if ( m_keyMappingInfo.m_pDimensionFile == NULL )
                m_keyMappingInfo.m_pDimensionFile = pDimensionFile;

            m_keyMappingInfo.m_CollumnArray.push_back( pDimensionColumn );

            parser->addDimensionInfo( pDimensionFile, pDimensionColumn );

            break;

        default:
            break;
        }

        ++it;
    }

    if ( m_keyMappingInfo.m_pDimensionFile != NULL )
        parser->addDimensionKeyInfo( m_keyMappingInfo );

    if(m_type == RandomFunc::MultiFileRandom)
    {
        uint8_t total_percentage = 0;
        uint8_t fileIdx = 0;
        for(const auto& depDimInfo : m_DepDimensionList)
        {
            for(int i = 0; i < depDimInfo.m_percentage; i++)
            {
                randomArr[i+total_percentage] = fileIdx;
            }
            fileIdx++;
            total_percentage += depDimInfo.m_percentage;
        }
        RandomArray(randomArr, RANDOM_ARR_SIZE);

        if( !m_DepDimensionList.empty() && total_percentage != 100 )
        {
            ACE_DEBUG((LM_ERROR, "Invalid total percentage in MultiFileRandom, please reconfigure!\n"));
            return false;
        }
    }
//cout << "End process data by RandomFunc for column   " << endl;
    return true;
}

void RandomFunc::RandomArray(int32_t a[], int32_t n)
{
    CTableObject* pTable = ( CTableObject* ) this->GetTableObject();

    if ( pTable == NULL )
    {
        ACE_DEBUG((LM_ERROR, "can not get the CTableObject for RandomFunc::randomArray().\n"));
        return;
    }

    int32_t index, tmp, i;
    //srand(time(NULL));
    for (i = 0; i < n; i++)
    {
        index =  pTable->rand()% (n - i) + i;
        if (index != i && index < n)
        {
            tmp = a[i];
            a[i] = a[index];
            a[index] = tmp;
        }
    }
}

void RandomFunc::AdddefaultValue( const char* defaultValue, uint32_t size )
{
    char* pToken = NULL;
    char* pSave = NULL;
    const char* pDelimiter = ",";

    pToken = strtok_r( ( char* ) defaultValue, pDelimiter, &pSave );

    while ( pToken )
    {
        m_defaultValues.push_back( pToken );
        pToken = strtok_r( NULL, pDelimiter, &pSave );
    }
}

bool RandomFunc::SetValueForRandom( const baseType* value )
{
    litealigndata::autoType valuetype = value->getType();

    litealigndata::autoType CurrentType = m_value.getType();

    if ( autoValue::ConvertAutoTypeToGenericType( valuetype ) != autoValue::ConvertAutoTypeToGenericType( CurrentType ) )
    {
        ACE_DEBUG((LM_ERROR, "Current column valueType %s is inconsistent with the value type %s.\n", 
                   litealigndata::autoTypeString[CurrentType], litealigndata::autoTypeString[valuetype] ));
        return false;
    }

    CTableObject* pTable = ( CTableObject* ) GetTableObject();
    if ( pTable == NULL )
    {
        ACE_DEBUG((LM_ERROR, "can not get the table Object at RandomFunc::SetValueForRandom.\n"));
        return false;
    }

    {
        CharArray const * pacPointer= dynamic_cast<CharArray const *>(value);
        if(pacPointer){
            char const * pacString = pacPointer->getValue();
            return SetValue(pacString, imsi_type);
        }
        else{
            uint64_t ivalue = ( ( numberType* ) value )->getValue();
            return SetValue( ivalue, imsi_type );
        }
    }

}


bool RandomFunc::PseudoRandomCaculate()
{
    uniform_int_distribution<unsigned> u(m_lowRange,m_highRange);
    uint64_t value = u(e);
    if ( !SetValue( value, imsi_type ) )
        return false;
    ACE_DEBUG((LM_DEBUG, "RandomFunc::PseudoRandomCaculate Get current value is %Q, the low range:%Q, the highRange:%Q.\n", 
               m_value.getnumber(), m_lowRange, m_highRange));

	return true;
}

bool RandomFunc::NatureRandomCaculate()
{
    uint64_t value = m_AlignDataTypeDef.randnumer( m_lowRange, m_highRange );

    if ( !SetValue( value, imsi_type ) )
        return false;

    ACE_DEBUG((LM_DEBUG, "RandomFunc::NatureRandomCaculate Get current value is %Q, the low range:%Q, the highRange:%Q.\n", 
               m_value.getnumber(), m_lowRange, m_highRange));

    return true;
}

bool RandomFunc::DefaulRandomCalculate()
{
    if(0 == m_defaultValues.size())
    {   
        // for NULL value
        if ( !SetValue( "") )
            return false;

        return true;
    }
    
    uint32_t idx = m_AlignDataTypeDef.randnumer( 0, m_defaultValues.size() - 1 );

    if ( !SetValue( m_defaultValues[idx].c_str() , imsi_type) )
        return false;

    if ( m_value.getnumber() == 0 )
        ACE_DEBUG((LM_DEBUG, "RandmonFunc::DefalutRandom Get current value is %s.\n", m_value.getString().c_str()));
    else
        ACE_DEBUG((LM_DEBUG, "RandmonFunc::DefalutRandom Get current value is %Q.\n", m_value.getnumber()));

    return true;
}

bool RandomFunc::PercentRandomCalculate()
{
    CTableObject* pTable = ( CTableObject* ) this->GetTableObject();

    if ( pTable == NULL )
    {
        ACE_DEBUG((LM_ERROR, "can not get the CTableObject for RandomFunc::PercentRandomCalculate().\n"));
        return false;
    }

    uint64_t x_value = (Random::MAX_RAND + 1 ) * m_percent /100;
    uint64_t value = 0;
    uint64_t rand_value = pTable->rand();
    if ( rand_value <= x_value )
    {
        value = 1;
    }

    if ( !SetValue( value , imsi_type) )
        return false;

    if ( m_value.getnumber() == 0 )
        ACE_DEBUG((LM_DEBUG, "RandmonFunc::PercentRandomCalculate Get current value is %s.\n", m_value.getString().c_str()));
    else
        ACE_DEBUG((LM_DEBUG, "RandmonFunc::PercentRandomCalculate Get current value is %Q.\n", m_value.getnumber()));

    return true;
}

bool RandomFunc::TableRandomCalculate( RandomType randomType )
{
    RandomFunc::dependencylist::const_iterator it = m_dependencylist.begin();
    CTableObject* pTable = ( CTableObject* )( it->m_pTableInfo );
    CColumnObject* pColumn = ( CColumnObject* )( it->m_pColumnInfo );

    if ( pTable == NULL || pColumn == NULL )
        return false;

    uint32_t max_idx = pTable->getCurFilledCount();
    if ( pTable != this->GetTableObject() )
        max_idx--;

    if ( randomType == RandomFunc::TableRandom )
    {
        uint32_t min_idx = 0;

        if ( m_lowRange > max_idx )
        {
            ACE_DEBUG((LM_ERROR, "RandomFunc::TableRandomCalculate Get the value from the table: %s.\n", pTable->getObjectName().c_str()));
            ACE_DEBUG((LM_ERROR, "%IInvalid lowRange: %Q, max_id: %u for the column: %s.\n", m_lowRange, max_idx, getColumn()->getObjectName().c_str()));
            return false;
        }

        if ( m_highRange < max_idx )
            max_idx = m_highRange;

        uint32_t idx = m_AlignDataTypeDef.randnumer( min_idx, max_idx );
        uint8_t* pdest = m_value.getData();
        uint8_t len = m_value.getLen();
        uint8_t* psrc = ( uint8_t* )( pTable->getData() ) + idx * pTable->getTableHeader().entry_size + pColumn->getOffset();

        memcpy( pdest, psrc, len );

        ACE_DEBUG((LM_DEBUG, "RandomFunc::TableRandomCalculate Get the value from the table: %s's idx %u and high range %Q max_id %u for the column: %s and  value: %Q.\n", 
                   pTable->getObjectName().c_str(), idx, m_highRange, max_idx, getColumn()->getObjectName().c_str(), getValue().getnumber()));
    }
    else if ( randomType == RandomFunc::LessRandom || randomType == RandomFunc::MoreRandom )
    {

        CTableObject* currentTable = ( CTableObject* ) GetTableObject();

        if ( currentTable == NULL )
            return false;

        uint32_t CurrentRow = currentTable->getCurFilledCount();

        if ( CurrentRow > max_idx )
        {
            ACE_DEBUG((LM_ERROR, "RandomFunc::TableRandomCalculate: the current Row is beyond the MaxLimit (%d) of column %s.\n",
                       max_idx, pColumn->getObjectName().c_str()));
            return false;
        }

        autoValue value;

        if ( !pColumn->GetColumnData( CurrentRow, value ) )
            return false;

        uint64_t iNumber = value.getnumber();
        uint64_t iValue = 0;

        if ( randomType == RandomFunc::LessRandom )
        {
            iValue = m_AlignDataTypeDef.randnumer( m_lowRange, iNumber );
        }
        else if ( randomType == RandomFunc::MoreRandom )
        {
            iValue = m_AlignDataTypeDef.randnumer( iNumber, m_highRange );
        }

        SetValue( iValue , imsi_type);

        ACE_DEBUG((LM_DEBUG, "RandomFunc::TableRandomCalculate Get value from the table: %s, column: %s value: %Q; the high range: %Q, low range: %Q, value: %Q, random Type: %u.\n", 
                   pTable->getObjectName().c_str(), pColumn->getObjectName().c_str(), 
                   iNumber, m_highRange, m_lowRange, m_value.getnumber(), randomType));
    }

    return true;
}

bool RandomFunc::FileRandomCalculate()
{
    if ( m_DepDimensionList.empty() )
        return false;

    RandomFunc::dependencylist::const_iterator it = m_DepDimensionList.begin();

    DimensionalFile* pTable = ( DimensionalFile* ) it->m_pTableInfo;

    DimensionFileColumn* pColumn = ( DimensionFileColumn* ) it->m_pColumnInfo;

    if ( pTable == NULL || pColumn == NULL )
    {
        ACE_DEBUG((LM_ERROR, "there is no depended dimensionFile and columns for the column %s\n", getColumn()->getObjectName().c_str()));
        return false;
    }

    //base on tableName
    CXmlParser* parser = getColumn()->getParser();

    const CXmlParser::DimensionfileDatas* pDimensionfileData = parser->findDimensionFileDatas( pTable->getObjectName() );

    if ( pDimensionfileData == NULL || pDimensionfileData->empty() )
    {
        ACE_DEBUG((LM_ERROR, "can not find the the proper dimension Data (%s) for column %s in the random calculation.\n",
                   pTable->getObjectName().c_str(), getColumn()->getObjectName().c_str()));
        return false;
    }

    uint32_t maxsize = pDimensionfileData->size() - 1;

    if ( m_lowRange >= maxsize )
        m_lowRange = maxsize;

    if ( m_highRange >= maxsize || m_highRange == 0 )
        m_highRange = maxsize;

    //get the rownumber
    uint32_t rowidx = m_AlignDataTypeDef.randnumer( m_lowRange, m_highRange );

    //get col idx
    uint32_t colIdx = pColumn->getMappingidx() - 1;

    const CXmlParser::Dimensioncolvalues* pRowdata = ( *pDimensionfileData )[rowidx];

    const baseType* pValue = ( *pRowdata )[colIdx];

    if ( !SetValueForRandom( pValue ) )
    {
        ACE_DEBUG((LM_ERROR, "RandomFunc::FileRandomCalculate() run SetValueForRandom failed!\n"));
        return false;
    }

    ACE_DEBUG((LM_DEBUG, "RandomFunc::FileRandomCalculate() Get current value is %Q.\n", m_value.getnumber()));

    return true;
}

bool RandomFunc::MultiFileRandomCalculate()
{
    if ( m_DepDimensionList.empty() )
        return false;

    CTableObject* pTable = ( CTableObject* ) this->GetTableObject();

    if ( pTable == NULL )
    {
        ACE_DEBUG((LM_ERROR, "can not get the CTableObject for RandomFunc::MultiFileRandomCalculate().\n"));
        return false;
    }
    if ( pTable != NULL && pTable->getCurFilledCount() == 0 )
    {
        total_num = 0;
    }

    total_num++;

    uint32_t fileIdx = 0;//randomly pick one DepDimension file

    fileIdx = randomArr[randomArrIndex];
    setFileIndex(fileIdx);
    randomArrIndex++;
    if(randomArrIndex == RANDOM_ARR_SIZE)
    {
        randomArrIndex = 0;
    }

    DependencyInfo &curDepInfo = m_DepDimensionList[fileIdx];
    DimensionalFile* pDimTable = ( DimensionalFile* )curDepInfo.m_pTableInfo;
    DimensionFileColumn* pColumn = ( DimensionFileColumn* )curDepInfo.m_pColumnInfo;
    if ( pDimTable == NULL || pColumn == NULL )
    {
        ACE_DEBUG((LM_ERROR, "there is no depended dimensionFile and columns for the column %s.\n",
                   getColumn()->getObjectName().c_str()));
        return false;
    }

    //base on tableName
    CXmlParser* parser = getColumn()->getParser();

    const CXmlParser::DimensionfileDatas* pDimensionfileData = parser->findDimensionFileDatas( pDimTable->getObjectName() );

    if ( pDimensionfileData == NULL || pDimensionfileData->empty() )
    {
        ACE_DEBUG((LM_ERROR, "can not find the the proper dimension Data (%s) for column %s in the random calculation.\n",
                   pDimTable->getObjectName().c_str(), getColumn()->getObjectName().c_str()));
        return false;
    }

    uint32_t dimFileSize = pDimensionfileData->size();

    if ( pTable != NULL && pTable->getCurFilledCount() == 0 )
    {
        total_num = 0;
        if ( !isDepDimensionInit )
        {
            fillDimensionInfo(m_DepDimensionList);
            isDepDimensionInit = true;
        }
    }

    if(curDepInfo.m_lastnumber == curDepInfo.m_endRow)
    {
        curDepInfo.m_lastnumber = curDepInfo.m_initRow;
    }

    if(curDepInfo.m_lastnumber == dimFileSize)
    {
        curDepInfo.m_lastnumber = 0;
    }

    uint32_t rowidx = curDepInfo.m_lastnumber++;

    //get col idx
    uint32_t colIdx = pColumn->getMappingidx() - 1;

    const CXmlParser::Dimensioncolvalues* pRowdata = ( *pDimensionfileData )[rowidx];

    const baseType* pValue = ( *pRowdata )[colIdx];

    if ( !SetValueForRandom( pValue ) )
    {
        ACE_DEBUG((LM_ERROR, "RandomFunc::FileRandomCalculate() run SetValueForRandom failed!\n"));
        return false;
    }

    pTable->dimItemCount[fileIdx * CTableObject::MAX_DIM_ROW_NUM + rowidx]++;

    ACE_DEBUG((LM_DEBUG, "RandomFunc::FileRandomCalculate() Get current value is %Q.\n", 
                        m_value.getnumber()));

    return true;
}

bool RandomFunc::MappingRandomCalculate()
{
    if ( m_dependencylist.empty() )
        return false;

    RandomFunc::dependencylist::const_iterator it = m_dependencylist.begin();

    char hashstring[256] =
    { 0 };

    while ( it != m_dependencylist.end() )
    {
        CColumnObject* pColumn = ( CColumnObject* ) it->m_pColumnInfo;

        if ( !pColumn )
            continue;

        autoValue value = pColumn->getFunc()->getValue();

        char strValue[64] =
        { 0 };
        sprintf( hashstring + strlen( hashstring ), "%s", value.GetHashString( strValue ) );
        ++it;
    }

    ACE_DEBUG((LM_DEBUG, "current Column hashString is %s.\n", hashstring));

    uint32_t hashKey2 = 0;//m_AlignDataTypeDef.RSHash( hashstring );

    CXmlParser* parser = getColumn()->getParser();

    const CXmlParser::DimensionfileDatas* pfileDatas = parser->findDimensionFileDatasforIdx(
                m_keyMappingInfo.m_pDimensionFile->getObjectName(), hashKey2 );

    if ( pfileDatas == NULL || pfileDatas->empty() )
    {
        RandomFunc::dependencylist::const_iterator it = m_dependencylist.begin();

        while ( it != m_dependencylist.end() )
        {
            CColumnObject* pColumn = ( CColumnObject* ) it->m_pColumnInfo;

            if ( !pColumn )
                continue;

            autoValue value = pColumn->getFunc()->getValue();

            char strValue[64] =
            { 0 };

            ACE_DEBUG((LM_DEBUG, "%s.\n", value.GetHashString( strValue )));

            ++it;
        }

        ACE_DEBUG((LM_ERROR, "can not find the the proper dimension Data (%s) for column %s in the random calculation.\n",
                   m_keyMappingInfo.m_pDimensionFile->getObjectName().c_str(), getColumn()->getObjectName().c_str()));
        return false;
    }

    uint32_t maxsize = pfileDatas->size() - 1;

    if ( m_lowRange >= maxsize )
        m_lowRange = maxsize;

    if ( m_highRange >= maxsize || m_highRange == 0 )
        m_highRange = maxsize;

    uint32_t rowidx = m_AlignDataTypeDef.randnumer( m_lowRange, m_highRange );

    RandomFunc::dependencylist::const_iterator dimensionIt = m_DepDimensionList.begin();

    if ( dimensionIt == m_DepDimensionList.end() )
    {

        ACE_DEBUG((LM_ERROR, "can not find the the proper dimension Column for column %s in the random calculation.\n",
                   getColumn()->getObjectName().c_str()));
        return false;
    }

    if ( dimensionIt->m_pColumnInfo == NULL )
        return false;

    uint32_t colIdx = ( ( DimensionFileColumn* )( dimensionIt->m_pColumnInfo ) )->getMappingidx() - 1;

    const CXmlParser::Dimensioncolvalues* pRowdata = ( *pfileDatas )[rowidx];

    const baseType* pValue = ( *pRowdata )[colIdx];

    if ( !SetValueForRandom( pValue ) )
    {
        ACE_DEBUG((LM_ERROR, "RandomFunc::MappingRandomCalculate() run SetValueForRandom failed!\n"));
        return false;
    }

    ACE_DEBUG((LM_DEBUG, "RandomFunc::MappingRandomCalculate() Get current value is %Q.\n", m_value.getnumber()));

    return true;
}

bool RandomFunc::Calculate()
{
    
    ACE_DEBUG((LM_DEBUG, "start Caculate data by RandomFunc for column: %s.\n", m_pColumn->getObjectName().c_str()));

    if ( !CBaseFunc::Calculate() )
        return false;

    if ( m_type == RandomFunc::DefalutRandom )
    {
        return DefaulRandomCalculate();
    }
    else if ( m_type == RandomFunc::PercentRandom )
    {
        return PercentRandomCalculate();
    }
    else if ( m_type == RandomFunc::NatureRandom )
    {
        return NatureRandomCaculate();
    }
    else if ( m_type == TableRandom || m_type == LessRandom || m_type == MoreRandom )
    {
        if ( m_dependencylist.empty() )
            return false;

        if ( !TableRandomCalculate( m_type ) )
            return false;
    }
    else if ( m_type == PseudoRandom)
    {
    	return PseudoRandomCaculate();
    }
    else if ( m_type == RandomFunc::FileRandom )
    {
        return FileRandomCalculate();
    }
    else if ( m_type == RandomFunc::MultiFileRandom )
    {
        return MultiFileRandomCalculate();
    }
    else if ( m_type == RandomFunc::MappingRandom )
    {
        return MappingRandomCalculate();
    }

    return true;
}
bool fileReaderFunc::process()
{

    ACE_DEBUG((LM_DEBUG, "start process data by fileReaderFunc for column: %s.\n", m_pColumn->getObjectName().c_str()));

    if ( !CBaseFunc::process() )
        return false;

    return true;
}

bool fileReaderFunc::Calculate()
{
    ACE_DEBUG((LM_DEBUG, "start Caculate data by fileReaderFunc for column: %s.\n", m_pColumn->getObjectName().c_str()));

    return true;
}

const char* AcculateFunc::AcculateName[] = { "InvalidAcculate", "File" };

void AcculateFunc::AddDepDimensionInfo( DependencyInfo info )
{
    m_DepDimensionList.push_back( info );
}

bool AcculateFunc::anyzeDepDimensionInfo( const std::string& parameterName, const std::string& ParameterValue, CAlignDataObject*& pDimentsionFile,
                                        CAlignDataObject*& pDimensionColumn )
{
    //analyze the dependency the DimensionFileInfo and DimensionColumnInfo.
    CAlignDataObject* pColumn = this->getColumn();

    if ( pColumn == NULL )
        return false;

    CXmlParser* parser = pColumn->getParser();

    if ( parser == NULL )
        return false;

    std::string strTableName, strColName;
    uint32_t rowNum;

    m_AlignDataTypeDef.ParserTableInfoFromString( ParameterValue.c_str(), ParameterValue.length(), strTableName, rowNum, strColName );

    if ( !strTableName.empty() && !strColName.empty() )
    {
        pDimentsionFile = parser->findlitealigndataObjectFileInMapper( strTableName, litealigndata::ELEMENT_DimensionalFile );

        if ( pDimentsionFile == NULL )
        {
            ACE_DEBUG((LM_ERROR, "current depended dimensionObject: %s does not exists, check the xml's configuration of CParameterObject = %s.\n",
                       strTableName.c_str(), parameterName.c_str()));
            return false;
        }

        pDimensionColumn = pDimentsionFile->findChildObject( strColName );

        if ( pDimensionColumn == NULL )
        {
            ACE_DEBUG((LM_ERROR, "current depended dimensionColumn: %s does not exists, check the xml's configuration of CParameterObject = %s.\n",
                       strColName.c_str(), parameterName.c_str()));
            return false;
        }

        return true;
    }

    ACE_DEBUG((LM_ERROR, "can not find relative table name or column name for parameterName: %s and value: %s.\n", 
               parameterName.c_str(), ParameterValue.c_str()));
    return true;
}


bool AcculateFunc::process()
{

    ACE_DEBUG((LM_DEBUG, "start process data by AcculateFunc for column: %s.\n", m_pColumn->getObjectName().c_str()));

    if ( !CBaseFunc::process() )
        return false;
    
    CAlignDataObject* pColumn = this->getColumn();
    
    if ( pColumn == NULL )
        return false;
    
    CXmlParser* parser = pColumn->getParser();
    
    if ( parser == NULL )
        return false;

    alignDataList::iterator it = this->m_parameterlist.begin();

    std::string initial;

    while ( it != this->m_parameterlist.end() )
    {
        std::string parameterName = ( *it )->getObjectName();

        litealigndata::parameterTag tag = ( litealigndata::parameterTag ) m_AlignDataTypeDef.FindRelativeTag( litealigndata::ParaTypeString, litealigndata::PARA_MAX, parameterName.c_str() );

        std::string ParameterValue = ( *it )->getStrValue();

        if ( ParameterValue.empty() )
        {
            ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is empty!\n", parameterName.c_str(), ParameterValue.c_str()));
            return false;
        }
        
        CAlignDataObject* pDimensionFile = NULL;
        CAlignDataObject* pDimensionColumn = NULL;

        switch ( tag )
        {
        case litealigndata::PARA_Invalid:
            ACE_DEBUG((LM_ERROR, "the parameterName: %s have not been defined, please check the configuration.\n", parameterName.c_str()));
            return false;

        case litealigndata::PARA_Prefix:
            if ( ParameterValue != "0" )
                this->m_prefix = ParameterValue;

            break;

        case litealigndata::PARA_Init:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal!\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }

            initial = ParameterValue;
            break;

        case litealigndata::PARA_Step:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal!\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }

            this->m_step = atoi( ParameterValue.c_str() );
            break;

        case litealigndata::PARA_DuplicatedUnit:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal!\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }

            this->m_duplicatedUnit = atoi( ParameterValue.c_str() );
            break;

        case litealigndata::PARA_LowRange:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal!\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }

            this->m_lowRange = atol( ParameterValue.c_str() );
            break;

        case litealigndata::PARA_HighRange:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal!\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }

            this->m_highRange = atol( ParameterValue.c_str() );
            break;

        case litealigndata::PARA_IMSIType:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal!\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }

            this->imsi_type = ( litealigndata::subIdType ) atoi( ParameterValue.c_str() );
            break;

        case litealigndata::PARA_Reset:
            this->isreset = atoi( ParameterValue.c_str() );
            break;

        case litealigndata::PARA_Percent:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal!\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }
            else
            {
                uint8_t percent = atoi( ParameterValue.c_str() );
                if ( percent > 100 )
                {
                    ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s should be less than or equal to 100!\n", 
                               parameterName.c_str(), ParameterValue.c_str()));
                    return false;
                }
                this->m_percent = percent;
            }
            break;

        case litealigndata::PARA_PercentInBatch:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal!\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }
            else
            {
                uint8_t percent = atoi( ParameterValue.c_str() );
                if ( percent > 100 )
                {
                    ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s should be less than or equal to 100!\n", 
                               parameterName.c_str(), ParameterValue.c_str()));
                    return false;
                }

                this->m_percentInBatch = percent;
            }
            break;

        case litealigndata::PARA_RowsInBatch:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal!\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }

            this->m_rowsInBatch = atoi( ParameterValue.c_str() );
            break;

        case litealigndata::PARA_SourceType:
            m_type = ( AcculateFunc::AcculateType ) m_AlignDataTypeDef.FindRelativeTag( AcculateFunc::AcculateName, AcculateMax, ParameterValue.c_str() );            

            if ( m_type != AcculateFunc::FileAcculate)            
            {                
                ACE_DEBUG((LM_ERROR, "the parameterName: %s do not find valid type for %s.\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;            
            }  
            initial = "0";
            break;

        case litealigndata::PARA_Value:
            if( m_type == AcculateFunc::FileAcculate )
            {
                DependencyInfo info;                
                if ( !anyzeDepDimensionInfo( parameterName, ParameterValue, pDimensionFile, pDimensionColumn ) )                    
                    return false;                
                if ( pDimensionFile != NULL && pDimensionColumn != NULL )                
                {                    
                    m_mappingidx = parser->addDimensionInfo( pDimensionFile, pDimensionColumn ); 
                    info.m_pTableInfo = pDimensionFile;                    
                    info.m_pColumnInfo = pDimensionColumn;                    
                    AddDepDimensionInfo( info );   //for calculating data.                
                }
            }
            break;

        default:
            break;
        }

        ++it;
    }

    char szInitals[21] = {0};

    if( initial.empty() )
    {
        ACE_DEBUG((LM_ERROR, "MUST configure the Init parameter!\n"));
        return false;
    }

    if( !m_prefix.empty() )
    {
        if( m_prefix == "current_+")
        {
            CTableObject* pTable = ( CTableObject* ) GetTableObject();
            m_init = pTable->getInitialTime() *1000000;
            m_lastnumber = m_init;
            return true;
        }
        std::size_t found = m_prefix.find("current_-");
        if (found!=std::string::npos)
        {
            if ( m_prefix.length() > 9 )
            {
                std::string str = m_prefix.substr (9);
                if ( !m_AlignDataTypeDef.isAlldigits( str.c_str(), str.length() ) )
                {
                    ACE_DEBUG((LM_ERROR, "rthe prefix value: %s is illegal.\n", m_prefix.c_str()));
                    return false;
                }
                snprintf( szInitals, 21, "%s", str.c_str() );
                uint64_t usecGap = atol( szInitals );
                CTableObject* pTable = ( CTableObject* ) GetTableObject();
                m_init = pTable->getInitialTime() *1000000 - usecGap*1000000;
                m_lastnumber = m_init;
                return true;
            }
            return false;
        }
        snprintf( szInitals, 21, "%s%s", m_prefix.c_str(), initial.c_str() );
    }
    else
    {
        snprintf( szInitals, 21, "%s", initial.c_str() );
    }

    m_init = atol( szInitals );
    m_lastnumber = m_init;

    return true;
}

bool AcculateFunc::FileAcculateCalculate()
{
    if ( m_DepDimensionList.empty() )
        return false;

    RandomFunc::dependencylist::const_iterator it = m_DepDimensionList.begin();

    DimensionalFile* pTable = ( DimensionalFile* ) it->m_pTableInfo;

    DimensionFileColumn* pColumn = ( DimensionFileColumn* ) it->m_pColumnInfo;

    if ( pTable == NULL || pColumn == NULL )
    {
        ACE_DEBUG((LM_ERROR, "there is no depended dimensionFile and columns for the column %s.\n", getColumn()->getObjectName().c_str()));
        return false;
    }

    //base on tableName
    CXmlParser* parser = getColumn()->getParser();

    const CXmlParser::DimensionfileDatas* pDimensionfileData = parser->findDimensionFileDatas( pTable->getObjectName() );

    if ( pDimensionfileData == NULL || pDimensionfileData->empty() )
    {
        ACE_DEBUG((LM_ERROR, "can not find the the proper dimension Data (%s) for column %s in the random calculation.\n",
                   pTable->getObjectName().c_str(), getColumn()->getObjectName().c_str()));
        return false;
    }

    uint32_t maxsize = pDimensionfileData->size() - 1;

    if ( m_lastnumber > maxsize )
    {
        m_lastnumber = 0;
    }

    //get col idx
    uint32_t colIdx = pColumn->getMappingidx() - 1;

    const CXmlParser::Dimensioncolvalues* pRowdata = ( *pDimensionfileData )[m_lastnumber];

    const baseType* pValue = ( *pRowdata )[colIdx];    

    litealigndata::autoType valuetype = pValue->getType();

    litealigndata::autoType CurrentType = m_value.getType();

    if ( autoValue::ConvertAutoTypeToGenericType( valuetype ) != autoValue::ConvertAutoTypeToGenericType( CurrentType ) )
    {
        ACE_DEBUG((LM_ERROR, "Current column valueType is %s is inconsistent with the value type %s\n",
                   litealigndata::autoTypeString[CurrentType], litealigndata::autoTypeString[valuetype]));
        return false;
    }

    {
        CTableObject* pTable = ( CTableObject* ) GetTableObject();
        if ( pTable == NULL )
        {
            ACE_DEBUG((LM_ERROR, "can not get the table Object at RandomFunc::SetValueForRandom.\n"));
            return false;
        }
        if(m_duplicatedUnit > 1)
        {
            if(m_duplicatedUnit > duplicatedUnit)
            {
                duplicatedUnit++;
            }
            else if( m_duplicatedUnit == duplicatedUnit)
            {
                duplicatedUnit = 1;
                m_lastnumber++;//next time, set this value.
            }
        }
        else
        {
            m_lastnumber++;//next time, set this value.
        }
        {
            CharArray const * pacPointer= dynamic_cast<CharArray const *>(pValue);
            if(pacPointer){
                char const * pacString = pacPointer->getValue();
                return SetValue(pacString, imsi_type);
            }
            else{
                uint64_t ivalue = ( ( numberType* ) pValue )->getValue();
                return SetValue( ivalue, imsi_type );
            }
        }
    }
    
    return true;
}

bool AcculateFunc::Calculate()
{
    ACE_DEBUG((LM_DEBUG, "start calculate data by AcculateFunc for column: %s.\n", m_pColumn->getObjectName().c_str()));

    if ( !CBaseFunc::Calculate() )
        return false;

    if ( m_type == AcculateFunc::FileAcculate)
    {
       FileAcculateCalculate();
       return true;
    }    

    CTableObject* pTable = ( CTableObject* )GetTableObject();

    if ( pTable != NULL && pTable->getCurFilledCount() == 0 )
    {
        if ( isreset != 0 )
        {
            m_lastnumber = m_init;
        }
        m_rowIdx = 0;
    }

    if ( m_lastnumber < m_lowRange )
        m_lastnumber = m_lowRange;

    if ( m_lastnumber > m_highRange )
        m_lastnumber = m_lowRange;

    uint64_t x_value = (Random::MAX_RAND + 1 ) * m_percent /100;
    uint64_t rand_value = pTable->rand();
    if ( rand_value < x_value )
    {
        if ( ( m_rowsInBatch != 0 ) && ( m_percentInBatch != 100 ) )
        {
            uint32_t resetRowIdx = m_rowsInBatch * m_percentInBatch / 100;
            if ( ( m_rowIdx % m_rowsInBatch ) == resetRowIdx )
            {
                m_lastnumber -= m_rowsInBatch * ( 100 - m_percentInBatch) / 100;
            }
        }

        SetValue( m_lastnumber, imsi_type );

        if(m_duplicatedUnit > 1)
        {
            if(m_duplicatedUnit == duplicatedUnit)
            {
                duplicatedUnit = 1;
                m_lastnumber += m_step;//next time, set this value
                m_rowIdx++;
            }
            else
            {
                duplicatedUnit++;
            }
        }
        else
        {
            m_lastnumber += m_step;//next time, set this value.
            m_rowIdx++;
        }
    }
    else
    {
        SetValue( (uint64_t)0, litealigndata::Invalid_type );
    }

    if ( m_value.getnumber() == 0 )
        ACE_DEBUG((LM_DEBUG, "AcculateFunc::Caculate Get current value is %s and the imsi_type %u.\n", 
                   m_value.getString().c_str(), imsi_type));
    else
        ACE_DEBUG((LM_DEBUG, "AcculateFunc::Caculate get the value: %Q and the imsi_type %u.\n", 
                   getValue().getnumber(), imsi_type));

    return true;
}

bool UniqueRandomFunc::process()
{

    ACE_DEBUG((LM_DEBUG, "start process data by UniqueRandomFunc for column: %s.\n", m_pColumn->getObjectName().c_str()));

    if ( !CBaseFunc::process() )
        return false;

    alignDataList::iterator it = this->m_parameterlist.begin();

    std::string initial;

    while ( it != this->m_parameterlist.end() )
    {
        std::string parameterName = ( *it )->getObjectName();

        litealigndata::parameterTag tag = ( litealigndata::parameterTag ) m_AlignDataTypeDef.FindRelativeTag( litealigndata::ParaTypeString, litealigndata::PARA_MAX, parameterName.c_str() );

        std::string ParameterValue = ( *it )->getStrValue();

        if ( ParameterValue.empty() )
        {
            ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is empty!\n", parameterName.c_str(), ParameterValue.c_str()));
            return false;
        }

        switch ( tag )
        {
        case litealigndata::PARA_Invalid:
            ACE_DEBUG((LM_ERROR, "the parameterName: %s have not been defined, please check the configuration.\n", parameterName.c_str()));
            return false;

        case litealigndata::PARA_LowRange:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal.\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }

            this->m_lowRange = atol( ParameterValue.c_str() );
            break;

        case litealigndata::PARA_HighRange:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal.\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }

            this->m_highRange = atol( ParameterValue.c_str() );
            break;

        case litealigndata::PARA_IMSIType:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal.\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }

            this->imsi_type = ( litealigndata::subIdType ) atoi( ParameterValue.c_str() );
            break;

        case litealigndata::PARA_Reset:
            this->isreset = atoi( ParameterValue.c_str() );
            break;

        case litealigndata::PARA_Percent:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal.\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }
            else
            {
                uint8_t percent = atoi( ParameterValue.c_str() );
                if ( percent > 100 )
                {
                    ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s should be less than or equal to 100!.\n", 
                               parameterName.c_str(), ParameterValue.c_str()));
                    return false;
                }
                this->m_percent = percent;
            }
            break;

        case litealigndata::PARA_PercentInBatch:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal.\n", 
                                    parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }
            else
            {
                uint8_t percent = atoi( ParameterValue.c_str() );
                if ( percent > 100 )
                {
                    ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s should be less \
                            than or equal to 100!.\n", \
                            parameterName.c_str(), ParameterValue.c_str()));
                    return false;
                }

                this->m_percentInBatch = percent;
            }
            break;

        case litealigndata::PARA_RowsInBatch:
            if ( !m_AlignDataTypeDef.isAlldigits( ParameterValue.c_str(), ParameterValue.length() ) )
            {
                ACE_DEBUG((LM_ERROR, "the parameterName: %s 's value %s is illegal.\n", parameterName.c_str(), ParameterValue.c_str()));
                return false;
            }

            this->m_rowsInBatch = atoi( ParameterValue.c_str() );
            break;

        default:
            break;
        }

        ++it;
    }

    m_lastnumber = m_lowRange;

    return true;
}

bool UniqueRandomFunc::Calculate()
{
    ACE_DEBUG((LM_DEBUG, "start calculate data by UniqueRandomFunc for column: %s.\n", m_pColumn->getObjectName().c_str()));

    if ( !CBaseFunc::Calculate() )
        return false;

    CTableObject* pTable = ( CTableObject* )GetTableObject();
    if ( pTable != NULL && pTable->getCurFilledCount() == 0 )
    {
        if ( isreset != 0 )
        {
            m_lastnumber = m_lowRange;
        }
        m_rowIdx = 0;
    }

    uint64_t total_x_value = (Random::MAX_RAND + 1 ) * m_percent /100;
    uint64_t total_rand_value = pTable->rand();
    if ( total_rand_value < total_x_value )
    {
        if ( m_rowIdx % m_rowsInBatch == 0 )
        {
            //reset resource pool
            uint32_t rowNum = m_rowsInBatch;
            uint32_t uniqRowNum = rowNum * ( 2 * m_percentInBatch - 100 ) / 100;
            uint32_t dupRowNum = (rowNum - uniqRowNum) / 2;

            for(uint32_t j = 0; j < uniqRowNum; j++)
            {
                uniqPool.push_back(m_lastnumber);

                if ( ++m_lastnumber > m_highRange )
                {
                    m_lastnumber = m_lowRange;
                }
            }

            for(uint32_t j = 0; j < dupRowNum; j++)
            {
                for(uint8_t i = 0; i < MAX_POOLS; i++)
                {
                    dupPool[i].push_back(m_lastnumber);
                }

                if ( ++m_lastnumber > m_highRange )
                {
                    m_lastnumber = m_lowRange;
                }
            }
        }

        uint64_t value = 0;
        bool fromDupPool = false;
        uint64_t x_value = (Random::MAX_RAND + 1 ) * m_percentInBatch /100;
        uint64_t rand_value = pTable->rand();
        if ( rand_value < x_value )
        {
            if ( !uniqPool.empty() )
            {
                value = uniqPool.back();
                uniqPool.pop_back();
            }
            else
            {
                fromDupPool = true;
            }
        }
        else
        {
            fromDupPool = true;
        }

        if ( fromDupPool )
        {
            if ( (dupPool[0].empty()) && (!dupPool[1].empty()) )
            {
                dupPool[0].swap(dupPool[1]);
            }

            if ( !dupPool[0].empty() )
            {
                value = dupPool[0].back();
                dupPool[0].pop_back();
            }
            else
            {
                //pick from uniqPool
                if ( !uniqPool.empty() )
                {
                    value = uniqPool.back();
                    uniqPool.pop_back();
                }
                else
                {
                    ACE_DEBUG((LM_ERROR, "uniqPool and both dupPool are empty!\n"));
                }
            }
        }

        SetValue( value, imsi_type );

        ACE_DEBUG((LM_DEBUG, "UniqueRandomFunc::Calculate value = %Q.\n", value));

        m_rowIdx++;
    }
    else
    {
        SetValue( (uint64_t)0, litealigndata::Invalid_type );

        ACE_DEBUG((LM_DEBUG, "UniqueRandomFunc::Calculate set value = %Q.\n", (uint64_t)0));
    }

    return true;
}

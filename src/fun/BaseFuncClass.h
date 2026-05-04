#ifndef _BASEFUNCCLASS_H_
#define _BASEFUNCCLASS_H_

#include <list>
#include <ctime>
#include <vector>
#include <random>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include "struct/AlignDataObject.h"
#include "com/AlignDataTypeDef.h"


class CXmlParser;

class CBaseFunc
{
public:
    typedef std::list<CAlignDataObject* > alignDataList;

public:
    CBaseFunc( litealigndata::funcTag tag );
    CBaseFunc( const CBaseFunc& rhs );
    CBaseFunc& operator=( const CBaseFunc& rhs );
    CAlignDataTypeDef& GetAlignDataTypeDef()
    {
        return m_AlignDataTypeDef;
    }
    virtual bool Initialize();
    virtual bool process();
    virtual bool Calculate();


    virtual ~CBaseFunc();

    const CAlignDataObject* getColumn() const
    {
        return m_pColumn;
    }

    CAlignDataObject* getColumn()
    {
        return m_pColumn;
    }

    void setColumn( const CAlignDataObject* pColumn )
    {
        m_pColumn = const_cast<CAlignDataObject* >( pColumn );
    }

    litealigndata::funcTag getTag() const
    {
        return m_tag;
    }

    void setTag( litealigndata::funcTag tag )
    {
        this->m_tag = tag;
    }

    void addParameter( CAlignDataObject* pParameter )
    {
        m_parameterlist.push_back( pParameter );
    }
    alignDataList& getparameterlist()
    {
        return m_parameterlist;
    }
    const autoValue& getValue() const
    {
        return m_value;
    }

    autoValue& getValue()
    {
        return m_value;
    }

    void setValue( const autoValue& value )
    {
        m_value = value;
    }

    uint32_t getCalTimes() const
    {
        return m_calTimes;
    }

    void setCalTimes( uint32_t calTimes )
    {
        m_calTimes = calTimes;
    }

    static const uint8_t SubControlKey_KeyFlagsIdx = 9;
    static const uint8_t SubControlKey_IP1 = 5;
    static const uint8_t SubControlKey_IP2 = 6;
    static const uint8_t SubControlKey_IPnode1 = 7;
    static const uint8_t SubControlKey_IPnode2 = 8;
    static const uint8_t SubControlKey_SubId = 2;    


    static const uint8_t SubUser_SubscriberIpID = 3;
    static const uint8_t SubUser_GateWay1 = 4;
    static const uint8_t SubUser_GateWay2 = 5;

    static const uint8_t SubMedia_outer_ip_src = 4;
    static const uint8_t SubMedia_outer_ip_dst = 5;
    static const uint8_t SubMedia_ip_src = 6;
    static const uint8_t SubMedia_ip_dst = 7;

    static const uint32_t MAX_UINT32 = 0xFFFFFFFE;
protected:
    CAlignDataObject* GetTableObject();

    uint8_t* GetRowData( uint32_t row );

    uint16_t GetCurrentColIdx();

    bool SetValue( const char* value, litealigndata::subIdType imsitype = litealigndata::Invalid_type );

    bool SetValue( uint64_t value, litealigndata::subIdType imsitype = litealigndata::Invalid_type, bool forBcdMerge = false );

protected:
    alignDataList m_parameterlist;  //point to the parameters.

    CAlignDataObject* m_pColumn;  //point to the columnOjbect or FunctionObject.

    litealigndata::funcTag m_tag;

    autoValue m_value;

    uint32_t m_calTimes;


    litealigndata::subIdType imsi_type; // all derive class use imsi type

    uint32_t m_right_align;
    CAlignDataTypeDef m_AlignDataTypeDef;

};

class AcculateFunc: public CBaseFunc
{
public:
    AcculateFunc( litealigndata::funcTag tag ): CBaseFunc( tag ), m_lowRange( 0 ), m_highRange( MAX_UINT64 ),
        m_init( 0 ), m_step( 0 ), m_duplicatedUnit(1),  duplicatedUnit(1), imsi_type( litealigndata::Invalid_type ), m_lastnumber( 0 ), isreset( 0 ), m_percent( 100 ),
        m_rowsInBatch( 0 ), m_percentInBatch ( 100 ), m_rowIdx ( 0 )
    {

    }
    
    AcculateFunc( const AcculateFunc& rhs ): CBaseFunc( rhs ), m_prefix(rhs.m_prefix), m_lowRange( rhs.m_lowRange ), m_highRange( rhs.m_highRange ),
            m_init( rhs.m_init ), m_step( rhs.m_step ), m_duplicatedUnit( rhs.m_duplicatedUnit ), duplicatedUnit( rhs.duplicatedUnit ), imsi_type( rhs.imsi_type ), m_lastnumber( rhs.m_lastnumber ), isreset( rhs.isreset ), m_percent( rhs.m_percent ),
            m_rowsInBatch( rhs.m_rowsInBatch ), m_percentInBatch ( rhs.m_percentInBatch), m_rowIdx ( rhs.m_rowIdx )
    {

    }

    AcculateFunc& operator=( const AcculateFunc& rhs )
    {
        if ( &rhs == this )
            return *this;
    
        CBaseFunc::operator= (rhs);
        m_prefix=rhs.m_prefix;
        m_lowRange= rhs.m_lowRange; 
        m_highRange= rhs.m_highRange;
        m_init= rhs.m_init; 
        m_step= rhs.m_step; 
        m_duplicatedUnit= rhs.m_duplicatedUnit;
        duplicatedUnit= rhs.duplicatedUnit;
        imsi_type= rhs.imsi_type; 
        m_lastnumber= rhs.m_lastnumber; 
        isreset= rhs.isreset ;
        m_percent = rhs.m_percent;
        m_rowsInBatch = rhs.m_rowsInBatch;
        m_percentInBatch = rhs.m_percentInBatch;
        m_rowIdx = rhs.m_rowIdx;
        
        return *this;
    }

    virtual bool process();
    virtual bool Calculate();

    enum AcculateType
    {
        InvalidAcculate = 0,
        FileAcculate,
        AcculateMax
    };

    typedef std::vector<DependencyInfo> dependencylist;

    static const char* AcculateName[];

    void AddDepDimensionInfo( DependencyInfo info );
    
    ~AcculateFunc() {}
    AcculateType getType() const
    {
        return m_type;
    }
    uint64_t getinit()
    {
       return m_init;
    }
    uint32_t getstep()
    {
        return m_step;;
    }

private:

    bool anyzeDepDimensionInfo(
        const std::string& parameterName,
        const std::string& ParameterValue,
        CAlignDataObject*& pDimentsionFile,
        CAlignDataObject*& pDimensionColumn );
    bool FileAcculateCalculate();
    
    std::string m_prefix;
    uint64_t m_lowRange;
    uint64_t m_highRange;
    uint64_t m_init;
    uint32_t m_step;
    uint32_t m_duplicatedUnit;
    uint32_t duplicatedUnit;
    litealigndata::subIdType imsi_type;
    uint64_t m_lastnumber;
    uint8_t isreset;
    uint8_t m_percent;
    uint32_t m_rowsInBatch;
    uint8_t m_percentInBatch;
    uint32_t m_rowIdx;
    uint64_t m_mappingidx;  //it is the index to get the data from the dimension table.
    dependencylist m_DepDimensionList;
    AcculateType m_type;
};

class UniqueRandomFunc: public CBaseFunc
{
public:
    UniqueRandomFunc( litealigndata::funcTag tag ): CBaseFunc( tag ), m_lowRange( 0 ), m_highRange( MAX_UINT64 ),
        imsi_type( litealigndata::Invalid_type ), m_lastnumber( 0 ), isreset( 0 ), m_percent( 100 ),
        m_rowsInBatch( MAX_ROWS_IN_BATCH ), m_percentInBatch ( 100 ), m_rowIdx ( 0 )
    {

    }

    virtual bool process();
    virtual bool Calculate();
    ~UniqueRandomFunc() {};


private:
    static constexpr const uint32_t MAX_ROWS_IN_BATCH = 2000;
    static constexpr const uint8_t MAX_POOLS = 2;    
    uint64_t m_lowRange;
    uint64_t m_highRange;
    litealigndata::subIdType imsi_type;
    uint64_t m_lastnumber;
    uint8_t isreset;
    uint8_t m_percent;
    uint32_t m_rowsInBatch;
    uint8_t m_percentInBatch;
    uint32_t m_rowIdx;
    std::vector<uint64_t> uniqPool;
    std::vector<uint64_t> dupPool[MAX_POOLS];   
};

class MergeFunc: public CBaseFunc
{
public:
    typedef std::list<CBaseFunc* > litealigndataFuncList;
public:
    MergeFunc( litealigndata::funcTag tag ): CBaseFunc( tag )
    {
    }
    virtual bool process();

    virtual bool Calculate();

    
    litealigndataFuncList getFunclist()
    {
        return m_Funclist;
    }
    ~MergeFunc() {}

public:
    void addSubFunc( CBaseFunc* pFunc );

private:
    litealigndataFuncList m_Funclist;

private:
    void SetSubscriberUserPlane();
    bool addToString(std::string& toBeAdd, litealigndataFuncList::iterator itFuncToAdd);
    bool addToString(std::string& toBeAdd, CBaseFunc* FuncToAdd);

};

class RandomFunc: public CBaseFunc
{
public:
    RandomFunc( litealigndata::funcTag tag ): CBaseFunc( tag ), m_type( InvalidRandom ), m_lowRange( 0 ), m_highRange( MAX_UINT64 ), 
        m_mappingidx( 0xFF ), isDepDimensionInit( false ), total_num ( 0 ), randomArrIndex(0), fileIdx(0)
    {
        m_defaultValues.clear();
        memset( (char*)&m_keyMappingInfo, 0, sizeof( m_keyMappingInfo ) );
        memset( hit_num, 0, sizeof( hit_num ) );
        memset( randomArr, 0, sizeof( randomArr ) );
    }

    enum RandomType
    {
        InvalidRandom = 0,
        TableRandom,
        FileRandom,
        DefalutRandom,
        NatureRandom,
        MappingRandom,
        LessRandom,
        MoreRandom,
        PercentRandom,
        MultiFileRandom,
        PseudoRandom,
        RandomMax
    };

    typedef std::vector<DependencyInfo> dependencylist;

    static const char* RandomName[];

    static constexpr int RANDOM_ARR_SIZE = 100;

    virtual bool process();

    virtual bool Calculate();

    typedef std::vector<std::string> strlist;

    ~RandomFunc() {}

    RandomType getType() const
    {
        return m_type;
    }

    void setType( RandomType type )
    {
        m_type = type;
    }
    void setFileIndex(uint32_t fileIdx)
    {
        this->fileIdx = fileIdx;
    }
    uint32_t getFileIndex(void)
    {
        return fileIdx;
    }

    void AdddefaultValue( const char* defaultValue, uint32_t size );

    void AddDendencyInfo( DependencyInfo info );

    void AddDepDimensionInfo( DependencyInfo info );
    strlist getdefaultValues()
    {
        return m_defaultValues;
    }
    uint64_t getlowRange()
    {
        return m_lowRange;
    }
    uint64_t gethighRange()
    {
        return m_highRange;
    }

private:
    bool anyzeDepDimensionInfo(
        const std::string& parameterName,
        const std::string& ParameterValue,
        CAlignDataObject*& pDimentsionFile,
        CAlignDataObject*& pDimensionColumn );
    void parseDimensionInfo( const std::string& parameterValue, std::string& firstPara, DependencyInfo& info );    
    bool fillDimensionInfo( dependencylist& depDimensionList );

    bool SetValueForRandom( const baseType* value );
    bool NatureRandomCaculate();
    bool PseudoRandomCaculate();
    bool DefaulRandomCalculate();
    bool PercentRandomCalculate();
    bool TableRandomCalculate( RandomFunc::RandomType );
    bool FileRandomCalculate();
    bool MultiFileRandomCalculate();
    bool MappingRandomCalculate();
    void RandomArray(int32_t a[], int32_t n);

    RandomType m_type;

    uint64_t m_lowRange;

    uint64_t m_highRange;

    uint32_t m_percent;
        
    uint64_t m_mappingidx;  //it is the index to get the data from the dimension table.

    dependencylist m_dependencylist;

    dependencylist m_DepDimensionList;

    bool isDepDimensionInit;

    strlist m_defaultValues;     //is for default value.

    LoadDimensionInfo m_keyMappingInfo;

    uint64_t total_num;
    uint64_t hit_num[3];

    int32_t randomArr[RANDOM_ARR_SIZE];
    uint32_t randomArrIndex;
    uint32_t fileIdx;

    std::default_random_engine e;
};

class fileReaderFunc: public CBaseFunc
{
public:
    fileReaderFunc( litealigndata::funcTag tag ): CBaseFunc( tag )
    {
    }

    virtual bool process();

    virtual bool Calculate();

    ~fileReaderFunc() {}
};


class fixFunc: public CBaseFunc
{
public:
    fixFunc( litealigndata::funcTag tag ): CBaseFunc( tag )
    {
    }

    virtual bool Calculate();

    ~fixFunc();
};

class FuncFactory
{
public:
    static const FuncFactory& Instance()
    {
        static FuncFactory thefactory;
        return thefactory;
    }

    /***
     *
     */
    static CBaseFunc* createBaseFuncObject( litealigndata::funcTag tag );

    static CBaseFunc* createBaseFuncObject( CBaseFunc& func );

private:
    FuncFactory()
    {
    }

    FuncFactory( const FuncFactory& );

    FuncFactory& operator = ( const FuncFactory& );

    ~FuncFactory()
    {
    }
};

#endif /* CBaseFuncCLASS_H_ */

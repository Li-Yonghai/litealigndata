#ifndef _COLUMNOBJECT_H_
#define _COLUMNOBJECT_H_

#include "ace/Log_Msg.h"
#include "fun/BaseFuncClass.h"
#include "com/AlignDataTypeDef.h"
#include "struct/AlignDataObject.h"

class CTableObject;
class subColumnObject;

class CColumnObject : public CAlignDataObject
{
public:
    CColumnObject(){};
    typedef std::list<CAlignDataObject*> litealigndataObjlist;

    //typedef
    CColumnObject( litealigndata::elementTag tag );

    CColumnObject( const CColumnObject& rhs );

    CColumnObject& operator=( const CColumnObject& rhs );

    bool Initialize( DOMNode* pNode, CAlignDataObject* pParent );
    void Initialize();
    bool processItem();
    virtual bool processOjbect( CAlignDataObject* pRoot );

    ~CColumnObject();

    const CBaseFunc* getFunc() const
    {
        return m_pFunc;
    }

    CBaseFunc* getFunc()
    {
        return m_pFunc;
    }

    void setFunc( CBaseFunc* pFunc )
    {
        m_pFunc = pFunc;
    }

    void addSubColumn( CAlignDataObject* pSub )
    {
        m_subColumn.push_back( pSub );
    }

    void ProduceData( autoValue* pValue );

    const CTableObject* getTable() const
    {
        return m_pTable;
    }

    CTableObject* getTable()
    {
        return m_pTable;
    }

    void setTable( CTableObject* pTable )
    {
        m_pTable = pTable;
    }

    // uint32_t getOffset() const
    // {
    //     return m_offset;
    // }

    // void setOffset( uint16_t offset )
    // {
    //     m_offset = offset;
    // }

    const litealigndataObjlist& getSubColumn() const
    {
        return m_subColumn;
    }

    void setSubColumn( const litealigndataObjlist& subColumn )
    {
        m_subColumn = subColumn;
    }

    uint16_t getColidx() const
    {
        return m_colidx;
    }

    void setColidx( uint16_t colidx )
    {
        m_colidx = colidx;
    }

   std::string& getColFieldValue()
    {
        return m_colfieldValue;
    }


    litealigndata::autoType getType()
    {
        if ( m_pFunc != NULL )
            return m_pFunc->getValue().getType();

        return litealigndata::AUTO_Invalid;
    }
    
    uint32_t GetallLen()
    {
        return m_toalLen;
    };
    void SetallLen(uint32_t toalLen)
    {    
        m_toalLen = toalLen;
    }

    bool GetColumnData( int rownumber, autoValue& value );


    int getMetricsNum()
    {
        std::string strMetricsNum;
        if ( !findAttriValue( litealigndata::ATTR_MetricsNum, strMetricsNum ) )
            {    
            return false;
            }
        return atol( strMetricsNum.c_str() );
    }

    static const uint8_t sub_Ctrldata_stats_metric_flag_idx = 6;

protected:
    bool PreInitials( DOMNode* pNode, CAlignDataObject* pParent );

    virtual bool createData();

protected:

    CTableObject* m_pTable;

    CBaseFunc* m_pFunc;

    litealigndataObjlist m_subColumn;

    //uint32_t m_offset;

    uint16_t m_colidx;

    std::string m_colfieldValue;
    bool isLocalKey;
    uint32_t m_toalLen;

};

class subColumnObject : public CColumnObject
{
public:
    subColumnObject(){};
    subColumnObject( litealigndata::elementTag tag );
    bool Initialize( DOMNode* pNode, CAlignDataObject* pParent );

    virtual bool processOjbect( CAlignDataObject* pRoot );

public:
    virtual bool createData();

    ~subColumnObject();

    uint8_t getBitlen() const
    {
        return m_bitlen;
    }
    int setIsDelimiter( int val)
    {
        return isDelimiter = val;
    }

    int getIsDelimiter( ) 
    {
        return isDelimiter;
    }

private:
    uint8_t m_bitlen;
    int  isDelimiter;
};

class DimensionFileColumn : public CAlignDataObject
{
public:
    DimensionFileColumn( litealigndata::elementTag tag ) :
        CAlignDataObject( tag ), m_type( litealigndata::AUTO_Invalid ), m_colidx( 0 ), m_mappingidx( 0xFF )
    {
    }

    bool Initialize( DOMNode* pNode, CAlignDataObject* pParent );

    uint8_t getColidx() const
    {
        return m_colidx;
    }

    void setColidx( uint8_t colidx )
    {
        m_colidx = colidx;
    }

    litealigndata::autoType getType() const
    {
        return m_type;
    }

    void setType( litealigndata::autoType type )
    {
        m_type = type;
    }

    uint8_t getMappingidx() const
    {
        return m_mappingidx;
    }

    void setMappingidx( uint8_t mappingidx )
    {
        m_mappingidx = mappingidx;
    }

    static const char* GetColvaluestringforhash( baseType* value, char* strValue );

private:
    litealigndata::autoType m_type;
    uint8_t m_colidx;
    uint8_t m_mappingidx;   //it is used to storage the idx in the memory.
    CAlignDataTypeDef m_AlignDataTypeDef;

};
#endif /* _COLUMNOBJECT_H_ */

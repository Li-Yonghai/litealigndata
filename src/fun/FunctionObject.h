#ifndef _FUNCTIONOBJECT_H_
#define _FUNCTIONOBJECT_H_


#include <iostream>
#include <iomanip>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "com/Comm.h"
#include "fun/BaseFuncClass.h"
#include "sub/parser/XmlParser.h"
#include "boost/tokenizer.hpp"
#include "struct/AlignDataObject.h"
#include "struct/ParameterObject.h"


class FunctionObject: public CAlignDataObject
{
public:
    FunctionObject( litealigndata::elementTag tag ): CAlignDataObject( tag ), m_pFunc( NULL )
    {

    }

    virtual ~FunctionObject()
    {
        delete m_pFunc;
    }

    virtual bool Initialize( DOMNode* pNode, CAlignDataObject* pParent );

    virtual bool processOjbect( CAlignDataObject* root );

    litealigndata::funcTag getFuncTag() const
    {
        if ( m_pFunc != NULL )
            return m_pFunc->getTag();

        return litealigndata::FUNC_Invalid;
    }

    void setFuncTag( litealigndata::funcTag funcTag )
    {
        if ( m_pFunc != NULL )
            return m_pFunc->setTag( funcTag );
    }

    litealigndata::autoType getType() const
    {
        if ( m_pFunc != NULL )
            return m_pFunc->getValue().getType();

        return litealigndata::AUTO_Invalid;
    }

    const autoValue& getValue() const
    {
        if ( m_pFunc != NULL )
            return m_pFunc->getValue();
    }

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

protected:
    CBaseFunc* m_pFunc;
    CAlignDataTypeDef m_AlignDataTypeDef;




};

class RedefineRowFunc: public CBaseFunc
{
public:
    RedefineRowFunc( litealigndata::funcTag tag ): CBaseFunc( tag ), m_currentFileNum( 0 )
    {
    }


    virtual bool process();
    virtual bool Calculate();

    uint8_t getCurrentFileNum() const
    {
        return m_currentFileNum;
    }

    void setCurrentFileNum( uint8_t currentFileNum )
    {
        m_currentFileNum = currentFileNum;
    }

    uint64_t getFilenumber() const
    {
        return m_filenumber;
    }

    void setFilenumber( uint64_t filenumber )
    {
        m_filenumber = filenumber;
    }

    typedef std::vector<uint32_t> percetnlist;
    typedef std::vector<DependencyInfo> dependencylist;

private:
    uint64_t m_iTotoalNum;           //all the records to be created calculated total files.
    uint8_t m_sourcetype;
    uint64_t m_filenumber;           //the number of the file to be created.
    percetnlist m_listPercent;

    dependencylist m_DepDimensionList;  //point to dimensionFile

    uint32_t m_currentFileNum;      //current fileNumber

    uint32_t m_leftRecordsnumber;        //how much records left.

    void calculateRowFordefault();
    void CalculateRowNumber( const CXmlParser::DimensionfileDatas* pDimensionfileData, DimensionFileColumn* pColumn );
    void CalculateOutofRownumber();
    void CalulateRowbaseOnPercent( uint64_t percent );
};
#endif /* FUNCTIONOBJECT_H_ */

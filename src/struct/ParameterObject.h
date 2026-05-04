#ifndef _PARAMETEROBJECT_H_
#define _PARAMETEROBJECT_H_

#include "fun/BaseFuncClass.h"
#include "struct/TableObject.h"
#include "struct/AlignDataObject.h"


class CParameterObject : public CAlignDataObject
{
public:
    CParameterObject( litealigndata::elementTag tag );
    CParameterObject( const CParameterObject& rhs );
    CParameterObject& operator=( const CParameterObject& rhs );
    void Initialize();
    virtual bool Initialize( DOMNode* pNode, CAlignDataObject* pParent );

    virtual bool processOjbect( CAlignDataObject* pRoot );
    ~CParameterObject();

    litealigndata::parameterTag getTag() const
    {
        return m_tag;
    }

    void setTag( litealigndata::parameterTag tag )
    {
        this->m_tag = tag;
    }

private:
    CTableObject* m_pTable;
    CBaseFunc* m_pFunc;

    litealigndata::parameterTag m_tag;
    CAlignDataTypeDef m_AlignDataTypeDef;
};

#endif /* m_PARAMETEROBJECT_H_ */

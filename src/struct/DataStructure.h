#ifndef _DATASTRUCT_H_
#define _DATASTRUCT_H_
#include "struct/AlignDataObject.h"

class CTableObject;

class CStruct  : public CAlignDataObject
{
    public:
        CStruct(litealigndata::elementTag tag);
        ~CStruct();
        bool Initialize( DOMNode* pNode, CAlignDataObject* pParent );
        virtual bool processOjbect( CAlignDataObject* pRoot )
        {
            return true;
        };
        uint32_t GetallLen()
        {
            return m_toalLen;
        };
        void SetallLen(uint32_t toalLen)
        {
            m_toalLen = toalLen;
        }
        bool GetusedFlag()
        {
            return m_usedFlag;
        };
        void SetusedFlag(bool usedFlag)
        {
            m_usedFlag = usedFlag;
        };

        virtual bool createData()
        {
            return true;
        };
    private:
        bool m_usedFlag;
        uint32_t m_toalLen;
        

};
class CUnion  : public CAlignDataObject
{
    public:
        CUnion(litealigndata::elementTag tag);
        ~CUnion();
        bool Initialize( DOMNode* pNode, CAlignDataObject* pParent );
        virtual bool processOjbect( CAlignDataObject* pRoot )
        {
            return true;
        };
        uint32_t GetallLen()
        {
            return m_toalLen;
        };
        void SetallLen(uint32_t toalLen)
        {
            m_toalLen = toalLen;
        };

        bool GetusedFlag()
        {
            return m_usedFlag;
        };
        void SetusedFlag(bool usedFlag)
        {
            m_usedFlag = usedFlag;
        };
        virtual bool createData()
        {
            return true;
        };
    private:
        bool m_usedFlag;
        uint32_t m_toalLen;
};
class CHead  : public CAlignDataObject
{
    public:
        CHead(litealigndata::elementTag tag);
        ~CHead();
        bool Initialize( DOMNode* pNode, CAlignDataObject* pParent );
        virtual bool processOjbect( CAlignDataObject* pRoot )
        {
            return true;
        };
        uint32_t GetallLen()
        {
            return m_toalLen;
        };
        void SetallLen(uint32_t toalLen)
        {    
            m_toalLen = toalLen;
        }
        uint32_t toalLen()
        {
            return m_toalLen;
        };
        virtual bool createData()
        {
            return true;
        };
    private:
        uint32_t m_toalLen;
};
class CBody  : public CAlignDataObject
{
    public:
        CBody(litealigndata::elementTag tag);
        ~CBody();
        bool Initialize( DOMNode* pNode, CAlignDataObject* pParent );
        virtual bool processOjbect( CAlignDataObject* pRoot )
        {
            return true;
        };
        uint32_t GetallLen()
        {
            return m_toalLen;
        };
        void SetallLen(uint32_t toalLen)
        {
            m_toalLen = toalLen;
        }
        uint32_t toallLen()
        {
            return m_toalLen;
        };
        virtual bool createData()
        {
            return true;
        };
    private:
        uint32_t m_toalLen;
};


#endif
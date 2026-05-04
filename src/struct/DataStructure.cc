#include "struct/DataStructure.h"
#include "sub/parser/XmlParser.h"

CStruct::CStruct(litealigndata::elementTag tag)
:CAlignDataObject( tag )
{
	m_usedFlag = false;
}
CStruct::~CStruct()
{

}
bool CStruct::Initialize( DOMNode* pNode, CAlignDataObject* pParent )
{
	if ( !CAlignDataObject::Initialize( pNode, pParent ) )
		return false;
	return true;
}
CUnion::CUnion(litealigndata::elementTag tag)
:CAlignDataObject( tag )
{
	m_usedFlag = false;
}
CUnion::~CUnion()
{

}
bool CUnion::Initialize( DOMNode* pNode, CAlignDataObject* pParent )
{

	if ( !CAlignDataObject::Initialize( pNode, pParent ) )
		return false;
	return true;
}
CHead::CHead(litealigndata::elementTag tag)
:CAlignDataObject( tag )
{

}
CHead::~CHead()
{

}
bool CHead::Initialize( DOMNode* pNode, CAlignDataObject* pParent )
{

	if ( !CAlignDataObject::Initialize( pNode, pParent ) )
		return false;

	return true;
}
CBody::CBody(litealigndata::elementTag tag)
:CAlignDataObject( tag )
{

}
CBody::~CBody()
{

}
bool CBody::Initialize( DOMNode* pNode, CAlignDataObject* pParent )
{

	if ( !CAlignDataObject::Initialize( pNode, pParent ) )
		return false;

	return true;
}
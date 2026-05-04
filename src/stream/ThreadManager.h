#ifndef _THREADMANAGER_H
#define _THREADMANAGER_H

#include <list>
#include "WorkThread.h"
#include "sub/parser/XmlParser.h"

class CWorkThread;

class CThreadManager
{
public:
    CThreadManager(unsigned short size = 8);
    virtual ~CThreadManager();
    bool Init(const char *pXmlFile, const char *outPutfile);
    void CreateConnection(int sock);
    void ExitSafety();
private:
    std::list<CWorkThread*> m_workthread_list;
    unsigned short m_size;
};

#endif

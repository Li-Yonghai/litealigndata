#ifndef _APP_CONTEXT_H_
#define _APP_CONTEXT_H_

#include <list>
#include <string>


struct ServiceInfo
{
    std::string name;
    unsigned short port;
    unsigned short thread_num;
};

class CAppContext
{
public:
    static CAppContext *GetInstance()
    {
        return m_instance;
    }
    std::list<ServiceInfo> GetServices() const;
    CAppContext()
    {
        ServiceInfo s = {"Server",  8008, 10};
        ServiceInfo s1 = {"Server1", 8081, 10};
        ServiceInfo s2 = {"Server2", 8002, 10};
        ServiceInfo s3 = {"Server3", 8003, 10};
        ServiceInfo s4 = {"Server4", 8004, 10};

        m_services.push_back(s);
        m_services.push_back(s1);
        m_services.push_back(s2);
        m_services.push_back(s3);
        m_services.push_back(s4);

    };
    ~CAppContext()
    {

    };
private:
    static CAppContext *m_instance;
    std::list<ServiceInfo> m_services;
};

#endif


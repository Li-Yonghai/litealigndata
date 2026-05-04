#ifndef _SERVER_H_
#define _SERVER_H_

#include <map>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include "AppContext.h"
#include "event2/event.h"
#include "ThreadManager.h"
#include "event2/listener.h"
#include "sub/parser/XmlParser.h"

class evconnlistener;
class WorkGroup;
struct event_base;
class CThreadManager;


class CServer
{
public:
    CServer();

    bool Init(const char *pXmlFile, const char *outPutfile);
    void Start();
    void Stop();
    void Dispatch();
    virtual ~CServer();
    std::thread* GetThread() const;
private:
    static void SConnListenerCb(struct evconnlistener *listen, evutil_socket_t sock, struct sockaddr *addr, int len, void *ctx);
    event_base *m_ev_base = nullptr;
    std::map<evconnlistener*, ServiceInfo> m_listener_map;
    std::map<std::string, CThreadManager*> m_thread_manager_map;
    int m_backlog = 0;
    std::thread *m_dispatch_thread = nullptr;
    bool m_server_quit = false;
};

#endif

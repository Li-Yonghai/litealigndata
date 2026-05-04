/*
 * =============================================================================
 *
 *       Filename:  Server.cc
 *
 *       Description: 
 *
 *       Version:  1.0
 *       Created:  04/01, 2024
 *       Revision:  V1
 *       Compiler:  g++
 *
 *       Author:  keen.lee 
 *       Company:  
 *
 * =============================================================================
 */
#include "Server.h"

using namespace std;

void CServer::SConnListenerCb(struct evconnlistener *listen, evutil_socket_t sock, struct sockaddr *addr, int len, void *ctx)
{
    // Parser clinet IP
    char ip[16] = {0};
    sockaddr_in *addr_in = (sockaddr_in *)addr;
    evutil_inet_ntop(AF_INET, &addr_in->sin_addr, ip, sizeof(ip));

    CServer *server = (CServer *)ctx;
    // different connection belongs different server
    auto it = server->m_listener_map.find(listen);
    if(it != server->m_listener_map.end())
    {
        ServiceInfo si = it->second;
        printf("[%s]vconnected [%s]success", ip, si.name.c_str());
        ACE_DEBUG((LM_INFO, "[%s]vconnected [%s]success\n", ip, si.name.c_str()));
        server->m_thread_manager_map[si.name]->CreateConnection(sock);
    }
    
}

CServer::CServer()
     : m_backlog(64)
{
}

CServer::~CServer()
{
    Stop();
}

thread *CServer::GetThread() const
{
    return m_dispatch_thread;
}

bool CServer::Init(const char *pXmlFile, const char *outPutfile)
{
    // created event Loop
    m_ev_base = event_base_new();
    if (!m_ev_base)
    {
        return false;
    }
    list<ServiceInfo> service_info_list = CAppContext::GetInstance()->GetServices();
    int size = service_info_list.size();

    for(auto it = service_info_list.begin(); it != service_info_list.end(); it++)
    {
        ServiceInfo si = *it;
        unsigned short port = si.port;
        sockaddr_in sin;
        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_port = htons(port);
        inet_aton("127.0.0.1",&sin.sin_addr);
        // created listener
        evconnlistener *listener = evconnlistener_new_bind(
            m_ev_base,
            SConnListenerCb,
            this,
            LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
            this->m_backlog,
            (sockaddr *)&sin,
            sizeof(sin));
        if (!listener)
        {
            return false;
        }
        CThreadManager *tm = new CThreadManager(si.thread_num);
        
        if(!tm->Init(pXmlFile, outPutfile))
        {
            return false;
        }
        m_thread_manager_map[si.name] = tm; //server name， ThreadManager
        m_listener_map[listener] = si;      //fd, server in struct
        printf("%s initialing thread total num%d port %d\n", si.name.c_str(), 
        si.thread_num, si.port);
        ACE_DEBUG((LM_INFO, "%s initialing thread total num%d port %d\n", \
                si.name.c_str(), si.thread_num, si.port));
        
    }
    printf("server initialed, wait client connecting...\n");
    ACE_DEBUG((LM_INFO, "server initialed, wait client connecting...\n"));
    return true;
}

void CServer::Start()
{
    if (!m_dispatch_thread)
    {
        m_server_quit = false;
        // start thread， reactor Loop
        m_dispatch_thread = new thread(&CServer::Dispatch, this);
    }
    ACE_DEBUG((LM_INFO, "sCServer::Start() success...\n"));
}

void CServer::Stop()
{
    if (m_dispatch_thread)
    {
        // Exit Event/Loop
        m_server_quit = true;
        m_dispatch_thread->join();
        delete m_dispatch_thread;
        m_dispatch_thread = nullptr;
    }

    for(auto it = m_thread_manager_map.begin(); it != m_thread_manager_map.end();)
    {
        CThreadManager* tm = it->second;
        tm->ExitSafety();
        m_thread_manager_map.erase(it);
        it++;
    }
}

void CServer::Dispatch()
{
    // Event Dispatch
    while (!m_server_quit)
    {
        event_base_loop(m_ev_base, EVLOOP_NONBLOCK);
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    for(auto it = m_listener_map.begin(); it != m_listener_map.end(); it++)
    {
        evconnlistener_free(it->first);
    }
    // close event_base
    event_base_free(m_ev_base);
    m_ev_base = nullptr;
}

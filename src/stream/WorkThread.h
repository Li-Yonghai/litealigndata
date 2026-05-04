#ifndef _WORKTHREAD_H
#define _WORKTHREAD_H

#include <list>
#include <mutex>
#include <thread>
#include <chrono>
#include <cstring>
#include <unistd.h>
#include "event2/event.h"
#include "sub/parser/XmlParser.h"
#include "event2/bufferevent.h"

class CAcceptor;

class CWorkThread
{
public:
    CWorkThread(unsigned short wid, const char *pXmlFile, const char *outPutfile);
    virtual ~CWorkThread();
    bool Init();
    void Start();
    void Stop();
    void Notify(const char *sign);
    void AddSocket(int fd);
    void Disconnection(CAcceptor *ac);
    int Size() const;
protected:
    virtual void Run();
private:
    static void EventCB(evutil_socket_t fd, short what, void* arg);
    unsigned short m_wid;
    void Activated(int fd);
    struct event_base *m_ev_base = nullptr;
    struct event* m_pipe_event = nullptr;
    bool m_is_stop = false;
    std::thread *m_pthread;
    std::list<CAcceptor*> m_acceptor_list;
    int m_pipefd[2]; // m_pipefd[0]read , m_pipefd[1]write
    std::mutex m_mtx;
    std::list<int> m_fd_list;
    const char *m_pXmlFile;
    const char *m_outPutfile;
};

#endif

#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include <string.h>
#include <unistd.h>
#include <iostream>
#include "WorkThread.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "sub/parser/XmlParser.h"
#include "event2/bufferevent.h"


class CXmlParser;
class CWorkThread;

class CAcceptor
{
public:
    CAcceptor(CWorkThread* work_thread, event_base* base, int fd, const  char *pXmlFile,const char *outPutfile, unsigned short heartbeat = 1);
    bool Init();
    ~CAcceptor(){};
    int Getfd()
    {
        return m_socket;
    }
    
private:
    static void ReadCB(bufferevent *buffev, void *arg);
    static void WriteCB(bufferevent *buffev, void *arg);
    static void EventCB(bufferevent *buffev, short what, void *arg);
    static bufferevent_filter_result FilterIn(evbuffer *src, evbuffer *dst, ev_ssize_t dst_limit,
                         enum bufferevent_flush_mode mode, void *ctx);
    static bufferevent_filter_result FilterOut(evbuffer *src, evbuffer *dst, ev_ssize_t dst_limit,
                          enum bufferevent_flush_mode mode, void *ctx);
    CWorkThread* m_work_thread = nullptr;
    struct event_base *m_ev_base = nullptr;
    int m_socket = 0;
    const char *m_pXmlFile;
    const char *m_outPutfile;
    unsigned short m_heartbeat;

};

#endif

#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include <string.h>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include "event2/event.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/listener.h"
#include "event2/bufferevent.h"

class  CConnector
{
public:
     CConnector();
    ~CConnector();
    bool Init(const char* host, unsigned short port);
    
private:
    static void ReadCB(struct bufferevent *bev, void *arg);
    static void EventCB(struct bufferevent *bev, short what, void *arg);
    static void read_msg_cb(int fd, short events, void* arg);
    struct event_base* base_;
    char* request_buf_;
    int request_buf_size_;
    char* m_buf;
    int m_sock;

};

#endif //_CONNECTOR_H_

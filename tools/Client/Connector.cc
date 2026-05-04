/*
 * =============================================================================
 *
 *       Filename:  Connector.cc
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

#include "Connector.h"

using namespace std;

//volatile bool m_stop = false;

CConnector::CConnector()
{

    m_buf = (char*)malloc(10240);
}

CConnector::~CConnector()
{
    if(m_buf)
        free(m_buf);
}
//终端输入回调
void  CConnector::read_msg_cb(int fd, short events, void* arg) {
    char msg[1024];
 
    int ret = read(fd, msg, sizeof(msg));
    if( ret < 0 )
    {
        perror("read fail ");
        exit(1);
    }
    //puts(msg);
    char* ptr = strstr(msg, "\n");
    *ptr = '\0';
    //得到bufferevent指针,目的是为了写到bufferevent的写缓冲区
    struct bufferevent* bev = (struct bufferevent*)arg;
 
    //把终端的消息发送给服务器端
    bufferevent_write(bev, msg, ret);
}

bool  CConnector::Init(const char *host, unsigned short port)
{
    base_ = event_base_new();
    if(!base_)
    {
        printf("连接创建失败\n");
        return false;
    }

    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    bufferevent *bev = bufferevent_socket_new(base_, m_sock, BEV_OPT_CLOSE_ON_FREE);

    struct event* ev_cmd = event_new(base_, STDIN_FILENO,   
                                    EV_READ | EV_PERSIST,     
                                    read_msg_cb, (void*)bev);
    
    event_add(ev_cmd, NULL);

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    inet_pton(AF_INET, host, &sin.sin_addr.s_addr);
    int ret = bufferevent_socket_connect(bev, (struct sockaddr*)&sin, sizeof(sin));
    if(ret != 0)
    {
        printf("socket创建失败\n");
        return false;
    }
    bufferevent_setcb(bev, ReadCB, nullptr, EventCB, this);
    bufferevent_enable(bev, EV_READ | EV_PERSIST | EV_WRITE);
    bufferevent_setwatermark(bev, EV_WRITE | EV_READ | EV_PERSIST, 1, 1024); 
    //bufferevent_set_max_single_read(bev, 1);
   // bufferevent_setwatermark(bev, EV_WRITE, 1024, 1024); 
    event_base_dispatch(base_);;
    event_free(ev_cmd);
    bufferevent_free(bev);
    event_base_free(base_);
    //m_stop = true;
    return true;
}

void  CConnector::ReadCB(bufferevent *bev, void *arg)
{
    //puts("enter CConnector::ReadCB\n");
    evbuffer *input = bufferevent_get_input(bev);
    int len =  evbuffer_get_length(input);
    CConnector *h = (CConnector*)arg;
    memset(h->m_buf, 0, 10240);
    bufferevent_flush(bev, EV_READ, BEV_FLUSH);
    bufferevent_read(bev, h->m_buf, len);
    bufferevent_flush(bev, EV_READ, BEV_FLUSH);
    printf("%s\n", h->m_buf);
    //puts("after  CConnector::ReadCB\n");
}

void  CConnector::EventCB(bufferevent *bev, short what, void *arg)
{
    printf("Connector::EventCB\n");
    
     CConnector *h = (CConnector*)arg;

    if(what == BEV_EVENT_CONNECTED)
    {
        printf("the client has connected to server\n");
       // bufferevent_write(bev, "yes", strlen("yes"));
    }
    if(what == (BEV_EVENT_EOF |  BEV_EVENT_READING))
    {
        //close(h->m_sock);
        // m_stop = true;
    }
}

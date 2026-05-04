/*
 * =============================================================================
 *
 *       Filename:  Acceptor.cc
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

#include "Acceptor.h"


using namespace std;

CAcceptor::CAcceptor(CWorkThread* work_thread, struct event_base* base, int fd, const char *pXmlFile, const char *outPutfile, unsigned short heartbeat)
    : m_work_thread(work_thread),
      m_ev_base(base),
      m_socket(fd),
      m_pXmlFile(pXmlFile),
      m_outPutfile(outPutfile),
      m_heartbeat(heartbeat)
{

}

bool CAcceptor::Init()
{
    bufferevent *bev = nullptr;
    bev = bufferevent_socket_new(m_ev_base, m_socket, BEV_OPT_CLOSE_ON_FREE);
    
    if (!bev)
    {
        return false;
    }
    bufferevent* bev_filter = bufferevent_filter_new(bev,
                                                     NULL,
                                                     NULL,
                                                     BEV_OPT_CLOSE_ON_FREE,
                                                     0,
                                                     0);
    
    bufferevent_setcb(bev_filter, ReadCB, NULL, EventCB, this);
    bufferevent_enable(bev_filter, EV_READ | EV_WRITE);
    bufferevent_setwatermark(bev_filter, EV_WRITE | EV_READ | EV_PERSIST, 1, 1024); 
    //bufferevent_set_max_single_write(bev, 1);
    timeval tv = {m_heartbeat, 0};
    //bufferevent_set_timeouts(bev_filter, &tv, NULL);
    return true;
}

void CAcceptor::ReadCB(bufferevent *buffev, void *arg)
{
    CAcceptor *h = (CAcceptor*)arg;
    char buffer[1024] = {0};
    int n = bufferevent_read(buffev, buffer, sizeof(buffer));
    if(strncmp(buffer, "get", strlen("get")+1))
    {
        bufferevent_write(buffev, "enter invalid, please contact admin\n", 
            strlen("enter invalid, please contact admin\n"));
        bufferevent_flush(buffev, EV_WRITE, BEV_FLUSH);
       //h->m_work_thread->Disconnection(h);
    }
    else
    {
		CXmlParser* parser = new CXmlParser(h->m_pXmlFile, h->m_outPutfile);
		if((access(h->m_pXmlFile,F_OK)) == -1)           
		{                                                  
			//ACE_DEBUG((LM_ERROR, "the file: %s is not exist.\n\n", xmlfile.c_str()));                                                                             
			return;                                     
		}  
			
		if ( !parser->Initialize() )
		{
			//ACE_DEBUG((LM_ERROR, "Failed to initialize Xml Parser, exit!\n"));
			return ;
		}
		parser->ProduceData(buffev);
    }

}

void CAcceptor::WriteCB(bufferevent *buffev, void *arg)
{
}

void CAcceptor::EventCB(bufferevent *buffev, short what, void *arg)
{
    CAcceptor *h = (CAcceptor*)arg;
    if (what & BEV_EVENT_TIMEOUT)
    {
        h->m_work_thread->Disconnection(h);
    }
    if (what & BEV_EVENT_EOF)
    {
        h->m_work_thread->Disconnection(h);
    }
    // 用户断开连接，释放服务端资源
    if (what & BEV_EVENT_ERROR)
    {
        //close(h->socket_);
        h->m_work_thread->Disconnection(h);
    }

}

bufferevent_filter_result CAcceptor::FilterIn(evbuffer *src, evbuffer *dst, ssize_t dst_limit, bufferevent_flush_mode mode, void *ctx)
{
    int len =  evbuffer_get_length(src);
    char *src_buf = new char[len];
    evbuffer_remove(src, src_buf, len);
    evbuffer_add(dst, src_buf, len);
    delete[] src_buf;
    return BEV_OK;
}

bufferevent_filter_result CAcceptor::FilterOut(evbuffer *src, evbuffer *dst, ssize_t dst_limit, bufferevent_flush_mode mode, void *ctx)
{
    int len =  evbuffer_get_length(src);
    char *src_buf = new char[len];
    evbuffer_remove(src, src_buf, len);
    evbuffer_add(dst, src_buf, len);
    delete[] src_buf;
    return BEV_OK;
}

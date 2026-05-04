/*
 * =============================================================================
 *
 *       Filename:  WorkThread.cc
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

#include "WorkThread.h"
#include "stream/Acceptor.h"

using namespace std;

CWorkThread::CWorkThread(unsigned short wid, const char *pXmlFile, const char *outPutfile)
    :m_wid(wid),
    m_pXmlFile(pXmlFile),
    m_outPutfile(outPutfile)
{

}

CWorkThread::~CWorkThread()
{
    if (m_pipe_event)
    {
        event_del(m_pipe_event);
        event_free(m_pipe_event);
        m_pipe_event = nullptr;
    }
    while(!m_acceptor_list.empty())
    {
        CAcceptor* ac = m_acceptor_list.front();
        m_acceptor_list.pop_front();
        delete ac;
    }
}

bool CWorkThread::Init()
{
    if (pipe(m_pipefd) == -1)
    {
       printf("\ncreated pip failed\n");
       ACE_DEBUG((LM_ERROR, "\ncreated pip failed\n"));
       return false;
    }
    event_config *ev_conf = event_config_new();
    event_config_set_flag(ev_conf, EVENT_BASE_FLAG_NOLOCK);
    this->m_ev_base = event_base_new_with_config(ev_conf);
    event_config_free(ev_conf);
    if (!m_ev_base)
    {
        printf("\nthread initialed failed\n");
        ACE_DEBUG((LM_ERROR, "\nthread initialed failed\n", m_wid));
        return false;
    }
    m_pipe_event = event_new(this->m_ev_base, m_pipefd[0], EV_READ | EV_PERSIST, EventCB, this);
    event_add(m_pipe_event, 0);
    return true;
}

void CWorkThread::Start()
{
    m_is_stop = false;
    m_pthread = new thread(&CWorkThread::Run, this);
}

void CWorkThread::Stop()
{
    m_is_stop = true;
    if(m_pthread)
    {
        m_pthread->join();
        delete m_pthread;
        m_pthread = nullptr;
    }
    printf("thread <%i> exit", m_wid);
    ACE_DEBUG((LM_INFO, "\nthread <%i> exit\n", m_wid));
}

void CWorkThread::Notify(const char *sign)
{
    write(this->m_pipefd[1], sign, strlen(sign));
}

void CWorkThread::AddSocket(int fd)
{
    std::lock_guard<std::mutex> gm(m_mtx);
    m_fd_list.push_back(fd);
}

void CWorkThread::Disconnection(CAcceptor *ac)
{
    printf("\nclient conncted closed...\n");
    ACE_DEBUG((LM_INFO, "\nclient conncted closed...\n"));
    fflush(stdout);
    std::lock_guard<std::mutex> gm(m_mtx);
    m_acceptor_list.remove(ac);
    close(ac->Getfd());
    delete ac;
}

int CWorkThread::Size() const
{
    return m_acceptor_list.size();
}

void CWorkThread::Run()
{
    while (!m_is_stop)
    {
        event_base_loop(this->m_ev_base, EVLOOP_NONBLOCK);
        this_thread::sleep_for(chrono::milliseconds(1));
    }
    event_base_free(this->m_ev_base);
}

void CWorkThread::EventCB(int fd, short what, void *arg)
{
    printf("\nthere is a thread connected\n");
    ACE_DEBUG((LM_INFO, "\nthere is a thread connected\n"));
    CWorkThread *work = (CWorkThread *)arg;
    if(what == EV_READ)
    {
        
        work->Activated(fd);
    }
}

void CWorkThread::Activated(int fd)
{
    char buf[10] = {0};
    read(fd, buf, 9);
    std::lock_guard<std::mutex> gm(m_mtx);
    if (strncmp(buf, "connected", strlen("connected")) == 0)
    {
        if(!m_fd_list.empty())
        {
            int fd = m_fd_list.front();
            m_fd_list.pop_front();
            CAcceptor *ac = new CAcceptor(this, m_ev_base, fd, m_pXmlFile, m_outPutfile);
            if(ac->Init())
            {
                m_acceptor_list.push_back(ac);
            }
        }
    }
    else
    {
        printf("\ninvalid instruction, not care...\n");
        ACE_DEBUG((LM_ERROR, "\ninvalid instruction, not care...\n"));
    }
}

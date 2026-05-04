/*
 * =============================================================================
 *
 *       Filename:  ThreadManager.cc
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

#include "ThreadManager.h"

using namespace std;

CThreadManager::CThreadManager(unsigned short size)
    : m_size(size)
{

}

CThreadManager::~CThreadManager()
{
}

bool CThreadManager::Init(const char *pXmlFile, const char *outPutfile)
{
    
    for(int i = 0; i < m_size; i++)
    {
        CWorkThread *wt = new CWorkThread(i,pXmlFile, outPutfile);
        if(!wt->Init())
        {
            return false;
        }
        wt->Start();
        m_workthread_list.push_back(wt);
    }
    return true;
}
void CThreadManager::CreateConnection(int sock)
{
    CWorkThread *wt = nullptr;
    auto iter = m_workthread_list.begin();
    wt = *iter;
    int min = (*iter)->Size();

    //pick 当前线程管理者名下：连接客户数（m_acceptor_list）最少的线程，fd 存入list，利用管道通知相关线程，
    //触发管道读callback，m_acceptor_list<--1:1--->客户端
    while(iter != m_workthread_list.end())
    {
        if(min > (*iter)->Size())
        {
            min = (*iter)->Size();
            wt = *iter;
        }
        iter++;
    }
    wt->AddSocket(sock);
    wt->Notify("connected");
}

void CThreadManager::ExitSafety()
{
    while (!m_workthread_list.empty())
    {
        CWorkThread *wt = m_workthread_list.front();
        m_workthread_list.pop_front();
        wt->Stop();
        delete wt;
    }
}
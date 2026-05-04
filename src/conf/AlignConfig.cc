/*
 * =============================================================================
 *
 *       Filename:  AlignConfig.cc
 *
 *       Description: 
 *
 *       Version:  1.0
 *       Created:  04/01, 2024
 *       Revision:  V1
 *       Compiler:  g++
 *
 *       Author:  keen.lee 
 *       Company
 *
 * =============================================================================
 */
#include "conf/AlignConfig.h" 

CAlignConfig* CAlignConfig::m_pInstance = 0;
CAlignConfig* CAlignConfig::GetInstance()
{                                                                    
    if (!m_pInstance)                                                
    {   
        m_pInstance = new CAlignConfig;
    }   
    return m_pInstance;
}
bool CAlignConfig::RunCmd(char *cmd)
{
    pid_t pid;
    char argv1[] = "sh";
    char argv2[] = "-c";
    char* const argv[] = {argv1, argv2, cmd, NULL};
    int status = posix_spawn(&pid, "/bin/sh", NULL, NULL, argv, environ);
    if (0 == status)
    {
        if(waitpid(pid, &status, 0) == -1)
        {
            ACE_DEBUG((LM_ERROR, "posix_spawn exited with status: %d\n", status));
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        ACE_DEBUG((LM_ERROR, "posix_spawn error: %s\n", strerror(status)));
        return false;
    }
}
void CAlignConfig::ReleaseInstance()   
{       
    if (m_pInstance)                                                 
    {   
        delete m_pInstance;
        m_pInstance = NULL;
    }   
}

CAlignConfig::~CAlignConfig()
{


}

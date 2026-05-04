/*
 * =============================================================================
 *
 *       Filename:  LogHandler.cc
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
#include <iostream>
#include "ace/Log_Msg.h"
#include "ace/Log_Record.h"
#include "log/LogHandler.h"
#include "conf/AlignConfig.h"
#include "com/AlignDataUtils.h"
#include "com/AlignDataTypeDef.h"

CLogHandler* CLogHandler::m_pInstance = 0;

CLogHandler* CLogHandler::GetInstance()
{
    if (!m_pInstance)
    {
        m_pInstance = new CLogHandler;
    }
    return m_pInstance;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// FUNCTION: CLogHandler::ReleaseInstance()
// NOTES   : 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CLogHandler::ReleaseInstance()        
{
	if (m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// FUNCTION: CLogHandler::LogHandler()
// NOTES   : 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CLogHandler::CLogHandler(void)
:m_ostream(0),m_flags(ACE_Log_Msg::OSTREAM),m_strStatLogName("")
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// FUNCTION: CLogHandler::~LogHandler()
// NOTES   : 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CLogHandler::~CLogHandler(void)
{
	CleanupOstream ();
}
bool CLogHandler::CreatelitealigndataPath()
{
    ACE_DEBUG((LM_INFO, "directory %s is destion which generation by litealigndata.\n", \
            (CAlignConfig::GetInstance()->GetOutputPath()).c_str()));
    DIR* dirptr = opendir((CAlignConfig::GetInstance()->GetOutputPath()).c_str() );
    if ( dirptr == NULL )
    {
        ACE_DEBUG((LM_INFO, "%s directory is NOT exist! Create it firstly.\n", \
                (CAlignConfig::GetInstance()->GetOutputPath()).c_str()));	
        std::string cmd = "mkdir -p " + CAlignConfig::GetInstance()->GetOutputPath();
        if ( system( cmd.c_str() ) )
        {
            ACE_DEBUG((LM_ERROR, "%s directory failed to create.\n",  \
                    (CAlignConfig::GetInstance()->GetOutputPath()).c_str()));
            litealigndata_destory();
            return false;
        }
    }
    closedir( dirptr );
    return true;
};
bool CLogHandler::Init(bool loglevel)
{
    char *ptr;
    static char levlArry[][128] = {"LM_INFO", "LM_WARNING | LM_INFO", "LM_DEBUG | LM_WARNING | LM_INFO", 
                         "LM_ERROR | LM_DEBUG | LM_WARNING | LM_INFO", 
                         "LM_CRITICAL | LM_ERROR | LM_DEBUG | LM_WARNING | LM_INFO",
                         ""};
    
    uint32_t  debug_level = LM_SHUTDOWN;
    switch(CAlignConfig::GetInstance()->GetDebugLevel())
    {
        case litealigndata::LOG_LEVEL_None:
        case litealigndata::LOG_LEVEL_Low:
            ptr = levlArry[0];
            debug_level = LM_INFO;
        break;
        case litealigndata::LOG_LEVEL_Medium:
            ptr = levlArry[1];
            debug_level = LM_WARNING | LM_INFO;
        break;
        case litealigndata::LOG_LEVEL_High:
            ptr = levlArry[2];
            debug_level = LM_DEBUG | LM_WARNING | LM_INFO;
        break;
        case litealigndata::LOG_LEVEL_Detail_High:
            ptr = levlArry[3];
            debug_level = LM_ERROR | LM_DEBUG | LM_WARNING | LM_INFO;
        break;
        case litealigndata::LOG_LEVEL_Very_Detail_High:
            ptr = levlArry[4];
            debug_level = LM_CRITICAL | LM_ERROR | LM_DEBUG | LM_WARNING | LM_INFO;
        break;
        default:
            ptr = levlArry[5];
            //debug_level = LM_SHUTDOWN;
            break;
    }

	if(CAlignConfig::GetInstance()->GetBVerbose())
    {
        ACE_SET_BITS (m_flags, ACE_Log_Msg::STDERR);
    }    

	std::string strLog("");
    if(!SetLogFile(strLog))
    {
        return false;
    }
    litealigndata_ACE_LOG_MSG_INIT();
    if(loglevel)
    {
        ACE_LOG_MSG->priority_mask( LM_DEBUG, ACE_Log_Msg::PROCESS );
        ACE_DEBUG((LM_DEBUG, "Set LM_DEBUG to LOG_LEVEL\n"));
    }
    else
    {
        ACE_LOG_MSG->priority_mask( debug_level, ACE_Log_Msg::PROCESS );
        ACE_DEBUG((LM_INFO, "Set \"%s\" to LOG_LEVEL\n", ptr));
    }
    ACE_DEBUG((LM_INFO, "CLogHandler::Init success..............\n"));
//	printf("Please see detail info in log file %s\n", strLog.c_str());
//    fflush(stdout);
    
    return true;
}

bool CLogHandler::SetLogFile(std::string& strLog)
{
    DIR* dirptr = opendir((CAlignConfig::GetInstance()->Getlogpath()).c_str());

    if(NULL == dirptr)
    {
        //printf("%s directory is NOT exist! Create it firstly.\n",
        //            (CAlignConfig::GetInstance()->Getlogpath()).c_str());
        std::string cmd = "mkdir -p " + CAlignConfig::GetInstance()->Getlogpath();
        if (!CAlignConfig::GetInstance()->RunCmd( (char*)cmd.c_str()))
        {
            printf("%s directory failed to create.\n", 
                    (CAlignConfig::GetInstance()->Getlogpath()).c_str());
            return false;
        }
    }
    closedir(dirptr);



	std::string strXmlFile = CAlignConfig::GetInstance()->GetXmlFile();

    auto n = strXmlFile.rfind('/');
    std::string templateName = strXmlFile.substr(n+1);
    auto x = templateName.rfind('.');
    std::string strTrafficName = templateName.substr(0,x);
	
    time_t cur_time;
    time( &cur_time );
    tm time = *gmtime(&cur_time);

    char curLog[256] = {0};
    snprintf( curLog, 256, "%ssys_%02d-%02d-%d_%s_pid%d.log",
     (CAlignConfig::GetInstance()->Getlogpath()).c_str(), 
        time.tm_mon +1, time.tm_mday, time.tm_year + 1900, strTrafficName.c_str(), getpid());
    m_strStatLogName = strLog = curLog;
    ACE_LOG_MSG->set_flags (ACE_Log_Msg::OSTREAM);
    ACE_OSTREAM_TYPE *outStream = new std::ofstream(curLog, std::ios::out | std::ios::app);
    ACE_LOG_MSG->msg_ostream (outStream, 1);

    return true;
}

void CLogHandler::CleanupOstream ()
{
	if(0 != m_ostream)
	{
		((std::ofstream*)m_ostream)->close();
		delete m_ostream;
		m_ostream = 0;
	}
}

void CLogHandler::SetMsgOstream (ACE_OSTREAM_TYPE *m)
{
	if (m_ostream == m)
    {
		return;
    }

	CleanupOstream ();
    m_ostream = m;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// FUNCTION: CLogHandler::GetMsgOstream()
///NOTES   : Get the ostream that is used to print error messages.
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ACE_OSTREAM_TYPE* CLogHandler::GetMsgOstream( void ) 
{
	if(!m_ostream)
	{
        std::string strLog("");
		SetLogFile(strLog);
	}
	return m_ostream;
}
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// FUNCTION: CLogHandler::log(ACE_Log_Record &log_record)
// NOTES   : 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CLogHandler::log(ACE_Log_Record &log_record)
{
	// Make sure that the lock is held during all this.
	std::lock_guard<std::mutex> lock( m_ostreamLock );

	const ACE_TCHAR* local_host = 0;
	unsigned long msg_severity = log_record.type();
    //ACE_Log_Priority prio = ACE_static_cast(ACE_Log_Priority, msg_severity);
    ACE_Log_Priority prio = (ACE_Log_Priority)msg_severity;
	
	if ((ACE_BIT_ENABLED (m_flags, ACE_Log_Msg::STDERR)) 
		|| (prio == LM_CRITICAL))
	{    
		log_record.print (local_host, m_flags, stderr);
	}
	
	// This must come last, after the other two print operations
	// (see the <ACE_Log_Record::print> method for details).
	if (ACE_BIT_ENABLED (m_flags,
                         ACE_Log_Msg::OSTREAM)
						&& m_ostream != 0)
	{				
		log_record.print (local_host,
                          m_flags,
                          *m_ostream
                          );
	}
}

void CLogHandler::LockOstream()
{
	m_ostreamLock.lock();
}
void CLogHandler::UnlockOstream()
{
	m_ostreamLock.unlock();
}

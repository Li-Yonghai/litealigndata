#ifndef __LOGHANDLER_H_
#define __LOGHANDLER_H_

//-----------------------------------------------------------------------------
/*
;
; FILE NAME:
;   CLogHandler.h
;
; FUNCTION:
;   Write log message.
;
*/
//-----------------------------------------------------------------------------

#include <mutex>
#include <spawn.h>
#include <fstream>
#include <dirent.h>
#include <iostream>
#include <sys/types.h>
#include "ace/OS.h"
#include "ace/ACE.h"
#include "com/Comm.h"
#include "ace/OS_main.h"
#include "ace/SString.h"
#include "ace/Log_Msg.h"
#include "ace/Log_Priority.h"
#include "ace/Log_Msg_Callback.h"

# define litealigndata_ACE_LOG_MSG_INIT() \
    { \
        ACE_LOG_MSG->clr_flags (ACE_Log_Msg::STDERR); \
        ACE_LOG_MSG->set_flags (ACE_Log_Msg::MSG_CALLBACK); \
        ACE_LOG_MSG->msg_callback (CLogHandler::GetInstance()); \
    } \

#define litealigndata_destory()  \
{\
	ACE_LOG_MSG->clr_flags(ACE_Log_Msg::OSTREAM);\
	CLogHandler::GetInstance()->ReleaseInstance();\
	CAlignConfig::GetInstance()->ReleaseInstance();\
	ACE_DEBUG((LM_INFO, "litealigndata_destory success..............\n"));\
}\
	
# define LOG_MSG_BLOCK(fun) \
    { \
        CLogHandler* pInstance_ = CLogHandler::GetInstance(); \
        pInstance_->LockOstream();\
        fun(*pInstance_->GetMsgOstream()); \
        pInstance_->UnlockOstream();\
    } \

# define LOG_MSG_OBJ_BLOCK(fun, obj) \
    { \
        CLogHandler* pInstance_ = CLogHandler::GetInstance(); \
        pInstance_->LockOstream();\
        fun(*pInstance_->GetMsgOstream(), obj); \
        pInstance_->UnlockOstream();\
    } \

class CLogHandler : public ACE_Log_Msg_Callback
{
public:
    static CLogHandler*  GetInstance();
    void ReleaseInstance();
	~CLogHandler(void);
    bool CreatelitealigndataPath();

	bool Init(bool loglevel = false);
    bool SetLogFile(std::string& strLog);

    virtual void log (ACE_Log_Record &log_record);

    /// Get the ostream that is used to print messages.
    ACE_OSTREAM_TYPE* GetMsgOstream( void );
    void LockOstream();
    void UnlockOstream();
private:
	CLogHandler(void);
	CLogHandler(const CLogHandler& rhs);
	CLogHandler& operator = (const CLogHandler& rhs);

    void CleanupOstream ();
    void SetMsgOstream (ACE_OSTREAM_TYPE *m);
private:
	/// Instance of CLogHandler
    static CLogHandler*  m_pInstance;
	
    /// The ostream where logging messages can be written.
    std::mutex        m_ostreamLock;
    ACE_OSTREAM_TYPE* m_ostream;
	u_long 		      m_flags;

    std::string       m_strStatLogName;
};

#endif


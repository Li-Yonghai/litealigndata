#ifndef _ALIGN_CONFIG_H_
#define _ALIGN_CONFIG_H_

#include <iostream>
#include <spawn.h>
#include "ace/Log_Priority.h"
#include "com/AlignDataTypeDef.h"

class CAlignConfig
{
public:
    static CAlignConfig*  GetInstance();
    void ReleaseInstance();
    bool RunCmd(char *cmd);
    const std::string& GetOutputPath()
    {
        return m_strOutputPath;
    };
    void SetOutputPath(std::string outPutPath)
    {
        m_strOutputPath = outPutPath;
    };
    void  SetDebugLevel(litealigndata::SysLogDebugLevel debugLevel)
    {
         m_debugLevel = debugLevel;
    };
    const litealigndata::SysLogDebugLevel& GetDebugLevel()
    {
        return m_debugLevel;
    };
    const bool GetBVerbose()
    {
    return m_bVerbose;
    };
    void SetBVerbose(bool bVerbose)
    {
        m_bVerbose = bVerbose;
    };
    const bool GetStream()
    {
    	return m_stream;

    };
    void SetStream(bool Stream)
    {
        m_stream = Stream;

    };
    std::string& GetXmlFile()
    {
       return m_strXmlFile;
    };
    void  SetXmlFile(std::string xml)
    {
        m_strXmlFile = xml;
    };
    void SetshuffleFlag(bool shuffleFlag)
    {
        m_shuffleFlag = shuffleFlag;
    };
    bool GetshuffleFlag()
    {
        return m_shuffleFlag;
    }
    void Setlogpath(std::string logpath)
    {
        m_logpath = logpath;
    };
    std::string Getlogpath()
    {
        return m_logpath;
    };
    void SetshuffleFlie(std::string shuffleFlie)
    {
        m_shuffleFlie = shuffleFlie;
    }
    std::string GetshuffleFlie()
    {
        return m_shuffleFlie;
    }
    void Setinifile(std::string inifile)
    {
        m_inifile = inifile;
    }
    std::string Getinifile()
    {
        return m_inifile;
    }
    ~CAlignConfig();
    CAlignConfig()
    {
        m_strOutputPath = "";	
    	m_debugLevel = litealigndata::LOG_LEVEL_None;
	    m_bVerbose = false;
	    m_strXmlFile = false;
	    m_stream = false;
        m_shuffleFlag = false;
        m_shuffleFlie = "";
        m_inifile = "";
	    
    };
private:
    std::string m_strOutputPath;
    static CAlignConfig*  m_pInstance;
    litealigndata::SysLogDebugLevel m_debugLevel;
    bool m_bVerbose;
    bool m_stream;
    std::string m_strXmlFile;
    bool m_shuffleFlag;
    std::string m_shuffleFlie;
    std::string m_inifile;
    std::string m_logpath;
};

#endif

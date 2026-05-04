#ifndef _ALIGNDATACONFIG_H_
#define _ALIGNDATACONFIG_H_
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <getopt.h>
#include "ace/ACE.h"
#include "ace/Log_Msg.h"
#include "log/LogHandler.h"
#include "conf/AlignConfig.h"
#include "conf/AligndataConfig.h"
#include "ace/Configuration_Import_Export.h"

struct SeedFileInfo;

class CAligndataConfig
{
public:
        CAligndataConfig()
        {
            isSeedFromDir = false;
        }
        void extractSeedFiles( const char* seedDir, std::vector<std::string>& vFiles );
        bool readIniConfig( const char* inifile, SeedFileInfo& seedFileInfo );
        bool setLog(std::string xmlfile, int retentTime);
        bool GetisSeedFromDir()
        {
            return isSeedFromDir;
        }
        void help();
        bool ParserOption(int argc, char* argv[]);
        bool shuffleFile();
        bool ProduceData();
        ~CAligndataConfig()
        { 
            litealigndata_destory();;
        }
private:
bool isSeedFromDir;
CAlignDataTypeDef m_AlignDataTypeDef;

};

#endif

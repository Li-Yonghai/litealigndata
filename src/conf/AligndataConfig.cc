/*
 * =============================================================================
 *
 *       Filename:  Config.cc
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

#include "sub/parser/XmlParser.h"
#include "sub/shuffle/Shuffle.h"
#include "stream/Server.h"

void CAligndataConfig::extractSeedFiles( const char* seedDir, std::vector<std::string>& vFiles )
{
	std::string completeDir;
	char realPath[MAX_PATH] = {0};
	struct stat s_buf;
	char cwd[MAX_PATH] = {0};
	getcwd( cwd, MAX_PATH );

	if ( realpath( seedDir, realPath ) == 0 )
	{
		ACE_DEBUG((LM_ERROR, "the directory %s is not exist!\n", seedDir));
		return;
	}

	stat( seedDir, &s_buf );

	if ( !S_ISDIR( s_buf.st_mode ) ) //if it's not directory, return
	{
		ACE_DEBUG((LM_ERROR, "%s is not a directory!\n", seedDir));
		return;
	}

	completeDir = realPath;

	if ( completeDir.at( completeDir.size() - 1 ) == '/' )
		completeDir.erase( completeDir.size() - 1 );

	chdir( cwd ); //restore to the original directory

	return;
}


bool CAligndataConfig::readIniConfig( const char* inifile, SeedFileInfo& seedFileInfo )
{
	if ( inifile == NULL )
	{
		ACE_DEBUG((LM_ERROR, "you must specific a *.ini file to set the seed file.\n"));
		return  false;
	}
	ACE_Configuration_Heap config;
	ACE_Ini_ImpExp ini(config);
	config.open();
	if(ini.import_config(inifile) < 0)
	{
		ACE_ERROR_RETURN((LM_ERROR, "error open config failed\n"), false);
	}
	ACE_Configuration_Section_Key interfaceinfokey;
	ACE_TString valuestr;
	if(config.open_section(config.root_section(), "interfaceinfo", 0, interfaceinfokey) < 0)
	{
		seedFileInfo.initItf = seedFileInfo.itfNum  = UINT32_MAX;
	}
	else
	{
		config.get_string_value(interfaceinfokey, "initItf", valuestr);
		seedFileInfo.initItf = atoi(valuestr.c_str());
    	config.get_string_value(interfaceinfokey, "itfNum", valuestr);
		seedFileInfo.itfNum = atoi(valuestr.c_str());
	}
	if(seedFileInfo.initItf == UINT32_MAX)
		seedFileInfo.initItf = 3;
	if(seedFileInfo.itfNum == UINT32_MAX)
		seedFileInfo.itfNum = 1;

	ACE_DEBUG((LM_DEBUG, "interfaceinfo: initItf= %d, itfNum= %d.\n", seedFileInfo.initItf, seedFileInfo.itfNum));

    ACE_Configuration_Section_Key seedfilekey;
	if(config.open_section(config.root_section(), "seedfile", 0, seedfilekey) < 0)
	{
		ACE_ERROR_RETURN((LM_ERROR, "root_section seedfile is not configuration\n"), false);
	}
	
	config.get_string_value(seedfilekey, "filenum", valuestr);
	seedFileInfo.filenumber = atoi(valuestr.c_str());
	if ( seedFileInfo.filenumber == 0 || seedFileInfo.filenumber > MAX_SEEDFILE_NUM )
	{
		ACE_ERROR_RETURN((LM_ERROR, "seedfile: filenum= %d, MAX_SEEDFILE_NUM= %d\n", \
				seedFileInfo.filenumber, MAX_SEEDFILE_NUM), false);
	}
	ACE_DEBUG((LM_INFO, "filenum = %d.\n", seedFileInfo.filenumber));

	SeedFile &seedfile = seedFileInfo.seedfile;
	memset( &seedfile, 0, sizeof( seedfile ) );
	for ( uint32_t i = 0; i < seedFileInfo.filenumber; ++i )
	{
		char filePath[64] = {0};
		snprintf( filePath, 64, "seedfile%u", i + 1 );
		char seedJson[64] = {0};
		snprintf( seedJson, 64, "seedJson%u", i + 1 );
		char tablename[64] = {0};
		snprintf( tablename, 64, "tablename%u", i + 1 );

		config.get_string_value(seedfilekey, filePath, valuestr);
		if ( valuestr.c_str() == NULL )
		{
			ACE_ERROR_RETURN((LM_ERROR, "seedfile: filePath is not configuration!\n"), false);
		}
		strncpy( seedfile.filePath[i], valuestr.c_str(), SEED_FILE_PATH_LEN );
		ACE_DEBUG((LM_DEBUG, "%s = %s\n", filePath, seedfile.filePath[i]));
        
		config.get_string_value(seedfilekey, seedJson, valuestr);
		if ( valuestr.c_str() == NULL )
		{
			ACE_ERROR_RETURN((LM_ERROR, "seedfile: seedJson is not configuration!\n"), false);
		}
		strncpy( seedfile.jsonFile[i], valuestr.c_str(), SEED_FILE_PATH_LEN );
		ACE_DEBUG((LM_DEBUG, "%s = %s\n", seedJson, seedfile.jsonFile[i]));

		config.get_string_value(seedfilekey, tablename, valuestr);
		if ( valuestr.c_str() == NULL )
		{
			ACE_ERROR_RETURN((LM_ERROR, "seedfile: tablename is not configuration!\n"), false);
		}
		strncpy( seedfile.tablename[i], valuestr.c_str(), SEED_FILE_PATH_LEN );
		ACE_DEBUG((LM_DEBUG, "%s = %s\n", tablename, seedfile.tablename[i]));
		
	}

	ACE_Configuration_Section_Key filetypekey;
	if(config.open_section(config.root_section(), "filetype", 0, filetypekey) < 0)
	{
		ACE_ERROR_RETURN((LM_ERROR, "filetype is not configuration!\n"), false);
	}

	for ( uint32_t i = 0; i < seedFileInfo.filenumber; ++i )
	{
		char seedfiletypevalue[64] = {0};
		snprintf( seedfiletypevalue, 64, "seedfile%u", i + 1 );

		config.get_string_value(filetypekey, seedfiletypevalue, valuestr);        
		if ( valuestr.c_str() == NULL )
		{
			ACE_ERROR_RETURN((LM_ERROR, "filetype: seedfile is not configuration!\n"), false);
		}
		strncpy( seedfile.fileType[i], valuestr.c_str(), MAX_TYPE_LEN );
		ACE_DEBUG((LM_DEBUG, "%s = %s\n", seedfiletypevalue, seedfile.fileType[i]));
	}
	ACE_ERROR_RETURN((LM_INFO, "readIniConfig success\n"), true);
}

bool CAligndataConfig::setLog(std::string xmlfile, int retentTime)
{
	std::string logPath = "./output/log/";
	DIR* dirptr = opendir( logPath.c_str() );

	if ( dirptr == NULL )
	{
		printf("%s directory is NOT exist! Create it firstly.\n", logPath.c_str());	
		std::string cmd = "mkdir -p " + logPath;

		if ( system( cmd.c_str() ) )
		{
			printf("%s directory failed to create.\n", logPath.c_str());
			return 1;
		}
	}
	closedir( dirptr );

	std::string cleanLog = "find " + logPath + " -type f -name '*.log' -mtime +" + std::to_string(retentTime) +" -exec rm {} \\;";
	if ( system( cleanLog.c_str() ) )
	{
		printf("%s cleaning log dir failed.\n", logPath.c_str());
		return 1;
	}

	char curLog[256] = {0};
	m_AlignDataTypeDef.setNewLogFile(xmlfile,curLog);
	printf("Please see detail info in log file %s\n", curLog);
	fflush(stdout);

	return 0;
}

void CAligndataConfig::help()
{
	printf( "Usage: use -i option to generate a template of Configuration file base on the seed file\n"
			"then according to your requirement to modify the Configuration file, \n");
	printf( "\t[-h] : general help\n" );
	printf( "\t[-f] : specific the configuration file(.xml), (default =./alignDataNetworkModel.xml)\n" );
	printf( "\t[-i] : specific file (*.ini) to set the seed file \n" );
	printf( "\t[-r] : specific file  to shuffle \n" );
	printf( "\t[-n] : fix the name which generation, if not use it, default litealigndata.xml\n" );
	printf( "\t[-s] : laungh Stream Mode \n" );
	printf( "\t[-s] : read all XML file from current direct \n" );
	printf( "\t[-d] : adjust Log level which output to log file:\n\
	\t\t0(default): LM_SHUTDOWN\n\
	\t\t1: LM_INFO\n\
	\t\t2: LM_WARNING | LM_INFO\n\
	\t\t3: LM_DEBUG | LM_WARNING | LM_INFO\n\
	\t\t4: LM_ERROR | LM_DEBUG | LM_WARNING | LM_INFO\n\
	\t\t5: LM_CRITICAL | LM_ERROR | LM_DEBUG | LM_WARNING | LM_INFO\n" );
	printf( "\texample:\n" );
	printf( "\tcreate default xml template, ./litealigndata -i seedfile.ini. -n Example.xml\n" );
	printf( "\tgeneration Data template, ./litealigndata -f Example/Example.xml.\n" );
	printf( "\tgeneration Data template, ./litealigndata -f Example/Example.xml -s.\n" );
}
bool CAligndataConfig::ParserOption(int argc, char* argv[] )
{
	if(argc < 2) 
	{
		help();
		ACE_DEBUG((LM_ERROR, "please run ./litealigndata -h for help\n"));
		//litealigndata_destory();
		return false;
	}
	int nextOpt;
	uint8_t optCount = 0;
	//CAligndataConfig AligndataConfig;
    const char* const CMD_OPTIONS = "hf:r:v:d:sn:";
    struct option long_options[] =
    {
		{NULL,0,NULL, 0},
	};
	CAlignConfig::GetInstance()->Setlogpath("./output/log/");
	CAlignConfig::GetInstance()->SetXmlFile(std::string("./litealigndata.xml")); //default xml from ini without using -n *xml
	CAlignConfig::GetInstance()->SetOutputPath(std::string("./output/litealigndata/"));
	int optIndex;
	while ( ( nextOpt = getopt_long( argc, argv, CMD_OPTIONS, long_options, &optIndex ) ) != -1 )
	{
		switch ( nextOpt )
		{
			case 'n':

				CAlignConfig::GetInstance()->SetXmlFile(optarg);
				break;
			case 's':

				CAlignConfig::GetInstance()->SetStream(true);
				break;
			case 'd':
            {  
				/*
				0(default): LM_SHUTDOWN
				1: LM_INFO
				2: LM_WARNING | LM_INFO
				3: LM_DEBUG | LM_WARNING | LM_INFO
				4: LM_ERROR | LM_DEBUG | LM_WARNING | LM_INFO
				5: LM_CRITICAL | LM_ERROR | LM_DEBUG | LM_WARNING | LM_INFO
				*/
                CAlignConfig::GetInstance()->SetDebugLevel(( litealigndata::SysLogDebugLevel ) atoi( optarg ));
		       if(atoi(optarg) >= litealigndata::LOG_LEVEL_Max || 
			   				atoi(optarg) < litealigndata::LOG_LEVEL_None )
		       {
	                help();	       
			   		return false;
		       }
               break;
            }
			case 'v':
            {
                CAlignConfig::GetInstance()->SetBVerbose(true);
                break;
            }
			case 'h':
				{
					optCount++;
					help();
					return false;
				}

			case 'f':
				{
					optCount++;
					CAlignConfig::GetInstance()->SetXmlFile(optarg);
					break;
				}
			case 'r':
                	CAlignConfig::GetInstance()->SetshuffleFlag(true);
					CAlignConfig::GetInstance()->SetshuffleFlie(optarg);
					break;
			
			case '?':
				{
					printf("Invalidate command!!\n\n");
					help();
					return false;;
				}

			default:
				{
					help();
					litealigndata_destory();
					return false;
				}
		}
	}

	CLogHandler::GetInstance()->Init();	
	shuffleFile();

	if(!CLogHandler::GetInstance()->CreatelitealigndataPath())
	{
		return false;
	}

	return true;

}
bool CAligndataConfig::shuffleFile()
{
	if(CAlignConfig::GetInstance()->GetshuffleFlag())
	{
		ACE_DEBUG((LM_INFO, "File %s is Shuffled Start.........\n", \
			(CAlignConfig::GetInstance()->GetshuffleFlie()).c_str()));
        CShuffleFileLines shuffle;
		shuffle.swapLines(CAlignConfig::GetInstance()->GetshuffleFlie());
		ACE_DEBUG((LM_INFO, "File %s is Shuffled End.........\n", \
			(CAlignConfig::GetInstance()->GetshuffleFlie()).c_str()));
		litealigndata_destory();
		return 0;
	}
	ACE_DEBUG((LM_INFO, "litealigndata Start..............\n"));	
	return 0;
}

bool CAligndataConfig::ProduceData()
{
	const char* fileName = CAlignConfig::GetInstance()->GetXmlFile().c_str();
	const char *c = "\n";
	char *p = (char*)strstr(fileName, c);
	if(p != NULL )
		*p = '\0';

	if(CAlignConfig::GetInstance()->GetStream())
	{
		ACE_DEBUG((LM_INFO, "ProduceData STREAM MODE Start..............\n"));
    	CServer server;
    	server.Init(fileName, 
			(CAlignConfig::GetInstance()->GetOutputPath()).c_str());
    	server.Start();
		while(1)
    	{
    		sleep(60);
    	}
		ACE_DEBUG((LM_INFO, "ProduceData STREAM MODE End..............\n"));
	}
    else
	{
		ACE_DEBUG((LM_INFO, "ProduceData FILE MODE Start..............\n"));
		CXmlParser* parser = new CXmlParser(fileName, 
				(CAlignConfig::GetInstance()->GetOutputPath()).c_str());

		if(access(fileName, F_OK) == -1)           
		{
			ACE_DEBUG((LM_ERROR, "the file: %s is not exist.\n\n", fileName));                                        
			help();
			litealigndata_destory();                                 
			return false;                                     
		}  
		
		if ( !parser->Initialize() )
		{
			ACE_DEBUG((LM_ERROR, "Failed to initialize Xml Parser, exit!\n"));
			litealigndata_destory();
			return false;
		}
		parser->ProduceData(NULL);

		delete parser;
		ACE_DEBUG((LM_INFO, "ProduceData FILE MODE END..............\n"));
	}
	return true;	
}
/*
 * =============================================================================
 *
 *       Filename:  Main.cc
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
#include "conf/AligndataConfig.h"

int main( int argc, char* argv[] )
{
	
	CAligndataConfig AligndataConfig;

	if(!AligndataConfig.ParserOption(argc, argv ))
	{
		ACE_ERROR((LM_ERROR, "Failed to ParserOption.\n"));
		return 1;
	}

	ACE_DEBUG((LM_INFO, "litealigndata Start..............\n"));
	if(!AligndataConfig.ProduceData())
	{
		ACE_ERROR((LM_ERROR, "Failed to produce data.\n"));
		ACE_DEBUG((LM_INFO, "litealigndata End..............\n"));
		return 1;
	}
	ACE_DEBUG((LM_INFO, "litealigndata End..............\n"));

	return 0;
}
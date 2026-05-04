/*
 * =============================================================================
 *
 *       Filename:  main.cc
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

#include <unistd.h>
#include <stdlib.h>
#include "Connector.h"

int main(int argc, char* argv[])
{
    if(argc < 3)
        return 1;

    CConnector cc;
    cc.Init(argv[1], atoi(argv[2]));//cc.Init("192.168.0.107", 8008);

    while(1)
    {
        //if(m_stop)
        //    break;
        sleep(60);
    }
    return 0;
}


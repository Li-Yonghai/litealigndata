#ifndef _SHUFFLE_H_
#define _SHUFFLE_H_

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

//#include "ace/Log_Msg.h"

class CShuffleFileLines
{
    #define MAX_LINEDATA_DATA_BYTE      (long)8*1024*1024*2
    #define MAX_LINE_COUNT_BYTE         (long)2*1024*1024*sizeof(uint64_t)
    public:
        CShuffleFileLines(){};
        ~CShuffleFileLines(){};
        int swapLines(std::string oriFile);
    private:
        int GetCommdData(const char* oriFile, char* data, unsigned int size);
        int swapFileLineBymmapshuf(std::string& oriFile);
        int swapFileLineBymmap(std::string& oriFile);
};

#endif

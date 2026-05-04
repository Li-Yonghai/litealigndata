/*
 * =============================================================================
 *
 *       Filename:  Shuffle.cc
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
#include "Shuffle.h"

int CShuffleFileLines::GetCommdData(const char* oriFile, char* data, unsigned int size)
{
    FILE *fp = NULL;
    char tmpCommd[128] = {'\0'};
    //K
    sprintf(tmpCommd, "du  %s  | awk '{print $1}'", oriFile);
    fp = popen(tmpCommd, "r");
    if(fp == NULL)
    {
        //ACE_DEBUG((LM_ERROR, "popen error!\n"));
        return 1;
    }

    if(fgets(data, size, fp) == NULL)
    {
        //ACE_DEBUG((LM_ERROR, "fgets error!\n"));
        return 1;
    }
    pclose(fp);
    return 0;

}

int CShuffleFileLines::swapFileLineBymmapshuf(std::string& oriFile)
{
    typedef struct
    {
        char *ptr;
        uint32_t len;
    }lineData;

    int fd;
    if( (fd = open(oriFile.c_str(), O_RDONLY)) < 0)
    {
        perror("open\n");
        return -1;
    }

    struct stat sb;
    if(stat(oriFile.c_str(), &sb) < 0)
    {
        return -1;
    }

	std::size_t pos = oriFile.rfind(".");
    std::size_t pos1 = oriFile.rfind("/");
	std::string oriName = oriFile.substr (pos1+1,pos-pos1-1); 
	std::string suffix = oriFile.substr (pos+1);
    std::string output = oriName + "_random." + suffix;
    char *base = (char*)mmap(NULL, sb.st_size, PROT_READ,MAP_SHARED,fd,0);
    char *ptr = base;
    if (ptr == NULL)
    {
        fprintf(stderr, "mmap: %s\n", strerror(errno));
        return -1;
    }

    FILE *fdshuf;
    std::string shuffile = oriName + "shuffile";
    fdshuf = fopen(shuffile.c_str(), "w+");
    uint64_t  lineCount = 0;
    uint32_t iCount = 0;
    char *point = ptr;
    while((*ptr)!='\0')
    {
        iCount++;
        if((*ptr) =='\n') 
        {

            fprintf(fdshuf, "%ld, %p, %u\n", lineCount, point, iCount);
            iCount = 0;
            lineCount++;
            point = ptr + 1;
        }
        ptr++;
    }

    int fdOut;
    if( (fdOut = open(output.c_str(), O_CREAT|O_RDWR|O_TRUNC, 0666)) < 0)
    {
        return -1;
    }
    ftruncate(fdOut, sb.st_size);  
    char *baseOut = (char*)mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE,MAP_SHARED,fdOut,0);
    char *ptrOut = baseOut;
    fclose(fdshuf);
    std::string shuffiletmp = oriName + "shuffiletmp";
    char tmp[128] = { '\0'};
    sprintf(tmp, "shuf %s -o %s", shuffile.c_str(), shuffiletmp.c_str());
    system(tmp);
    fdshuf = fopen(shuffiletmp.c_str(), "r");
    uint32_t shufCount = 0;
    uint64_t  shuflineCount = 0;
    char *shufpoint = NULL;
	for ( uint64_t i = 0; i < lineCount; i++ )
	{
        fscanf(fdshuf, "%lu, %p, %u", &shuflineCount, (void**)&shufpoint, &shufCount);
        memcpy(ptrOut, shufpoint, shufCount); 
        ptrOut = ptrOut + shufCount;

	}
    if((msync((char*)baseOut, sb.st_size, MS_SYNC)) == -1) perror("msync");
    munmap((char*)baseOut, sb.st_size);
    munmap((char*)base, sb.st_size);
    fclose(fdshuf);
    char rmfile[128]={'\0'};
    sprintf(rmfile, "rm -f %s %s", shuffile.c_str(), shuffiletmp.c_str());
    system(rmfile);
    close(fdOut);
    close(fd);
	return 0;
}
int CShuffleFileLines::swapFileLineBymmap(std::string& oriFile)
{
    typedef struct
    {
        char *ptr;
        uint32_t len;
    }lineData;

    int fd;
    if( (fd = open(oriFile.c_str(), O_RDONLY)) < 0)
    {
        perror("open\n");
        return -1;
    }

    struct stat sb;
    if(stat(oriFile.c_str(), &sb) < 0)
    {
        return -1;
    }

	std::size_t pos = oriFile.rfind(".");
    std::size_t pos1 = oriFile.rfind("/");
	std::string oriName = oriFile.substr (pos1+1,pos-pos1-1); 
	std::string suffix = oriFile.substr (pos+1);
    std::string output = oriName + "_random." + suffix;
    char *base = (char*)mmap(NULL, sb.st_size, PROT_READ,MAP_SHARED,fd,0);
    char *ptr = base;
    if (ptr == NULL)
    {
        fprintf(stderr, "mmap: %s\n", strerror(errno));
        return -1;
    }

    lineData *lineptr = (lineData *)mmap(NULL, MAX_LINEDATA_DATA_BYTE, PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    uint64_t *lineCountIndex = (uint64_t *)mmap(NULL, MAX_LINE_COUNT_BYTE, PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    uint64_t  lineCount = 0;
    uint32_t iCount = 0;
    char *point = ptr;
    while((*ptr)!='\0')
    {
        iCount++;
        if((*ptr) =='\n') 
        {
            lineptr[lineCount].ptr = point;
            lineptr[lineCount].len = iCount;
            lineCountIndex[lineCount] = lineCount;
            iCount = 0;
            lineCount++;
            point = ptr + 1;
        }
        ptr++;
    }

    int fdOut;
    if( (fdOut = open(output.c_str(), O_CREAT|O_RDWR|O_TRUNC, 0666)) < 0)
    {
        return -1;
    }
    ftruncate(fdOut, sb.st_size);  
    char *baseOut = (char*)mmap(NULL, sb.st_size, PROT_READ|PROT_WRITE,MAP_SHARED,fdOut,0);
    char *ptrOut = baseOut;
    uint64_t index = 0;
	for ( uint64_t i = 0; i < lineCount; i++ )
	{
	    index =  (rand()*32767 + rand())%(lineCount - i) + i;
        if (index != i && index < lineCount)
        {
              lineCountIndex[index] = lineCountIndex[index] ^ lineCountIndex[i];
              lineCountIndex[i] = lineCountIndex[index] ^ lineCountIndex[i];
              lineCountIndex[index] = lineCountIndex[index] ^ lineCountIndex[i];
        }
        memcpy(ptrOut, lineptr[lineCountIndex[i]].ptr, lineptr[lineCountIndex[i]].len);
        ptrOut = ptrOut + lineptr[lineCountIndex[i]].len;

	}
    if((msync((char*)baseOut, sb.st_size, MS_SYNC)) == -1) perror("msync");
    munmap((char*)baseOut, sb.st_size);
    munmap((char*)base, sb.st_size);
    munmap((char*)lineCountIndex, MAX_LINE_COUNT_BYTE);
    munmap((char*)lineptr, MAX_LINEDATA_DATA_BYTE);
    close(fdOut);
    close(fd);
	return 0;
}

int CShuffleFileLines::swapLines(std::string oriFile)
{
   char data[128] = {'\0'};
   if(GetCommdData(oriFile.c_str(), data, 100))
       return 1; 
   unsigned long num = atoll(data);
   //ACE_DEBUG((LM_INFO, "ShuffleFile file: %s is about %ldKb.\n\n", oriFile.c_str(),
                                    //num)); 
   if(num > (1 << 20)*500)//( 500G, +oo )
   {
       //ACE_DEBUG((LM_ERROR, "%s is too big !\n", oriFile.c_str()));
       return 1;
   }
   else if( num > (1 << 20)*200 )//( 200G, 500G ]
   {
      
      //ACE_DEBUG((LM_DEBUG, "ShuffleFile file: ( 200G, 500G ].\n\n"));
      swapFileLineBymmapshuf(oriFile);
   }
   else//(0G, 200G ]
   {
        //ACE_DEBUG((LM_DEBUG, "ShuffleFile file: (0G, 200G ].\n\n"));
        swapFileLineBymmap(oriFile);
   }

   return 0;
}
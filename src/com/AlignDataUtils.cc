/*
 * =============================================================================
 *
 *       Filename:  alignDataUtils.cc
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
#include <string>
#include <fstream>
#include <iostream>
#include "ace/Log_Msg.h"
#include "AlignDataUtils.h"


Random* Random::m_Instance = NULL;


Random::Random(uint16_t totalSThreads):m_seedNum(totalSThreads)
{
    //Set seeds for each thread:
    for (uint16_t idx = 0; idx < m_seedNum; ++idx)
    {
        m_nexts.push_back(idx + 10);    //In case the slave thread id can be used as index of vector 
    }
    m_nexts.push_back(m_seedNum + 10); //This is for master thread, netable server will use it.
    
}

uint32_t  Random::rand(uint16_t thread)
{
    if (thread == MAX_THREADS)
        thread = m_nexts.size() - 1;  //by default master thread want a random number for its NE table.
    if (thread >= m_nexts.size())
    {
        ACE_DEBUG((LM_ERROR, "Error, wrong thread number in geting rand: %u.\n", thread));
        thread = Random::MAX_THREADS + 1;  
    }
    m_nexts[thread] = m_nexts[thread] * 1103515245 + 12345;
    return((uint32_t)(m_nexts[thread]/65536) % (MAX_RAND + 1) );
 
}


bool  Random::createInstance(uint16_t InsNum)
{
    if (InsNum > (MAX_THREADS - 1) )
    {
        ACE_DEBUG((LM_ERROR, "Only support 32 threads maximum. Current num: %u\n", InsNum));
        return false;
    }
    //if (m_Instance) return false;
    if (m_Instance) return true;
    
    m_Instance = new Random(InsNum);
    return (m_Instance != NULL) ? true : false;
}

uint32_t countFileLine(char const * const fileName)
{ 
    int number_of_lines = 0;
    std::string line;
    std::ifstream myfile(fileName);

    while (std::getline(myfile, line))
        ++number_of_lines;

    return number_of_lines;
}

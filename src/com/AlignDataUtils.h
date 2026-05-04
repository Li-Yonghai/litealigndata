#ifndef _ALIGNDATAUtils_H_
#define _ALIGNDATAUtils_H_

#include <vector>

class Random
{
public:
    static bool createInstance(uint16_t totalSThreads = MAX_THREADS);
    static Random*  getIns() 
       {  return (m_Instance != NULL) ? m_Instance : NULL ; };
       
    uint32_t    rand(uint16_t thread = MAX_THREADS); 
    static const uint16_t MAX_THREADS = 33;
    static const uint32_t MAX_RAND = 32767;   
    
private:
    Random(uint16_t totalSThreads = MAX_THREADS) ;
    virtual ~Random(){};
    Random(const Random& );
    Random& operator = (const Random&); 
    
private:
    static Random *m_Instance;
    std::vector<uint64_t>  m_nexts;
    uint32_t      m_seedNum; 
    
};


uint32_t countFileLine(char const * const fileName);
#endif


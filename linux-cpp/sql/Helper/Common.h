#ifndef __COMMON_20140429_H__
#define __COMMON_20140429_H__

#include <string>
#include <time.h>

#include "TypeDef.h"

#define     _S(x)   #x


struct FuncTime
{
    public:
        FuncTime(const std::string& func);
        ~FuncTime();

    private:
        struct timespec  m_from;
        uint64 m_logGap;
        uint64 m_fromSec;
        std::string m_func;
};


#define     FTime(f)  FuncTime func(f)

#endif

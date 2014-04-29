#include "Common.h"


FuncTime::FuncTime(std::string& func) : m_logGap(0)
                                        ,m_func(func)
{
    clock_gettime(CLOCK_REALTIME, &m_from);
    clock_gettime(CLOCK_REALTIME, &m_from);
}

FuncTime::~FuncTime()
{
    struct timespec end_tv;
    clock_gettime(CLOCK_REALTIME, &end_tv);

    uint64 end = (uint64)end_tv.tv_sec*1000000000L + end_tv.tv_nsec;
    uint64 from = (uint64)m_from.tv_sec*1000000000L + m_from.tv_nsec;

    std::cout << m_func << " cost: " << end-from << " 毫秒" << std::endl;
}

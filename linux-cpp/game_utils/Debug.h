#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <string>
#include <execinfo.h>

std::string debug(size_t frame = 1)
{
    std::string info;
    if (frame > 10)
        return info;

    void *arr[15];
    size_t size = backtrace(arr, frame + 2);
    char **symbs = backtrace_symbols(arr, size);
    if (symbs)
    {
        for (size_t i = 1; i < size; i++)
        {
            info += symbs[i];
            info += "|";
        }
        free(symbs);
    }

    return info;
}

#endif
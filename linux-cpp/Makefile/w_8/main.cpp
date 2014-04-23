#include <iostream>

/*
#ifndef OPENAPIV3_ERROR
#define OPENAPIV3_ERROR(fmt, args...) \
        _snprintf(m_errmsg, sizeof(m_errmsg), "[%s][%d][%s]"fmt, \
                             __FILE__, __LINE__,__FUNCTION__, ##args)
#endif

#define OutPutDebug(fmt, args...)  printf(fmt,##args)
*/

#ifndef OPENAPIV3_ERROR
#define OPENAPIV3_ERROR(fmt, args,...) \
    _snprintf(m_errmsg, sizeof(m_errmsg), "[%s][%d][%s]"fmt, \
            __FILE__, __LINE__,__FUNCTION__, ##args)
#endif

#define OutPutDebug(fmt, args,...)  printf(fmt,##args)

int main()
{
    std::cout<<"test"<<std::endl;
    return 0;
}


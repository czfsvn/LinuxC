#include "Log.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef linux
#define getMsg(msg, msglen, pat) \
	do \
{ \
	va_list ap;\
	bzero(msg, msglen); \
	va_start(ap, pat);  \
	vsnprintf(msg, msglen - 1, pat, ap);    \
	va_end(ap); \
}while(false)
#else
#define getMsg(msg, msglen, pat) \
    do \
{ \
    va_list ap;\
    memset(msg, 0, msglen -1); \
    va_start(ap, pat);  \
    vsnprintf(msg, msglen - 1, pat, ap);    \
    va_end(ap); \
}while(false)
#endif


Log::Log() : m_path("")
{

}

Log::~Log()
{

}

void Log::InitPath(std::string path)
{
	m_path = path;
}

void Log::LogText(const char* info)
{
	if (!info)
		return;

	time_t lt = time(NULL);

	FILE* f = fopen(m_path.c_str(), "a" ) ;
	fwrite(ctime(&lt),1,24 ,f);
	fwrite( "\t", 1, 1, f ) ;
	fwrite( info, 1, strlen(info) , f ) ;
	fwrite( "\n", 1, 1, f ) ;
	fclose(f) ;
}

void Log::LogMsg(const char* pattern, ...)
{
    char buf[4096] = {0};
    getMsg(buf, 4096, pattern);
    LogText(buf);
}

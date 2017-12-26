#ifndef __LOG_20111227_H__
#define __LOG_20111227_H__

#include <iostream>
#include <stdio.h>

#include <boost/serialization/singleton.hpp>


#define MAX_LOG_CHAR			4096

#ifdef linux
#define sprintf_s  snprintf
#endif


class Log : public boost::serialization::singleton<Log>
{
public:
	Log();
	virtual ~Log();

	void InitPath(std::string path);

	void LogText(const char* info);

	void LogMsg(const char* pattern, ...);
protected:
private:
	std::string m_path;
};



#endif

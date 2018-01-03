#ifndef __MYSQLRES_20111101_H__
#define __MYSQLRES_20111101_H__

#include <iostream>
#include <mysql/mysql.h>

#ifdef WIN32
#include <WinSock.h>
#else
#endif


typedef unsigned int     uint32;
typedef signed  int      int32;


class MysqlRes
{
	friend class MysqlCon;
public:
	MysqlRes();
	~MysqlRes();

	uint32 GetRecordCount();
	uint32 GetFieldCount();
	uint32 GetFieldLength(uint32 idx);

	bool GetRecord();
	const char* GetFieldVal(uint32 idx);

	void Release();
	
protected:
private:
	MYSQL_RES* m_res;
	MYSQL_ROW m_row;
	unsigned long* m_adwlengths;
};
#endif

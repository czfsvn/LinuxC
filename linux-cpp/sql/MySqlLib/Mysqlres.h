#ifndef __MYSQLRES_20111101_H__
#define __MYSQLRES_20111101_H__

/*
#ifndef linux
#include <WinSock.h>
#include "mysql.h"
#else
#endif
*/

#include <mysql.h>
#include "TypeDef.h"

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

	uint64 GetUInt64(uint32 idx);
	uint32 GetUInt(uint32 idx);
	uint16 GetUInt16(uint32 idx);
	uint8 GetUInt8(uint32 idx);

	int64 GetInt64(uint32 idx);
	int32 GetInt(uint32 idx);
	int16 GetInt16(uint32 idx);
	int8 GetInt8(uint32 idx);

	float GetFloat(uint32 idx);

	std::string GetStr(uint32 idx);
protected:
private:
	MYSQL_RES* m_res;
	MYSQL_ROW m_row;
	unsigned long* m_adwlengths;
};
#endif

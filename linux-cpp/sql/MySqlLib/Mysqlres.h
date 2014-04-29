#ifndef __MYSQLCON_20111101_H__
#define __MYSQLCON_20111101_H__

#include "Mysqlres.h"
#include <iostream>

enum DBErr
{
	DB_ERR_CONN						= -2,
	DB_ERR_UNKNOWN					= -3,
	DB_SUCCESS						= 0,
	DB_NO_RECORD					= 1,
	DB_HAS_RECORD					= 2,
	CR_CONN_HOST_ERROR				= 2003,
	CR_SERVER_GONE_ERROR			= 2006,
	CR_SERVER_LOST					= 2013,
};

class MysqlCon
{
public:
	MysqlCon(std::string& ip, std::string& userName, std::string& pwd, std::string& dbName);
	~MysqlCon();

	bool Connection();
	void Close();
	bool ReConnect();
	bool CheckConnection();

	uint32 EscapeString(std::string& str);

	int32 ExcuteSql(std::string& sql);
	int32 ExcuteSqlEx(std::string& sql, MysqlRes& res);

	int32 ExcuteSql(const char* sql);
	int32 ExcuteSqlEx(const char* sql, MysqlRes& res);
protected:
private:
	std::string m_host;
	std::string m_dbName;
	std::string m_usr;
	std::string m_pwd;
	char* m_socket;
	uint32 m_clientFlag;
	uint32 m_port;
    std::string m_CodeQuery;

	bool m_isConnect;
	MYSQL* m_mysql;
};
#endif

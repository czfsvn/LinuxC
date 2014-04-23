#ifndef DBSIMPLE_H_20091026
#define DBSIMPLE_H_20091026

//#include <WinSock.h>
#include <string>

#include <iostream>
#include <mysql/mysql.h>

typedef long int int64;
typedef int   int32;
typedef short int16;
typedef char int8;
typedef unsigned long int uint64;
typedef unsigned int uint32;
typedef unsigned short  uint16;
typedef unsigned char   uint8;

using namespace std;

class DBSimple
{
public:
	DBSimple(string ip, string userName, string pwd, string dbName);
	~DBSimple();

	void DBConnect(string h,string un,string ps,string db,int p,string so,int cl);

	MYSQL_RES* GetResult(string& sql, int32& num, int32& errNo);

	uint32 EscapeString(std::string& str);

	bool GetState(){ return m_conFlag; }
	//uint16 Query(string sql);

protected:
	void _FreeResult(MYSQL_RES* res)
	{
		if (res == NULL)
			return;
		mysql_free_result(res);
		int next = mysql_next_result(m_mySql);
		while (next == 0)
		{
			res = mysql_store_result(m_mySql);
			mysql_free_result(res);
			next = mysql_next_result(m_mySql);
		}

		res = NULL;
	}
private:
	MYSQL *m_mySql;
	MYSQL_RES* m_res;
	bool m_conFlag;

	string m_host;
	string m_userName;
	string m_pwd;
	string m_dataBase;
	int m_port;
	string m_socket;
	int m_clientFlag;

    std::string m_CodeQuery;
};

#endif

#include "DBSimple.h"


DBSimple::DBSimple(string ip, string userName, string pwd, string dbName)
:m_conFlag(false), m_mySql(mysql_init(NULL)), m_host(ip), m_userName(userName),
m_pwd(pwd), m_dataBase(dbName), m_port(3306), m_socket("\0"), m_res(NULL),
m_clientFlag(CLIENT_MULTI_STATEMENTS)
{
    m_CodeQuery = "set names utf8";
	DBConnect(m_host, m_userName, m_pwd, m_dataBase, m_port, m_socket, m_clientFlag);
	mysql_query(m_mySql, m_CodeQuery.c_str());
}

DBSimple::~DBSimple()
{
	mysql_close(m_mySql);
	m_conFlag = false;
}

void DBSimple::DBConnect(string h, string un, string ps, string db, int p, string so, int cl)
{
	if(mysql_real_connect(m_mySql,h.c_str(),un.c_str(),ps.c_str(),
		db.c_str(), p, so.c_str(),cl))
	{
		m_conFlag = true;
	}
	else
	{
		m_conFlag = false;
	}
	uint32 dwErr = mysql_errno(m_mySql);
	if (dwErr)
	{
		std::cout<<"DB connection error,Error Num = "<<dwErr<<std::endl;
	}
	bool val  = true;
	mysql_options(m_mySql, MYSQL_OPT_RECONNECT, (char*)&val);
}
//
//MYSQL_RES* DBSimple::GetResult(std::string sql)
//{
//	if (!m_conFlag)
//	{
//		DBConnect(m_host, m_userName, m_pwd, m_dataBase, m_port, m_socket, m_clientFlag);
//		mysql_query(m_mySql,"set names gb2312");
//	}
//	if (m_res != NULL)
//	{
//		mysql_free_result(m_res);
//	}
//
// 	if (!(mysql_query(m_mySql, sql.c_str())))
//	{
//		m_res = mysql_store_result(m_mySql);
//	}
//	else
//	{
//	}
//
//
//	uint32 dwErr = mysql_errno(m_mySql);
//	if (dwErr)
//	{
//		std::cout<<"DB GetResult£ºError Num = "<<dwErr<<", sql = "<<sql<<std::endl;
//	}
//
//	mysql_next_result(m_mySql);
//	mysql_store_result(m_mySql);
//
//	return m_res;
//}
//
//uint16 DBSimple::Query(string sql)
//{
//	if (!m_conFlag)
//	{
//		DBConnect(m_host, m_userName, m_pwd, m_dataBase, m_port, m_socket, m_clientFlag);
//		mysql_query(m_mySql,"set names gb2312");
//	}
//// 	if (m_res != NULL)
//// 	{
//// 		mysql_free_result(m_res);
//// 	}
//// 	if (!(mysql_query(m_mySql, sql.c_str())))
//// 		m_res = mysql_store_result(m_mySql);
//
//	uint32 t = mysql_query(m_mySql, sql.c_str());
//	uint16 affect_num = (uint16)mysql_affected_rows(m_mySql);
//	uint32 dwErr = mysql_errno(m_mySql);
//	if (dwErr)
//	{
//		std::cout<<"DB Query£ºError Num = "<<dwErr<<", sql = "<<sql<<std::endl;
//		affect_num = 0;
//	}
//
// 	mysql_next_result(m_mySql);
// 	mysql_store_result(m_mySql);
//	return affect_num;
//}

//
// uint16 DBSimple::Query(string sql)
// {
// 	{
// 		DBConnect(m_host, m_userName, m_pwd, m_dataBase, m_port, m_socket, m_clientFlag);
// 		mysql_query(m_mySql,"set names gb2312");
// 	}
//
// 	_FreeResult(m_res);
//
// 	if (0 != mysql_query(m_mySql, sql.c_str()))
// 	{
// 		uint32 dwErr = mysql_errno(m_mySql);
//
//
// 		std::cout<<"DB Query£ºError Num = "<<dwErr<<", sql = "<<sql<<std::endl;
//
// 		return 0;
// 	}
//
// 	m_res = mysql_store_result(m_mySql);
// 	return (uint32)mysql_affected_rows(m_mySql);
// }

MYSQL_RES* DBSimple::GetResult(std::string& sql, int32& num, int32& errNo)
{
execute:
	if (!m_conFlag)
	{
		DBConnect(m_host, m_userName, m_pwd, m_dataBase, m_port, m_socket, m_clientFlag);
		mysql_query(m_mySql, m_CodeQuery.c_str());
		goto execute;
	}

	_FreeResult(m_res);

	if (0 != mysql_query(m_mySql, sql.c_str()))
	{
		errNo = mysql_errno(m_mySql);

		std::cout<<"DB GetResult£ºError Num = "<<errNo<<", sql = "<<sql<<std::endl;

		if (errNo == 2006)
		{
			DBConnect(m_host, m_userName, m_pwd, m_dataBase, m_port, m_socket, m_clientFlag);
			mysql_query(m_mySql, m_CodeQuery.c_str());
			goto execute;
		}
		num = -1;
		m_res = mysql_store_result(m_mySql);
		return NULL;
	}

	errNo = 0;
	m_res = mysql_store_result(m_mySql);
	num = (uint16)mysql_affected_rows(m_mySql);
	return m_res;
}

uint32 DBSimple::EscapeString(std::string& str)
{
	//if(str.empty())
	//	return 0;
	//static uint32 buffSize = 1024;
	//static char* buf = new char[buffSize];

	//uint32 needSize = str.size()*2+1;
	//if (needSize > buffSize)
	//{
	//	delete[] buf;
	//	buffSize = needSize;
	//	buf = new char[buffSize];
	//}

	//if (mysql_real_escape_string(m_mySql, buf, str.c_str(), str.size()))
	//{
	//	str = buf;
	//	return str.size();
	//}

	//return 0;


	if(str.empty())
		return 0;

	char* buf = new char[str.size()*2+1];
	if (mysql_real_escape_string(m_mySql, buf, str.c_str(), str.size()))
	{
		str = buf;
		delete[] buf;
		return str.size();
	}
	delete[] buf;
	return 0;
}

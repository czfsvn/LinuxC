#include "MysqlCon.h"

MysqlCon::MysqlCon(std::string& ip, std::string& userName, std::string& pwd, std::string& dbName) 
	:m_host(ip)
	,m_dbName(dbName)
	,m_usr(userName)
	,m_pwd(pwd)
	,m_socket(NULL)
	,m_clientFlag(CLIENT_MULTI_STATEMENTS)
	,m_port(3306)
	,m_mysql(mysql_init(NULL))
{
	m_CodeQuery = "set names utf8";	
	Connection();
	//ExcuteSql(m_CodeQuery);
}

MysqlCon::~MysqlCon()
{
	mysql_close(m_mysql);
	m_isConnect = false;
}

void MysqlCon::Close()
{
	if (m_mysql != NULL)
	{
		mysql_close(m_mysql);
		m_mysql = NULL;
	}
	m_isConnect = false;
}

bool MysqlCon::ReConnect()
{
	Close();
	return Connection();
}

bool MysqlCon::CheckConnection()
{
	if (NULL == m_mysql)
	{
		return Connection();
	}
	return m_isConnect;
}

bool MysqlCon::Connection()
{
	if (NULL == m_mysql)
	{
		m_mysql = mysql_init(NULL);
	}

	if (NULL == m_mysql)
	{
		return false;
	}
	
    std::string charSet = "utf8";
	bool val  = true;
	mysql_options(m_mysql, MYSQL_OPT_RECONNECT, (char*)&val);
	mysql_options(m_mysql, MYSQL_SET_CHARSET_NAME, charSet.c_str());

	if (mysql_real_connect(m_mysql, m_host.c_str(), m_usr.c_str(), m_pwd.c_str(), m_dbName.c_str(), m_port, m_socket, m_clientFlag))
	{
		m_isConnect = true;
		//ExcuteSql(m_CodeQuery);
	}
	else
	{
		m_isConnect = false;
		uint32 err = mysql_errno(m_mysql);
		if (err)
		{
			std::cout<<"DB connection Err = "<<err<<std::endl;
		}
		Close();
		return false;
	}
	
	return true;	
}

int32 MysqlCon::ExcuteSql(std::string& sql)
{
	return ExcuteSql(sql.c_str());
}

int32 MysqlCon::ExcuteSqlEx(std::string& sql, MysqlRes& res)
{
	return ExcuteSqlEx(sql.c_str(), res);
}

int32 MysqlCon::ExcuteSql(const char* sql)
{
	if (false == CheckConnection())
	{
		std::cout << "ExcuteSql lost connection, sql = "<<sql << std::endl;
		return DB_ERR_CONN;
	}

	if (0 != mysql_query(m_mysql, sql))
	{
		uint32 err = mysql_errno(m_mysql);
		if (err == CR_SERVER_LOST || err == CR_SERVER_GONE_ERROR || err == CR_CONN_HOST_ERROR)
		{
			if (false == ReConnect())
			{
				std::cout << "ExcuteSql ReConnect failed, sql = "<<sql << std::endl;
				return DB_ERR_CONN;
			}

			if (0 != mysql_query(m_mysql, sql))
			{
				std::cout<< "err = "<<mysql_errno(m_mysql)<<", sql = "<<sql<<std::endl;
				return DB_ERR_UNKNOWN;
			}
		}
		else
		{
			std::cout<< "err = "<<mysql_errno(m_mysql)<<", sql = "<<sql<<std::endl;
			return DB_ERR_UNKNOWN;
		}
	}

	mysql_free_result(mysql_store_result(m_mysql));
	return (uint32)mysql_affected_rows(m_mysql);
}

int32 MysqlCon::ExcuteSqlEx(const char* sql, MysqlRes& res)
{
	if (false == CheckConnection())
	{
		std::cout << "ExcuteSqlEx lost connection, sql = "<<sql << std::endl;
		return DB_ERR_CONN;
	}

	if (0 != mysql_query(m_mysql, sql))
	{
		uint32 err = mysql_errno(m_mysql);
		if (err == CR_SERVER_LOST || err == CR_SERVER_GONE_ERROR || err == CR_CONN_HOST_ERROR)
		{
			if (false == ReConnect())
			{
				std::cout << "ExcuteSqlEx ReConnect failed, sql = "<<sql << std::endl;
				return DB_ERR_CONN;
			}

			if (0 != mysql_query(m_mysql, sql))
			{
				std::cout<< "err = "<<mysql_errno(m_mysql)<<", sql = "<<sql<<std::endl;
				return DB_ERR_UNKNOWN;
			}
		}
		else
		{
			std::cout<< "err = "<<mysql_errno(m_mysql)<<", sql = "<<sql<<std::endl;
			return DB_ERR_UNKNOWN;
		}
	}

	if (res.m_res != NULL)
	{
		mysql_free_result(res.m_res);
	}

	res.m_res = mysql_store_result(m_mysql);
	if (NULL == res.m_res)
	{
		if (0 == mysql_field_count(m_mysql))
		{
			return DB_NO_RECORD;
		}
		else
		{
			return DB_ERR_UNKNOWN;
		}

		return DB_HAS_RECORD;
	}
	else
	{
		if (mysql_num_rows(res.m_res) != 0)
		{
			return DB_HAS_RECORD;
		}
		else
		{
			return DB_NO_RECORD;
		}
	}

	return DB_ERR_UNKNOWN;
}

uint32 MysqlCon::EscapeString(std::string& str)
{
	if(str.empty())
		return 0;

	char* buf = new char[str.size()*2+1];	
	if (mysql_real_escape_string(m_mysql, buf, str.c_str(), str.size()))
	{
		str = buf;
		delete[] buf;
		return str.size();
	}
	delete[] buf;
	return 0;
}

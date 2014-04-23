#include "DBMgr.h"

DBMgr::DBMgr()
{
	//m_pingNode.Register(1, 1*3600*1000, this, &DBMgr::_OnTimer, true);
}

DBMgr::~DBMgr()
{
	//YU0923
	//for (int32 idx = 1 ; idx  <= m_dbMap.GetMaxIndex() ; ++idx)
	for (int32 idx = 1 ; idx  <= 3 ; ++idx)
	{
		if (m_dbMap[idx])
		{
			delete m_dbMap[idx] ;
		}
	}
}

bool DBMgr::CreateCon(uint16 conId, std::string &ip, std::string &usr, std::string &pass, std::string &db)
{
	DBSimple* dbSimp = m_dbMap[conId];
	if (dbSimp == NULL)
	{
		DBSimple* dbSimp = new DBSimple(ip, usr, pass, db);
		if (dbSimp->GetState())
		{
			m_dbMap.insert(std::make_pair(conId, dbSimp));
            return true;
		}
		delete dbSimp;
		return false;
	}
	return false;
}

DBSimple* DBMgr::GetDBSimple(uint16 conId)
{
	if (!m_dbMap[conId])
		return NULL;
	return m_dbMap[conId];
}

void DBMgr::_OnTimer(const uint32 eId)
{
	std::string query = "select 1";
	int32 num = 0, errNo = 0;
	//uint16 maxId = m_dbMap.GetMaxIndex();
	uint16 maxId = 3;
	for (uint16 idx = 0; idx <= maxId; idx++)
	{
		DBSimple* con = m_dbMap[idx];
		if (con)
			con->GetResult(query, num, errNo);
	}
	//m_pingNode.Start(0, 1*3600*1000);
}

MYSQL_RES* DBMgr::GetResult( string& sql, int32& num, int32& errNo,uint16 conId )
{
	DBSimple* con = GetDBSimple(conId);
	if (!con)
	{
		std::cout <<"ExecuteSql failed"<<sql<<std::endl;
		return NULL;
	}

	return con->GetResult(sql,num,errNo);
}

uint32 DBMgr::EscapeString( std::string& str )
{
	DBSimple* con = NULL;
	for (uint16 idx = 0; idx != 3; ++idx )
	{
		con = GetDBSimple(idx);
		if (con)
		{
			return con->EscapeString(str);
		}
	}
	return 0;
}

#ifndef DBMGR_H_20100713
#define DBMGR_H_20100713

#include <map>
#include "DBSimple.h"


class DBMgr
{
public:
	DBMgr();
	~DBMgr();

	bool CreateCon(uint16 conId, std::string&ip, std::string&usr, std::string&pass, std::string&db);

	DBSimple* GetDBSimple(uint16 conId);
	MYSQL_RES* GetResult(string& sql, int32& num, int32& errNo,uint16 conId);
	uint32 EscapeString(std::string& str);

protected:
	void _OnTimer(const uint32 eId);
private:
	//DBSimpMap m_dbMap;
	//FastMap<DBSimple*, 10> m_dbMap;
    std::map<uint32, DBSimple*> m_dbMap;
	//TTimerNode<DBMgr> m_pingNode;
};


#endif

#include "Engine.h"
#include "Common.h"

#define     MAX_INSERT_CNT      100
#define     MAX_STR_SIZE        1000
#define     MAX_IDX             10000000
/*
#define     IP                  "127.0.0.1"
#define     USER                "czf"
#define     PWD                 "123"
#define     DB                  "test"
*/

Engine::Engine()
{
    std::string IP        = "192.168.122.230";
    std::string USER      = "czf";
    std::string PWD       = "123";
    std::string DB        = "test";
    m_conn = new MysqlCon(IP, USER, PWD, DB);
}

Engine::~Engine()
{

}

void Engine::Run(uint32 type)
{
    FTime("Run"_S(__LINE__)_S(type));

    _CreateTables(type);
    _SingleInsert(type);
    _SingleUpdate(type);
}

void Engine::RunCom(uint32 type)
{
    _CreateTables(type);
    _ComInsert(type);
}

void Engine::_Init(uint32 type)
{

}

void Engine::_CreateTables(uint32 type)
{
    std::cout<< "\nEngine::_CreateTables\ttype = "<<type<<std::endl;
    FTime("Run"_S(__LINE__)_S(type));

    std::string engine = "MyISAM";
    std::string table = "TEST_TABLE_" + IntBStr(type);
    if (type == OP_I_SINGLE || type == OP_I_COMPLEX)
        engine = "InnoDB";

    std::string check = "DROP TABLE IF EXISTS ";
    check += table;

    std::string create = "CREATE TABLE ";
    create += table;
    create += "(";
    create += "ID int(10) unsigned NOT NULL default '0',";
    create += "NAME varchar(33) NOT NULL default '',";
    create += "COUNTRY int(10) unsigned NOT NULL default '0',";
    create += "SEPTID int(10) unsigned NOT NULL default '0',";
    create += "POINT int(10) unsigned NOT NULL default '0',";
    create += "RECORDSEC int(10) unsigned NOT NULL default '0',";
    create += "DAYMAX int(10) unsigned NOT NULL default '0',";
    create += "PRIMARY KEY (`ID`)";
    create += ")ENGINE=";
    create += engine;
    create += ";";

    m_conn->ExcuteSql(check);
    m_conn->ExcuteSql(create);
}

void Engine::_SingleInsert(uint32 type)
{
    std::cout<< "\nEngine::_SingleInsert\ttype = "<<type<<std::endl;

    FTime("Run"_S(__LINE__)_S(type));

    std::string table = "TEST_TABLE_" + IntBStr(type);
    for (uint32 idx = 1; idx <= MAX_IDX; ++idx)
    {
        char sql[MAX_STR_SIZE] = {0};
        snprintf(sql, MAX_STR_SIZE,
                "INSERT INTO %s VALUES(%u, 'Test_%u', %u, %u, %u, %u, %u);",
                table.c_str(),
                idx, idx, idx, idx, idx, idx, idx);

        m_conn->ExcuteSql(sql);
    }
}

void Engine::_SingleUpdate(uint32 type)
{
    std::cout<< "\nEngine::_SingleUpdate\ttype = "<<type<<std::endl;
    FTime("Run"_S(__LINE__)_S(type));

    std::string table = "TEST_TABLE_" + IntBStr(type);
    for (uint32 idx = 1; idx <= MAX_IDX; ++idx)
    {
        char sql[MAX_STR_SIZE] = {0};
        snprintf(sql, MAX_STR_SIZE, "UPDATE %s set COUNTRY=%u, SEPTID=%u, POINT=%u, RECORDSEC=%u, DAYMAX=%u WHERE ID=%u;",
                table.c_str(), idx + 1, idx + 1, idx + 1,
                idx + 1, idx + 1, idx);
        m_conn->ExcuteSql(sql);
    }
}
void Engine::_ComInsert(uint32 type)
{
    std::cout<< "\nEngine::_SingleInsert\ttype = "<<type<<std::endl;

    FTime("Run"_S(__LINE__)_S(type));

    std::string table = "TEST_TABLE_" + IntBStr(type);
    std::string sql = "INSERT INTO ";
    sql += table;
    sql += " VALUES";

    uint32 cnt = 0;
    for (uint32 idx = 1; idx <= MAX_IDX; ++idx)
    {
        if (cnt >= MAX_INSERT_CNT)
        {
            sql += ";";
            m_conn->ExcuteSql(sql);
            sql = "INSERT INTO ";
            sql += table;
            sql += " VALUES";
            cnt = 0;
        }

        if (cnt)
            sql += ",";

        std::string str = IntBStr(idx);
        sql += "(" + str + ", 'Test_" + str + "',";
        sql += str + ", " + str + ",";
        sql += str + ", " + str + ",";
        sql += str + ")";

        cnt ++;
    }

    if (!cnt)
    {
        sql += ";";
        m_conn->ExcuteSql(sql);
    }
}

/*
void Engine::_ComInsert(uint32 type)
{
    std::cout<< "\nEngine::_SingleInsert\ttype = "<<type<<std::endl;

    FTime("Run"_S(__LINE__)_S(type));

    std::string insSql;
    uint32 cnt = 0;
    std::string table = "TEST_TABLE_" + IntBStr(type);
    for (uint32 idx = 1; idx <= MAX_IDX; ++idx)
    {
        char sql[MAX_STR_SIZE] = {0};
        snprintf(sql, MAX_STR_SIZE,
                "INSERT INTO %s VALUES(%u, 'Test_%u', %u, %u, %u, %u, %u)",
                table.c_str(),
                idx, idx, idx, idx, idx, idx, idx);

        insSql += sql;
        cnt ++;

        if (cnt >= MAX_INSERT_CNT)
        {
            insSql += ";";
            m_conn->ExcuteSql(insSql);
            cnt = 0;
            insSql = "";
        }
        else
            insSql += ",";
    }

    if (cnt >= MAX_INSERT_CNT)
    {
        insSql += ";";
        m_conn->ExcuteSql(insSql);
    }
}
*/



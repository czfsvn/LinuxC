#include "Mysqlres.h"

MysqlRes::MysqlRes() : m_row(NULL), m_res(NULL), m_adwlengths(NULL) {}

MysqlRes::~MysqlRes()
{
    if (m_res != NULL)
    {
        mysql_free_result(m_res);
    }
}

uint32 MysqlRes::GetRecordCount()
{
    return ((m_res != NULL) ? (mysql_num_rows(m_res)) : 0);
}

uint32 MysqlRes::GetFieldCount()
{
    return ((m_res != NULL) ? ( uint32 )mysql_num_fields(m_res) : 0);
}

bool MysqlRes::GetRecord()
{
    if (NULL == m_res)
        return false;

    m_row = mysql_fetch_row(m_res);
    if (m_row != NULL)
    {
        m_adwlengths = mysql_fetch_lengths(m_res);
    }

    return (m_row != NULL);
}

const char* MysqlRes::GetFieldVal(uint32 idx)
{
    if (NULL == m_row)
    {
        return NULL;
    }
    if (idx >= GetFieldCount())
    {
        return NULL;
    }
    return m_row[ idx ];
}

uint32 MysqlRes::GetFieldLength(uint32 idx)
{
    if (NULL == m_row)
    {
        return 0;
    }
    if (idx >= GetFieldCount())
    {
        return 0;
    }
    return m_adwlengths[ idx ];
}

void MysqlRes::Release()
{
    if (m_res != NULL)
    {
        mysql_free_result(m_res);
    }
    m_res = NULL;
    m_row = NULL;

    delete this;
}

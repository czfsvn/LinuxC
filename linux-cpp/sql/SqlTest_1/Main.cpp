#include <iostream>
#include "MysqlCon.h"


using namespace std;



//先测试数据库引擎对比

int main()
{
    std::string   IP="127.0.0.1";
    std::string   USER="czf";
    std::string   PWD="123456";
    std::string   DB="test";
    MysqlCon* conn = new MysqlCon(IP, USER, PWD, DB);
    return 0;
}

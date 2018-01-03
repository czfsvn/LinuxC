#include <iostream>

#include "MysqlCon.h"

using namespace std;

void TestMysql()
{
    std::string user="czf";
    std::string db ="test";
    std::string host ="localhost";
    std::string pwd ="123456";

    MysqlCon* con = new MysqlCon(host, user, pwd, db);
}

int main()
{
    TestMysql(); 
    std::cout<<"Hello, main"<<std::endl;
    return 0;
}

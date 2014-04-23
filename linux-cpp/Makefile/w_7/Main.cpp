#include <iostream>
#include "DBMgr.h"

using namespace std;

int main()
{
    std::string server    = "192.168.0.213";
    std::string user      = "czf";
    std::string pwd       = "123";
    std::string database  = "game";

    DBMgr* mgr = new DBMgr();
    return 0;
}

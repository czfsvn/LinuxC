#include <iostream>
#include "Engine.h"


using namespace std;



//先测试数据库引擎对比

int main()
{
    Engine::get_mutable_instance().Run(OP_M_SINGLE);
    Engine::get_mutable_instance().Run(OP_I_SINGLE);
    return 0;
}

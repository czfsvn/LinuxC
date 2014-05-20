#include <iostream>
#include "Engine.h"


using namespace std;



//先测试数据库引擎对比

int main()
{
#ifdef D_CZF
    std::cout<<"D_CZF" << std::endl;
#else
    std::cout<<"D_CZF NOT" << std::endl;
#endif

    sleep(5);
    /*
    Engine::get_mutable_instance().Run(OP_M_SINGLE);
    Engine::get_mutable_instance().Run(OP_I_SINGLE);
    */
    Engine::get_mutable_instance().RunCom(OP_M_COMPLEX);
    Engine::get_mutable_instance().RunCom(OP_I_COMPLEX);
    return 0;
}

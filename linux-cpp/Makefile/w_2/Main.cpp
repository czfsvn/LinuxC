#include <iostream>
#include "Test.h"
#include "Orgin.h"

using namespace std;


int main()
{
    Test t;
    t.Init(5);
    std::cout<<"段落测试段落hh"<<std::endl;
    Orgin* o = new Orgin(4);
    o->Print();
    //std::cout<<"段落测试段落hh"<<std::endl;
    std::cout<<"段落测试段落hh"<<std::endl;
    return 1;
}

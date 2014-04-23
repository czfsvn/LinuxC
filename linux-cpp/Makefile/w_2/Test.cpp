#include <iostream>
#include "Test.h"
#include "Orgin.h"

Test::Test()
{
    std::cout<<"construct"<<std::endl;
}

Test::~Test()
{
    std::cout<<"destruct"<<std::endl;
}

void Test::Init(int num)
{
    if (num <= 0)
    {
        std::cout<<"Init num < 0"<<std::endl;
        return;
    }

    std::cout<<"Init num ="<<num<<std::endl;

    /*
    for (int i = 0; i < num; ++i)
        m_orginList.push_back(new Orgin(i));
        */
}

void Test::Print()
{
    std::cout<<"Test::Print begin"<<std::endl;
    /*
    for (std::list<Orgin*>::iterator it = m_orginList.begin();
            it != m_orginList.end(); ++it)
    {
        Orgin* o= *it;
        if (!o)
            continue;

        o->Print();
    }
    */
    std::cout<<"Test::Print end"<<std::endl;
}

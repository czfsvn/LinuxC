#include <iostream>
#include "Orgin.h"

using namespace std;

Orgin::Orgin(int val) : m_val(val)
{
    std::cout<<"orgin construct m_val = "<<m_val<<std::endl;
}

Orgin::~Orgin()
{
    std::cout<<"orgin destruct"<<std::endl;
}

void Orgin::Print()
{
    std::cout<<"orgin Print val = "<<m_val<<std::endl;
}

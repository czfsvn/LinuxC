#include <iostream>

using namespace std;

#define PRINT   std::cout << __func__ << ", line:" << __LINE__ << std::endl;

struct Copyable
{
    Copyable()  
    {
        PRINT;
    }

    Copyable(const Copyable& o)  
    {
        PRINT;
    }

    Copyable(Copyable& o)  
    {
        PRINT;
    }
    Copyable(Copyable&& o)  
    {
        PRINT;
    }
};

Copyable   ReturnRValue()   
{   
    PRINT;
    return Copyable();  
}
void AcceptVal(Copyable)    
{
    PRINT;
}

void AcceptRef(const Copyable&)
//void AcceptRef(Copyable&)
{
    PRINT;
}

void AcceptRValRef(Copyable&& s)
{
    PRINT;
}

int main()
{
    std::cout << "Pass By Val:" << std::endl;
    AcceptVal(ReturnRValue());
    std::cout << "\nPass By Ref:" << std::endl;
    AcceptRef(ReturnRValue());
    std::cout << "\nPass By right Ref:" << std::endl;
    AcceptRValRef(ReturnRValue());          // 与AcceptRef 一样的调用输出 
}

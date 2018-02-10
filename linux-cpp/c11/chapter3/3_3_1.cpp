#include <iostream>

using namespace std;

#define PRINT   std::cout << "func: " << __func__ << ": " << __LINE__ << std::endl;

class HasPtrMem
{
    public:
        HasPtrMem() : d(new int(0)) 
        {
            PRINT; 
        }
        HasPtrMem(const HasPtrMem& h) :
            d(new int(*h.d))    
        {
            PRINT; 
        }
        ~HasPtrMem()    
        {   
            delete d;   
            PRINT;
        }

        int* d;
};

int main()
{
    HasPtrMem   a;
    HasPtrMem   b(a);
    std::cout << "a.d : " << *a.d << std::endl; 
    std::cout << "b.d : " << *b.d << std::endl; 
    return 0;
}


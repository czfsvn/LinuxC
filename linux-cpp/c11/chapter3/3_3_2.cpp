#include <iostream>
using namespace std;

#define PRINT   std::cout << "func: " << __func__ << ": " << __LINE__ << std::endl;

class HasPtrMem 
{
    public:
        HasPtrMem(): d(new int(0)) 
        {
            PRINT;
        }
        HasPtrMem(HasPtrMem& h):
            d(new int(*h.d)) // 拷贝 构造 函数， 从 堆 中 分配 内存， 并用* h. d 初始化
        {
            PRINT;
        }

        ~HasPtrMem()
        {
            PRINT;
            delete d;
        }

        int *d;
};

int main()
{
    HasPtrMem   a;
    HasPtrMem   b(a);
    std::cout << "a.d : " << *a.d << std::endl; 
    std::cout << "b.d : " << *b.d << std::endl; 
    return 0;
}


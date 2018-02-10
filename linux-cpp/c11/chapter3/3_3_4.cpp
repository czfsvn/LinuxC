#include <iostream>

using namespace std;

class HasPtrMem
{
    public:
        HasPtrMem() : d(new int(4))
        {
            std::cout << "consturct : " << ++n_cstr << std::endl;
        }

        HasPtrMem(const HasPtrMem& h) : d(new int(*h.d))
        {
            std::cout << "copy consturct : " << ++n_cptr << std::endl;
        }

        HasPtrMem(HasPtrMem&& h) : d(h.d)
        {
            h.d = nullptr;
            std::cout << "move consturct : " << ++n_mvtr << std::endl;
        }

        ~HasPtrMem()
        {
            std::cout << "Desturct : " << ++n_dstr << std::endl;
        }

        int* d;
        static int n_cstr;
        static int n_dstr;
        static int n_cptr;
        static int n_mvtr;
};

int HasPtrMem::n_cstr = 0;
int HasPtrMem::n_dstr = 0;
int HasPtrMem::n_cptr = 0;
int HasPtrMem::n_mvtr = 0;

HasPtrMem   GetTemp()   
{   
    HasPtrMem h;
    std::cout << "resource from " << __func__ << ": " << hex << h.d << std::endl;
    return h; 
}

int main()
{
    HasPtrMem b = GetTemp();
    std::cout << "resource from " << __func__ << ": " << hex << b.d << std::endl;
    //HasPtrMem& a = GetTemp();         // 编译报错
    //const HasPtrMem& a = GetTemp();
    return 0;
}

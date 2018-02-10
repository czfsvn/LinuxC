#include <iostream>
#include <sstream>
#include <vector>

using namespace std;
#define PRINT   std::cout << "line: " << __LINE__ << std::endl;

class HugeMem {
    public:
        HugeMem(int size) : sz(size > 0 ? size : 1)
        {
            c = new int[sz];
            PRINT;
        }
        ~HugeMem()
        {
            PRINT;
            delete[] c;
        }
        HugeMem(const HugeMem& m) : sz(m.sz)
        {
            c = new int[sz];
            PRINT;
        }
        HugeMem(HugeMem&& hm) : sz(hm.sz)
                                , c(hm.c)
        {
            hm.c = nullptr;
            PRINT;
        }
        int* c;
        int sz;
};

class Moveable {
    public:
        Moveable(): i(new int(3)), h(1024)
        {
            PRINT;
        }
        ~Moveable()
        {
            delete i;
            PRINT;
        }
        Moveable(Moveable&& m): i(m.i), h(move(m.h))
        {
            m.i = nullptr;
            PRINT;
        }

        Moveable(const Moveable& m) : i(new int(*m.i)), h(m.h)
        {
            PRINT;
        }

        int* i;
        HugeMem h;
};

Moveable GetTemp()
{
    Moveable tmp = Moveable();
    //std::cout << hex << " Huge Mem from " << __func__
    //    << " @" << tmp.h.c << std::endl;
    return tmp;
}

void test_move()
{
    std::string str = "Hello";
    std::vector<std::string> v;
    //调用常规的拷贝构造函数，新建字符数组，拷贝数据
    v.push_back(str);
    std::cout << "After copy, str is \"" << str << "\"\n";
    //调用移动构造函数，掏空str，掏空后，最好不要使用str
    v.push_back(std::move(str));
    std::cout << "After move, str is \"" << str << "\"\n";
    std::cout << "The contents of the vector are \"" << v[0]
        << "\", \"" << v[1] << "\"\n";

    std::string a1 = "hello";
    //std::string&& a2 = a1;        // a1 是左值
    std::string&& a3 = std::move(a1);
    a1 = "word";
    std::string a4 = a3;
    std::cout << "After move, str is \"" << a4 << "\"\n";
}

int string_test()
{
    std::ostringstream oss;
    std::string str;
    for (int idx = 0; idx < 1000; idx++)
    {
        oss << " idx :" << idx;
    }
    str.append(oss.str());
    std::cout << "str : " << str << std::endl; 
}

int main()
{
    Moveable a(GetTemp());
    //std::cout << hex << " Huge Mem from " << __func__
    //    << " @" << a.h.c << std::endl;
    //string_test();
    test_move();
    return 0;
}

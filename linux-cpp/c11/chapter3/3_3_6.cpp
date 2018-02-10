#include <iostream>

using namespace std;

#define PRINT   std::cout << "line: " << __LINE__ << std::endl;

class Moveable
{
    public:

        Moveable(): i(new int(3))
    {
        PRINT;
    }

        ~Moveable()
        {
            delete i;
            PRINT;
        }

        Moveable(const Moveable& m): i(new int(*m.i))
        {
            PRINT;
        }

        Moveable(Moveable&& m) : i(m.i)
    {
        m.i = nullptr;
        PRINT;
    }


        int* i;
};

int main()
{
    Moveable a;
    Moveable b(a);
    Moveable c(move(a));
    Moveable d(c);
    Moveable e(move(c));
    std::cout << "a.i: " << *a.i << std::endl;
}

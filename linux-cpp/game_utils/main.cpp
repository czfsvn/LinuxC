#include <iostream>
#include "Switches.h"
#include "Debug.h"

class SingTest : public Singleton<SingTest>
{
public:
    void print()
    {
        if (sw::test_sw() == 1)
        {
            std::cout << "Hello, world  1 \n";
        }
        else
        {
            std::cout << "Hello, world  0\n";
        }
        std::cout << "Hello, world\n";
        debug(7);
    }
};

int main()
{
    SingTest::getMe().print();
    return 0;
}
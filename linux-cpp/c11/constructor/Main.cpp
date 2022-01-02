#include <iostream>

using namespace std;

namespace construct
{
    class Base
    {
    public:
        Base(int val) {}
    };

    class Tracer
    {
    public:
        Tracer()                = default;
        Tracer(const Tracer& t) = default;
        Tracer& operator=(const Tracer& t) = default;
        Tracer(Tracer&& t)                 = default;
        Tracer& operator=(Tracer&& t) = default;
        ~Tracer()                     = default;
    };

    void Test_1()
    {
        Base* aBase = new Base(1);

        Base* bBase = aBase;

        Tracer* t1 = new Tracer;
        Tracer* t2 = new Tracer(*t1);
        Tracer* t3 = new Tracer;
        Tracer* t4 = new Tracer;
    }
}  // namespace construct

namespace ns_test1
{
    class TestClass
    {
    public:
        TestClass()
        {
            std::cout << "constructor" << std::endl;
        }
        ~TestClass()
        {
            std::cout << "destructor" << std::endl;
        }
        TestClass& operator=(const TestClass& test)
        {
            std::cout << "assign constructor" << std::endl;
            return *this;
        }
        TestClass(const TestClass& test)
        {
            std::cout << "copy constructor" << std::endl;
        }
    };

    TestClass getTestClass()
    {
        return TestClass();
    }

    void test_main()
    {
        TestClass test = getTestClass();
        std::cout << "main" << std::endl;
    }
}  // namespace ns_test1

namespace ns_test2
{
    
}

int main()
{
    ns_test1::test_main();
    return 0;
}

#include <iostream>
#include <functional>

#define PRINT_FUNC std::cout << __FUNCTION__ << std::endl;

namespace ns_fu1
{
    void func()
    {
        PRINT_FUNC
    }

    class Foo
    {
    public:
        static int foo_func(int a)
        {
            PRINT_FUNC
            return a;
        }
    };

    class Bar
    {
    public:
        int operator()(int a)
        {
            PRINT_FUNC
            return a;
        }
    };

    void test()
    {
        std::function<void(void)> f1 = func;
        f1();

        std::function<int(int)> f2 = Foo::foo_func;
        std::cout << f2(12) << std::endl;

        Bar bar;
        std::function<int(int)> f3 = bar;
        std::cout << f3(12) << std::endl;
    }

} // namespace ns_fu1

namespace ns_fu2
{
    typedef std::function<void()> CALLBACK;
    class Base
    {
        CALLBACK callback_;

    public:
        Base(const CALLBACK &cb) : callback_(cb) {}

        void notify() { callback_(); }
    };

    class Foo
    {
    public:
        void operator()()
        {
            PRINT_FUNC
        }
    };

    void test()
    {
        Foo foo;
        Base base(foo);
        base.notify();
    }
} // namespace ns_fu2

namespace ns_fu3
{
    void call_when_even(int x, const std::function<void(int)> &func)
    {
        if (!(x & 1)) // <====> x % 2 == 0
        {
            func(x);
        }
    }

    void output(int x) { std::cout << "x: " << x << std::endl; }

    void test()
    {
        for (uint32_t idx = 0; idx < 10; ++idx)
        {
            call_when_even(idx, output);
        }
    }
} // namespace ns_fu3

int main()
{
    std::cout << "Hello world\n";
    //ns_fu1::test();
    //ns_fu2::test();
    ns_fu3::test();
    return 0;
}
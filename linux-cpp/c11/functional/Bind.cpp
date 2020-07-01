#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>

/*std::bind 
    std::bind 用来将可调用对象与其参数一起进行绑定。
        绑定后的结果可以使用std::function 来进行保存，
        并延迟调用到任何我们需要的时候
        1）将可调用对象与其参数一起绑定成一个仿函数
        2）讲多元（多个参数）棵调用对象转成一元或者（n-1）元可调用对象
            即只绑定部分参数
*/
#define PRINT_FUNC std::cout << "========= " << __FUNCTION__ << " ================= " << std::endl;

namespace ns_b1
{
    void output(int32_t x) { std::cout << x << " "; }
    void output2(int32_t x) { std::cout << x + 2 << " "; }
    void output3(int32_t x, uint32_t y) { std::cout << x + y << " "; }

    void call_when_even(int32_t x, const std::function<void(int)> &func)
    {
        if (!(x & 1))
        {
            func(x);
        }
    }

    void test()
    {
        {
            auto fr = std::bind(output, std::placeholders::_1);
            for (uint32_t idx = 0; idx < 10; idx++)
                call_when_even(idx, fr);
            std::cout << std::endl;
        }
        {
            auto fr = std::bind(output2, std::placeholders::_1);
            for (uint32_t idx = 0; idx < 10; idx++)
                call_when_even(idx, fr);
            std::cout << std::endl;
        }
        {
            auto fr = std::bind(output3, std::placeholders::_1, std::placeholders::_1);
            for (uint32_t idx = 0; idx < 10; idx++)
                call_when_even(idx, fr);
            std::cout << std::endl;
        }
    }

} // namespace ns_b1

namespace ns_b2
{
    /*
    std::placeholders 决定空位参数将会属于调用发生时的第几个参数
    */
    void output(int32_t x, uint32_t y) { std::cout << x << ", " << y << "\n"; }

    void test()
    {
        std::bind(output, 1, 2)(); // 调用output(1, 2)

        std::bind(output, std::placeholders::_1, std::placeholders::_2)(5, 6); // 掉能用 output(5, 6)
        std::bind(output, std::placeholders::_2, std::placeholders::_1)(5, 6); // 掉能用 output(6, 5)

        std::bind(output, std::placeholders::_1, 2)(4); // 调用output(4, 2)   参数4被吞掉了
        //std::bind(output, std::placeholders::_1)(3, 4); // 编译不过
        std::bind(output, 2, std::placeholders::_1)(6); // 调用output(2, 6)

        //std::bind(output, 2, std::placeholders::_2)(3); // 编译不过
        std::bind(output, 2, std::placeholders::_2)(3, 4); // 调用output(2, 4) 参数3 倍吞掉了
        std::bind(output, 2, std::placeholders::_1)(3, 4); // 调用output(2, 3) 参数4 倍吞掉了
    }
} // namespace ns_b2

namespace ns_b3
{
    class Base
    {
    public:
        uint32_t val = 0;
        void output(int32_t x, uint32_t y) { std::cout << x << ", " << y << "\n"; }
    };
    void test()
    {
        Base base;
        std::function<void(int32_t, int32_t)> fr =
            std::bind(&Base::output, &base, std::placeholders::_1, std::placeholders::_2);
        fr(1, 2);

        std::function<uint32_t &(void)> fr_value = std::bind(&Base::val, &base);
        fr_value() = 123;
        std::cout << "value: " << base.val << std::endl;
    }
} // namespace ns_b3

namespace ns_b4
{
    void test_1()
    {
        PRINT_FUNC
        std::vector<uint32_t> vec = {10, 20, 30, 40, 50, 60};
        // 查找大于30的数量
        size_t count1 = std::count_if(vec.begin(), vec.end(),
                                      std::bind1st(std::less<uint32_t>(), 30));

        std::cout << "count : " << count1 << std::endl;

        // 查找小于30的数量
        size_t count2 = std::count_if(vec.begin(), vec.end(),
                                      std::bind2nd(std::less<uint32_t>(), 30));

        std::cout << "count : " << count2 << std::endl;
    }

    void test_2()
    {
        PRINT_FUNC
        using std::placeholders::_1;
        std::vector<uint32_t> vec = {10, 20, 30, 40, 50, 60};
        size_t count1 = std::count_if(vec.begin(), vec.end(),
                                      std::bind(std::less<uint32_t>(), 30, _1));

        std::cout << "count : " << count1 << std::endl;

        // 查找小于30的数量
        size_t count2 = std::count_if(vec.begin(), vec.end(),
                                      std::bind(std::less<uint32_t>(), _1, 30));

        std::cout << "count : " << count2 << std::endl;
    }

    void test_3()
    {
        PRINT_FUNC
        using std::placeholders::_1;
        std::vector<uint32_t> vec = {10, 20, 30, 40, 50, 60};
        auto func = std::bind(std::logical_and<bool>(),
                              std::bind(std::greater<int>(), _1, 20),   // 大于20
                              std::bind(std::less_equal<int>(), _1, 50) // 小于等于50
        );

        //返回大于20 小于等于50的数量
        size_t count2 = std::count_if(vec.begin(), vec.end(), func);

        std::cout << "count : " << count2 << std::endl;
    }

    void test()
    {
        test_1();
        test_2();
        test_3();
    }
} // namespace ns_b4

int main()
{
    PRINT_FUNC
    //ns_b1::test();
    //ns_b2::test();
    //ns_b3::test();
    ns_b4::test();
    return 0;
}
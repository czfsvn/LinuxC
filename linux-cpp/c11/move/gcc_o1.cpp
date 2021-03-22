#include <algorithm>
#include <iostream>

const int& Min(const int& a, const int& b)
{
    // std::cout << "a" << std::endl;
    return a > b ? b : a;
}

#define PRINT_LINE     std::cout << "line: " << __LINE__ << std::endl;
#define PRINT_INTERVAL std::cout << __FUNCTION__ << "=========================" << std::endl;

namespace ns_ret_ref
{
    class TestStruct
    {
    public:
        TestStruct()
        {
            PRINT_LINE;
        }
        ~TestStruct()
        {
            PRINT_LINE;
        }

        TestStruct(const TestStruct& lhs)
        {
            PRINT_LINE;
        }

        TestStruct& operator=(const TestStruct& lhs)
        {
            PRINT_LINE;
            return *this;
        }
    };

    TestStruct& getSTruct()
    {
        static TestStruct rt;
        return rt;
    }

    void test()
    {
        PRINT_INTERVAL;
        TestStruct rt = getSTruct();
    }
    void test_ref()
    {
        PRINT_INTERVAL
        TestStruct& rt = getSTruct();
    }

    void test_const_ref()
    {
        PRINT_INTERVAL;
        const TestStruct& rt = getSTruct();
    }

    void main()
    {
        test();
        test_ref();
        test_const_ref();
        test();
        test_ref();
        test_const_ref();
        PRINT_INTERVAL;
        return;
    }

}  // namespace ns_ret_ref

void Test()
{
    auto&& val = Min(4, 8);
    std::cout << "val: " << val << std::endl;
}

int main()
{
    Test();
    ns_ret_ref::main();
    return 0;
}

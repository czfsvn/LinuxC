#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std;

template <typename CONT>
void Print_Element(const CONT& cont, const char* str)
{
    std::cout << "=============== Print_Element " << str << std::endl;
    typename CONT::const_iterator it = cont.begin();
    for (; it != cont.end(); it++)
    {
        std::cout << *it << "\n";
    }
    std::cout << "\n=============== Print_Element End\n" << std::endl;
}

bool comp(const std::string& s1, const std::string& s2)
{
    return  s1.size() < s2.size();
}

void Test_1()
{
    std::vector<std::string> v{"This", "is", "a", "predicate", "."};
    std::sort(v.begin(), v.end());
    Print_Element(v, "Test_1::v");

    std::sort(v.begin(), v.end(), comp);
    Print_Element(v, "Test_1::v after comp");

    std::sort(v.begin(), v.end(), [](const std::string& s1, const std::string& s2)
            {
                return s1.size() > s2.size();
            });
    Print_Element(v, "Test_1::v after lambda");
}

void Test_2()
{
    auto add = [](int a, int b) {   return a + b;   };
    std::cout << "add(1, 2) = " << add(1, 2) << std::endl;
    
    // 留意这种用法
    int result = [](int a, int b) { return a * b;   }(2, 3);
    std::cout << result << std::endl;
}


bool istarget(const std::string& s)
{
    return s.size() > 2;
}

class IsTarget
{
    unsigned int max;
    public:
        IsTarget(int m) : max(m)    {}
        bool operator() (const std::string& s)
        {
            return s.size() < max;
        }
};

bool istarget_(const std::string& s, int sz)
{
    return s.size() < sz;
}

void Test_7()
{
}
void Test_3()
{
    std::vector<std::string> v{"This", "is", "a", "predicate", "."};
    auto found = std::find_if(v.begin(), v.end(), istarget);
    std::cout << "Test_3::found1: " << *found << std::endl;


    // IsTarget
    std::string test("Hi");
    IsTarget it(2);
    std::cout << it(test) << std::endl;

    found = std::find_if(v.begin(), v.end(), IsTarget(2));
    std::cout << "Test_3::found2: " << *found << std::endl;

    int max = 2;
    found = std::find_if(v.begin(), v.end(), [max](const std::string& s)
            {
                return s.size() < max;
            });
    std::cout << "Test_3::found3: " << *found << std::endl;

    {
        std::vector<string> v{"This","is","a","predicate","."};
        auto isTarget = std::bind(istarget_, std::placeholders::_1, 3);
        found = std::find_if(v.begin(), v.end(), isTarget);
        std::cout << "Test_3::found4: " << *found << std::endl;
    }
}

void Test_4()
{
    {   // 值捕获--multiply 中的factor不随 外部的fafactor变化
        int factor = 2;
        auto multiply = [factor](int value)
        {
            std::cout << "factor: " << factor << std::endl;
            return factor *  value;
        };
        std::cout << "first call: " << multiply(3) << std::endl;
        factor = 4;
        std::cout << "second call: " << multiply(3) << std::endl;
    }
    
    {   // 引用捕获--multiply 中的factor随 外部的factor变化
        int factor = 2;
        auto multiply = [&factor](int value)
        {
            std::cout << "factor: " << factor << std::endl;
            return factor *  value;
        };
        std::cout << "first call: " << multiply(3) << std::endl;
        factor = 4;
        std::cout << "second call: " << multiply(3) << std::endl;
    }
}

void Test_5()
{
    std::vector<int> score{45,70,56,86,28,60,90};
    auto found = std::find_if(score.begin(), score.end(), [](int v) {   return v >= 60; });
    std::cout << "first found: " << *found << std::endl;

    {   // 值捕获时，counter不会被改变
        int counter = 2;
        found = std::find_if(score.begin(), score.end(), [counter](int v) mutable 
                {
                    return (v >= 60) && (--counter == 0);
                });
        std::cout << "second found: " << *found << ", counter: " << counter << std::endl;
    }
    {   // 引用捕获时，counter会被改变
        int counter = 2;
        found = std::find_if(score.begin(), score.end(), [&counter](int v) mutable 
                {
                    return (v >= 60) && (--counter == 0);
                });
        std::cout << "second found: " << *found << ", counter: " << counter << std::endl;
    }
}

void Test_6()
{
    //auto factorial = [&](int n) -> int          //编译不过
    function<int(int)> factorial = [&](int n) -> int
    {
        if (n < 2)
            return 1;
        return n * factorial(n - 1);
    };

    std::cout << "factorial(6): " << factorial(6) << std::endl;
}

auto test_3_lbd = [](int a, int b)->int
{
    return a * b;
};

auto test_2_lbd = [](int a, int b)->int
{
    auto test_1_lbd = [](int a, int b)->int
    {
        return a + b;
    };

    int sum = test_1_lbd(a, b);
    int mul = test_3_lbd(a, b);
    return test_1_lbd(mul, sum);
};

void Test_8()
{
    std::cout << test_2_lbd(2, 4) << std::endl;
}


int main()
{
    Test_8();
    return 0;
}

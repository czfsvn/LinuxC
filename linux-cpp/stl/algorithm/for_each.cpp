/*
 * examples from: http://blog.csdn.net/u014613043/article/details/50619254
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std;

template <typename T>
void PrintEle(T& cont, const char* str)
{
    std::cout << "PrintEle "<<str << ": ";
    typename T::const_iterator it = cont.cbegin();
    for (; it != cont.cend(); it++)
    {
        std::cout << *it << ", ";
    }
    std::cout << std::endl;
}

template <typename T>
void print(T& t)
{
    std::cout << t << std::endl;
}

void print_(int x, const char* str)
{
    std::cout << str << ":" << x << std::endl;
}

namespace ns_test1
{
    struct Play1
    {
        void operator () (int i)
        {
            std::cout << i << std::endl;
        }
    };

    struct Play2
    {
        Play2(std::string s) : str(s)   {}
        void operator () (int i)
        {
            std::cout << str << ":" << i << std::endl;
        }
        std::string str;
    };

    void Test_1()
    {
        int a[] = {1, 2, 3, 4};
        std::vector<int> v(a, a + sizeof(a) / sizeof(int)); 

        // use template 
        PrintEle(v, "ns_test1::Test_1");

        // use function template
        std::for_each(v.begin(), v.end(), print<int>);

        // use function , with other params
        std::for_each(v.begin(), v.end(), bind2nd(ptr_fun(print_), "bind2nd"));

        // use function , with other params
        std::for_each(v.begin(), v.end(), std::bind(print_, std::placeholders::_1, "bind"));

        // use function object
        std::for_each(v.begin(), v.end(), Play1());
        std::for_each(v.begin(), v.end(), Play2("play2"));
    }
};

int main()
{
    ns_test1::Test_1();
    return 1;
}

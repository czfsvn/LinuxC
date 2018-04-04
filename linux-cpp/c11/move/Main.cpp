#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

#define PRINT_LINE  std::cout << "func:" << __func__ << ":" << __LINE__ << std::endl;

template <typename T>
void Print_Vec(T x)
{
    std::cout << "Print_Vec : " << x << std::endl;
    PRINT_LINE;
}
/*
template <>
void Print_Vec(int x)
{
    std::cout << "Print_Vec : " << x << std::endl;
    PRINT_LINE;
}

template <>
void Print_Vec(std::string x)
{
    std::cout << "Print_Vec : " << x << std::endl;
    PRINT_LINE;
}

template <>
void Print_Vec(const char* x)
{
    std::cout << "Print_Vec : " << x << std::endl;
    PRINT_LINE;
}
*/

template <typename T>
void PrintSeqCont(T& t, std::string str)
{
    std::cout << "elements: ";
    typename T::const_iterator it = t.begin();
    for (; it != t.end(); it++)
    {
        std::cout << *it << ", ";
    }
    std::cout << std::endl;
}

template <typename T>
void Print_Vec1(T x)
{
    std::cout << "Print_Vec : " << x << std::endl;
}

/*
//template <>
void Print_Vec1(std::string& x)
{
    std::cout << "Print_Vec : " << x << std::endl;
}
//template <>
//void Print_Vec1(const char* x)
//{
//   std::cout << "Print_Vec : " << x << std::endl;
//}

*/

void Test_move()
{
    std::string str = "Hello";
    std::vector<std::string> v;
    v.push_back(str);
    std::cout << "After copy, str is " << str << std::endl;
    std::for_each(v.begin(), v.end(), Print_Vec1<std::string>);

    v.push_back(std::move(str));
    std::cout << "After move, str is " << str << std::endl;
    std::for_each(v.begin(), v.end(), Print_Vec1<std::string>);
}

void Test_1()
{
    std::vector<int> v = {1,2,3,4};
    for (auto && i : v)
    {
        std::cout << "i : " << i << std::endl;
    }
    std::cout << "\nprint again: \n";
    for (auto && i : v)
    {
        std::cout << "i : " << i << std::endl;
    }
    PrintSeqCont(v, "Test_1::v");
}

void Test_2()
{
    std::vector<int> v = {1,2,3,4};
    std::for_each(v.begin(), v.end(), Print_Vec<int>);
    Print_Vec("Hello");
    Print_Vec(1);
    PrintSeqCont(v, "vector v ");
}
int main()
{
    std::vector<int> v = {1,2,3,4};
    std::for_each(v.begin(), v.end(), Print_Vec1<int>);

    std::cout << "Hello,World!\n"; 

    Test_move();
    Test_1();
    Test_2();
    return 0;
}

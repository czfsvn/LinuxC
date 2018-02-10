#include <iostream>
#include <vector>

using namespace std;

/*
template <typename T>
void Print_Vec(T x)
{
    std::cout << "Print_Vec : " << x << std::endl;
}

template <>
*/
void Print_Vec(std::string x)
{
    std::cout << "Print_Vec : " << x << std::endl;
}

//template <>
void Print_Vec1(int x)
{
    std::cout << "Print_Vec : " << x << std::endl;
}

void Test_move()
{
    std::string str = "Hello";
    std::vector<std::string> v;
    v.push_back(str);
    std::cout << "After copy, str is " << str << std::endl;
    std::for_each(v.begin(), v.end(), Print_Vec);

    v.push_back(std::move(str));
    std::cout << "After move, str is " << str << std::endl;
    std::for_each(v.begin(), v.end(), Print_Vec);
}
int main()
{
    std::vector<int> v = {1,2,3,4};
    std::for_each(v.begin(), v.end(), Print_Vec1);

    std::cout << "Hello,World!\n"; 

    Test_move();
    return 0;
}

#include <iostream>
#include <algorithm>

const int& Min(const int& a, const int& b)
{
    //std::cout << "a" << std::endl;
    return a > b ? b : a;
}

void Test()
{
    auto&& val = Min(4, 8);
    std::cout << "val: " << val << std::endl;
}

int main()
{
    Test();
    return 0;
}


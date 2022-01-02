#include <iostream>
#include <vector>

using namespace std;

bool istarget(const std::string& s, int sz)
{
    return s.size() < sz;
}

void Test_1()
{
    std::vector<string> v{ "This", "is", "a", "predicate", "." };
    auto                isTarget = std::bind(istarget, _1, 2);
    auto                found    = std::find_if(v.begin(), v.end(), isTarget);
}

int main()
{
    return 0;
}

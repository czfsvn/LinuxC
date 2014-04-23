#include <iostream>
#include "interest.h"
#include "name.h"

using namespace std;

int main()
{
    std::string test="test";
    print_my_interest(test.c_str());
    print_my_name(test.c_str());
    //print_my_interest("print_my_interest");
    //print_my_name("print_my_name");
    return 0;
}

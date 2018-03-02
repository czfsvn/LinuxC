#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <set>
#include <string.h>
#include <functional>

using namespace std;

#define LINE_PRINT  std::cout << "line: " << __LINE__ << std::endl;

template <class T>
void PRINT_ELEMENT(const T& cont, const char* optstr = "")
{
    typename T::const_iterator pos;
    std::cout << optstr << "   begin : ==================\n";
    for (pos = cont.begin(); pos != cont.end(); pos ++)
        std::cout << *pos << "\n";

    std::cout << optstr << "   end: ==================\n";
}


struct ToInt
{
    int operator()(std::string& str)
    {
        return std::atoi(str.c_str());
    }
};

void Test_transform()
{
    std::vector<std::string> str_vec  = {"1", "2", "3"};
    PRINT_ELEMENT(str_vec, "str_vec");

    std::vector<int> int_vec;
    std::transform(str_vec.begin(), str_vec.end(),
            std::inserter(int_vec, int_vec.end()), ToInt());
    PRINT_ELEMENT(int_vec, "int_vec");

    int_vec.clear();
    std::transform(str_vec.begin(), str_vec.end(),
            std::back_inserter(int_vec), ToInt());
    PRINT_ELEMENT(int_vec, "int_vec");

}

/// string tok
template<typename Container>
void strtok(const std::string& str, Container& cont, const std::string& deli)
{
    cont.clear();
    const std::string::size_type len = str.length();
    std::string::size_type i = 0;
    int count = 0;
    
    while (i < len)
    {
        i = str.find_first_not_of(deli, i);
        if (i == std::string::npos)
            return;

        std::string::size_type j = str.find_first_of(deli, i);
        count ++;

        if (j == std::string::npos)
        {
            cont.push_back(str.substr(i));
            return;
        }
        cont.push_back(str.substr(i, j - i));
        i = j + 1;
    }
}

template<typename Container>
void strtok_dept2(const std::string& str, Container& cont, 
        const std::string& first_deli, const std::string& sec_deli)
{
    std::vector<string> first_vec;
    strtok(str, first_vec, first_deli);
    for (auto sec_str : first_vec)
    {
        typename Container::value_type sec_cont;
        strtok(sec_str, sec_cont, sec_deli);
        cont.push_back(sec_cont);
    }
}

void Test_stringtok()
{
    {
        std::string str_1 = "123,fad,ta;ga;gar";
        std::vector<std::string> str_vec;
        strtok(str_1, str_vec, ",;");
        PRINT_ELEMENT(str_vec, "Test_stringtok::str_vec 1");
    }
    {
        std::string str_1 = "1,2,3|4,5,6|7,8,9";
        std::vector<std::string> str_vec;
        strtok(str_1, str_vec, "|");
        PRINT_ELEMENT(str_vec, "Test_stringtok::str_vec 2");

        std::vector<std::list<std::string>> vec_2;
        strtok_dept2(str_1, vec_2, "|", ",");
        for (auto first_vec : vec_2)
            PRINT_ELEMENT(first_vec, "Test_stringtok::str_vec 3");
    }
}

void Test_removeif()
{
    std::vector<int> v1 = {1, 2, 3, 4, 3, 3}; 
    PRINT_ELEMENT(v1, "before remove");

    //std::remove(v1.begin(), v1.end(), 3);
    //PRINT_ELEMENT(v1, "after remove1");

    //v1.erase(std::remove(v1.begin(), v1.end(), 3), v1.end());
    //PRINT_ELEMENT(v1, "after remove2");

    //v1.erase(std::remove_if(v1.begin(), v1.end(), bind2nd(equal<int>(), 3)), 
    //        v1.end());

    //PRINT_ELEMENT(v1, "after remove3");
    //std::remove_if(v1.begin(), v1.end(), [](int x)->bool {    return x == 3;  });
    std::remove(v1.begin(), v1.end(), 3);           // 删除第一个
    PRINT_ELEMENT(v1, "after std::remove");
}


void Test_initArr()
{
    char name[1][30] = {{0}};
    LINE_PRINT;

    name[0][30] = {0};
    LINE_PRINT;

    /*
    for (int idx = 0; idx < 30; idx++)
        name[0][idx] = '\0';
    LINE_PRINT;

    //bzero(name[0], sizeof(name[0]));
    LINE_PRINT;
    */
}

void Test_copy_if()
{
    std::vector<int> v1 = {1, 2, 3, 4, 3, 3}; 
    PRINT_ELEMENT(v1, "before .. ");

    std::set<int> s1 = {2, 3};

    std::vector<int> v2;
    //std::copy()
}

bool isOdd(int val)
{
    return (val % 2) > 0;
}

// 拷贝符合条件的元素到另外一个容器
void Test_remove_copy_if()
{
    std::set<int> s1 = {2, 3};
    auto is_inSet = [&] (int val)->bool
    {
        return s1.count(val);
    };

    auto is_in = [] (int val, std::set<int>& iSet)->bool
    {
        return iSet.count(val);
    };

    std::vector<int> v1 = {1, 2, 3, 4, 3, 3}; 
    PRINT_ELEMENT(v1, "before .. ");
    
    std::list<int> v2;
    remove_copy_if(v1.begin(), v1.end(), back_inserter(v2), is_inSet);
    PRINT_ELEMENT(v1, "after .. ");
    PRINT_ELEMENT(v2, "after  .. ");

    std::list<int> v3;
    copy_if(v1.begin(), v1.end(), back_inserter(v3), is_inSet);
    PRINT_ELEMENT(v3, "v3");

    std::set<int> s2;
    copy_if(v1.begin(), v1.end(), inserter(s2, s2.end()), is_inSet);
    PRINT_ELEMENT(s2, "s2");

    std::set<int> s3;
    copy_if(v1.begin(), v1.end(), inserter(s3, s3.end()), std::bind(is_in, std::placeholders::_1, s1));
    PRINT_ELEMENT(s3, "s3");
}

int main()
{
    //Test_transform();
    //Test_stringtok();
    //Test_removeif();
    //Test_initArr();
    Test_remove_copy_if();
    return 0;
}

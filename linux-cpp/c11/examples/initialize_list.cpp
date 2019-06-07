#include <iostream>
#include <vector>
#include <list>

using namespace std;

template<typename Container>
void print_element(Container& t)
{
    /*
    std::cout << "print_element =============\n";
    typename Container::const_iterator it = t.begin();
    for(; it != t.end(); ++it)
    {
        std::cout << *it << ","; 
    }
    std::cout << "\n============================\n";
    */
    std::cout << "print_element =============\n";
    for (auto v = begin(t); v != end(t); v++)
    {
        std::cout << *v << ","; 
    }
    std::cout << "\n============================\n";
}

class Tester
{
    public:
        Tester(int v)  : m_value(v *2)  
    {
        print();
    }

        void print()    {   std::cout <<"Tester::print: " << m_value << std::endl;  }
    private:
        int m_value;
};

void Test_init()
{
    Tester test_arr[] = {1,2,3,4,5};

    std::vector<int> iVec = {2,3,4,5,6};
    print_element(iVec);

    std::vector<int> iList = {2,3,4,5,6};
    print_element(iList);
}

int main()
{
    Test_init();
    return 0;
}

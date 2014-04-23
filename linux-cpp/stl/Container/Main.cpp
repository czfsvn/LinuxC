#include <iostream>
#include <set>
#include <list>
#include <vector>
#include <algorithm>

using namespace std;

//      set 删除最后一个元素
//


class TestCla
{
    public:
        TestCla(int idx) : m_idx(idx)
        {
            std::cout<<"construct : "<<m_idx<<std::endl;
        }
        ~TestCla()
        {
            std::cout<<"destruct: "<<m_idx<<std::endl;
        }
    private:
        int m_idx;
};

void TestSet()
{
    std::set<int> uSet;
    for (int idx = 0; idx < 5; ++idx)
        uSet.insert(idx);

    while (uSet.size())
    {
        std::set<int>::reverse_iterator it = uSet.rbegin();
        std::cout<<"*it = "<<*it<<std::endl;
        uSet.erase(*it);
    }

    std::set<TestCla*> tSet;
    for (int idx = 0; idx < 5; ++idx)
        tSet.insert(new TestCla(idx));

    while (tSet.size() > 2)
    {
        delete (*(tSet.rbegin()));
        tSet.erase((*(tSet.rbegin())));
    }
}

#define SAFE_DELETE_VEC(x)  {   if(x) { delete [](x); (x) = NULL;   }}

typedef std::vector<int> IntVec;

void Print(int idx)
{
    std::cout<<"val :"<<idx<<endl;
}

void Del(IntVec* vec)
{
    std::cout<<"Del2"<<endl;
    std::for_each(vec->begin(), vec->end(), Print);
    //SAFE_DELETE_VEC(vec);
}

void TestSafeDeleteVec()
{
    std::cout<<"TestSafeDeleteVec   1"<<endl;
    std::list<IntVec*> vList;
    for (int idx = 0; idx < 2; ++idx)
    {
        IntVec* vec = new IntVec;
        for (int k = 0; k < 3; ++k)
            vec->push_back(k);

        std::for_each(vec->begin(), vec->end(), Print);
        vList.push_back(new IntVec);
    }

    std::cout<<"TestSafeDeleteVec   2"<<endl;
    std::for_each(vList.begin(), vList.end(), Del);
}

int main()
{
    //TestSet();
    TestSafeDeleteVec();

    return 0;
}


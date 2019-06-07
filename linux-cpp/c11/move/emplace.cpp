#include <iostream>
#include <vector>


//g11 emplace.cpp -fno-elide-constructors

using namespace std;

#define FUNC(x)    //std::cout << __FUNCTION__ << "---" << (x) << std::endl;

#define MAX_LEN     100000000

class Base
{
    public:
        Base(int v) : base(v)      {   FUNC("def")      }
        ~Base()     {   FUNC("dest")        }
        Base(const Base& b) {   FUNC("copy")    }
        Base(Base&& b)      {   FUNC("move")    }

        int base = 0;
};

class Joker
{
    public:
        Joker(Base* p) : pBase(p)   {   FUNC("def") } 
        ~Joker()                    {   FUNC("dest") } 
        Joker(const Joker& jk)         {   FUNC("copy") }
        Joker(Joker&& jk) : pBase(jk.pBase)         {
            FUNC("move") 
            jk.pBase = nullptr;
        }

        Base* pBase = nullptr;
};

class FuncTime
{
    public:
        FuncTime(const std::string& line)  
        {
            tag = std::move(line);
            start = clock();
        }
        ~FuncTime()
        {
            int64_t end = clock();
            std::cout << tag << " cost " << (end - start) / 1000 << " msec\n";
        }

    private:
        int64_t   start;
        std::string tag;
};

#define TIME_COST(str)  std::shared_ptr<FuncTime> __time_cost = std::make_shared<FuncTime>(str);

namespace ns_ep_1
{
    void test1()
    {
        std::vector<Base> bVec, cVec;
        // call defautl construction and copy construct if not define move construction, 
        // or will call construction and move construction
        bVec.push_back(Base(1));     

        std::cout << "=======================\n";
        // call defautl construction and copy construct if not define move construction, 
        // or will call construction and move construction
        cVec.emplace_back(Base(2)); 
        std::cout << "=======================\n";
    }

    void test2()
    {
        std::vector<Joker> jVec, kVec;
        for (int i = 0; i < 1; i++)
        {
            jVec.push_back(Joker(new Base(i)));
        }
        std::cout << "before emplace\n";
        for (auto& item : jVec)
        {
            std::cout << item.pBase->base << std::endl;
        }
        std::cout << "=======================\n";
        for (auto& item : jVec)
        {
            kVec.emplace_back(item);
        }
        std::cout << "after emplace, kVec:\n";
        for (auto& item : kVec)
        {
            std::cout << item.pBase->base << std::endl;
        }
        std::cout << "after emplace, jVec:\n";
        for (auto& item : jVec)
        {
            std::cout << item.pBase->base << std::endl;
        }

    }

    void test3()
    {
        std::vector<Base> vBase;
        vBase.reserve(MAX_LEN);
        TIME_COST("test3 of push_back\n");
        for (int32_t idx = 0; idx < MAX_LEN; idx++)
        {
            vBase.push_back(Base(idx)); 
        }
    }
    void test4()
    {
        std::vector<Base> vBase;
        vBase.reserve(MAX_LEN);
        TIME_COST("test4 of emplace_back\n");
        for (int32_t idx = 0; idx < MAX_LEN; idx++)
        {
            vBase.emplace_back(Base(idx)); 
        }
    }
    void test5()
    {
        std::vector<Base> vBase;
        vBase.reserve(MAX_LEN);
        TIME_COST("test5 of push_back\n");
        for (int32_t idx = 0; idx < MAX_LEN; idx++)
        {
            Base b(idx);
            vBase.push_back(b); 
        }
    }
    void test6()
    {
        std::vector<Base> vBase;
        vBase.reserve(MAX_LEN);
        TIME_COST("test6 of emplace_back\n");
        for (int32_t idx = 0; idx < MAX_LEN; idx++)
        {
            Base b(idx);
            vBase.emplace_back(b); 
        }
    }
}

namespace ns_string
{
    void test1()
    {
        std::vector<std::string> v;
        v.reserve(MAX_LEN);
        TIME_COST("push_back string:");
        for (int i = 0; i < MAX_LEN; i++)
        {
            std::string tmp("ceshi");
            v.push_back(tmp);
        }
    }
    void test2()
    {
        std::vector<std::string> v;
        v.reserve(MAX_LEN);
        TIME_COST("push_back move string:");
        for (int i = 0; i < MAX_LEN; i++)
        {
            std::string tmp("ceshi");
            v.push_back(std::move(tmp));
        }
    }
    void test3()
    {
        std::vector<std::string> v;
        v.reserve(MAX_LEN);
        TIME_COST("push_back (string):");
        for (int i = 0; i < MAX_LEN; i++)
        {
            v.push_back(std::string("ceshi"));
        }
    }
    void test4()
    {
        std::vector<std::string> v;
        v.reserve(MAX_LEN);
        TIME_COST("push_back cstring:");
        for (int i = 0; i < MAX_LEN; i++)
        {
            v.push_back("ceshi");
        }
    }
    void test5()
    {
        std::vector<std::string> v;
        v.reserve(MAX_LEN);
        TIME_COST("push_back string:");
        for (int i = 0; i < MAX_LEN; i++)
        {
            v.emplace_back("ceshi");
        }
    }

    void Main()
    {
        test1();
        test2();
        test3();
        test4();
        test5();
    }
}

int main()
{
    //ns_ep_1::test1();
    //ns_ep_1::test3();
    //ns_ep_1::test4();
    //ns_ep_1::test5();
    //ns_ep_1::test6();
    //ns_ep_1::test2();
    ns_string::Main();
    return 0;
}

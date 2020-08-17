#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <time.h>

/*
 *
 * compile with option  -fno-elide-constructors
 * g++ Main.cpp -std=gnu++11 -lrt 
 */

using namespace std;

#define PRINT_LINE std::cout << "func:" << __func__ << ":" << __LINE__ << std::endl;
#define PRINT_FUNC(x) std::cout << x << std::endl;

struct FunctTime
{
    struct timespec tv1;
    struct timespec tv2;
    const char *func_name;
    const char *desc;

    FunctTime(const char *func = NULL, const char *des = NULL) : func_name(func), desc(des)
    {
        clock_gettime(CLOCK_REALTIME, &tv1);
    }
    ~FunctTime()
    {
        clock_gettime(CLOCK_REALTIME, &tv2);
        unsigned long end = tv2.tv_sec * 1000000L + tv2.tv_nsec / 1000L;
        unsigned long begin = tv1.tv_sec * 1000000L + tv1.tv_nsec / 1000L;
        std::cout << func_name << "execute time:" << end - begin << "ms\n";
    }
};

#define FUNCTIME FunctTime ftime(__PRETTY_FUNCTION__);

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
void PrintSeqCont(T &t, std::string str)
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
    std::vector<int> v = {1, 2, 3, 4};
    for (auto &&i : v)
    {
        std::cout << "i : " << i << std::endl;
    }
    std::cout << "\nprint again: \n";
    for (auto &&i : v)
    {
        std::cout << "i : " << i << std::endl;
    }
    PrintSeqCont(v, "Test_1::v");
}

void Test_2()
{
    std::vector<int> v = {1, 2, 3, 4};
    std::for_each(v.begin(), v.end(), Print_Vec<int>);
    Print_Vec("Hello");
    Print_Vec(1);
    PrintSeqCont(v, "vector v ");
}

namespace ns_test1
{
    void Test_1()
    {
        int x = 20;
        int &rx = x;

        const int y = 10;
        const int &rx2 = y;
        //int& rx3 = 10;          // error
    }

    void Test_2()
    {
        int x = 10;
        const int cx = 20;
        const int &rx1 = x;
        const int &rx2 = cx;
        const int &rx3 = 9;
    }

    void Test_3()
    {
        int &&ry = 200;

        int x = 20;
        //int&&   rrx1 = x;     // error, x is a left value
        int &&rrxx = 20;
        //const int&& rrx = x;  // error, x is a left value
    }

    void Test_4()
    {
        int x = 20;
        int &&rx = x * 2;
        int y = rx + 2;
        rx = 100;
    }
} // namespace ns_test1

namespace ns_test2
{
    void fun(int &lref)
    {
        std::cout << "l-value reference\n";
    }
    void fun(int &&lref)
    {
        std::cout << "r-value reference\n";
    }

    void fun(const int &clref)
    {
        std::cout << "l-value const reference\n";
    }

    void Test_1()
    {
        int x = 10;
        fun(x);
        fun(20);
    }

    void Test()
    {
        Test_1();
    }
} // namespace ns_test2

namespace ns_test3
{
    template <typename T>
    class DynArray
    {
    public:
        explicit DynArray(int size)
            : m_size(size), m_array{new T[size]}
        {
            std::cout << "Constructor: dyn array is created\n";
        }

        virtual ~DynArray()
        {
            delete[] m_array;
            std::cout << "Destructor: dyn array is destroyed\n";
        }

        DynArray(const DynArray &rhs)
            : m_size{rhs.m_size}
        {
            m_array = new T[m_size];
            for (int i = 0; i < m_size; i++)
                m_array[i] = rhs.m_array[i];
            std::cout << "Copy constructor: dyn array is created\n";
        }

        DynArray &operator=(const DynArray &rhs)
        {
            std::cout << "Copy assignment operator is called\n";
            if (this == &rhs)
                return *this;

            delete[] m_array;
            m_size = rhs.m_size;
            m_array = new T[m_size];
            for (int i = 0; i < m_size; i++)
                m_array[i] = rhs.m_array[i];

            return *this;
        }

        DynArray(DynArray &&rhs)
            : m_size(rhs.m_size), m_array{rhs.m_array}
        {
            rhs.m_size = 0;
            rhs.m_array = nullptr;
            std::cout << "Move constructor: dyn array is moved\n";
        }

        DynArray &operator=(DynArray &&rhs)
        {
            std::cout << "Move assignment operator is called\n";
            if (this == &rhs)
                return *this;
            delete[] m_array;
            m_size = rhs.m_size;
            m_array = rhs.m_array;
            rhs.m_size = 0;
            rhs.m_array = nullptr;

            return *this;
        }

        T &operator[](int idx)
        {
            return m_array[idx];
        }

        const T &operator[](int idx) const
        {
            return m_array[idx];
        }

        int size() const
        {
            return m_size;
        }

    private:
        T *m_array;
        int m_size;
    };

    DynArray<int> arrayFactory(int size)
    {
        DynArray<int> arr{size};
        return arr;
    }

    void Test()
    {
        {
            DynArray<int> arr = arrayFactory(10);
        }
        return;
    }
} // namespace ns_test3

namespace ns_test4
{

#define __PRINT__ std::cout << "func:" << __func__ << ":" << __LINE__ << "--idx: " << idx << std::endl;

    class Base1
    {
    public:
        Base1(int i) : idx(i)
        {
            std::cout << "cons Base1 idx:" << idx << std::endl;
        }
        ~Base1()
        {
            std::cout << "des Base1 idx:" << idx << std::endl;
        }
        Base1(const Base1 &h) : idx(h.idx)
        {
            std::cout << "copy cons Base1 idx:" << idx << std::endl;
        }
        Base1 &operator=(const Base1 &h)
        {
            idx = h.idx;
            std::cout << "assigned Base1 idx:" << idx << std::endl;
        }

        int idx;
    };
    class Base2
    {
    public:
        Base2(int i) : idx(i)
        {
            std::cout << "cons Base2 idx:" << idx << std::endl;
        }
        Base2(const Base2 &i) : idx(i.idx)
        {
            idx = i.idx;
            std::cout << "copy idx:" << idx << std::endl;
        }
        Base2(const Base2 &&i) : idx(i.idx)
        {
            std::cout << "move idx:" << idx << std::endl;
        }
        Base2 &operator=(const Base2 &h)
        {
            idx = h.idx;
            std::cout << "assigned Base2 idx:" << idx << std::endl;
            return *this;
        }
        Base2 &operator=(const Base2 &&h)
        {
            std::cout << "move assigned Base2 idx:" << idx << std::endl;
            return *this;
        }
        ~Base2()
        {
            std::cout << "des idx:" << idx << std::endl;
        }

        int idx;
    };

#define MAX_LOOP 10
    void Test_1()
    {
        FUNCTIME;
        std::cout << "ns_test4::Test_1 =====================\n";
        std::vector<Base1> uVec;
        for (int i = 0; i < MAX_LOOP; i++)
        {
            std::cout << "loop: " << i << ", size:" << uVec.size() << "/" << uVec.capacity() << std::endl;
            uVec.push_back(Base1(i));
        }
        std::cout << "ns_test4::Test_1 END=====================\n";
    }

    void Test_2()
    {
        FUNCTIME;
        std::cout << "ns_test4::Test_2 =====================\n";
        std::vector<Base2> uVec;
        for (int i = 0; i < MAX_LOOP; i++)
        {
            std::cout << "loop: " << i << ", size:" << uVec.size() << "/" << uVec.capacity() << std::endl;
            uVec.push_back(std::move(Base2(i)));
        }
        std::cout << "ns_test4::Test_1 END=====================\n";
    }
} // namespace ns_test4

namespace ns_stringstream
{
    struct ATest
    {
        int i = 100;
    };

    ostream &operator<<(ostream &s, const ATest &a)
    {
        s << a.i;
        return s;
    }

    struct BTest
    {
        int i = 1000;
        friend ostream &operator<<(ostream &s, const BTest &a)
        {
            s << a.i;
            return s;
        }
    };

    void test_1()
    {
        ATest a;
        std::cout << a << std::endl;

        BTest s;
        std::cout << s << std::endl;
    }

} // namespace ns_stringstream

namespace ns_test_emplace
{
    // 1、一个容器从另外一个容器 emplace 数据， 另外一个容器中的内容是否为空 ?
    // 2、使用-fno-elide-constructors前后区别
    // 3、是否使用move construction的区别
    // 4、容器中元素序列化成字符串输出
    //

    class Pointer
    {
    public:
        Pointer(int a) : poi(a) {}
        int poi = 100;
    };

    class A
    {
    public:
        A(int i, Pointer *ap) : a(i), point(ap) { PRINT_LINE; };
        ~A() { PRINT_LINE; }

        friend ostream &operator<<(ostream &s, const A &a)
        {
            s << a.a << " - " << (a.point != nullptr ? a.point->poi : 200000);
            return s;
        }

    private:
        int a = 100;
        Pointer *point = nullptr;
    };

    void test_1()
    {
        std::vector<A> aVec, bVec;
        for (int i = 10; i < 12; i++)
            aVec.emplace_back(A(i, new Pointer(i)));

        std::cout << "bef move to bVec -----------------------------------------\n";
        PrintSeqCont(aVec, "12");

        std::cout << "-----------------------------------------\n";
        for (int i = 0; i < aVec.size(); i++)
            bVec.emplace_back(aVec[i]);

        std::cout << "after move to bVec -----------------------------------------\n";
        PrintSeqCont(aVec, "12");

        std::cout << "bVec -----------------------------------------\n";
        PrintSeqCont(aVec, "12");
    }

    class B
    {
    public:
        B(int i, Pointer *ap) : a(i), point(ap){PRINT_FUNC("B construction ")};
        ~B() { PRINT_FUNC("B destruction"); }
        B(const B &b) : a(b.a), point(b.point) { PRINT_FUNC("copy construction "); }
        B &operator=(const B &b)
        {
            a = b.a;
            point = b.point;
            PRINT_FUNC("assign construction");
        }

        B(B &&b) : a(b.a), point(b.point)
        {
            b.point = nullptr;
            PRINT_FUNC("move construction");
        }
        B &&operator=(B &&b)
        {
            a = b.a;
            point = b.point;
            b.point = nullptr;
            PRINT_FUNC("assgin move construction");
        }

        friend ostream &operator<<(ostream &s, const B &a)
        {
            s << a.a << " - " << (a.point != nullptr ? a.point->poi : 200000);
            return s;
        }

    private:
        int a = 100;
        Pointer *point = nullptr;
    };

    void test_2()
    {
        std::vector<B> srcVec, destVec;
        for (int i = 10; i < 12; i++)
            srcVec.emplace_back(B(i, new Pointer(i)));

        std::cout << "bef move to srcVec -----------------------------------------\n";
        PrintSeqCont(srcVec, "12");

        std::cout << "-----------------------------------------\n";
        for (int i = 0; i < srcVec.size(); i++)
            destVec.emplace_back(srcVec[i]);

        std::cout << "after move to destVec -----------------------------------------\n";
        PrintSeqCont(srcVec, "12");

        std::cout << "destVec -----------------------------------------\n";
        PrintSeqCont(destVec, "12");
    }
} // namespace ns_test_emplace

namespace ns_move_bigobj
{
#define MAX_SIZE 1000
#define ELEMENT_SIZE 1000
    struct PakcetObj
    {
        uint32_t uniq_key = 0;
        uint32_t char_id = 0;
        std::string name = "";
        uint32_t country = 0;
        uint32_t money = 0;
        uint32_t time_stamp = 0;

        std::map<uint32_t, uint32_t> obj_map = {};
        std::set<uint32_t> tmp_sets = {};
        std::map<uint32_t, uint32_t> user_map = {};
    };

    void test_emplace(uint16_t type)
    {
        FUNCTIME;
        std::map<uint32_t, PakcetObj> obj_map;
        for (uint32_t i = 0; i < MAX_SIZE; i++)
        {
            PakcetObj tmp;
            tmp.uniq_key = i;
            tmp.char_id = i;
            tmp.country = i;
            tmp.money = i;
            tmp.time_stamp = time(nullptr);
            tmp.name = "abc" + i;

            for (uint32_t idx = 0; idx < ELEMENT_SIZE; idx++)
            {
                tmp.tmp_sets.emplace(idx);
                tmp.user_map.emplace(idx, 0);
                tmp.obj_map.emplace(idx, 0);
            }

            if (type == 1)
            {
                obj_map.insert(std::make_pair(i, tmp));
            }
            else if (type == 2)
            {
                obj_map.insert(std::make_pair(i, std::move(tmp)));
            }
            else if (type == 3)
            {
                obj_map.emplace(i, tmp);
            }
            else if (type == 4)
            {
                obj_map.emplace(i, std::move(tmp));
            }
        }
    }

    void main()
    {
        test_emplace(1);
        test_emplace(2);
        test_emplace(3);
        test_emplace(4);
    }

} // namespace ns_move_bigobj

int main()
{
    /*
    std::vector<int> v = {1,2,3,4};
    std::for_each(v.begin(), v.end(), Print_Vec1<int>);

    std::cout << "Hello,World!\n"; 

    Test_move();
    Test_1();
    Test_2();
    ns_test2::Test();
    ns_test3::Test();
    */
    //ns_test4::Test_1();
    //ns_test4::Test_2();
    //ns_stringstream::test_1();
    //ns_test_emplace::test_2();

    ns_move_bigobj::main();

    return 0;
}

#include <iostream>
#include <string.h>

using namespace std;

namespace ns_test1
{
    template <class T>
    inline T const &max(T const &a, T const &b)
    {
        return a < b ? b : a;
    }

    template <typename T1, typename T2, typename RT>
    inline RT max1(T1 const &a, T2 const &b)
    {
        return max<RT>(a, b);
    }

    template <typename RT, typename T1, typename T2>
    inline RT max2(T1 const &a, T2 const &b)
    {
        return max<RT>(a, b);
    }

    /*
    模板会被编译两次：
    1）实例化之前，先检查模板代码本身，查看语法是否正确；
        // // 在这里会发现错误的语法，如遗漏分号等。

    2）实例化期间，检查模板代码，查看是否所有的调用都有效。
        这里会发现发小的嗲用，如该实例化类型不支持某些函数调用
    // // // */
    void main1()
    {
        int32_t i = 42;
        std::cout << "max(7, i) : " << max(7, i) << std::endl;
        double f1 = 3.4;
        double f2 = -6.7;
        std::cout << "max(f1, f2) : " << max(f1, f2) << std::endl;

        std::string s1 = "mathemta";
        std::string s2 = "math";
        std::cout << "max(s1, s2) : " << ::max(s1, s2) << std::endl;
    }

    void main2()
    {
        uint32_t i = 42;
        // 这里不允许进行自动类型转换，每个 T 都必须正确的匹配
        //std::cout << "max(7, i) : " << max(7, i) << std::endl;uint

        // 显式指定(或限定) T 的类型, 用 uint32_t 来实例化 T
        std::cout << "max(7, i) : " << max<uint32_t>(7, i) << std::endl;

        // 对实参进行强制类型转换，使它们可以相互匹配
        std::cout << "max(7, i) : " << max(static_cast<uint32_t>(7), i) << std::endl;

        // 这种情况下，必须显示的指定模板实参列表
        double f1 = 3.14;
        double res1 = max1<int32_t, float, double>(i, f1);

        // 这种情况下，必须显示的指定模板实参RT,
        //  而 T1， T2 可以根据实际调用分别演绎为 uint32_t, double
        double f2 = 3.14;
        double res2 = max2<uint32_t>(i, f1);
    }

    void main()
    {
        main1();
        main2();
    }

} // namespace ns_test1

namespace ns_test2 // 重载函数模板
{

    /*
        1）一个非模板函数可以和一个同名的函数模板同时存在，
            而且函数模板还可以被实例化为这个非模板函数

        2）对于非模板函数和同名的函数模板，如果其他条件都相同的话，
            那么在调用的时候，重载解析过程会调用非模板函数，
            而不会从该模板产生出一个实例
        
        3）如果模板可以产生一个具有更好匹配的函数，那么将选择模板
    */
    inline uint32_t const &max(uint32_t const &a, uint32_t const &b)
    {
        return a < b ? b : a;
    }

    inline int32_t const &max(int32_t const &a, int32_t const &b)
    {
        return a < b ? b : a;
    }

    template <class T>
    inline T const &max(T const &a, T const &b)
    {
        return a < b ? b : a;
    }

    template <class T>
    inline T const &max(T const &a, T const &b, T const &c)
    {
        return max(max(a, b), c);
    }

    void main1()
    {
        // 调用具有3个参数的max
        max(7, 42, 68);

        // 调用 max<double>, 通过实参演绎
        max(7.0, 42.0);

        // 调用 max<char>, 通过实参演绎
        max('a', 'b');
        max('a', 'b', '1');

        // 调用 int 重载的非模板函数
        max(7, 42);

        // 调用 max<int> 模板函数, 通过实参演绎，
        // max<> ： 只有模板才能匹配这个调用，而且所有的模板参数都应该根据调用实参演绎
        max<>(7, 43);

        // 调用 max<double> 模板函数, 显式指定
        max<double>(7, 43);

        // 调用 int 重载的非模板函数
        // 因为模板不允许自动类型转换，但是普通函数可以进行自动类型转换
        max('a', 43.7);
    }

    void main()
    {
        main1();
    }
} // namespace ns_test2

namespace ns_test3
{
    template <class T>
    inline T const &max(T const &a, T const &b)
    {
        //return a < b ? b : a;
        return ::max(a, b);
    }

    template <class T>
    inline T *const &max(T *const &a, T *const &b)
    {
        return *a < *b ? b : a;
    }

    inline char const *const &max(char const *const &a, char const *const &b)
    {
        return ::strcmp(a, b) < 0 ? b : a;
    }

    void main1()
    {
        int32_t a = 7;
        int32_t b = 42;
        max(a, b); // max() 求两个int值的最大值

        std::string s = "hello";
        std::string t = "world";
        ::max(s, t);
        //max(s, t);
        // max() 求两个 std::string 类型的最大值
        std::cout << "max(s, t) : " << ::max(s, t) << std::endl;

        int32_t *p1 = &a;
        int32_t *p2 = &b;
        // max() 求两个 指针 所指向值的最大者
        std::cout << "max(p1, p2) : " << max(p1, p2) << std::endl;

        const char *s1 = "hello";
        const char *s2 = "world";
        // max() 求两个 c 字符串的最大值
        std::cout << "max(s1, s2) : " << max(s1, s2) << std::endl;
    }
    void main()
    {
        main1();
    }
} // namespace ns_test3

namespace ns_test4
{
    // 两个任意类型值的最大者---通过传引用进行调用
    template <class T>
    inline T const &max(T const &a, T const &b)
    {
        //return a < b ? b : a;
        return ::max(a, b);
    }
    // 两个 c 字符串的最大者---通过传值进行调用
    inline char const *max(char const *a, char const *b)
    {
        return ::strcmp(a, b) < 0 ? b : a;
    }

    // 求3个任意类型值的最大者---通过传引用进行调用
    template <typename T>
    inline T const &max(T const &a, T const &b, T const &c)
    {
        return max(max(a, b), c);
    }

    void main()
    {
        max(7, 42, 68);

        const char *s1 = "hello";
        const char *s2 = "world";
        const char *s3 = "cpp";

        std::cout << " " << max(s1, s2, s3) << std::endl;
    }
} // namespace ns_test4

namespace ns_test5
{
    // 两个任意类型值的最大者---通过传引用进行调用
    template <class T>
    inline T const &max(T const &a, T const &b)
    {
        //return a < b ? b : a;
        return ::max(a, b);
    }

    // 求3个任意类型值的最大者---通过传引用进行调用
    template <typename T>
    inline T const &max(T const &a, T const &b, T const &c)
    {
        return max(max(a, b), c); // 使用模板的版本，即使有下面声明的int版本，但是该声明来的太迟了
    }

    // 求两个int值的最大者
    inline int32_t const &max(int32_t const &a, int32_t const &b)
    {
        return a < b ? b : a;
    }

} // namespace ns_test5

//'可爱浪漫雅典娜\|蜀月弓神\|陌千佰\|潇洒DE狼哥哥\|呼叫潇洒哥\|HAPPY9'

int main()
{
    //ns_test1::main();
    //ns_test2::main();
    //ns_test3::main();
    ns_test4::main();
    return 0;
}
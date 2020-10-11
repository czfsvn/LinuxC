#include <iostream>
#include <vector>

using namespace std;

namespace ns_desc1
{
    template <typename T>
    class List  //作为名字空间作用域的类模板
    {
    public:
        template <typename T2>  //成员函数模板
        List(List<T2> const&);  //构造函数
    };

    template <typename T>
    template <typename T2>
    List<T>::List(const List<T2>& para)  // 位于类外部的成员函数模板定义
    {
    }

    template <typename T>
    int32_t length(const List<T>);  // 位于外部名字空间作用域的函数模板

    class Collection
    {
        template <typename T>  // 位于类内部的成员类模板
        class Node             // 该类模板的定义
        {
        };

        template <typename T>  // 另一个作为成员（即位于外围类的内部）的类模板
        class Handle;          // 该类模板在此没有定义

        template <typename T>  // 位于类内部的成员函数模板的定义
        T* alloc()             // 因此也是一个显式内联函数
        {
        }
    };

    template <typename T>     // 一个在类的外部定义的成员类模板
    class Collection::Handle  // 该类模板的定义
    {
    };

    template <typename T>
    union AllocChunk  // union 的模板
    {
        T             object;
        unsigned char bytes[sizeof(T)];
    };

    template <typename T>  // 函数模板声明也可以具有缺省调用实参
    void report_top(const std::vector<T>& vec, int32_t num = 10);

    template <typename T>  // 函数模板声明也可以具有缺省调用实参
    void fill(const std::vector<T>& vec, const T& = T())
    {
    }
};  // namespace ns_desc1

namespace ns_test1
{
    class Value
    {
    private:
        /* data */
    public:
        Value(int32_t) {}
        // Value() {}
    };

    void test()
    {
        Value zero(0);

        std::vector<Value> vec;
        ns_desc1::fill(vec, zero);  // 没有使用 =T

        // ns_desc1::fill(vec);  // 错误： 使用了T(), 当 T=Value 是缺省构造参数无效,
        // 必须要有缺省构造函数才行 Value() {}
    }

}  // namespace ns_test1

namespace ns_virtual_member_function
{
    template <typename T>
    class Dynamic
    {
    public:
        virtual ~Dynamic(){};  // ok, 每个 Dynamic 值对应一个析构函数

        /*
        template <typename T2>
        virtual void copy(const T2&)  // error： 在确定 Dynamic<T> 实例的时候，并不知道copy的个数
        {
        }
        */
    };

}  // namespace ns_virtual_member_function

namespace ns_no_type_para  // 非类型参数
{
    // 非类型参数表示的：在编译期或者链接期可以确定的常值。必须是下面的一种：
    // 1) 整型或者枚举类型
    // 2) 指针类型(包含普通对象的指针类型、函数指针类型、指向成员的指针类型)
    // 3) 引用类型(指向对象或者指向函数的引用都是允许的)

    // 函数和数组类型也可以被指定为非模板参数，但是要先把它们隐式的转换为指针类型，这种类型也成为 decay
    /*
    template<int buf[5]> class Lexer{}; // buf 实际上是一个 int* 类型
    template<int* buf> class Lexer{};   // buf 实际上是一个 int* 类型
    */

    // 非类型模板参数的声明和便令的声明类似。但不能具有 static、mutable 等修饰。
    // 只能具有const、volatile 限定符

    template <int const length>  // const 被系统忽略
    class Buffer1
    {
    };

    template <int length>
    class Buffer2
    {
    };

    // 非类型模板参数只能是右值：不能被取址，也不能被赋值

}  // namespace ns_no_type_para

namespace ns_template_para  //模板的模板参数
{
    // 模板的模板参数是代表类的占位符。
    // 声明和类模板的声明类似，但不能使用 struct 和 union
    template <template <typename X> class C>  //
    void f(C<int>* p)
    {
    }

    /*
    template <template <typename X> struct C>  //
    void f(C<int>* p)
    {
    }

    template <template <typename X> union C>  //
    void f(C<int>* p)
    {
    }
    */

    class MyAllocator
    {
    };

    template <template <typename T, typename A = MyAllocator> class Container>
    class Adaptation
    {
        Container<int> cont;  // 隐式等同于Container<int32_t, MyAllocator>
    };

    template <template <typename T, T*> class Buf>
    class Lexer
    {
        // static T*                       cont;
        static char                        storage[5];
        Buf<char, &Lexer<Buf>::storage[0]> buf;
    };

    auto lbd = []() -> void {

    };
}  // namespace ns_template_para

int main()
{
    uint32_t a      = 10;  //测试
    uint16_t bbbbbb = 100;
    if (a)
    {
    }
    std::cout << "hello, world\n";
    return 0;
}

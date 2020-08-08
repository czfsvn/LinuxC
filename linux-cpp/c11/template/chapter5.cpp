#include <iostream>
#include <deque>
#include <vector>

using namespace std;

namespace ns_typename
{
    template <typename T>
    void prontAll(const T &cont)
    {
        // 为了访问模板类型为T从const_iterator 类型，
        //      需要在声明开始处使用关键字typename来加以限定
        typename T::const_iterator pos;
        typename T::const_iterator end(cont.end());

        for (pos = cont.begin(); pos != end; ++pos)
            std::cout << "pos: " << *pos << ", ";

        std::cout << std::endl;
    }

} // namespace ns_typename

// .template 构造   参见 9.3.3
namespace ns_template
{

} // namespace ns_template

namespace ns_this
{
    //对于那些在基类中声明，并且依赖于模板参数的符号（函数或者变量等），
    //  你应该在它们前面使用this->或者Base<T>::。
    //  如果希望完全避免不确定性，你可以（使用诸如this->和Base<T> : ：等）
    //  限定（模板中）所有的成员访问。
    template <typename T>
    class Base
    {
    public:
        void exit(int a)
        {
            std::cout << "Base::Exit : " << a << std::endl;
        }
    };

    template <typename T>
    class Derived : Base<T>
    {
    private:
        /* data */
    public:
        void func()
        {

            this->exit(10);    // 调用父类 Base::exit()
            Base<T>::exit(11); // 调用父类 Base::exit()

            exit(10); // 调用外部的exit 或出现错误
        }
    };

    void main()
    {
        Derived<int32_t> derived1;
        derived1.func();
    }

} // namespace ns_this

// 模板的模板参数--让模板参数成为模板
namespace ns_test2
{
    // 类成员可以是模板；嵌套类和成员函数都可以作为模板
    template <typename T, template <typename ELEM, typename = std::allocator<ELEM>> class CONT = std::deque>
    //template <typename T, template <typename, typename> class CONT = std::deque>
    class Stack
    {
    private:
        CONT<T> elems;

    public:
        void push(const T &);
        void pop();
        T top() const;
        bool empty() const
        {
            return elems.empty();
        }

        template <typename T2, template <typename ELEM2, typename = std::allocator<ELEM2>> class CONT2>
        Stack<T, CONT> &operator=(const Stack<T2, CONT2> &);
    };

    template <typename T, template <typename, typename> class CONT>
    void Stack<T, CONT>::push(const T &elem)
    {
        elems.push_back(elem);
    }

    template <typename T, template <typename, typename> class CONT>
    void Stack<T, CONT>::pop()
    {
        if (elems.empty())
            throw std::out_of_range("Stack<>::pop() empty stack");
        elems.pop_back();
    }

    template <typename T, template <typename, typename> class CONT>
    T Stack<T, CONT>::top() const
    {
        if (elems.empty())
            throw std::out_of_range("Stack<>::top() empty stack");
        return elems.back();
    }

    template <typename T, template <typename, typename> class CONT>
    template <typename T2, template <typename, typename> class CONT2>
    Stack<T, CONT> &Stack<T, CONT>::operator=(const Stack<T2, CONT2> &op2)
    {
        if ((void *)this == (void *)&op2)
            return *this;

        Stack<T2, CONT2> tmp(op2);
        elems.clear();
        while (!tmp.empty())
        {
            elems.push_back(tmp.top());
            tmp.pop();
        }
        return *this;
    }

    void main1()
    {
        try
        {
            Stack<int32_t> intStack;
            Stack<float> floatStack;

            intStack.push(42);
            intStack.push(7);

            floatStack.push(7.7);

            floatStack = intStack;

            std::cout << "floatStack.top() : " << floatStack.top() << std::endl;
            floatStack.pop();
            std::cout << "floatStack.top() : " << floatStack.top() << std::endl;
            floatStack.pop();
            std::cout << "floatStack.top() : " << floatStack.top() << std::endl;
            floatStack.pop();
        }
        catch (const std::exception &e)
        {
            std::cerr << "exception: " << e.what() << '\n';
        }
    }

    void main2()
    {
        try
        {
            Stack<int32_t, std::vector> intStack;
            intStack.push(42);
            intStack.push(7);

            std::cout << "intStack.top() : " << intStack.top() << std::endl;
            intStack.pop();
        }
        catch (const std::exception &e)
        {
            std::cerr << "exception: " << e.what() << '\n';
        }
    }

    void main()
    {
        main1();
        main2();
    }

} // namespace ns_test2

namespace ns_template_init
{
    template <typename T>
    class Base
    {
    private:
        T val1;       // val1 是一个不确定的数值
        T *ptr;       // ptr 指向一个未知的内存
        T val2 = T(); // 这样会调用T的缺省构造函数来初始化，就不会是一个未知的数值
    };

} // namespace ns_template_init

namespace ns_template_char
{
    template <typename T>
    inline const T &max1(const T &a, const T &b)
    {
        return a < b ? b : a;
    }

    void main1()
    {
        std::string s;
        // OK, 相同类型的实参   类型： const char[6]
        max1("apple", "peach");

        // ERROR, 不同类型的实参  类型： const char[6], const char[7]
        //max1("apple", "peach1");

        // ERROR, 不同类型的实参     类型： const char[6], std::string
        //max1("apple", s);
    }

    template <typename T>
    inline const T &max2(const T a, const T b)
    {
        return a < b ? b : a;
    }

    void main2()
    {
        std::string s;

        // OK, 相同类型的实参   类型： const char[6]
        max2("apple", "peach");

        // OK, 退化(decay) 为相同类型，对于非引用类型的参数，在实参演义中，出现数组导致真的雷西那个转换
        max2("apple", "peach1");

        // ERROR, 不同类型的实参     类型： const char[6], std::string
        //max2("apple", s);
    }

    template <typename T>
    void ref(const T &x)
    {
        std::cout << "x in ref(const T &x) : " << typeid(x).name() << std::endl;
    }

    template <typename T>
    void noref(const T &x)
    {
        std::cout << "x in noref(const T x) : " << typeid(x).name() << std::endl;
    }

    void main3()
    {
        ref("Hello");
        noref("Hello");
    }

    void main()
    {
        main1();
        main2();
        main3();
    }

} // namespace ns_template_char

int main()
{
    //ns_this::main();
    //ns_test2::main();
    ns_template_char::main();
    return 0;
}
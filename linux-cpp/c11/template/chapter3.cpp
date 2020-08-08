#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <stdexcept>

using namespace std;
/*
    1) 类模板是具有是具有如下性质的类： 
        在类的实现中，可以有一个或多个类型还未指定

    2）为了使用类模板，可以传入某个具体类型作为模板实参；
        然后编译器将会基于该类型来实例化类模板

    3）对于类模板而言，只有那些被调用的成员函数才会被实例化

    4）可以用某种特定类型特化类模板

    5）可以用某种特定类型局部特化模板

    6）可以为类模板的参数定义缺省值，这些值还可以引用之前的模板参数
*/

/*
    对于成员函数：只有被调用的时候，才会产生这些函数的实例化代码
    对于类模板：只有在被使用的时候才会被实例化
*/
namespace ns_stack
{
    template <typename T>
    class Stack
    {
    private:
        std::vector<T> elems; //存储元素的容器
        static T static_memter;

    public:
        Stack()
        {
            // todo...
        }
        // 拷贝构造函数， 此时必须用 Stack<T>
        Stack(const Stack<T> &)
        {
            // todo...
        }

        // 赋值运算符，此时必须用 Stack<T>
        Stack<T> &operator=(const Stack<T> &)
        {
            // todo...
        }

        void push(T const &);
        void pop();
        T top() const;
        bool empty() const { return elems.empty(); }
    };

    template <typename T>
    void Stack<T>::push(T const &a)
    {
        elems.push_back(a);
    }

    template <typename T>
    void Stack<T>::pop()
    {
        if (elems.empty())
            throw std::out_of_range("Stack<>::pop():: empty stack");
        elems.pop_back();
    }

    template <typename T>
    T Stack<T>::top() const
    {
        if (elems.empty())
            throw std::out_of_range("Stack<>::pop():: empty stack");
        return elems.back();
    }
} // namespace ns_stack

namespace ns_test1
{
    void main()
    {
        try
        {
            // 构造函数， push 都被实例化了 int 和string版本
            // pop 只实例化了string版本
            // 其他函数则未实例化
            // 静态成员变量：static_memter 则实例化了两个版本
            ns_stack::Stack<int> intStack;
            intStack.push(2);

            ns_stack::Stack<std::string> strStack;
            strStack.push("hello");
            strStack.push("world");
            strStack.pop();
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return EXIT_FAILURE; // 程序退出，且带有ERROR标记
        }
    }
} // namespace ns_test1

// 类模板特化
namespace ns_test2
{
    template <typename T>
    class Stack
    {
    private:
        std::vector<T> elems; //存储元素的容器
        static T static_memter;

    public:
        Stack()
        {
            // todo...
        }
        // 拷贝构造函数， 此时必须用 Stack<T>
        Stack(const Stack<T> &)
        {
            // todo...
        }

        // 赋值运算符，此时必须用 Stack<T>
        Stack<T> &operator=(const Stack<T> &)
        {
            // todo...
        }

        void push(T const &);
        void pop();
        T top() const;
        bool empty() const { return elems.empty(); }
    };

    template <typename T>
    void Stack<T>::push(T const &a)
    {
        elems.push_back(a);
    }

    template <typename T>
    void Stack<T>::pop()
    {
        if (elems.empty())
            throw std::out_of_range("Stack<>::pop():: empty stack");
        elems.pop_back();
    }

    template <typename T>
    T Stack<T>::top() const
    {
        if (elems.empty())
            throw std::out_of_range("Stack<>::pop():: empty stack");
        return elems.back();
    }

    template <>
    class Stack<std::string>
    {
    private:
        std::list<std::string> elems; //存储元素的容器
        static std::string static_memter;

    public:
        Stack()
        {
            // todo...
        }
        // 拷贝构造函数， 此时必须用 Stack<T>
        Stack(const Stack<std::string> &)
        {
            // todo...
        }

        // 赋值运算符，此时必须用 Stack<T>
        Stack<std::string> &operator=(const Stack<std::string> &)
        {
            // todo...
            return *this;
        }

        void push(std::string const &);
        void pop();
        std::string top() const;
        bool empty() const { return elems.empty(); }
    };

    void Stack<std::string>::push(std::string const &a)
    {
        elems.push_back(a);
    }

    void Stack<std::string>::pop()
    {
        if (elems.empty())
            throw std::out_of_range("Stack<>::pop():: empty stack");
        elems.pop_back();
    }

    std::string Stack<std::string>::top() const
    {
        if (elems.empty())
            throw std::out_of_range("Stack<>::pop():: empty stack");
        return elems.back();
    }

    void main1()
    {
    }
    void main()
    {
    }
} // namespace ns_test2

// 局部特化
namespace ns_test3
{
    template <typename T1, typename T2>
    class MyClass
    {
    };

    template <typename T>
    class MyClass<T, T>
    {
    };

    template <typename T>
    class MyClass<T, int>
    {
    };

    template <typename T1, typename T2>
    class MyClass<T1 *, T2 *>
    {
    };

    void main1()
    {
        MyClass<int, float> m1; // 使用 MyClass<T1, T2>

        MyClass<float, float> m2; // 使用 MyClass<T, T>

        //MyClass<int, int> m3;     // 错误，同等程度的匹配 MyClass<T, T> 和 MyClass<T, int>

        MyClass<uint32_t, uint32_t> m4; // 使用 MyClass<T, T>

        MyClass<uint32_t *, float *> m5; // 使用 MyClass<T1*, T2*>

        // MyClass<uint32_t *, uint32_t *> m6; // 错误，同等程度的匹配 MyClass<T, T> 和 MyClass<T1*, T2*>
    }

    // 解决 m6 的二义性，提供指向相同类型的指针的特化
    template <typename T>
    class MyClass<T *, T *>
    {
    };

    void main2()
    {
        MyClass<uint32_t *, uint32_t *> m6; // 使用MyClass<T *, T *>
    }

    void main()
    {
        main1();
        main2();
    }

} // namespace ns_test3

// 缺省模板实参
namespace ns_test4
{
    template <typename T, typename CONT = std::vector<T>>
    class Stack
    {
    private:
        CONT elems; //存储元素的容器
        static T static_memter;

    public:
        Stack()
        {
            // todo...
        }
        // 拷贝构造函数， 此时必须用 Stack<T>
        Stack(const Stack<T, CONT> &)
        {
            // todo...
        }

        // 赋值运算符，此时必须用 Stack<T>
        Stack<T, CONT> &operator=(const Stack<T, CONT> &)
        {
            // todo...
        }

        void push(T const &);
        void pop();
        T top() const;
        bool empty() const { return elems.empty(); }
    };

    template <typename T, typename CONT>
    void Stack<T, CONT>::push(T const &a)
    {
        elems.push_back(a);
    }

    template <typename T, typename CONT>
    void Stack<T, CONT>::pop()
    {
        if (elems.empty())
            throw std::out_of_range("Stack<>::pop():: empty stack");
        elems.pop_back();
    }

    template <typename T, typename CONT>
    T Stack<T, CONT>::top() const
    {
        if (elems.empty())
            throw std::out_of_range("Stack<>::pop():: empty stack");
        return elems.back();
    }

    class Help
    {
    };
    void main()
    {
        Stack<int> intStack;

        Stack<double, std::list<double>> doubleStack;

        Stack<float, std::set<float>> floatStack;
        //错误floatStack 声明可以通过， 但是在实例化 push 的时候，set中并无 push_back 成员函数
        //floatStack.push(5.0);

        Stack<Help, std::list<Help>> helpStack;
    }
} // namespace ns_test4

int main()
{
    ns_test1::main();
    ns_test3::main();
    ns_test4::main();
    return 0;
}
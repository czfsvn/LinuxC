#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

namespace ns_stack
{
    template <typename T, int MAXSIZE>
    class Stack
    {
    private:
        T elems[MAXSIZE];
        int numElems = 0;

    public:
        Stack(){};
        void push(T const &);
        void pop();
        T top() const;
        bool empty() const
        {
            return numElems == 0;
        }

        bool full() const
        {
            return numElems == MAXSIZE;
        }
    };

    template <typename T, int MAXSIZE>
    void Stack<T, MAXSIZE>::push(const T &elem)
    {
        if (numElems == MAXSIZE)
            throw std::out_of_range("Stack<>::push stack is full");
        elems[numElems] = elem;
        ++numElems;
    }

    template <typename T, int MAXSIZE>
    void Stack<T, MAXSIZE>::pop()
    {
        if (numElems <= 0)
            throw std::out_of_range("Stack<>::pop stack is empty");
        --numElems;
    }

    template <typename T, int MAXSIZE>
    T Stack<T, MAXSIZE>::top() const
    {
        if (numElems <= 0)
            throw std::out_of_range("Stack<>::pop stack is empty");
        return elems[numElems - 1];
    }

    void main()
    {
        try
        {
            // int40Stack 和 int20Stack 属于不同的类型,
            //      而且两种类型之间也不存在显示或者隐式的类型转换
            //      两者之间不能互相替换，更不能相互赋值
            Stack<int32_t, 20> int20Stack;
            Stack<int32_t, 40> int40Stack;
            Stack<double, 20> double20Stack;
            Stack<std::string, 40> stringStack;
        }
        catch (const std::exception &e)
        {
            std::cerr << "exception: " << e.what() << '\n';
            return EXIT_FAILURE;
        }
    }

} // namespace ns_stack

namespace ns_test1
{
    template <typename T, int32_t VAL>
    T addValue(const T &x)
    {
        return x + VAL;
    }
    /*
        模板可以具有模板参数，而不仅仅是类型模板参数
        对于非类型模板参数，不能使用浮点数、class类型的对象和内部链接对象(如:std::string)作为实参
    */

    //template <typename T, double VAL>         浮点数 不能作为非类型模板参数
    //template <typename T, float VAL>          浮点数 不能作为非类型模板参数
    //template <typename T, std::string VAL>    类对象 不能作为非类型模板参数
    // 字符串文字是内部链接对象（两个具名相同名称但处于不同模块的字符串，是两个完全不同的对象）
    template <typename T, const char *VAL>
    T addValue1(const T &x)
    {
        return x + VAL;
    }
    /*
    
    */

    void main()
    {
        std::vector<int32_t> ivec1, ivec2;
        for (int32_t idx = 0; idx < 10; idx++)
            ivec1.emplace_back(idx);

        // 此处会宕机
        //std::transform(ivec1.begin(), ivec1.end(), ivec2.begin(), addValue<int, 5>);
        // 此处会宕机
        //std::transform(ivec1.begin(), ivec1.end(), ivec2.begin(), (int (*)(int const &))addValue<int, 5>);

        return;
    }

} // namespace ns_test1

int main()
{
    ns_stack::main();
    ns_test1::main();
    return 0;
}
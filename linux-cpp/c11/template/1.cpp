#include <iostream>

using namespace std;

namespace ns_test1
{
    template <typename T>
        class Complex
        {
            public:
                friend ostream& operator << (ostream& out, Complex& c)
                {
                    out << c.a << " + " << c.b << "i";
                    return out;
                }

                Complex(T a, T b)
                {
                    this->a = a;
                    this->b = b;
                }

                Complex<T> operator + (Complex& c)
                {
                    Complex<T> tmp(this->a + c.a, this->b + c.b);
                    return tmp;
                }

                Complex MyAdd(Complex& c1, Complex& c2)
                {
                    Complex tmp(c1.a + c2.a, c1.b + c2.b);
                    return tmp;
                }

                void Print()
                {
                    std::cout << "a = " << a << ", b = " << b << std::endl;
                }

            private:
                T a;
                T b;
        };

    void Test_1()
    {
        Complex<int> a(10, 20);
        Complex<int> b(20, 30);
        Complex<int> c = a + b;
        c.Print();
    }

    void Test_2()
    {
        Complex<int> a(10, 20);
        Complex<int> b(20, 30);
        Complex<int> c = a + b;

        std::cout << c << std::endl;
    }
}

namespace ns_test2
{
    template <typename T>
        class Parent
        {
            public:
                Parent(T p)
                {
                    this->p = p;
                }
            private:
                T p;
        };

    // 子类不是模板类，需要指明父类的具体类型
    class ChildOne : public Parent<int>
    {
        public:
            ChildOne(int a, int b) 
                : Parent(b)
            {
                this->one = a;
            }
        private:
            int one;
    };

    // 子类是模板类，可以用子类的反省来表示父类
    template <typename T>
        class ChildTwo : public Parent<T>
    {
        public:
            ChildTwo(T a, T b)
                : Parent<T>(b)
            {
                this->two = a;
            }
        private:
            T two;
    };
}

int main()
{
    ns_test1::Test_2();
    return 0;
}

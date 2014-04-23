#include <iostream>

using namespace std;

class MyClass
{
    public:
        MyClass()
        {
            cout <<"Constructors"<< endl;
        }
        ~MyClass()
        {
            cout <<"Destructors"<< endl;
        }

};

int main()
{
    MyClass* pMyClass =new MyClass;
    pMyClass->~MyClass();
    pMyClass->MyClass::MyClass();
    delete pMyClass;

}

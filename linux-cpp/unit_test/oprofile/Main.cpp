#include <iostream>

int fast_multiply(int x, int y)
{
    return x * y;
}

int slow_multiply(int x, int  y)
{
    int sum = 0;
    for (int i = 0; i < x; i++)
    {
        sum += y;
    }
    return sum;
}

void Test_1()
{
    int x,y; 
    for (int i = 0; i < 200; i ++) 
    { 
        for (int j = 0; j <  30 ; j++) 
        {
            x = fast_multiply(i, j); 
            y = slow_multiply(i, j); 
        } 
    } 
    std::cout << "x = " << x << ", y = "<< y <<std::endl;
}

int main()
{
    std::cout<<"Hello, world!\n";
    Test_1();
    return 1;
}

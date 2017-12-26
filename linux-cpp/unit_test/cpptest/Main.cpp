#include "cpptest.h"
#include <fstream> 


class MyTest : public Test::Suite
{
    public:
        void func1()
        {
            std::cout<<"MyTest::func1\n";
        }
        void func2()
        {
            std::cout<<"MyTest::func2\n";
        }

        MyTest()
        {
            TEST_ADD(MyTest::func1);
            TEST_ADD(MyTest::func2);
        }
};

int Test_1()
{
    MyTest tests;
    Test::TextOutput output(Test::TextOutput::Verbose);
    return tests.run(output);
}

class FailTest1 : public Test::Suite
{
    void allways_Fail()
    {
        TEST_FAIL("This is allways_Fail");
    }

public:
    FailTest1()
    {
        TEST_ADD(FailTest1::allways_Fail);
    }
};

int Test_2()
{
    FailTest1 tests;
    Test::TextOutput output(Test::TextOutput::Terse);
    return tests.run(output) ? 1 : 0;
}

int Test_3()
{
    FailTest1 tests;
    Test::CompilerOutput output;
    return tests.run(output) ? 1 : 0;
}

int Test_4()
{
    FailTest1 tests;
    std::ofstream ofile;
    ofile.open("test.log");
    Test::CompilerOutput output(Test::CompilerOutput::MSVC, ofile);
    return tests.run(output) ? 1 : 0;
}

void Test_5()
{
#if 0
    FailTest1 test1;
    std::ofstream ofile;
    ofile.open("test.log");
    Test::HtmlOutput output( );
    test1.run(output); 
    output.generate(ofile);
    return 0;
#endif
}

int main()
{
    return Test_4();
}

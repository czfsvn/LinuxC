#include <iostream>
#include <cstring>
#include <cppunit/TestCase.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace std;

//===================================
class MyString
{
    char* buffer;
    int length;
public:
    void setBuffer(char* s)
    {
        buffer = s;
        length = strlen(s);
    }
    char& operator[](const int idx)
    {
        return buffer[idx];
    }
    int size()  {   return length;  }

};

class MyStringTest : public CppUnit::TestCase
{
public:
    void runTest()
    {
        MyString s;
        CPPUNIT_ASSERT_MESSAGE("String Length Non-Zero", s.size() == 0);
    }

};

int Test_1()
{
    MyStringTest test;
    CppUnit::TextTestRunner runner;
    runner.addTest(&test);

    runner.run();
    return 0;
}
//===================================
class MyStringTest_2 : public CppUnit::TestCase
{
public:
    void checkLength()
    {
        MyString s;
        CPPUNIT_ASSERT_MESSAGE("String Length Non-Zero", s.size() == 0);
    }

    void checkValue()
    {
        MyString s;
        s.setBuffer("Hello, world!\n");
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Corrupt String Data", s[0], 'w');
    }

    CPPUNIT_TEST_SUITE(MyStringTest_2);
    CPPUNIT_TEST(checkLength);
    CPPUNIT_TEST(checkValue);
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(MyStringTest_2);
void Test_2()
{
    CppUnit::Test* test = CppUnit::TestFactoryRegistry::getRegistry().makeTest();
    CppUnit::TextTestRunner runner;
    runner.addTest(test);
    runner.run();
}

int main()
{
    //std::cout<<"Hello, world!"<<std::endl;
    Test_2();
    return 0;
}

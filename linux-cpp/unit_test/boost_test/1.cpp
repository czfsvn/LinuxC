#define BOOST_TEST_MODULE stringtest

#include <boost/test/included/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <cmath>
using boost::test_tools::output_test_stream;
//using boost::unit_test;

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

        char&   operator[](const int idx) {   return buffer[idx]; }
        int     size()  {   return length;  }

};

//BOOST_AUTO_TEST_SUITE (stringtest) // name of the test suite is stringtest

BOOST_AUTO_TEST_CASE(test1)
{
    MyString s;
    BOOST_CHECK(s.size() == 0);
}

BOOST_AUTO_TEST_CASE(test2)
{
    MyString s;
    s.setBuffer("Hello, world");
    BOOST_REQUIRE_EQUAL('H', s[0]);
}

//BOOST_AUTO_TEST_SUITE_END() // name of the test suite is stringtest


//BOOST_AUTO_TEST_SUITE(enumtest)

BOOST_AUTO_TEST_CASE (test3)
{
    typedef enum {
        red = 8, 
        blue, 
        green = 1, 
        yellow, 
        black 
    } color;
    color c = green;
    BOOST_WARN(sizeof(green) > sizeof(char));
    BOOST_CHECK(c == 2); 
    BOOST_REQUIRE(yellow > red);
    BOOST_CHECK(black != 4);
}

int predefined_user_func()
{
    return 0;
}

BOOST_AUTO_TEST_CASE(test4)
{
    output_test_stream output("run.log", true);
    output << predefined_user_func();
    BOOST_CHECK( output.match_pattern() );
}

//BOOST_AUTO_TEST_SUITE_END()

//BOOST_AUTO_TEST_SUITE(float_test)
/*
BOOST_AUTO_TEST_CASE(test1)
{
    float f1 = 567.0102;
    float result = sqrt(f1);
    BOOST_REQUIRE(f1 == result * result);
    BOOST_CHECK_CLOSE_FRACTION(f1, result * result, 0.000000001);
    BOOST_WARN(5 > 6);
    BOOST_CHECK(5 > 6);
    BOOST_REQUIRE(5 > 6);
}

//BOOST_AUTO_TEST_SUITE_END()

//BOOST_AUTO_TEST_SUITE(predicate)

boost::test_tools::predicate_result validate_list(std::list<int>& L1)
{
    for (auto val : L1)
    {
        if (val <= 1)
            return false;
    }
    return true;
}

bool validate_list_2(std::list<int>& L1)
{
    for (auto val : L1)
    {
        if (val <= 1)
            return false;
    }
    return true;
}

BOOST_AUTO_TEST_CASE(test1)
{
    std::list<int> iList{0, 2, 3, 4, 5};
    BOOST_CHECK(validate_list(iList));
    //BOOST_CHECK_PREDICATE(validate_list_2, iList);
}

BOOST_AUTO_TEST_CASE(test2)
{
}


boost::unit_test::test_suite* init_unit_test_suite()
{
    boost::unit_test::test_suite* ts1 = BOOST_TEST_SUITE("test_suite1");
    ts1->add(BOOST_TEST_CASE(&test2));
    return 0;
}
*/
//BOOST_AUTO_TEST_SUITE_END()



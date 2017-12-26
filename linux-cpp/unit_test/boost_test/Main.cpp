#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/assign.hpp>
#include <boost/static_assert.hpp>

using namespace boost;

BOOST_AUTO_TEST_SUITE(s_smart_ptr)

BOOST_AUTO_TEST_CASE(t_scope_ptr)
{
    //STATIC_ASSERT(5 >= 6);
    scoped_ptr<int> p(new int(874));
    BOOST_CHECK(p);
    BOOST_CHECK_EQUAL(*p, 874);

    p.reset();
    BOOST_CHECK(p);
}

BOOST_AUTO_TEST_CASE(t_smart_ptr)
{
    shared_ptr<int> p(new int(10));
    BOOST_CHECK(p);
    BOOST_CHECK_EQUAL(*p, 874);
    BOOST_CHECK_EQUAL(p.use_count(), 1);

    shared_ptr<int> p2 = p;
    BOOST_CHECK_EQUAL(p, p2);
    BOOST_CHECK_EQUAL(p2.use_count(), 2);
    *p2 = 255;

    BOOST_CHECK_EQUAL(*p, 255);
    BOOST_CHECK_GT(*p, 300);
}

BOOST_AUTO_TEST_SUITE_END()


struct global_fixture
{
    global_fixture()    {   std::cout<<"global setup\n";    }
    ~global_fixture()    {   std::cout<<"global teardown\n";    }
};

BOOST_GLOBAL_FIXTURE(global_fixture);

struct assign_fixture
{
    assign_fixture()    {   std::cout<<"assign setup\n";    }
    ~assign_fixture()    {   std::cout<<"assign teardown\n";    }

    std::vector<int>    v;
};

BOOST_FIXTURE_TEST_SUITE(s_assign, assign_fixture)

BOOST_AUTO_TEST_CASE(t_assign1)
{
    using namespace boost::assign;
    v += 1, 2, 3, 4;
    sleep(10);
    BOOST_CHECK_EQUAL(v.size(), 4);
    BOOST_CHECK_EQUAL(v[2], 3);
}

BOOST_AUTO_TEST_CASE(t_assign2)
{
    using namespace boost::assign;
    sleep(10);
    push_back(v)(10)(20)(30);
    BOOST_CHECK_EQUAL(v.empty(), false);
    BOOST_CHECK_EQUAL(v[2], v[3]);
}



BOOST_AUTO_TEST_SUITE_END()


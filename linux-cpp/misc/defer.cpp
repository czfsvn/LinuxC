#include <iostream>
#include <functional>


using namespace std;


namespace ns_defer1
{
    class defer
    {
        using OnDestructorAction = std::function<void()>;
    public:
        defer(OnDestructorAction action) : OnDestructor(action) {}
        ~defer() { OnDestructor(); }

    protected:
        OnDestructorAction OnDestructor;
    };
}

namespace ns_defer2
{
    class defer
    {
        using OnDestructorAction = std::function<void()>;
    public:
        defer(defer&) = delete;
        defer(OnDestructorAction action) : OnDestructor(action) {}
        ~defer() { OnDestructor(); }

        defer operator=(defer&) = delete;

    protected:
        OnDestructorAction OnDestructor;
    };
}

namespace ns_defer3
{
    class defer
    {
        using OnDestructorAction = std::function<void()>;
    public:
        defer(defer&) = delete;
        defer(OnDestructorAction action) : OnDestructor(action) {}
        ~defer() { OnDestructor(); }

        defer operator=(defer&) = delete;
        void* operator new(size_t) = delete;

    protected:
        OnDestructorAction OnDestructor;
    };
}

namespace ns_test
{
    void fun(bool doit)
    {
        ns_defer3::defer clean([]()
                {
                    std::cout << "clean" << std::endl;
                });
        if (!doit)
        {
            std::cout << "! doit" << std::endl;
            return;
        }
        ns_defer3::defer clean2([]()
                {
                    std::cout << "clean 2" << std::endl;
                });
        std::cout << "doit" << std::endl;

        return;
    }

    void main()
    {
        std::cout << "fun(false)" << std::endl;
        fun(false);
        std::cout << "\nfunc(true)" << std::endl;
        fun(true);
    }
}


int main()
{
    std::cout << "hello defer\n";
    ns_test::main();
}

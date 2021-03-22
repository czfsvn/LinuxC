/*
 * examples from: http://blog.csdn.net/u014613043/article/details/50619254
 */

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

using namespace std;

template <typename T>
void PrintEle(T& cont, const char* str)
{
    std::cout << "PrintEle " << str << ": ";
    typename T::const_iterator it = cont.cbegin();
    for (; it != cont.cend(); it++)
    {
        std::cout << *it << ", ";
    }
    std::cout << std::endl;
}

template <typename T>
void print(T& t)
{
    std::cout << t << std::endl;
}

template <typename T, int val>
void print_1(T& t)
{
    std::cout << t << std::endl;
}

template <typename T>
void print_2(T& t, int str)
{
    std::cout << t << std::endl;
}

void print_(int x, const char* str)
{
    std::cout << str << ":" << x << std::endl;
}

template <typename T, char str>
void play(T ele)
{
    std::cout << str << ele << std::endl;
}

namespace ns_test1
{
    struct Play1
    {
        void operator()(int i)
        {
            std::cout << i << std::endl;
        }
    };

    struct Play2
    {
        Play2(std::string s) : str(s) {}
        void operator()(int i)
        {
            std::cout << str << ":" << i << std::endl;
        }
        std::string str;
    };

    template <typename T, typename V>
    class Play3
    {
    private:
        V _str;

    public:
        Play3(V str) : _str(str) {}
        void operator()(T ele)
        {
            std::cout << _str << "-" << ele << std::endl;
        }
    };

    template <typename T, typename V>
    class Play4
    {
    public:
        Play4(V s) : str_(s) {}
        void operator()(T ele)
        {
            std::cout << str_ << ": " << ele << std::endl;
        }

    private:
        V str_;
    };

    void Test_1()
    {
        int              a[] = { 1, 2, 3, 4 };
        std::vector<int> v(a, a + sizeof(a) / sizeof(int));

        // use template
        PrintEle(v, "ns_test1::Test_1");

        // use function template
        std::for_each(v.begin(), v.end(), print<int>);
        std::for_each(v.begin(), v.end(), print_1<int, 22>);
        // std::for_each(v.begin(), v.end(), bind2nd(ptr_fun(print_2<int>), 222));

        // use function , with other params
        std::for_each(v.begin(), v.end(), bind2nd(ptr_fun(print_), "bind2nd"));

        // use function , with other params
        std::for_each(v.begin(), v.end(), std::bind(print_, std::placeholders::_1, "bind"));

        // use function object
        std::for_each(v.begin(), v.end(), Play1());
        std::for_each(v.begin(), v.end(), Play2("play2"));
        std::for_each(v.begin(), v.end(), Play3<int, const char*>("element"));

        Play2 p2("p2");
        std::for_each(v.begin(), v.end(), p2);

        std::for_each(v.begin(), v.end(), play<int, 'a'>);

        std::for_each(v.begin(), v.end(), Play3<int>());

        std::for_each(v.begin(), v.end(), Play4<int, std::string>("Play4 "));
    }
};  // namespace ns_test1

namespace ns_test2
{
    class Door
    {
    public:
        void open() const
        {
            cout << "open door horizontally\n";
        }
    };

    class DoorController
    {
    protected:
        std::vector<Door> doorVec_;

    public:
        // use mem_fun          if pointer in container
        // use mem_fun_ref      if object  in container
        void addDoor(Door door)
        {
            doorVec_.push_back(door);
        }
        void openDoor() const
        {
            std::for_each(doorVec_.begin(), doorVec_.end(), mem_fun_ref(&Door::open));
        }
    };

    void Test_1()
    {
        DoorController dc;
        dc.addDoor(Door());
        dc.addDoor(Door());
        dc.addDoor(Door());
        dc.openDoor();
    }
}  // namespace ns_test2

namespace ns_test3
{
    class AbstractDoor
    {
    public:
        virtual void open(const char* str) const = 0;
        virtual ~AbstractDoor() {}
    };

    class HorizontalDoor : public AbstractDoor
    {
    public:
        virtual void open(const char* str) const
        {
            std::cout << "open door horizontally\n";
        }
    };

    class VerticalDoor : public AbstractDoor
    {
    public:
        virtual void open(const char* str) const
        {
            std::cout << "open door vertically\n";
        }
    };

    class DoorContoller
    {
    protected:
        vector<AbstractDoor*> doorVec_;

    public:
        void addDoor(AbstractDoor* door)
        {
            doorVec_.push_back(door);
        }

        // use mem_fun          if pointer in container
        // use mem_fun_ref      if object  in container
        void openDoor() const
        {
            std::for_each(doorVec_.begin(), doorVec_.end(), std::bind2nd(mem_fun(&AbstractDoor::open), "John"));
        }

        ~DoorContoller()
        {
            std::for_each(doorVec_.begin(), doorVec_.end(), [](AbstractDoor* door) {
                delete door;
                door = nullptr;
            });
            doorVec_.clear();
        }
    };

    void Test_1()
    {
        DoorContoller dc;
        dc.addDoor(new HorizontalDoor);
        dc.addDoor(new VerticalDoor);
        virtual void open(const char* str) const = 0;
        virtual ~AbstractDoor() {}
    };

    class HorizontalDoor : public AbstractDoor
    {
    public:
        void open(const char* str) const
        {
            std::cout << str << "open door horiaontally\n";
        }
    };
    class VerticalDoor : public AbstractDoor
    {
    public:
        void open(const char* str) const
        {
            std::cout << str << "open door vertically\n";
        }
    };
    class DoorController
    {
    protected:
        std::vector<AbstractDoor*> doorVec_;

    public:
        void addDoor(AbstractDoor* door)
        {
            doorVec_.push_back(door);
        }
        void openDoor()
        {
            std::for_each(doorVec_.begin(), doorVec_.end(), std::bind2nd(mem_fun(&AbstractDoor::open), "John "));
        }
        ~DoorController()
        {
            std::for_each(doorVec_.begin(), doorVec_.end(), [](AbstractDoor* p) {
                delete p;
                p = nullptr;
            });
        }
    };
    void Test_1()
    {
        DoorController dc;
        dc.addDoor(new HorizontalDoor());
        dc.addDoor(new VerticalDoor());
        dc.openDoor();
    }
}  // namespace ns_test3

namespace ns_test4
{
    class MeanValue
    {
    private:
        long num_;
        long sum_;

    public:
        MeanValue() : num_(0), sum_(0) {}
        void operator()(int ele)
        {
            ++num_;
            sum_ += ele;
        }
        double value()
        {
            return static_cast<double>(sum_) / static_cast<double>(num_);
        }
        operator double()
        {
            return static_cast<double>(sum_) / static_cast<double>(num_);
        }
    };
    // with return value
    void Test_1()
    {
        std::vector<int> v   = { 1, 2, 3, 4, 5, 6, 7, 8 };
        double           sum = std::for_each(v.begin(), v.end(), MeanValue());
        std::cout << "val : " << sum << std::endl;

        MeanValue mv = std::for_each(v.begin(), v.end(), MeanValue());
        std::cout << "val : " << mv.value() << std::endl;
    }
}  // namespace ns_test4

int main()
{
    // ns_test1::Test_1();
    // ns_test3::Test_1();
    ns_test4::Test_1();
    return 1;
}

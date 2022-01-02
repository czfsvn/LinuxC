#include <chrono>
#include <iostream>

using namespace std;

namespace ns_duration
{
    void test1()
    {
        std::chrono::milliseconds ms{ 3 };
        std::chrono::microseconds us = 2 * ms;

        std::chrono::duration<double, std::ratio<1, 30>> hz30{ 3.5 };

        std::cout << "3 ms duration has " << ms.count() << " ticks\n";
        std::cout << "6000 us duration has " << us.count() << " ticks\n";
    }

    void test2()
    {
        using namespace std::chrono;
        minutes t1(10);
        seconds t2(60);
        seconds t3 = t1 - t2;
        std::cout << "t3.count() : " << t3.count() << std::endl;
        std::cout << "t3.count() : " << duration_cast<minutes>(t3).count() << std::endl;
        std::cout << "t3.count() : " << duration_cast<hours>(t3).count() << std::endl;
    }
    void main()
    {
        test1();
        test2();
    }
}  // namespace ns_duration

namespace ns_time_point
{
    void test1()
    {
        using namespace std::chrono;
        typedef duration<int32_t, std::ratio<60 * 60 * 24>> day_type;

        time_point<system_clock, day_type> today = time_point_cast<day_type>(system_clock::now());
        std::cout << today.time_since_epoch().count() << " days since epoch\n";
        std::cout << duration_cast<seconds>(today.time_since_epoch()).count() << " days since epoch\n";
    }

    void test2()
    {
        using namespace std::chrono;
        system_clock::time_point now = system_clock::now();

        // to_time_t    将一个  time_point    转换成  ctime
        // from_time_t  将一个  ctime         转换成  time_point
        std::time_t last = system_clock::to_time_t(now - hours(24));
        std::time_t next = system_clock::to_time_t(now + hours(24));

#ifdef WIN32
        std::cout << "last day: " << std::put_time(std::localtime(&last), "%F %T") << std::endl;
#endif
    }
    void main()
    {
        test1();
        test2();
    }
}  // namespace ns_time_point

namespace ns_timer
{
    using namespace std::chrono;
    class Timer
    {
    public:
        Timer() : begin(high_resolution_clock::now()) {}

        void reset()
        {
            begin = high_resolution_clock::now();
        }

        // 默认输出毫秒
        template <typename Duartion = milliseconds>
        int64_t elapsed() const
        {
            return duration_cast<Duartion>(high_resolution_clock::now() - begin).count();
        }

        // 输出微秒
        int64_t elapsed_micro() const
        {
            return elapsed<microseconds>();
        }

        // 输出纳秒
        int64_t elapsed_nano() const
        {
            return elapsed<nanoseconds>();
        }

        // 输出秒
        int64_t elapsed_seconds() const
        {
            return elapsed<seconds>();
        }

        // 输出分
        int64_t elapsed_minutes() const
        {
            return elapsed<minutes>();
        }

        // 输出时
        int64_t elapsed_hours() const
        {
            return elapsed<hours>();
        }

    private:
        time_point<high_resolution_clock> begin = {};
    };

    class BlockCost
    {
    public:
        BlockCost(const char* func, uint32_t lines) : func_name(func), line(lines) {}

        ~BlockCost()
        {
            std::cout << func_name << ":" << line << line << ", cost: " << timer.elapsed_micro() << std::endl;
        }

    private:
        std::string func_name = {};
        uint32_t    line      = 0;
        Timer       timer     = {};
    };
}  // namespace ns_timer

int main()
{
    // ns_duration::main();
    ns_time_point::main();

    std::cout << "Hello, chrono\n";
    return 0;
}
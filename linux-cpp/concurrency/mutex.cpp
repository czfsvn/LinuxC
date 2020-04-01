#include <iostream>
#include <vector>
#include <list>
#include <thread>
#include <mutex>

using namespace std;

#define LINE    cout << "\n=========\t" << __FUNCTION__ << "\t ============\n";

class ScopedThread
{   
    public:
        explicit ScopedThread(std::thread t) : thrd(std::move(t))
        {   
            if (!thrd.joinable())
                throw std::logic_error("No thread");
        }   
        ~ScopedThread() {   thrd.join();  }
        ScopedThread(const ScopedThread&)   = delete;
        ScopedThread& operator= (const ScopedThread&) = delete;
    private:
        std::thread thrd;
};

void sleep_for()
{
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //c++11 可以用
    std::this_thread::sleep_for(1s);    // c++14 以后可以这样用
    std::this_thread::sleep_for(1000ms);    // c++14 以后可以这样用
    std::this_thread::sleep_for(1000ns);    // c++14 以后可以这样用
    std::this_thread::sleep_for(1000us);    // c++14 以后可以这样用
    //std::this_thread::sleep_for(1min);    // c++14 以后可以这样用
    //std::this_thread::sleep_for(1h);    // c++14 以后可以这样用
    //std::this_thread::sleep_for(1d);    // c++14 以后可以这样用
}

namespace ns_m1
{
    class Work
    {
        public:
            void run1()
            {   // 直接使用 lock 和 unlock 的时候容易造成不能配对使用的bug
                mtx.lock();
                LINE;
                std::cout << "thread: " << std::this_thread::get_id() << std::endl;
                sleep_for();
                mtx.unlock();
            }

            void run2()
            {
                // lock_guard 在构造的时候调用 mtx.lock(), 析构的时候调用 mtx.unlock()
                std::lock_guard<std::mutex> guard(mtx);
                LINE;
                std::cout << "thread: " << std::this_thread::get_id() << std::endl;
                sleep_for();
            }

            void run3()
            {
                // unique_lock 在构造的时候调用 mtx.lock(), 析构的时候调用 mtx.unlock()
                // 提供了比std::lock_guard 更多的灵活性
                // 但是占用了更多的空间，比 std::lock_guard 略慢
                std::unique_lock<std::mutex> guard(mtx);
                LINE;
                std::cout << "thread: " << std::this_thread::get_id() << std::endl;
                sleep_for();
            }
        private:
            std::mutex mtx;
    };

    void test_mutex()
    {
        Work work;
        ScopedThread scope_thrd1(std::thread(&Work::run1, &work));
        ScopedThread scope_thrd2(std::thread(&Work::run1, &work));
        ScopedThread scope_thrd3(std::thread(&Work::run1, &work));
    }

    void test_lock_guard()
    {
        Work work;
        ScopedThread scope_thrd1(std::thread(&Work::run2, &work));
        ScopedThread scope_thrd2(std::thread(&Work::run2, &work));
        ScopedThread scope_thrd3(std::thread(&Work::run2, &work));
    }

    void test_uniq_lock()
    {
        Work work;
        ScopedThread scope_thrd1(std::thread(&Work::run3, &work));
        ScopedThread scope_thrd2(std::thread(&Work::run3, &work));
        ScopedThread scope_thrd3(std::thread(&Work::run3, &work));
    }

    void main()
    {
        // 三种实现方式达到相同的效果，推荐使用 std::lock_guard
        test_mutex();
        test_lock_guard();
        test_uniq_lock();
    }
}

namespace ns_dead_lock
{
    class Work
    {
        public:
            void push1()
            {
                LINE;
                for (uint32_t idx = 0; idx < 100000; idx++)
                {
                    std::lock_guard<std::mutex> g1(push_mtx);
                    //std::lock_guard<std::mutex> g2(pop_mtx);
                    std::cout << "push1: " << idx << std::endl;
                    uVec.emplace_back(idx);
                }
            }
            
            void pop1()
            {
                LINE;
                for (uint32_t idx = 0; idx < 100000; idx++)
                {
                    //std::lock_guard<std::mutex> g1(pop_mtx);
                    //std::lock_guard<std::mutex> g2(push_mtx);
                    // 如果跟push1 一样的lock顺序就不会死锁
                    // 始终使用相同的顺序锁定这两个互斥元
                    std::lock_guard<std::mutex> g1(push_mtx);
                    //std::lock_guard<std::mutex> g2(pop_mtx);
                    std::cout << "pop1 : " << idx << std::endl;
                    uVec.pop_back();
                }
            }

            void push2()
            {
                LINE;
                for (uint32_t idx = 0; idx < 100000; idx++)
                {
                    std::cout << "push2: " << idx << std::endl;
                    std::lock(push_mtx, pop_mtx);
                    std::lock_guard<std::mutex> push_lock(push_mtx, std::adopt_lock);
                    std::lock_guard<std::mutex> pop_lock(pop_mtx, std::adopt_lock);
                    // std::adopt_lock 告知 std::lock_guard 对象该互斥元已被锁定，
                    // 在lock_guard构造函数中无需调用锁定
                    uVec.emplace_back(idx);
                }
            }
            void pop2()
            {
                LINE;
                for (uint32_t idx = 0; idx < 100000; idx++)
                {
                    std::cout << "pop2: " << idx << std::endl;
                    std::lock(pop_mtx, push_mtx);
                    std::lock_guard<std::mutex> pop_lock(pop_mtx, std::adopt_lock);
                    std::lock_guard<std::mutex> push_lock(push_mtx, std::adopt_lock);
                    uVec.pop_back();
                }
            }
        private:
            std::mutex push_mtx;
            std::mutex pop_mtx;
            std::list<uint32_t> uVec = {};
    };

    void test_dead()
    {
        LINE;
        Work work;
        ScopedThread th1(std::thread(&Work::pop1, &work));
        ScopedThread th2(std::thread(&Work::push1, &work));
    }

    void test_dead2()
    {
        LINE;
        Work work;
        ScopedThread th1(std::thread(&Work::pop2, &work));
        ScopedThread th2(std::thread(&Work::push2, &work));
    }

    void main()
    {
        test_dead();
        //test_dead2();
    }
}

int main()
{
    ns_m1::main();
    //ns_dead_lock::main();
    LINE;
    std::cout << "Hello\n";
    return 0;
}

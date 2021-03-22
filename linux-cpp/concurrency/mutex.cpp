#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <thread>
#include <mutex>
#include <boost/thread/shared_mutex.hpp>

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
        ~ScopedThread() 
        {   \
            thrd.join();  
        }
        ScopedThread(const ScopedThread&)   = delete;
        ScopedThread& operator= (const ScopedThread&) = delete;
    private:
        std::thread thrd;
};

void sleep_for()
{
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //c++11 �?以用
    std::this_thread::sleep_for(1s);    // c++14 以后�?以这样用
    std::this_thread::sleep_for(1000ms);    // c++14 以后�?以这样用
    std::this_thread::sleep_for(1000ns);    // c++14 以后�?以这样用
    std::this_thread::sleep_for(1000us);    // c++14 以后�?以这样用
    //std::this_thread::sleep_for(1min);    // c++14 以后�?以这样用
    //std::this_thread::sleep_for(1h);    // c++14 以后�?以这样用
    //std::this_thread::sleep_for(1d);    // c++14 以后�?以这样用
}

namespace ns_m1
{
    class Work
    {
        public:
            void run1()
            {   // 直接使用 lock �? unlock 的时候�?�易造成不能配�?�使用的bug
                mtx.lock();
                LINE;
                std::cout << "thread: " << std::this_thread::get_id() << std::endl;
                sleep_for();
                mtx.unlock();
            }

            void run2()
            {
                // lock_guard 在构造的时候调�? mtx.lock(), 析构的时候调�? mtx.unlock()
                std::lock_guard<std::mutex> guard(mtx);
                LINE;
                std::cout << "thread: " << std::this_thread::get_id() << std::endl;
                sleep_for();
            }

            void run3()
            {
                // unique_lock 在构造的时候调�? mtx.lock(), 析构的时候调�? mtx.unlock()
                // 提供了比std::lock_guard 更�?�的灵活�?
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
        // 三�?�实现方式达到相同的效果，推荐使�? std::lock_guard
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
            void push3()
            {
                LINE;
                for (uint32_t idx = 0; idx < 100000; idx++)
                {
                    std::cout << "push3: " << idx << std::endl;
                    std::lock(push_mtx, pop_mtx);
                    uList.emplace_back(idx);

                    // 使用lock 会尝试锁定两�?或�?�个互斥元，要么两个都锁定，要么都不锁定
                    push_mtx.unlock();
                    pop_mtx.unlock();
                }
            }
            void pop3()
            {
                LINE;
                for (uint32_t idx = 0; idx < 100000; idx++)
                {
                    std::lock(pop_mtx, push_mtx);

                    if (uList.empty())
                    {
                        push_mtx.unlock();
                        pop_mtx.unlock();
                        continue;
                    }

                    std::cout << "pop3: " << idx << std::endl;
                    uList.pop_front();
                    push_mtx.unlock();
                    pop_mtx.unlock();
                }
            }

            void push1()
            {
                LINE;
                for (uint32_t idx = 0; idx < 100000; idx++)
                {
                    std::lock_guard<std::mutex> g1(push_mtx);
                    std::lock_guard<std::mutex> g2(pop_mtx);
                    std::cout << "push1: " << idx << std::endl;
                    uList.emplace_back(idx);
                }
            }
            
            void pop1()
            {
                LINE;
                for (uint32_t idx = 0; idx < 100000; idx++)
                {
                    //std::lock_guard<std::mutex> g1(pop_mtx);
                    //std::lock_guard<std::mutex> g2(push_mtx);
                    // 以上与push1 一起配合造成死锁
                    // 如果跟push1 一样的lock顺序就不会�?�锁
                    // 始终使用相同的顺序锁定这两个互斥�?
                    std::lock_guard<std::mutex> g1(push_mtx);
                    std::lock_guard<std::mutex> g2(pop_mtx);
                    if (uList.empty())
                        continue;

                    std::cout << "pop1 : " << idx << std::endl;
                    uList.pop_front();
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
                    // std::adopt_lock 告知 std::lock_guard 对象该互斥元已�??锁定�?
                    // 在lock_guard构造函数中无需调用锁定
                    uList.emplace_back(idx);
                }
            }
            void pop2()
            {
                LINE;
                for (uint32_t idx = 0; idx < 100000; idx++)
                {
                    std::lock(pop_mtx, push_mtx);
                    std::lock_guard<std::mutex> pop_lock(pop_mtx, std::adopt_lock);
                    std::lock_guard<std::mutex> push_lock(push_mtx, std::adopt_lock);

                    if (uList.empty())
                        continue;

                    std::cout << "pop2: " << idx << std::endl;
                    uList.pop_front();
                }
            }

            void push4()
            {
                LINE;
                for (uint32_t idx = 0; idx < 100000; idx++)
                {
                    std::cout << "push4: " << idx << std::endl;
                    std::lock(push_mtx, pop_mtx);
                    std::unique_lock<std::mutex> push_lock(push_mtx, std::adopt_lock);
                    std::unique_lock<std::mutex> pop_lock(pop_mtx, std::adopt_lock);
                    // std::adopt_lock 告知 std::unique_lock 对象该互斥元已�??锁定�?
                    // �? unique_lock 构造函数中无需调用锁定
                    // 而在 std::unique_lock 的析构函数中仍然需要调用unlock解锁�?
                    uList.emplace_back(idx);
                }
            }
            void pop4()
            {
                LINE;
                for (uint32_t idx = 0; idx < 100000; idx++)
                {
                    std::lock(pop_mtx, push_mtx);
                    std::unique_lock<std::mutex> pop_lock(pop_mtx, std::adopt_lock);
                    std::unique_lock<std::mutex> push_lock(push_mtx, std::adopt_lock);

                    if (uList.empty())
                        continue;

                    std::cout << "pop4: " << idx << std::endl;
                    uList.pop_front();
                }
            }

            void push5()
            {
                LINE;
                for (uint32_t idx = 0; idx < 100000; idx++)
                {
                    std::cout << "push5: " << idx << std::endl;

                    std::unique_lock<std::mutex> push_lock(push_mtx, std::defer_lock);
                    std::unique_lock<std::mutex> pop_lock(pop_mtx, std::defer_lock);
                    std::lock(push_lock, pop_lock);

                    // std::defer_lock 告知 std::unique_lock/std::lock_guard 
                    // 对象该互斥元�?�?锁定�?
                    // 需要在 unique_lock 构造函数中需调用锁定
                    // 而在 std::unique_lock 的析构函数中仍然需要调用unlock解锁�?
                    uList.emplace_back(idx);
                }
            }
            void pop5()
            {
                LINE;
                for (uint32_t idx = 0; idx < 100000; idx++)
                {
                    std::unique_lock<std::mutex> pop_lock(pop_mtx, std::defer_lock);
                    std::unique_lock<std::mutex> push_lock(push_mtx, std::defer_lock);
                    std::lock(pop_lock, push_lock);

                    if (uList.empty())
                        continue;

                    std::cout << "pop5: " << idx << std::endl;
                    uList.pop_front();
                }
            }

        private:
            std::mutex push_mtx;
            std::mutex pop_mtx;
            std::list<uint32_t> uList = {};
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

    void test_dead3()
    {
        LINE;
        Work work;
        ScopedThread th1(std::thread(&Work::pop3, &work));
        ScopedThread th2(std::thread(&Work::push3, &work));
    }
    void test_dead4()
    {
        LINE;
        Work work;
        ScopedThread th1(std::thread(&Work::pop4, &work));
        ScopedThread th2(std::thread(&Work::push4, &work));
    }
    void test_dead5()
    {
        LINE;
        Work work;
        ScopedThread th1(std::thread(&Work::pop5, &work));
        ScopedThread th2(std::thread(&Work::push5, &work));
    }

    void main()
    {
        //test_dead();
        //test_dead2();
        //test_dead3();
        //test_dead4();
        test_dead5();
    }
}

namespace ns_uniq
{
    std::mutex  mtx;
    void do_something1()
    {
        LINE;
    }
    void do_something2()
    {
        LINE;
    }
    
    // 成员函数 lock�? unlock
    void test1()
    {
        std::unique_lock<std::mutex> ulock(mtx);
        
        // do something

        ulock.unlock();

        // do something

        ulock.lock();

        // do other things
    }

    // 成员函数 release
    void test2()
    {
        //返回它所管理的mutex对象指针，并释放所有权�?
            //也就�?说，这个unique_lock和mutex不再有关�?
            //如果原来mutex对像处于加锁状态，你有责任接�?�过来并负责解锁�?
            //（release返回的是原�?�mutex的指针）
            // 测试
        std::unique_lock<std::mutex> sbguard(mtx);
        std::mutex *ptx = sbguard.release(); 
    }

    // unique_lock �?二个参数：std::try_to_lock
    void test3()
    {
        //尝试用mutex的lock()去锁定这个mutex
            //如果没有锁定成功，也会立即返回，并不会阻塞在那里�?
        //用这个try_to_lock的前提是你自己不能先lock
        std::unique_lock<std::mutex> sbguard(mtx, std::try_to_lock);
        if (sbguard.owns_lock())
            std::cout << "获得锁\n";
        else
            std::cout << "获得锁失�?\n";
    }

    // unique_lock �?二个参数：std::defer_lock
    void test4()
    {
        //尝试给互斥量加锁�?
            //如果拿不到锁，返回false,
            //如果拿到了锁，返回true,这个函数�?不阻塞的
        std::unique_lock<std::mutex> sbguard(mtx, std::defer_lock);
        if (sbguard.try_lock())
            std::cout << "获得锁\n";
        else
            std::cout << "获得锁失�?\n";
    }

    // unique_lock �?二个参数：std::adopt_lock
    void test5()
    {
        // mtx 先lock，然后在sbguard�?不需要调用构造函数里的lock
            // 但是析构函数仍然会自动调�? unlock 
        mtx.lock();
        std::unique_lock<std::mutex> sbguard(mtx, std::adopt_lock);
    }


    //从函数中返回一�?局部的unique_lock对象�?�?以的
    std::unique_lock<std::mutex> rtn_unique_lock()
    {
        std::unique_lock<std::mutex> tmpguard(mtx);
        return tmpguard;
    }

    // �?移unique_lock 的所有权
        // unique_lock �?移动，不�?复制
    void test6()
    {
        std::unique_lock<std::mutex> lock = rtn_unique_lock();
        
        //std::unique_lock<std::mutex> lock2(lock); 调用非法，无法调用�?�制构造函�?

        // 调用std::move 后， lock3 拥有 lock 的所有权，lock已经没有所有权
        std::unique_lock<std::mutex> lock3(std::move(lock));
    }
};

namespace ns_call_once
{
    class TestSingleton;


    std::mutex mtx;
    std::once_flag  flag;
    class TestSingleton
    {
        private:
            static TestSingleton* inst1 = NULL;//nullptr;
            static TestSingleton* inst2 = nullptr;

        public:
            static TestSingleton* getInst1();
            static TestSingleton* getInst2();

            static void createInst();

            void print();

            class Destroy1
            {
                public:
                    ~Destroy1()
                    {
                        if (TestSingleton::inst1)
                        {
                            delete TestSingleton::inst1;
                            TestSingleton::inst1 = nullptr;
                        }
                    }
            };
            class Destroy2
            {
                public:
                    ~Destroy2()
                    {
                        if (TestSingleton::inst2)
                        {
                            delete TestSingleton::inst2;
                            TestSingleton::inst2 = nullptr;
                        }
                    }
            };
    };

    TestSingleton* TestSingleton::inst2;
    TestSingleton* TestSingleton::inst1;

    TestSingleton* TestSingleton::getInst1()
    {
        if (!inst1)
        {
            std::cout << "TestSingleton getInst1()\n";
            std::lock_guard<std::mutex> guard(mtx);
            if (!inst1)
            {
                inst1 = new TestSingleton();
                static TestSingleton::Destroy1 destroy;
            }
        }
        return inst1;
    }

    TestSingleton* TestSingleton::getInst2()
    {
        //std::call_once(flag, TestSingleton::createInst());
        std::cout << "thread id " << std::this_thread::get_id() << ", getInst2\n";
        std::call_once(flag, createInst);
        return inst2;
    }

    void TestSingleton::createInst()
    {
        std::cout << "thread id " << std::this_thread::get_id() << ", TestSingleton createInst\n";
        inst2 = new TestSingleton();
        static TestSingleton::Destroy2 destroy;
    }

    void TestSingleton::print()
    {
        LINE;
    }

    void func1()
    {
        ns_call_once::TestSingleton::getInst1()->print();
    }

    void func2()
    {
        TestSingleton::getInst2()->print();
    }

    void test1()
    {
        ScopedThread thrd1(std::thread(ns_call_once::func1)); 
        ScopedThread thrd2(std::thread(ns_call_once::func1)); 
        ScopedThread thrd3(std::thread(ns_call_once::func1)); 
        ScopedThread thrd4(std::thread(ns_call_once::func1)); 
        ScopedThread thrd5(std::thread(ns_call_once::func1)); 
        ScopedThread thrd6(std::thread(ns_call_once::func1)); 
    }

    void test2()
    {
        ScopedThread thrd1(std::thread(ns_call_once::func2)); 
        ScopedThread thrd2(std::thread(ns_call_once::func2)); 
        ScopedThread thrd3(std::thread(ns_call_once::func2)); 
        ScopedThread thrd4(std::thread(ns_call_once::func2)); 
        ScopedThread thrd5(std::thread(ns_call_once::func2)); 
        ScopedThread thrd6(std::thread(ns_call_once::func2)); 
    }

    void main()
    {
        test1();
        //test2();
    }
};

namespace ns_shared_mutex
{   // 单个线程�?占式写，多个线程读的并发访问
    class DnsEntry  {};

    class DnsCache
    {
        private:
            std::map<std::string, DnsEntry> entries;
            mutable boost::shared_mutex entry_mutex;
        public:
            DnsEntry findEntry(const std::string& domain) const
            {
                boost::shared_lock<boost::shared_mutex> lk(entry_mutex);
                LINE;
                std::map<std::string, DnsEntry>::const_iterator iter = entries.find(domain);
                return (iter == entries.end() ? DnsEntry() : iter->second);
            }

            void updateOrAddEntry(const std::string& domain, const DnsEntry& entry)
            {
                std::lock_guard<boost::shared_mutex> lk(entry_mutex);
                LINE;
                entries[domain] = entry;
            }
    };

    void test1()
    {
        // 测试一下中�?

    }

    void main()
    {
    }
}

int main()
{
    //ns_m1::main();
    //ns_dead_lock::main();
    //ns_call_once::main();
    ns_shared_mutex::main();
    LINE;
    std::cout << "Hello\n";
    return 0;
}

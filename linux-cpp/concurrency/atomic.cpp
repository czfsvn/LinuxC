#include <iostream>
#include <future>
#include <thread>
#include <atomic>
#include <chrono>

using namespace std;

#define LINE    cout << "\n=====\t" << __FUNCTION__ << ": thread: " << std::this_thread::get_id() << "\t=======\n";

class ScopeThread
{
    public:
        explicit ScopeThread(std::thread& t) : thrd(std::move(t))
        {   
            LINE;
            if (!thrd.joinable())
                throw std::logic_error("No thread");
        }   
        ~ScopeThread() {   thrd.join();  }
        ScopeThread(const ScopeThread&)   = delete;
        ScopeThread& operator= (const ScopeThread&) = delete;
    private:
        std::thread thrd;
};

class TimeCost
{
    public:
        TimeCost(const std::string& func) : func_name(func)
        {
        }
        ~TimeCost()
        {
            //uint32_t interval = time(NULL) - from;
            std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
            auto dura = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            std::cout << func_name << " cost: " << dura <<" ms\n";
            //std::cout << func_name << " cost: " << interval <<" seconsd\n";
        }

    private:
        std::chrono::system_clock::time_point start = chrono::system_clock::now();
        std::string func_name;
        //uint32_t from = time(NULL);
};

#if 0
class TimeCost
{
    public:
        TimeCost(const std::string& func) : func_name(func)
        {
        }
        ~TimeCost()
        {
            clock_t end = clock();
            std::cout << func_name << " cost: " << ((double)(end - start)) / CLOCKS_PER_SEC << " seconds\n";
        }

    private:
        clock_t start = clock();
        std::string func_name;
};
#endif

#define TIME_COST   TimeCost __time_cost__(__FUNCTION__)

namespace ns_atomic
{
    uint32_t add_val_mux = 0;
    std::atomic<uint32_t> atomic_val = 0;
    uint32_t lool_count = 100000000;

    std::mutex mtx;
    void add_with_mtx()
    {
        LINE;
        for (uint32_t idx = 0; idx < lool_count; idx++)
        {
            std::lock_guard<std::mutex> lk(mtx);
            add_val_mux ++;
        }
    }

    void add_with_atomic()
    {
        LINE;
        for (uint32_t idx = 0; idx < lool_count; idx++)
        {
            atomic_val ++;
        }
    }

    void test_with_mutex()
    {
        LINE;
        TIME_COST;
        std::thread t1(add_with_mtx);
        std::thread t2(add_with_mtx);
        std::thread t3(add_with_mtx);
        std::thread t4(add_with_mtx);
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        /*
        ScopeThread thrd1(std::thread(add_with_mtx));
        ScopeThread thrd2(std::thread(add_with_mtx));
        ScopeThread thrd3(std::thread(add_with_mtx));
        ScopeThread thrd4(std::thread(add_with_mtx));
        ScopeThread thrd5(std::thread(add_with_mtx));
        ScopeThread thrd6(std::thread(add_with_mtx));
        ScopeThread thrd7(std::thread(add_with_mtx));
        ScopeThread thrd8(std::thread(add_with_mtx));
        ScopeThread thrd9(std::thread(add_with_mtx));
        */
        std::cout << "add_val_mux :: " << add_val_mux << std::endl;
    }

    void test_with_atomic()
    {
        TIME_COST;
        std::thread t1(add_with_atomic);
        std::thread t2(add_with_atomic);
        std::thread t3(add_with_atomic);
        std::thread t4(add_with_atomic);
        t1.join();
        t2.join();
        t3.join();
        t4.join();
        //ScopeThread thrd4(t4);
        /*
        //ScopeThread thrd1(std::thread(add_with_atomic));
        ScopeThread thrd2(std::thread(add_with_atomic));
        ScopeThread thrd3(std::thread(add_with_atomic));
        ScopeThread thrd4(std::thread(add_with_atomic));
        ScopeThread thrd5(std::thread(add_with_atomic));
        ScopeThread thrd6(std::thread(add_with_atomic));
        ScopeThread thrd7(std::thread(add_with_atomic));
        ScopeThread thrd8(std::thread(add_with_atomic));
        ScopeThread thrd9(std::thread(add_with_atomic));
        */
        std::cout << "atomic_val :: " << atomic_val << std::endl;
    }

    void test_time()
    {
        //test_with_mutex();
        test_with_atomic();
    }

    void main()
    {
        test_time();
    }
}

int main()
{
    LINE;
    ns_atomic::main();

    return 0;
}

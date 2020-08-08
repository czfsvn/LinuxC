#include <iostream>
#include <future>
#include <vector>

using namespace std;

#define LINE    cout << "\n=====\t" << __FUNCTION__ << ": thread: " << std::this_thread::get_id() << "\t=======\n";

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

namespace ns_package
{
    // std::package_task:  打包任务，把任务包装起来
    // 类模板，参数是各种可调用对象；
    // 通过std::package_task 把各种可调用对象包装起来，方便作为线程的入口函数

    uint32_t get_result1(uint32_t num)
    {
        LINE;
        std::this_thread::sleep_for(5s);

        return num;
    }
    
    void test_thread()
    {
        LINE;
        // 通过package_task 包装 get_result1
        std::packaged_task<uint32_t(uint32_t)> pack(get_result1);
        std::thread thrd1(std::ref(pack), 10);      // 线程开始执行
        thrd1.join();

        // 貌似下面的部分会宕机 ?
        //ScopedThread sc_thrd1(std::thread(std::ref(pack), 2));

        // future 包含get_result1 返回的结果
        std::future<uint32_t> result = pack.get_future();

        std::cout << "result : " << result.get() << std::endl;

        std::cout << "test_thread end\n"; 
    }

    void test_lbd()
    {
        LINE;
        std::packaged_task<uint32_t(uint32_t)> pack([](uint32_t num)
                {
                    LINE;
                    std::this_thread::sleep_for(5s);

                    return num;
                });

        std::thread thrd1(std::ref(pack), 10);      // 线程开始执行
        thrd1.join();

        // future 包含get_result1 返回的结果
        std::future<uint32_t> result = pack.get_future();

        std::cout << "result : " << result.get() << std::endl;

        std::cout << "test_lbd end\n"; 
    }

    // 直接调用 packaged_task 不会启动新线程 
    void test_packaged_tast()
    {
        LINE;
        std::packaged_task<uint32_t(uint32_t)> pack([](uint32_t num)
                {
                    LINE;
                    std::this_thread::sleep_for(5s);

                    return num;
                });

        pack(100);
        std::future<uint32_t> result = pack.get_future();
        std::cout << "result : " << result.get() << std::endl;
        std::cout << "test_packaged_tast end\n"; 
    }

    void test_test_packaged_task_group()
    {
        std::packaged_task<uint32_t(uint32_t)> pack1([](uint32_t num)
                {
                    LINE;
                    std::this_thread::sleep_for(5s);

                    return num;
                });

        std::packaged_task<uint32_t(uint32_t)> pack2(get_result1);
        std::packaged_task<uint32_t(uint32_t)> pack3(get_result1);

        std::vector<std::packaged_task<uint32_t(uint32_t)>> packaged_vec;
        packaged_vec.emplace_back(std::move(pack1));   // 进入容器之后，pack1 为空不能使用了
        packaged_vec.emplace_back(std::move(pack2));
        packaged_vec.emplace_back(std::move(pack3));

        std::packaged_task<uint32_t(uint32_t)> pack4 = std::move(*packaged_vec.begin());
        packaged_vec.erase(packaged_vec.begin());

        pack4(10);
    }

    void main()
    {
        //test_thread();
        //test_lbd();
        //test_packaged_tast();
        test_test_packaged_task_group();
    }
}

namespace ns_promise
{
    // std::promise  
    // 类模板，能够在某个线程给他赋值，然后在其他线程中，把这个值取出来使用
    //

    void get_result(std::promise<uint32_t>& tmp, uint32_t num)
    {
        LINE;
        std::this_thread::sleep_for(5s);
        int result = num * 2;
        tmp.set_value(result);          // 结果存放在promise 对象中
    }

    void test_result()
    {
        std::promise<uint32_t> promise_obj;
        ScopedThread sp(std::thread(get_result, std::ref(promise_obj), 20));

        std::future<uint32_t> fu1 = promise_obj.get_future();   // future 和 promise 绑定，用于获取线程返回值
        uint32_t res = fu1.get();       // get 只能调用一次, get 调用的是移动构造函数，
                                            // 将结果移动给res 了，导致get里面没有东西了

        std::cout << "test_result : " << res << std::endl;
    }


    void main()
    {
        test_result();
    }
}

namespace ns_future
{
    // std::future_status
    uint32_t get_result(uint32_t num)
    {
        LINE;
        //std::this_thread::sleep_for(10s);
        std::chrono::seconds dura(num);
        std::this_thread::sleep_for(dura);
        int result = num * 2;

        std::cout << "get_result end\n";
        return result;
    }

    void test_result()
    {
        //std::future<uint32_t> f1 = std::async(get_result, 10);
        std::future<uint32_t> f1 = std::async(std::launch::deferred, get_result, 10);
        std::cout << "test_result continue ....\n";

        // wait_for 等待一段时间 
        std::future_status status = f1.wait_for(std::chrono::seconds(11));
        if (status == std::future_status::timeout)
        {
            // 超时表示 线程未执行完
            std::cout << "timeout，线程尚未执行完毕\n";
        }
        else if (status == std::future_status::ready)
        {
            // 超时表示 线程未执行完
            std::cout << "ready, 线程执行完毕，成功返回\n";
        }
        else if (status == std::future_status::deferred)
        {
            // async 第一个参数设置为 std::launch::deferred 时会执行到此
            // 超时表示 线程未执行完
            std::cout << "deferred, 延时执行\n";
        }

    }
    void main()
    {
        test_result();
    }
}

namespace ns_shared_future
{
    // std::shared_future 也是一个类模板
    // 但是它的get是复制数据，而不是移动
    //
    uint32_t get_result(uint32_t num)
    {
        LINE;
        //std::this_thread::sleep_for(10s);
        std::chrono::seconds dura(num);
        std::this_thread::sleep_for(dura);
        int result = num * 2;

        std::cout << "get_result end\n";
        return result;
    }

    void test_result()
    {
        std::future<uint32_t> fu1 = std::async(get_result, 10);
        std::cout << "test_result continue ....\n";

        //uint32_t res = fu1.get();       // get 只能调用一次, get 调用的是移动构造函数，
                                            // 将结果移动给res 了，导致get里面没有东西了
        //res = fu1.get();
        std::cout << "before : fu1.valid(): " << fu1.valid() << std::endl;
        // 使用shared_future 保存 fu1, 这样fu1 就不能在使用了，而shared_future可重复使用
        //std::shared_future<uint32_t> s_f1(std::move(fu1));
        // 使用std::move(fu1) 和 fu1.share() 一样的效果
        std::shared_future<uint32_t> s_f1(fu1.share());
        std::cout << "after: fu1.valid(): " << fu1.valid() << std::endl;

        uint32_t res = s_f1.get();
        res = s_f1.get();

        std::cout << "test_result : " << res << std::endl;
    }

    void test_result1()
    {
        LINE;
        std::packaged_task<uint32_t(uint32_t)> pack([](uint32_t num)
                {
                    LINE;
                    std::this_thread::sleep_for(5s);
                    return num;
                });

        ScopedThread thrd(std::thread(std::ref(pack), 19));

        // future 包含get_result1 返回的结果
        std::shared_future<uint32_t> s_f1(pack.get_future());
        uint32_t res = s_f1.get();
        res = s_f1.get();
        std::cout << "test_result1 : " << res << std::endl;
    }

    void main()
    {
        //test_result();
        test_result1();
    }
};

int main()
{
    //ns_package::main();
    //ns_promise::main();
    //ns_future::main();
    ns_shared_future::main();
    LINE;
    return 0;
}

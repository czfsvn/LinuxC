#include <iostream>
#include <future>

using namespace std;

//#define LINE    cout << "\n=========\t" << __FUNCTION__ << "\t============\n";
#define LINE    cout << "\n=====\t" << __FUNCTION__ << ": thread: " << std::this_thread::get_id() << "\t=======\n";


namespace test_function
{
    uint32_t get_result()
    {
        LINE;
        std::this_thread::sleep_for(10s);
        std::cout << "get_result end\n";
        return 10;
    }

    uint32_t get_result2(uint32_t num)
    {
        LINE;
        return num;
    }

    // only with function
    void test_1()
    {
        LINE;
        std::future<uint32_t> fu = std::async(get_result);
        std::this_thread::sleep_for(1s);
        // get 成员函数等待线程执行结束并返回结果, 且一个future 的get成员函数只能调用一次
        std::cout << "fu: " << fu.get() << std::endl;

        std::future<uint32_t> fu2 = std::async(get_result2, 20);
        std::this_thread::sleep_for(1s);
        std::cout << "fu2: " << fu2.get() << std::endl;

        std::future<uint32_t> fu3 = std::async(get_result2, 20);
        std::this_thread::sleep_for(1s);
        // wait 只会等待线程执行完毕，本身并不返回结果
        fu3.wait();
    }

    void test_2()
    {
        LINE;
        std::future<uint32_t> fu = std::async(get_result);
        std::this_thread::sleep_for(1s);
        // 不适用get 和 wait的情况，程序依然会正常执行，并结束
        
        std::cout << "test2 end\n";
    }

    void main()
    {
        test_1();
        test_2();
    }
}

namespace test_obj
{
    struct AClass
    {
        void print1(uint32_t val, const std::string& str)
        {
            LINE;
        }
        std::string ret_str(const std::string& str)
        {
            LINE;
            return str;
        }
    };

    struct FuncObj
    {
        FuncObj()
        {
            LINE;
        }
        uint32_t operator()(uint32_t val)
        {
            LINE;
            return val;
        }
    };

    void test_obj()
    {
        // f2.get() 调用了之后，就会比 f1 更早执行
        AClass a;
        auto f1 = std::async(&AClass::print1, &a, 10, "World");
        auto f2 = std::async(&AClass::ret_str, &a, "Hello");
        std::this_thread::sleep_for(1s);
        //std::cout << "f1 : " << f1.get() << std::endl;
        std::cout << "f2 : " << f2.get() << std::endl;
    }

    // std::async : 创建一个异步任务，返回类型为std::futrue
        // future 会保存线程函数调用的结果 
    void test_async_launch()
    {
        // std::launch::async : 会启动一个新的线程来执行对应的函数
            // 且马上开始执行，并非等待get或者wait才开始执行 
        AClass a;
        auto f2 = std::async(std::launch::async, &AClass::ret_str, &a, "Hello");
        std::this_thread::sleep_for(1s);
        std::cout << "f2 : " << f2.get() << std::endl;
    }

    void test_async_deferred()
    {
        // std::launch::defferred: 不会启动新线程，
            // 而且是在f2.get()或f1.wait()的时候才会在原线程中延时调用 
            // 如果没有使用 get 或者 wait 的时候， 线程不会创建，且不会执行
        AClass a;
        auto f2 = std::async(std::launch::deferred, &AClass::ret_str, &a, "Hello");
        std::this_thread::sleep_for(1s);
        std::cout << "f2 : " << f2.get() << std::endl;
    }

    void test_async_launch_deferred()
    {
        // std::launch::defferred | std::launch::async:
            // 与默认的策略相同，有系统根据系统的资源情况来决定使用同步或异步的方式 
        AClass a;
        auto f2 = std::async(std::launch::async | std::launch::deferred, 
                &AClass::ret_str, &a, "Hello");
        std::this_thread::sleep_for(1s);
        std::cout << "f2 : " << f2.get() << std::endl;
    }
    
    void test_func_obj_async()
    {
        // 此时std::launch::async: 先调用FuncObj 构造函数， 在get的时候调用 operator 函数
        auto f1 = std::async(std::launch::async, FuncObj(), 1);
        LINE;
        f1.get();
    }

    void test_func_obj_deferred()
    {
        // 此时std::launch::deferred: 先调用FuncObj 构造函数， 在get的时候调用 operator 函数
        auto f1 = std::async(std::launch::deferred, FuncObj(), 1);
        LINE;
        f1.get();
    }

    void main()
    {
        //test_obj();
        //test_async_launch();
        //test_async_deferred();
        //test_async_launch_deferred();

        test_func_obj_async();
        test_func_obj_deferred();
    }
}


int main()
{
    test_function::main();
    //test_obj::main();
    LINE;
    return 0;
}

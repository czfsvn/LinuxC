#include <iostream>
#include <thread>
#include <vector>
#include <algorithm>

using namespace std;

#define LINE    cout << "\n=========\t" << __FUNCTION__ << "\t ============\n";

namespace ns_create
{
    void do_work1()         
    {
        std::cout << "do_work1 thread: " << std::this_thread::get_id() << std::endl;
    }

    class WorkTask
    {
        public:
            void operator()() const
            {
                std::cout << "WorkTask thread: " << std::this_thread::get_id() << ", address: " << this << std::endl;
            }
    };

    class WorkTask2
    {
        public:
            void run1()
            {
                std::cout << "WorkTask2 run1: " << std::this_thread::get_id() << ", address: " << this << std::endl;
            }

            void run2(uint32_t num)
            {
                std::cout << "WorkTask2 run2: " << std::this_thread::get_id() << ", address: " << this << ", num: " << num << std::endl;
            }
    };
    

    void sleep()
    {
        std::chrono::milliseconds ms(3000);
        std::this_thread::sleep_for(1s);
        std::vector<uint32_t> uVec;
        for (uint32_t idx = 0; idx < 1000000; ++idx)
            uVec.emplace_back(idx);
    }

    void test_function()
    {
        LINE;
        std::thread thrd1(do_work1);    //通过函数的方式创建线程,并以do_work1为入口开始执行
        thrd1.join();
        sleep();
    }
    
    void test_obj1()
    {
        LINE;
        WorkTask task;
        std::cout << "task: " << &task << std::endl;
        std::thread thrd2(task);    // 在 thrd2 中 task 会copy一份到thread中

        sleep();

        std::thread thrd3(std::move(task));

        sleep();
        
        std::thread thrd4{WorkTask()};

        //std::thread thrd5(WorkTask());        // 语法解析歧义，编译不通过
        std::thread thrd5((WorkTask()));

        thrd2.join();
        thrd3.join();
        thrd4.join();
        thrd5.join();
    }

    void test_lbd()
    {
        LINE;
        std::thread thrd4([]()
                {
                    std::cout << "lambda thread: " << std::this_thread::get_id() << std::endl;
                });
        thrd4.join();
    }

    void test_obj2()        //多线程调用成员函数
    {
        LINE;
        WorkTask2 task;
        std::cout << "task addr : " << &task << std::endl;
        std::thread thrd1(&WorkTask2::run1, task);

        std::thread thrd2(&WorkTask2::run1, &task);

        std::thread thrd3(&WorkTask2::run1, std::ref(task));
        // todo ...
        // 经过测试，发现thrd1，thrd2 传递进去的task 与原task 地址一样，
        // std::ref(task) 却另外一个地址 ？ 需要查阅资料确认
        //
        // 传递参数
        std::thread thrd4(&WorkTask2::run2, std::ref(task), 10);

        thrd1.join();
        thrd2.join();
        thrd3.join();
        thrd4.join();
    }
    void main()
    {
        test_function();
        test_obj1();
        test_lbd();
        test_obj2();
    }
}

namespace ns_pass_para
{
    class TestObj
    {
        public:
            TestObj()   {   std::cout << "constructor\n";            }
            ~TestObj()  {   std::cout << "destructor\n";           }
            TestObj(const TestObj& o)   {   std::cout << "copy constructor\n";  }
            TestObj(TestObj&& o)        {   std::cout << "move constructor\n";  } 
    };

    void update_obj1(TestObj obj)   {   }
    void update_obj2(TestObj& obj)   {   }
    void update_obj3(const TestObj& obj)   {   }
    void update_obj4(const TestObj& obj, uint32_t val)   {   }
    void update_obj5(std::unique_ptr<TestObj> ptr)  {}

    void test_update_obj1()
    {
        LINE;
        TestObj obj;

        std::thread thrd1(update_obj1, obj);    
        thrd1.join();
    }
    void test_update_obj2()
    {
        LINE;
        TestObj obj;

        //std::thread thrd1(update_obj2, obj);  // 不加引用编译失败    
        std::thread thrd1(update_obj2, std::ref(obj));    
        thrd1.join();
    }
    void test_update_obj3()
    {
        LINE;
        TestObj obj;

        std::thread thrd1(update_obj3, std::ref(obj)); 
        thrd1.join();
    }
    void test_update_obj4()
    {
        LINE;
        TestObj obj;

        std::thread thrd1(update_obj4, std::ref(obj), 10);    
        thrd1.join();
    }

    void test_update_obj5()
    {
        LINE;
        std::unique_ptr<TestObj> ptr = std::make_unique<TestObj>();
        std::thread thrd(update_obj5, std::move(ptr));
        thrd.join();
    }

    void test_update_obj()
    {
        test_update_obj1();
        test_update_obj2();
        test_update_obj3();
        test_update_obj4();
        test_update_obj5();
    }
    void main()
    {
        test_update_obj();
    }
};

namespace ns_multi
{
    //保证线程在退出之前线程已经完成  
    class ThreadGuard
    {
        public:
            explicit ThreadGuard(std::thread& th) : thrd(th){}
            ~ThreadGuard()
            {
                if (thrd.joinable())
                {
                    thrd.join();
                }
            }

        private:
            std::thread& thrd;
    };

    //保证线程在退出之前线程已经完成  
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

    void work() 
    {
        LINE;
        std::cout << std::this_thread::get_id() << std::endl;
    };

    void test()
    {
        LINE;
        std::thread thrd1(work);
        ThreadGuard guard(thrd1);                   //配合使用

        ScopedThread scope(std::thread(work));      // 这样用更简洁
    }

    void test_thread_group()
    {
        LINE;
        std::vector<std::thread> threads;
        for (uint32_t idx = 0; idx < 20; idx ++)
        {
            threads.emplace_back((std::thread(work)));
        }
        std::for_each(threads.begin(), threads.end(), [](std::thread& th){    th.join();  });
    }

    void test_thread_assign()
    {
        LINE;
        std::thread thrd1(work);
        //std::thread thrd2 = thrd1;          // 不能像这样直接赋值
        std::thread thrd3 = std::move(thrd1);
        thrd3.join();
    }

    void main()
    {
        test();
        test_thread_group();
        test_thread_assign();
    }
}

int main()
{
    //ns_create::main();
    ns_pass_para::main();
    //ns_multi::main();
    LINE;
    std::cout << "main thread: " << std::this_thread::get_id() << std::endl;
    std::cout << "湖北麻城\n";
    return 0;
}

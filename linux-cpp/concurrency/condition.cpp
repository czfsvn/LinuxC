#include <iostream>
#include <thread>
#include <mutex>
#include <list>
#include <queue>
#include <condition_variable>

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
        {   
            thrd.join();  
        }
        ScopedThread(const ScopedThread&)   = delete;
        ScopedThread& operator= (const ScopedThread&) = delete;
    private:
        std::thread thrd;
};

namespace ns_condition
{
    class ConditionTest
    {
        public:
            void push_data()
            {
                for (uint32_t idx = 0; idx < 1000000; idx ++)
                {
                    std::this_thread::sleep_for(1ms);
                    std::lock_guard<std::mutex> lk(mtx);
                    data_list.emplace_back(idx);
                    cond.notify_one();
                }
            }

            void pop_data()
            {
                while (1)
                {
                    // 此处使用std::unique_lock--等待中的线程在等待期间必须解锁互斥元
                        //并在这之后重新将其锁定，而std::lock_guard 无此灵活性

                    // wait: 实现检查条件(lambda函数),在满足时返回(lambda返回true)；
                        // 如果条件满足-lambda返回true， wait 直接返回，程序继续执行
                        // 如果条件不满足-lambda返回false，这时，wait解锁互斥元
                            // 并将该线程至于阻塞或者等待状态,直到其他线程调用notify_one为止
                        // 如果没有第二个参数-lambda表达式，则效果与第二个参数返回false效果一样
                    // 当来自数据准备线程中对 notify_one()的调用通知条件变量时，
                        //线程从睡眠状态中苏醒-解除其锁定，重新获得互斥元上的锁，
                        //并再次检查条件，
                            //如果条件已经满足，就从wait中返回值，互斥元仍被锁定
                            //如果条件不满足，该线程解锁互斥元，并恢复等待
                    std::unique_lock<std::mutex> lk(mtx);
                    cond.wait(lk, [this]{   return !data_list.empty();  });
                    uint32_t data = data_list.front();
                    data_list.pop_front();
                    lk.unlock();

                    std::cout << "data : " << data << std::endl;
                }
            }
        private:
            std::list<uint32_t> data_list;
            std::mutex mtx;
            std::condition_variable cond;
    };
}

namespace thread_safe_queue
{
    // from: <<C++并发编程>>
    template <typename T>
        class threadsafe_queue
        {
            public:
                threadsafe_queue()  {}
                threadsafe_queue(const threadsafe_queue& other)
                {
                    std::lock_guard<std::mutex> lk(other.mtx);
                    data_queue = other.data_queue;
                }

                void push(T new_value)
                {
                    std::lock_guard<std::mutex> lk(mtx);
                    data_queue.push(new_value);
                    data_cond.notify_one();
                }

                void wait_and_pop(T& value)
                {
                    std::unique_lock<std::mutex> lk(mtx);
                    data_cond.wait(lk, [this]{  !data_queue.empty();    });
                    value = data_queue.front();
                    data_queue.pop();
                }

                std::shared_ptr<T> wait_and_pop()
                {
                    std::unique_lock<std::mutex> lk(mtx);
                    data_cond.wait(lk, [this]{  !data_queue.empty();    });
                    std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
                    data_queue.pop();
                    return res;
                }

                bool try_pop(T& value)
                {
                    std::lock_guard<std::mutex> lk(mtx);
                    if (data_queue.empty())
                        return false;

                    value = data_queue.front();
                    data_queue.pop();
                    return true;
                }

                std::shared_ptr<T> try_pop()
                {
                    std::lock_guard<std::mutex> lk(mtx);
                    if (data_queue.empty())
                        return std::shared_ptr<T>();
                    std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
                    data_queue.pop();
                    return res;
                }

                bool empty() const
                {
                    std::lock_guard<std::mutex> lk(mtx);
                    return data_queue.empty();
                }
            private:
                mutable std::mutex  mtx;
                std::queue<T>       data_queue;
                std::condition_variable     data_cond;
        };
}

namespace ns_sync_queue
{
    // from : <<深入应用C++11: 代码优化与工程级应用>>   5.5
    template<typename T>
        class SyncQueue
        {
            public:
                SyncQueue(int size) : max_size(size)    {}
                
                void push(const T& data)
                {
                    std::lock_guard<std::mutex> lk(mtx);
                    while (isFull())
                    {
                        std::cout << "缓冲区满了，需要等待...\n";
                        not_full.wait(mtx); 
                    }
                    data_queue.emplace_back(data);
                    not_empty.notify_one();
                }

                T pop()
                {
                    std::lock_guard<std::mutex> lk(mtx);
                    while (isEmpty())
                    {
                        std::cout << "缓冲区满了，需要等待...\n";
                        not_empty.wait(mtx);
                    }
                    T data = data_queue.front();
                    data_queue.pop_front();
                    not_full.notify_one();
                    return data;
                }

                bool Empty()
                {
                    std::lock_guard<std::mutex> lk(mtx);
                    return data_queue.empty();
                }

                size_t Size()
                {
                    std::lock_guard<std::mutex> lk(mtx);
                    return data_queue.size();
                }

                uint32_t Count()
                {
                    return data_queue.size();
                }

            private:
                bool isFull() const
                {
                    return data_queue.size() == max_size;
                }
                bool isEmpty() const
                {
                    return data_queue.empty(); 
                }

            private:
                std::list<T>    data_queue;
                std::mutex      mtx;
                std::condition_variable_any     not_empty;
                std::condition_variable_any     not_full;
                uint32_t        max_size    =  0;
        };
};

namespace ns_sync_queue_new
{
    // from : <<深入应用C++11: 代码优化与工程级应用>>   5.6
    template <class T>
        class SyncQueue
        {
            public:
                SyncQueue() {   }

                void push(const T& data)
                {
                    std::lock_guard<std::mutex> lk(mtx);
                    data_queue.emplace_back(data);
                    condition.notify_one();
                }

                T pop()
                {
                    std::lock_guard<std::mutex> lk(mtx);
                    condition.wait(lk, [this]{  return !data_queue.empty();  });
                    T data = data_queue.front();
                    data_queue.pop_front();
                    return data;
                }

                bool Empty()
                {
                    std::lock_guard<std::mutex> lk(mtx);
                    return data_queue.empty();
                }

                size_t Size()
                {
                    std::lock_guard<std::mutex> lk(mtx);
                    return data_queue.size();
                }

            private:
                std::list<T>    data_queue;
                std::mutex      mtx;
                std::condition_variable condition;
        };
}

int main()
{
    LINE;
    return 0;
}

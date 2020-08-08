#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <functional>
#include <algorithm>
#include <numeric>
#include <list>
#include <type_traits>

using namespace std;

#define LOOP_COUNT      10000000

class TimeCost
{
	public:
		TimeCost(const std::string& func): func_name(func) {}
		~TimeCost()
		{
			std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
			auto dura = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			std::cout << func_name.c_str() << " cost: " << dura << " ms\n";
		}
	private:
		std::chrono::system_clock::time_point start = chrono::system_clock::now();
		std::string func_name;
};

namespace ns_queue
{
    // from: <<C++并发编程实战>> chapter 6  list：6.9
    template<typename T>
        class threadsafe_queue
        {
            private:
                struct Node
                {
                    std::shared_ptr<T>      data;
                    std::unique_ptr<Node>   next;
                };

                std::mutex      head_mutex;
                std::mutex      tail_mutex;
                std::unique_ptr<Node>   head;
                Node*                   tail;
                std::condition_variable data_cond;

            private:
                Node* get_tail()
                {
                    std::lock_guard<std::mutex>  tail_lk(tail_mutex);
                    return tail;
                }

                std::unique_ptr<Node> pop_head()
                {
                    std::unique_ptr<Node> old_head = std::move(head);
                    head = std::move(old_head->next);
                    return old_head;
                }

                std::unique_lock<std::mutex> wait_for_data()
                {
                    std::unique_lock<std::mutex> head_lock(head_mutex);
                    data_cond.wait(head_lock, [&]{  head.get() != get_tail();   });
                    return std::move(head_lock);
                }

                std::unique_ptr<Node> wait_pop_head()
                {
                    std::unique_ptr<std::mutex> head_lock(wait_for_data());
                    return pop_head();
                }

                std::unique_ptr<Node> wait_pop_head(T& value)
                {
                    std::unique_ptr<std::mutex> head_lock(wait_for_data());
                    value = std::move(*head->data);
                    return pop_head();
                }

            private:
                std::unique_ptr<Node> try_pop_head()
                {
                    std::lock_guard<std::mutex> head_lock(head_mutex);
                    if (head.get() == get_tail())
                    {
                        return std::unique_ptr<Node>();
                    }
                    return pop_head();
                }

                std::unique_ptr<Node> try_pop_head(T& value)
                {
                    std::lock_guard<std::mutex> head_lock(head_mutex);
                    if (head.get() == get_tail())
                    {
                        return std::unique_ptr<Node>();
                    }
                    value = std::move(*head->data);
                    return pop_head();
                }

            public:
                threadsafe_queue() : head(new Node), tail(head.get())
                {
                }

                threadsafe_queue(const threadsafe_queue& other) = delete;
                threadsafe_queue& operator =(const threadsafe_queue& other) = delete;

                void push(T new_val)
                {
                    std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_val)));
                    std::unique_ptr<Node> p(new Node);
                    {
                        std::lock_guard<std::mutex> tail_lock(tail_mutex);
                        tail->data = new_data;
                        const Node* new_tail = p.get();
                        tail->next = std::move(p);
                        tail = new_tail;
                    }
                    data_cond.notify_one();
                }

                std::shared_ptr<T> wait_and_pop()
                {
                    std::unique_ptr<Node> const old_head = wait_pop_head();
                    return old_head->data;
                }

                void wait_and_pop(T& value)
                {
                    std::unique_ptr<Node> const old_head = wait_pop_head(value);
                }

                std::shared_ptr<T> try_pop()
                {
                    std::unique_ptr<Node> old_head = try_pop_head();
                    return old_head ? old_head->data : std::shared_ptr<T>();
                }

                bool try_pop(T& value)
                {
                    std::unique_ptr<Node> const old_head = try_pop_head(value);
                    return old_head ? true : false;
                }

                void empty()
                {
                    std::lock_guard<std::mutex> head_lock(head_mutex);
                    return (head.get() == get_tail());
                }
        };

    class ThreadJoin
    {
    public:
        ThreadJoin(std::vector<std::thread>& thrds) :threads(thrds)	{}
        ~ThreadJoin()
        {
            for (uint32_t index = 0; index < threads.size(); ++index)
            {
                threads[index].join();
            }
        }
    private:
        std::vector<std::thread>& threads;
    };
}

namespace ns_pool
{
    

    class thread_pool
    {
    private:
        std::atomic_bool done;
        ns_queue::threadsafe_queue<std::function<void()>> work_queue;
        std::vector<std::thread> threads;
        ns_queue::ThreadJoin joinner;

        void worker_thread()
        {
            while (!done)
            {
                std::function<void()> task;
                if (work_queue.try_pop(task))
                {
                    task();
                }
                else
                {
                    std::this_thread::yield();
                }
            }
        }
    public:
        thread_pool() : done(false), joinner(threads)
        {
            unsigned const thread_count = std::thread::hardware_concurrency();
            try
            {
                for (uint32_t idx = 0; idx < thread_count; ++idx)
                {
                    threads.push_back(std::thread(&thread_pool::worker_thread, this));
                }                    
            }
            catch (...)
            {
                done = true;
                throw;
            }
        }

        ~thread_pool()
        {
            done = true;
        }

        template<typename FUNC>
        void submit(FUNC f)
        {
            work_queue.push(std::function<void()>(f));
        }
    };
}

namespace ns_pool2
{
    class function_wrapper
    {
        struct impl_base
        {
            virtual void call() = 0;
            virtual ~impl_base() {}
        };

        template<typename FUNC>
        struct impl_type :impl_base
        {
            FUNC f;
            impl_type(FUNC&& f_) : f(std::move(f_)) {}
            void call() override    {       f();       }
        };

    private:
        std::unique_ptr<impl_base>  impl;

    public:
        template<typename FUNC>
        function_wrapper(FUNC&& f_) : impl(new impl_type<FUNC>(std::move(f_)))
        {   }        

        function_wrapper(function_wrapper&& other) : impl(std::move(other.impl))
        {   }

        function_wrapper& operator = (function_wrapper&& other)
        {
            impl = std::move(other.impl);
            return *this;
        }

        void operator()()   {   impl->call();    }

        function_wrapper()  =  default;
        function_wrapper(const function_wrapper&)   =   delete;
        function_wrapper(function_wrapper&)   =   delete;
        function_wrapper& operator = (const function_wrapper& other) = delete;
    };

    class thread_pool
    {
        std::atomic_bool done;
        ns_queue::threadsafe_queue<function_wrapper>  work_queue;
        std::vector<std::thread> threads;
        ns_queue::ThreadJoin joinner;
        
        void worker_thread()
        {
            while (!done)
            {
                function_wrapper task;
                if (work_queue.try_pop(task))
                {
                    task();
                }
                else
                {
                    std::this_thread::yield();
                }
            }
        }
    
    public:
        thread_pool() : done(false), joinner(threads)
        {
            unsigned const thread_count = std::thread::hardware_concurrency();
            try
            {
                for (uint32_t idx = 0; idx < thread_count; ++idx)
                {
                    threads.push_back(std::thread(&thread_pool::worker_thread, this));
                }                    
            }
            catch (...)
            {
                done = true;
                throw;
            }
        }

        ~thread_pool()
        {
            done = true;
        }

        template<typename FUNC>
        std::future<typename std::result_of<FUNC()>::type> submit(FUNC f)
        {
            typedef typename std::result_of<FUNC()>::type result_type;

            std::packaged_task<result_type()> task(std::move(f));
            std::future<result_type> res(task.get_future());
            work_queue.push(std::move(task));
            return res;
        }
    };


    template<typename Iterator, typename T>
    struct accumulate_block
    {
        T operator()(Iterator first, Iterator last)
        {
            //每个子序列累加，不能通过线程的返回值返回累加结果，而是通过一个result引用将结果返回给主线程
            return std::accumulate(first, last, 0);
        }
    };

    template <typename Iterator, typename T>
    T parallel_accumulate(Iterator first, Iterator last, T init)
    {
        uint64_t const length = std::distance(first, last);
        if (!length)
            return init;

        uint64_t const block_size = 25;
        uint64_t const num_blocks = (length + block_size - 1) / block_size;

        std::vector<std::future<T>> futures(num_blocks - 1);
        ns_pool2::thread_pool pool;

        Iterator block_start = first;
        for (uint64_t i = 0; i < (num_blocks - 1); ++i)
        {
            Iterator block_end = block_start;
            std::advance(block_end, block_size);
            futures[i] = pool.submit(accumulate_block<Iterator, T>()(block_start, block_end));
            block_start = block_end;
        }

        T last_result = accumulate_block<Iterator, T>()(block_start, last);
        T result = init;
        for (uint64_t i = 0; i < (num_blocks - 1); ++i)
        {
            result += futures[i].get();
        }
        result += last_result;
        return result;
    }

    void test1()
    {
        std::vector<uint64_t> uList;
        for (uint64_t idx = 0; idx < LOOP_COUNT; idx++)
        {
            uList.emplace_back(idx);
        }
        {
            TimeCost cost("for loop");
            uint64_t sum = 0;
            for (const auto& ele : uList)
            {
                sum += ele;
            }
            std::cout << "sum: " << sum << std::endl;
        }
        {   // 经过测试，for_each 比 for 耗时稍多
            TimeCost cost("for loop");
            uint64_t sum = 0;
            std::for_each(uList.begin(), uList.end(), [&sum](uint64_t& val){    sum += val;   });
            
            std::cout << "sum: " << sum << std::endl;
        }
        {
            TimeCost cost("paralel");
            uint64_t sum = 0;
            sum = parallel_accumulate(uList.begin(), uList.end(), sum);

            std::cout << "sum: " << sum << std::endl;
        }
    }
    void main()
    {
        test1();
    }
}

int main()
{
    ns_pool2::main();
    std::cout <<"Helo, Chapter9\n";
    return 0;
}

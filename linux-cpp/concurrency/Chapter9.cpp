#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <algorithm>

using namespace std;

namespace ns_pool
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
            ThreadJoin(std::vector<std::thread> thrds) :threads(std::move(thrds))	{}
            ~ThreadJoin()
            {
                for (uint32_t index = 0; index < threads.size(); ++index)
                {
                    threads[index].join();
                }
            }
        private:
            std::vector<std::thread> threads;
        };

        class thread_pool
        {
        private:
            std::atomic_bool done;
            threadsafe_queue<std::function<void()>> work_queue;
            std::vector<std::thread> threads;
            ThreadJoin joinner;

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
            thread_pool() : done(false), joinner(std::move(threads))
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

int main()
{
    std::cout <<"Helo, Chapter9\n";
    return 0;
}
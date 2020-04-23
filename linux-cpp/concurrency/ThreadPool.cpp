#include <iostream>
//#include <boost/thread.hpp>
#include <list>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

using namespace std;

namespace ns_pool
{
    // from: <<深入应用C++11代码优化与工程应用>> chapter 9
    template <typename T>
        class SyncQueue
        {
            public:
                SyncQueue(uint32_t maxsize) : max_size_(maxsize)
                {
                }
                ~SyncQueue()
                {
                    data_list_.clear();
                }

                void stop()
                {
                    {
                        std::lock_guard<std::mutex> lk(mutex_);
                        need_stop_ = true;
                    }
                    not_full_condition_.notify_all();
                    not_empty_condition_.notify_all();
                }

                bool full()
                {
                    std::lock_guard<std::mutex> lk(mutex_);
                    return data_list_.size() >= max_size_;
                }

                bool empty()
                {
                    std::lock_guard<std::mutex> lk(mutex_);
                    return data_list_.empty();
                }

                size_t size()
                {
                    std::lock_guard<std::mutex> lk(mutex_);
                    return data_list_.size();
                }

                uint32_t count()
                {
                    return data_list_.size();
                }

                void put(T&& data)
                {
                    add(std::forward<T>(data));
                }

                void put(const T& data)
                {
                    add(data);
                }

                void take(std::list<T>& tlist)
                {
                    std::unique_lock<std::mutex> lk(mutex_);
                    not_empty_condition_.wait(lk, [this]{    return need_stop_ || notEmpty(); });
                    if (need_stop_)
                        return;

                    tlist = std::move(data_list_);
                    not_full_condition_.notify_one();
                }

                void take(T& data)
                {
                    std::unique_lock<std::mutex> lk(mutex_);
                    not_empty_condition_.wait(lk, [this]{    return need_stop_ || notEmpty(); });
                    if (need_stop_)
                        return;

                    data = std::move(data_list_.front());
                    data_list_.pop_front();
                    not_full_condition_.notify_one();
                }
            private:
                bool notEmpty() const
                {
                    bool empty = data_list_.empty();
                    if (empty)
                        std::cout << "queue is empty : " << std::this_thread::get_id() << std::endl;

                    return !empty;
                }

                bool notFull() const
                {
                    bool full = data_list_.size() >= max_size_;
                    if (full)
                        std::cout << "queue is full : " << std::this_thread::get_id() << std::endl;

                    return !full;
                }

                template <typename F>
                    void add(F&& data)
                    {
                        std::unique_lock<std::mutex> lk(mutex_);
                        not_full_condition_.wait(lk, [this]{ return need_stop_ || notFull(); });

                        if (need_stop_)
                            return;

                        data_list_.emplace_back(std::forward<F>(data));
                        not_empty_condition_.notify_one();
                    }

            private:
                uint32_t        max_size_ = 0;
                bool            need_stop_ = false;
                std::list<T>    data_list_;
                std::mutex      mutex_;
                std::condition_variable not_full_condition_;
                std::condition_variable not_empty_condition_;
        };

    const uint32_t max_task_count = 100;
    class ThreadPool
    {
        public:
            using Task = std::function<void()>;

            ThreadPool(uint32_t num_threads = std::thread::hardware_concurrency()) : tasks_(max_task_count)
            {
                start(num_threads);
            }

            ~ThreadPool()
            {
                stop();
            }

            void stop()
            {
                std::call_once(once_flag_, [this]   {   stopThreadGroup();  });
            }

            void addTask(Task&& task)
            {
                tasks_.put(std::forward<Task>(task));
            }

            void addTask(const Task& task)
            {
                tasks_.put(task);
            }

        private:
            void start(uint32_t num_threads)
            {
                is_running_ = true;
                for (uint32_t idx = 0; idx < num_threads; idx++)
                    threads_.emplace_back(std::make_shared<std::thread>(&ThreadPool::run, this));
            }
            void run()
            {
                while (is_running_)
                {
//#define ALL_TAKE
#ifdef ALL_TAKE
                    std::list<Task> task_list;
                    tasks_.take(task_list);
                    for (auto& task : task_list)
                    {
                        if (!is_running_)
                            return;

                        task();
                    }
#else
                    Task task;
                    tasks_.take(task);
                    if (!is_running_)
                        return;

                    if (task)
                        task();
#endif
                }
            }

            void stopThreadGroup()
            {
                tasks_.stop();
                is_running_ = false;
                for (auto& thrd : threads_)
                {
                    if (thrd)
                        thrd->join();
                }
                threads_.clear();
            }
        private:
            std::list<std::shared_ptr<std::thread>> threads_;
            SyncQueue<Task>     tasks_;
            std::once_flag      once_flag_;
            std::atomic<bool>   is_running_ = false;
            uint32_t            queue_size_ = 0;
    };

    void main()
    {
        ThreadPool pool(4);

        std::thread thrd1([&pool]
                {
                    for (uint32_t i = 0; i < 10; i++)
                    {
                        auto thrd_id = std::this_thread::get_id();
                        pool.addTask([thrd_id]
                                {
                                std::cout << " 1 thread id : " << thrd_id << std::endl;
                                });
                    }
                });

        std::thread thrd2([&pool]
                {
                    for (uint32_t i = 0; i < 10; i++)
                    {
                        auto thrd_id = std::this_thread::get_id();
                        pool.addTask([thrd_id]
                                {
                                std::cout << " 2 thread id : " << thrd_id << std::endl;
                                });
                    }
                });
        std::thread thrd3([&pool]
                {
                    for (uint32_t i = 0; i < 10; i++)
                    {
                        auto thrd_id = std::this_thread::get_id();
                        pool.addTask([thrd_id]
                                {
                                std::cout << " 3 thread id : " << thrd_id << std::endl;
                                });
                    }
                });

        std::this_thread::sleep_for(2s);
        getchar();
        pool.stop();
        thrd1.join();
        thrd2.join();
        thrd3.join();
    }
}

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
                    return old_head;
                }

                void empty()
                {
                    std::lock_guard<std::mutex> head_lock(head_mutex);
                    return (head.get() == get_tail());
                }
        };
}

namespace ns_lookup_table
{
    // from: <<C++并发编程>> list 6.12
    template<typename Key, typename Value, typename Hash = std::hash<Key>>
        class threadsafe_lookup_table
        {
            private:
                class bucket_type
                {
                    private:
                        typedef std::pair<Key, Value>       bucket_value;
                        typedef std::list<bucket_value>     bucket_data;
                        typedef typename bucket_data::iterator bucket_iterator;

                        bucket_data data;
                        //boost::shared_mutex mutex;

                        bucket_iterator find_entry_for(Key const& key) const
                        {
                            return std::find_if(data.begin(), data.end(),
                                    [&](bucket_value const& item)
                                    {   return item.first == key;   });
                        }

                    public:
                        Value value_for(Key const& key, Value const& default_value) const
                        {
                            //boost::shared_ptr<boost::shared_mutex> lk(mutex);
                            bucket_iterator const iter = find_entry_for(key);
                            return (iter != data.end() ? iter->second : default_value);
                        }

                        void add_or_update_mapping(Key const& key, Value const& value)
                        {
                            //std::unique_lock<boost::shared_mutex> lk(mutex);
                            bucket_iterator const iter = find_entry_for(key);
                            if (iter == data.end())
                            {
                                data.push_back(bucket_value(key, value));
                            }
                            else
                            {
                                iter->second = value;
                            }
                        }

                        void remove_mapping(Key const& key)
                        {
                            //std::unique_lock<boost::shared_mutex> lk(mutex);
                            bucket_iterator iter = find_entry_for(key);
                            if (iter != data.end())
                            {
                                data.erase(iter);
                            }
                        }
                };

            private:
                std::vector<std::unique_ptr<bucket_type>> buckets;
                Hash hasher;

                bucket_type& get_bucket(Key const& key) const
                {
                    std::size_t const bucket_index = hasher(key) % buckets.size();
                    return *buckets[bucket_index];
                }

            public:
                typedef Key     key_type;
                typedef Value   mapped_type;
                typedef Hash    hash_type;

                threadsafe_lookup_table(uint32_t nums = 19, 
                        Hash const& hash = Hash()) : buckets(nums), hasher(hash)
                {
                    for (uint32_t idx = 0; idx < buckets; idx++)
                    {
                        buckets[idx].reset(new bucket_type);
                    }
                }

                threadsafe_lookup_table(threadsafe_lookup_table const& other) = delete;
                threadsafe_lookup_table operator = (threadsafe_lookup_table const& o) = delete;

                Value value_for(Key const& key, Value const& default_value = Value())
                {
                    return get_bucket(key).value_for(key, default_value);
                }

                void add_or_update_mapping(Key const& key, Value const& value)
                {
                    return get_bucket(key).add_or_update_mapping(key, value);
                }

                void remove_mapping(Key const& key)
                {
                    get_bucket(key).remove_mapping(key);
                }


        };
}

namespace ns_threadsafe_list
{
    // from: <<C++并发编程>> list 6.13
    template<typename T>
        class threadsafe_list
        {
            struct Node
            {
                std::mutex              mtx;
                std::shared_ptr<T>      data;
                std::unique_ptr<Node>   next;
                Node() : next()
                {
                }
                Node(T const& val) : data(std::make_shared<T>(val))
                {
                }

            };

            Node head;

            public:
            threadsafe_list()  {}
            ~threadsafe_list()
            {
                remove_if([](Node const&)   {   return true;    }  );
            }

            threadsafe_list(threadsafe_list const& other) = delete;
            threadsafe_list& operator = (threadsafe_list const& other) = delete;

            void push_front(T const& value)
            {
                std::unique_ptr<Node> new_node(new Node(value));
                std::lock_guard<std::mutex> lk(head.mtx);
                new_node->next = std::move(head.next);
                head.next = std::move(new_node);
            }

            template<typename Function>
                void for_each(Function f)
                {
                    Node* cur = &head;
                    std::unique_lock<std::mutex> lk(head.mtx);
                    while (Node* const next = cur->next.get())
                    {
                        std::unique_lock<std::mutex> next_lk(next->mtx);
                        lk.unlock();
                        f(*next->data);
                        cur = next;
                        lk = std::move(next_lk);
                    }
                }

            template <typename Predicate>
                std::shared_ptr<T> find_if(Predicate pre)
                {
                    Node* cur = &head;
                    std::unique_lock<std::mutex> lk(head.mtx);
                    while (Node* const next = cur->next.get())
                    {
                        std::unique_lock<std::mutex> next_lk(next->mtx);
                        lk.unlock();
                        if (pre(*next->data))
                        {
                            return next->data;
                        }
                        cur = next;
                        lk = std::move(next_lk);
                    }

                    return std::shared_ptr<T>();
                }

            template <typename Predicate>
                void remove_if(Predicate pre)
                {
                    Node* cur = &head;
                    std::unique_lock<std::mutex> lk(head.mtx);
                    while (Node* const next = cur->next.get())
                    {
                        std::unique_lock<std::mutex> next_lk(next->mtx);
                        if (pre(*next->data))
                        {
                            std::unique_ptr<Node> old_next = std::move(cur->next);
                            cur->next = std::move(next->next);
                            next_lk.unlock();
                        }
                        else
                        {
                            lk.unlock();
                            cur = next;
                            lk = std::move(next_lk);
                        }
                    }
                }
        };
}

int main()
{
    ns_pool::main();
    std::cout << "Hello\n";
    return 0;
}

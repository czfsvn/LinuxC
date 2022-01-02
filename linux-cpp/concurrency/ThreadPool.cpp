#include <iostream>
//#include <boost/thread.hpp>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>
#include <queue>
#include <thread>

using namespace std;

namespace ns_pool
{
    // from: <<深入应用C++11代码优化与工程应用>> chapter 9
    template <typename T>
    class SyncQueue
    {
    public:
        SyncQueue(uint32_t maxsize) : max_size_(maxsize) {}
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
            not_empty_condition_.wait(lk,
                                      [this]
                                      {
                                          return need_stop_ || notEmpty();
                                      });
            if (need_stop_)
                return;

            tlist = std::move(data_list_);
            not_full_condition_.notify_one();
        }

        void take(T& data)
        {
            std::unique_lock<std::mutex> lk(mutex_);
            not_empty_condition_.wait(lk,
                                      [this]
                                      {
                                          return need_stop_ || notEmpty();
                                      });
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
            not_full_condition_.wait(lk,
                                     [this]
                                     {
                                         return need_stop_ || notFull();
                                     });

            if (need_stop_)
                return;

            data_list_.emplace_back(std::forward<F>(data));
            not_empty_condition_.notify_one();
        }

    private:
        uint32_t                max_size_  = 0;
        bool                    need_stop_ = false;
        std::list<T>            data_list_;
        std::mutex              mutex_;
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
            std::call_once(once_flag_,
                           [this]
                           {
                               stopThreadGroup();
                           });
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
        SyncQueue<Task>                         tasks_;
        std::once_flag                          once_flag_;
        std::atomic<bool>                       is_running_ = false;
        uint32_t                                queue_size_ = 0;
    };

    void main()
    {
        ThreadPool pool(4);

        std::thread thrd1(
            [&pool]
            {
                for (uint32_t i = 0; i < 10; i++)
                {
                    auto thrd_id = std::this_thread::get_id();
                    pool.addTask(
                        [thrd_id]
                        {
                            std::cout << " 1 thread id : " << thrd_id << std::endl;
                        });
                }
            });

        std::thread thrd2(
            [&pool]
            {
                for (uint32_t i = 0; i < 10; i++)
                {
                    auto thrd_id = std::this_thread::get_id();
                    pool.addTask(
                        [thrd_id]
                        {
                            std::cout << " 2 thread id : " << thrd_id << std::endl;
                        });
                }
            });
        std::thread thrd3(
            [&pool]
            {
                for (uint32_t i = 0; i < 10; i++)
                {
                    auto thrd_id = std::this_thread::get_id();
                    pool.addTask(
                        [thrd_id]
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
}  // namespace ns_pool

namespace ns_queue
{
    // from: <<C++并发编程实战>> chapter 6  list：6.9
    template <typename T>
    class threadsafe_queue
    {
    private:
        struct Node
        {
            std::shared_ptr<T>    data;
            std::unique_ptr<Node> next;
        };

        std::mutex              head_mutex;
        std::mutex              tail_mutex;
        std::unique_ptr<Node>   head;
        Node*                   tail;
        std::condition_variable data_cond;

    private:
        Node* get_tail()
        {
            std::lock_guard<std::mutex> tail_lk(tail_mutex);
            return tail;
        }

        std::unique_ptr<Node> pop_head()
        {
            std::unique_ptr<Node> old_head = std::move(head);
            head                           = std::move(old_head->next);
            return old_head;
        }

        std::unique_lock<std::mutex> wait_for_data()
        {
            std::unique_lock<std::mutex> head_lock(head_mutex);
            data_cond.wait(head_lock,
                           [&]
                           {
                               head.get() != get_tail();
                           });
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
        threadsafe_queue() : head(new Node), tail(head.get()) {}

        threadsafe_queue(const threadsafe_queue& other) = delete;
        threadsafe_queue& operator=(const threadsafe_queue& other) = delete;

        void push(T new_val)
        {
            std::shared_ptr<T>    new_data(std::make_shared<T>(std::move(new_val)));
            std::unique_ptr<Node> p(new Node);
            {
                std::lock_guard<std::mutex> tail_lock(tail_mutex);
                tail->data           = new_data;
                const Node* new_tail = p.get();
                tail->next           = std::move(p);
                tail                 = new_tail;
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
}  // namespace ns_queue

namespace ns_lookup_table
{
    // from: <<C++并发编程>> list 6.12
    template <typename Key, typename Value, typename Hash = std::hash<Key>>
    class threadsafe_lookup_table
    {
    private:
        class bucket_type
        {
        private:
            typedef std::pair<Key, Value>          bucket_value;
            typedef std::list<bucket_value>        bucket_data;
            typedef typename bucket_data::iterator bucket_iterator;

            bucket_data data;
            // boost::shared_mutex mutex;

            bucket_iterator find_entry_for(Key const& key) const
            {
                return std::find_if(data.begin(), data.end(),
                                    [&](bucket_value const& item)
                                    {
                                        return item.first == key;
                                    });
            }

        public:
            Value value_for(Key const& key, Value const& default_value) const
            {
                // boost::shared_ptr<boost::shared_mutex> lk(mutex);
                bucket_iterator const iter = find_entry_for(key);
                return (iter != data.end() ? iter->second : default_value);
            }

            void add_or_update_mapping(Key const& key, Value const& value)
            {
                // std::unique_lock<boost::shared_mutex> lk(mutex);
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
                // std::unique_lock<boost::shared_mutex> lk(mutex);
                bucket_iterator iter = find_entry_for(key);
                if (iter != data.end())
                {
                    data.erase(iter);
                }
            }
        };

    private:
        std::vector<std::unique_ptr<bucket_type>> buckets;
        Hash                                      hasher;

        bucket_type& get_bucket(Key const& key) const
        {
            std::size_t const bucket_index = hasher(key) % buckets.size();
            return *buckets[bucket_index];
        }

    public:
        typedef Key   key_type;
        typedef Value mapped_type;
        typedef Hash  hash_type;

        threadsafe_lookup_table(uint32_t nums = 19, Hash const& hash = Hash()) : buckets(nums), hasher(hash)
        {
            for (uint32_t idx = 0; idx < buckets; idx++)
            {
                buckets[idx].reset(new bucket_type);
            }
        }

        threadsafe_lookup_table(threadsafe_lookup_table const& other) = delete;
        threadsafe_lookup_table operator=(threadsafe_lookup_table const& o) = delete;

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
}  // namespace ns_lookup_table

namespace ns_threadsafe_list
{
    // from: <<C++并发编程>> list 6.13
    template <typename T>
    class threadsafe_list
    {
        struct Node
        {
            std::mutex            mtx;
            std::shared_ptr<T>    data;
            std::unique_ptr<Node> next;
            Node() : next() {}
            Node(T const& val) : data(std::make_shared<T>(val)) {}
        };

        Node head;

    public:
        threadsafe_list() {}
        ~threadsafe_list()
        {
            remove_if(
                [](Node const&)
                {
                    return true;
                });
        }

        threadsafe_list(threadsafe_list const& other) = delete;
        threadsafe_list& operator=(threadsafe_list const& other) = delete;

        void push_front(T const& value)
        {
            std::unique_ptr<Node>       new_node(new Node(value));
            std::lock_guard<std::mutex> lk(head.mtx);
            new_node->next = std::move(head.next);
            head.next      = std::move(new_node);
        }

        template <typename Function>
        void for_each(Function f)
        {
            Node*                        cur = &head;
            std::unique_lock<std::mutex> lk(head.mtx);
            while (Node* const next = cur->next.get())
            {
                std::unique_lock<std::mutex> next_lk(next->mtx);
                lk.unlock();
                f(*next->data);
                cur = next;
                lk  = std::move(next_lk);
            }
        }

        template <typename Predicate>
        std::shared_ptr<T> find_if(Predicate pre)
        {
            Node*                        cur = &head;
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
                lk  = std::move(next_lk);
            }

            return std::shared_ptr<T>();
        }

        template <typename Predicate>
        void remove_if(Predicate pre)
        {
            Node*                        cur = &head;
            std::unique_lock<std::mutex> lk(head.mtx);
            while (Node* const next = cur->next.get())
            {
                std::unique_lock<std::mutex> next_lk(next->mtx);
                if (pre(*next->data))
                {
                    std::unique_ptr<Node> old_next = std::move(cur->next);
                    cur->next                      = std::move(next->next);
                    next_lk.unlock();
                }
                else
                {
                    lk.unlock();
                    cur = next;
                    lk  = std::move(next_lk);
                }
            }
        }
    };
}  // namespace ns_threadsafe_list

namespace ns_thrd_pool
{
    class ThreadPool
    {
    public:
        using PoolSeconds = std::chrono::seconds;

        /** 线程池的配置
         * core_threads: 核心线程个数，线程池中最少拥有的线程个数，初始化就会创建好的线程，常驻于线程池
         *
         * max_threads: >=core_threads，当任务的个数太多线程池执行不过来时，
         * 内部就会创建更多的线程用于执行更多的任务，内部线程数不会超过max_threads
         *
         * max_task_size: 内部允许存储的最大任务个数，暂时没有使用
         *
         * time_out: Cache线程的超时时间，Cache线程指的是max_threads-core_threads的线程,
         * 当time_out时间内没有执行任务，此线程就会被自动回收
         */
        struct ThreadPoolConfig
        {
            int         core_threads;
            int         max_threads;
            int         max_task_size;
            PoolSeconds time_out;
        };

        /**
         * 线程的状态：有等待、运行、停止
         */
        enum class ThreadState
        {
            kInit    = 0,
            kWaiting = 1,
            kRunning = 2,
            kStop    = 3
        };

        /**
         * 线程的种类标识：标志该线程是核心线程还是Cache线程，Cache是内部为了执行更多任务临时创建出来的
         */
        enum class ThreadFlag
        {
            kInit  = 0,
            kCore  = 1,
            kCache = 2
        };

        using ThreadPtr         = std::shared_ptr<std::thread>;
        using ThreadId          = std::atomic<int>;
        using ThreadStateAtomic = std::atomic<ThreadState>;
        using ThreadFlagAtomic  = std::atomic<ThreadFlag>;

        /**
         * 线程池中线程存在的基本单位，每个线程都有个自定义的ID，有线程种类标识和状态
         */
        struct ThreadWrapper
        {
            ThreadPtr         ptr;
            ThreadId          id;
            ThreadFlagAtomic  flag;
            ThreadStateAtomic state;

            ThreadWrapper()
            {
                ptr = nullptr;
                id  = 0;
                state.store(ThreadState::kInit);
            }
        };
        using ThreadWrapperPtr = std::shared_ptr<ThreadWrapper>;
        using ThreadPoolLock   = std::unique_lock<std::mutex>;

    private:
        ThreadPoolConfig config_;

        std::list<ThreadWrapperPtr> worker_threads_;

        std::queue<std::function<void()>> tasks_;
        std::mutex                        task_mutex_;
        std::condition_variable           task_cv_;

        std::atomic<int> total_function_num_;
        std::atomic<int> waiting_thread_num_;
        std::atomic<int> thread_id_;  // 用于为新创建的线程分配ID

        std::atomic<bool> is_shutdown_now_;
        std::atomic<bool> is_shutdown_;
        std::atomic<bool> is_available_;

    public:
        ThreadPool(ThreadPoolConfig config);
        bool IsValidConfig(ThreadPoolConfig config);
        bool Start();
        // 关掉线程池，内部还没有执行的任务会继续执行
        void ShutDown();

        // 执行关掉线程池，内部还没有执行的任务直接取消，不会再执行
        void ShutDownNow();

        // private
        void ShutDown(bool is_now);
        void AddThread(int id);
        int  GetTotalThreadSize();
        int  GetWaitingThreadSize();
        void AddThread(int id, ThreadFlag thread_flag);

        // 放在线程池中执行函数
        template <typename F, typename... Args>
        auto Run(F&& f, Args&&... args) -> std::shared_ptr<std::future<std::result_of_t<F(Args...)>>>
        {
            if (this->is_shutdown_.load() || this->is_shutdown_now_.load() || !IsAvailable())
            {
                return nullptr;
            }
            if (GetWaitingThreadSize() == 0 && GetTotalThreadSize() < config_.max_threads)
            {
                AddThread(GetNextThreadId(), ThreadFlag::kCache);
            }

            using return_type = std::result_of_t<F(Args...)>;
            auto task         = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
            total_function_num_++;

            std::future<return_type> res = task->get_future();
            {
                ThreadPoolLock lock(this->task_mutex_);
                this->tasks_.emplace(
                    [task]()
                    {
                        (*task)();
                    });
            }
            this->task_cv_.notify_one();
            return std::make_shared<std::future<std::result_of_t<F(Args...)>>>(std::move(res));
        }
    };

    ThreadPool::ThreadPool(ThreadPoolConfig config) : config_(config)
    {
        this->total_function_num_.store(0);
        this->waiting_thread_num_.store(0);

        this->thread_id_.store(0);
        this->is_shutdown_.store(false);
        this->is_shutdown_now_.store(false);

        if (IsValidConfig(config_))
        {
            is_available_.store(true);
        }
        else
        {
            is_available_.store(false);
        }
    }

    bool ThreadPool::IsValidConfig(ThreadPoolConfig config)
    {
        if (config.core_threads < 1 || config.max_threads < config.core_threads || config.time_out.count() < 1)
        {
            return false;
        }
        return true;
    }

    bool ThreadPool::IsAvailable() const
    {
        return is_available_.load();
    }

    // 开启线程池功能
    bool ThreadPool::Start()
    {
        if (!IsAvailable())
        {
            return false;
        }
        int core_thread_num = config_.core_threads;
        cout << "Init thread num " << core_thread_num << endl;
        while (core_thread_num-- > 0)
        {
            AddThread(GetNextThreadId());
        }
        cout << "Init thread end" << endl;
        return true;
    }

    // 关掉线程池，内部还没有执行的任务会继续执行
    void ThreadPool::ShutDown()
    {
        ShutDown(false);
        cout << "shutdown" << endl;
    }

    // 执行关掉线程池，内部还没有执行的任务直接取消，不会再执行
    void ThreadPool::ShutDownNow()
    {
        ShutDown(true);
        cout << "shutdown now" << endl;
    }

    // private
    void ThreadPool::ShutDown(bool is_now)
    {
        if (is_available_.load())
        {
            if (is_now)
            {
                this->is_shutdown_now_.store(true);
            }
            else
            {
                this->is_shutdown_.store(true);
            }
            this->task_cv_.notify_all();
            is_available_.store(false);
        }
    }

    void ThreadPool::AddThread(int id)
    {
        AddThread(id, ThreadFlag::kCore);
    }

    int ThreadPool::GetTotalThreadSize()
    {
        return this->worker_threads_.size();
    }

    int ThreadPool::GetWaitingThreadSize()
    {
        return this->waiting_thread_num_.load();
    }

    void ThreadPool::AddThread(int id, ThreadFlag thread_flag)
    {
        cout << "AddThread " << id << " flag " << static_cast<int>(thread_flag) << endl;
        ThreadWrapperPtr thread_ptr = std::make_shared<ThreadWrapper>();
        thread_ptr->id.store(id);
        thread_ptr->flag.store(thread_flag);
        auto func = [this, thread_ptr]()
        {
            for (;;)
            {
                std::function<void()> task;
                {
                    ThreadPoolLock lock(this->task_mutex_);
                    if (thread_ptr->state.load() == ThreadState::kStop)
                    {
                        break;
                    }
                    cout << "thread id " << thread_ptr->id.load() << " running start" << endl;
                    thread_ptr->state.store(ThreadState::kWaiting);
                    ++this->waiting_thread_num_;
                    bool is_timeout = false;
                    if (thread_ptr->flag.load() == ThreadFlag::kCore)
                    {
                        this->task_cv_.wait(lock,
                                            [this, thread_ptr]
                                            {
                                                return (this->is_shutdown_ || this->is_shutdown_now_ || !this->tasks_.empty() || thread_ptr->state.load() == ThreadState::kStop);
                                            });
                    }
                    else
                    {
                        this->task_cv_.wait_for(lock, this->config_.time_out,
                                                [this, thread_ptr]
                                                {
                                                    return (this->is_shutdown_ || this->is_shutdown_now_ || !this->tasks_.empty() || thread_ptr->state.load() == ThreadState::kStop);
                                                });
                        is_timeout = !(this->is_shutdown_ || this->is_shutdown_now_ || !this->tasks_.empty() || thread_ptr->state.load() == ThreadState::kStop);
                    }
                    --this->waiting_thread_num_;
                    cout << "thread id " << thread_ptr->id.load() << " running wait end" << endl;

                    if (is_timeout)
                    {
                        thread_ptr->state.store(ThreadState::kStop);
                    }

                    if (thread_ptr->state.load() == ThreadState::kStop)
                    {
                        cout << "thread id " << thread_ptr->id.load() << " state stop" << endl;
                        break;
                    }
                    if (this->is_shutdown_ && this->tasks_.empty())
                    {
                        cout << "thread id " << thread_ptr->id.load() << " shutdown" << endl;
                        break;
                    }
                    if (this->is_shutdown_now_)
                    {
                        cout << "thread id " << thread_ptr->id.load() << " shutdown now" << endl;
                        break;
                    }
                    thread_ptr->state.store(ThreadState::kRunning);
                    task = std::move(this->tasks_.front());
                    this->tasks_.pop();
                }
                task();
            }
            cout << "thread id " << thread_ptr->id.load() << " running end" << endl;
        };
        thread_ptr->ptr = std::make_shared<std::thread>(std::move(func));
        if (thread_ptr->ptr->joinable())
        {
            thread_ptr->ptr->detach();
        }
        this->worker_threads_.emplace_back(std::move(thread_ptr));
    }

    void main()
    {
        cout << "hello" << endl;
        ThreadPool pool(ThreadPool::ThreadPoolConfig{ 4, 5, 6, std::chrono::seconds(4) });
        pool.Start();
        std::this_thread::sleep_for(std::chrono::seconds(4));
        cout << "thread size " << pool.GetTotalThreadSize() << endl;
        std::atomic<int> index;
        index.store(0);
        std::thread t(
            [&]()
            {
                for (int i = 0; i < 10; ++i)
                {
                    pool.Run(
                        [&]()
                        {
                            cout << "function " << index.load() << endl;
                            std::this_thread::sleep_for(std::chrono::seconds(4));
                            index++;
                        });
                    // std::this_thread::sleep_for(std::chrono::seconds(2));
                }
            });
        t.detach();
        cout << "=================" << endl;

        std::this_thread::sleep_for(std::chrono::seconds(4));
        pool.Reset(ThreadPool::ThreadPoolConfig{ 4, 4, 6, std::chrono::seconds(4) });
        std::this_thread::sleep_for(std::chrono::seconds(4));
        cout << "thread size " << pool.GetTotalThreadSize() << endl;
        cout << "waiting size " << pool.GetWaitingThreadSize() << endl;
        cout << "---------------" << endl;
        pool.ShutDownNow();
        getchar();
        cout << "world" << endl;
    }
}  // namespace ns_thrd_pool

int main()
{
    ns_pool::main();
    std::cout << "Hello\n";
    return 0;
}

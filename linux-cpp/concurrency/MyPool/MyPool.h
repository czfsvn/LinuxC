#include "Random.h"
#include <assert.h>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

static Random               rnd_(0xdeadbeef);
static std::atomic_uint32_t task_idx = 0;

void inline sleepfor_seconds(const uint32_t sec)
{
    std::this_thread::sleep_for(std::chrono::seconds(sec));
}

void inline sleepfor_microseconds(const uint32_t micr_sec)
{
    std::this_thread::sleep_for(std::chrono::microseconds(micr_sec));
}

void inline sleepfor_milliseconds(const uint32_t mill_sec)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(mill_sec));
}

static uint32_t inline get_task_idx()
{
    return ++task_idx;
}

namespace helper
{
    template <typename T>
    class SyncQueueBlocking
    {
    public:
        SyncQueueBlocking() {}
        ~SyncQueueBlocking()
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

        template <typename F>
        void add(F&& data)
        {
            std::unique_lock<std::mutex> lk(mutex_);
            not_full_condition_.wait(lk,
                                     [this]
                                     {
                                         return need_stop_;
                                     });

            if (need_stop_)
                return;

            data_list_.emplace_back(std::forward<F>(data));
            not_empty_condition_.notify_one();
        }

    private:
        bool                    need_stop_ = false;
        std::list<T>            data_list_;
        std::mutex              mutex_;
        std::condition_variable not_full_condition_;
        std::condition_variable not_empty_condition_;
    };

    template <typename T>
    class SyncQueue
    {
    public:
        SyncQueue() {}
        ~SyncQueue()
        {
            data_list_.clear();
        }

        void stop()
        {
            std::lock_guard<std::mutex> lk(mutex_);
            need_stop_ = true;
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
            tlist = std::move(data_list_);
        }

        void take(T& data)
        {
            std::unique_lock<std::mutex> lk(mutex_);
            data = std::move(data_list_.front());
            data_list_.pop_front();
        }

    private:
        bool notEmpty() const
        {
            bool empty = data_list_.empty();
            if (empty)
                std::cout << "queue is empty : " << std::this_thread::get_id() << std::endl;

            return !empty;
        }

        template <typename F>
        void add(F&& data)
        {
            if (need_stop_)
                return;

            {
                std::unique_lock<std::mutex> lk(mutex_);
                data_list_.emplace_back(std::forward<F>(data));
            }
        }

    private:
        bool         need_stop_ = false;
        std::list<T> data_list_;
        std::mutex   mutex_;
    };

    class Thread
    {
    protected:
        Thread(const Thread&) = delete;
        Thread& operator=(const Thread&) = delete;

    protected:
        std::atomic_bool             stopped_  = true;
        std::unique_ptr<std::thread> thrd_ptr_ = nullptr;

    public:
        Thread()
        {
            stopped_.store(true);
        }
        virtual ~Thread() {}

        // base option
        virtual void start()
        {
            stopped_ = false;
            if (!thrd_ptr_)
            {
                thrd_ptr_ = std::unique_ptr<std::thread>(new std::thread(std::bind(&Thread::thrdloop, this)));
            }
        }

        virtual void stop()
        {
            stopped_ = true;
        }

        virtual void join()
        {
            if (thrd_ptr_ && thrd_ptr_->joinable())
            {
                thrd_ptr_->join();
            }
        }

        virtual void thrdloop()
        {
            while (!isStop())
            {
                run();
                // sleepfor_microseconds(1);
            }
        }

        virtual bool isStop()
        {
            return stopped_;
        }

        // TO DO
        virtual void run() = 0;
    };
}  // namespace helper

namespace threadpool
{
    class Task;
    class TaskPool;
    class TaskThread;
    using TaskPtr       = std::shared_ptr<Task>;
    using TaskPoolPtr   = std::shared_ptr<TaskPool>;
    using TaskThreadPtr = std::shared_ptr<TaskThread>;

    class Task
    {
    public:
        virtual bool work() = 0;
        virtual bool done()
        {
            return true;
        }
    };

    class TaskThread : public helper::Thread
    {
    public:
        TaskThread(const TaskPoolPtr& ptr);
        bool addTask(const TaskPtr& task);
        void run() override;

    private:
        TaskPoolPtr                pool_ptr_   = nullptr;
        helper::SyncQueue<TaskPtr> task_queue_ = {};
    };

    class TaskPool : public std::enable_shared_from_this<TaskPool>
    {
    public:
        TaskPool(uint16_t size);

        bool init();

        void addDone(TaskPtr& task);

        bool addNewTask(const TaskPtr& task);

        bool addNewTask(Task* task);

        void runInMain();

        void joinAll();

        void startAll();

    private:
        TaskThreadPtr getThread();

    private:
        uint16_t                   thread_size_    = 0;
        std::atomic<uint32_t>      cur_thread_idx_ = 0;
        std::vector<TaskThreadPtr> thread_vec_     = {};
        helper::SyncQueue<TaskPtr> task_done_      = {};
    };

}  // namespace threadpool

namespace threadpool_test
{
    class TestTask : public threadpool::Task
    {
    public:
        TestTask(uint32_t idx) : task_id(idx)
        {
            std::cout << "test task thread: " << std::this_thread::get_id() << ", construct: " << task_id << std::endl;
        }

        ~TestTask()
        {
            std::cout << "test task thread: " << std::this_thread::get_id() << ", destruct: " << task_id << std::endl;
        }

        virtual bool work()
        {
            std::cout << "test task thread: " << std::this_thread::get_id() << ", work: " << task_id << std::endl;
            sleepfor_microseconds(rnd_.Next() % 1000);
            return true;
        }

        virtual bool done()
        {
            std::cout << "test task thread: " << std::this_thread::get_id() << ", done: " << task_id << std::endl;
            sleepfor_microseconds(rnd_.Next() % 1000);
            return true;
        }

    private:
        uint32_t task_id = 0;
    };

    class TestTaskThread : public helper::Thread
    {
    public:
        TestTaskThread(threadpool::TaskPoolPtr ptr) : pool_ptr_(ptr) {}
        virtual void run() override;

    private:
        threadpool::TaskPoolPtr pool_ptr_ = nullptr;

        uint32_t task_count = 0;
    };

    void main();
}  // namespace threadpool_test
#include "MyPool.h"

namespace threadpool
{
    TaskThread::TaskThread(const TaskPoolPtr& ptr) : pool_ptr_(ptr)
    {
        std::cout << "init thread: " << std::this_thread::get_id() << std::endl;
    }

    bool TaskThread::addTask(const TaskPtr& task)
    {
        if (!task)
            return false;

        task_queue_.put(task);
        return true;
    }

    void TaskThread::run()
    {
        std::list<TaskPtr> task_list_ = {};
        task_queue_.take(task_list_);

        if (task_list_.empty())
        {
            // sleepfor_microseconds(1);
            return;
        }

        for (auto& taskptr : task_list_)
        {
            if (!taskptr)
                continue;

            if (taskptr->work() && pool_ptr_)
            {
                pool_ptr_->addDone(taskptr);
            }
        }
    }

    TaskPool::TaskPool(uint16_t size) : thread_size_(size)
    {
        // init();
    }

    TaskThreadPtr TaskPool::getThread()
    {
        return thread_vec_.at(cur_thread_idx_++ % thread_size_);
    }

    void TaskPool::addDone(TaskPtr& task)
    {
        task_done_.put(task);
    }

    bool TaskPool::addNewTask(const TaskPtr& task)
    {
        TaskThreadPtr thrd = getThread();
        if (!thrd)
            return false;

        thrd->addTask(task);
        return true;
    }

    bool TaskPool::addNewTask(Task* task)
    {
        return true;
        // return addNewTask(TaskPtr(task));
    }

    void TaskPool::runInMain()
    {
        std::list<TaskPtr> task_list_ = {};
        task_done_.take(task_list_);

        for (auto& task : task_list_)
        {
            if (!task)
                continue;

            task->done();
        }
    }

    bool TaskPool::init()
    {
        if (thread_size_ < 1)
        {
            assert(false);
            return false;
        }

        for (uint16_t idx = 0; idx < thread_size_; idx++)
        {
            thread_vec_.emplace_back(std::make_shared<TaskThread>(shared_from_this()));
        }

        return true;
    }

    void TaskPool::startAll()
    {
        std::for_each(thread_vec_.begin(), thread_vec_.end(),
                      [](TaskThreadPtr& thrd)
                      {
                          if (!thrd)
                              return;

                          thrd->start();
                      });
    }

    void TaskPool::joinAll()
    {
        std::for_each(thread_vec_.begin(), thread_vec_.end(),
                      [](TaskThreadPtr& thrd)
                      {
                          if (!thrd)
                              return;

                          thrd->join();
                      });
    }
}  // namespace threadpool

namespace threadpool_test
{
    void TestTaskThread::run()
    {
        // std::shared_ptr<TestTask> newtask = std::make_shared<TestTask>(get_task_idx());
        // pool_ptr_->addNewTask(new TestTask(get_task_idx()));
        pool_ptr_->addNewTask(std::make_shared<TestTask>(get_task_idx()));
        sleepfor_microseconds(10);

        if (task_count++ >= 20)
            stop();
    }

    void test()
    {
        threadpool::TaskPoolPtr pool = std::make_shared<threadpool::TaskPool>(4);
        pool->init();
        TestTaskThread* test_thrd = new TestTaskThread(pool);
        test_thrd->start();
        pool->startAll();

        while (1)
        {
            pool->runInMain();
            sleepfor_microseconds(100);
        }

        test_thrd->join();
        pool->joinAll();
    }

    void main()
    {
        test();
    }
}  // namespace threadpool_test
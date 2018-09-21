#include "Thread.h"
#include <signal.h>
#include <algorithm>

#define SAFE_DELETE(x)      {   if (x)  {   delete (x); (x) == NULL; }  }
#define SAFE_DELETE_VEC(x)      {   if (x)  {   delete[] (x); (x) == NULL; }  }

#define SAFE_SUB(x, y)          ((x) > (y) ? (x) - (y) : 0)
#define SAFE_ADD(x, y, max)     ((x) + (y) > (max) ? (max) : (x) + (y))

namespace ns_thread 
{
    void *Thread::threadFunc(void* arg)
    {
        Thread* thread = (Thread*)arg;

        int seed = time(NULL);

        thread->mlock.lock();
        thread->alive = true;
        thread->cond.broadcast();
        thread->mlock.unlock();

        sigset_t    sig_mask;
        sigfillset(&sig_mask);
        sigdelset(&sig_mask, SIGSEGV);
        //sigdelset(&sig_mask, SIGPROF);
        pthread_sigmask(SIG_SETMASK, &sig_mask, NULL);

        //mysql_thread_init();

        thread->run();

        //mysql_thread_end();

        thread->mlock.lock();
        thread->alive = false;
        thread->cond.broadcast();
        thread->mlock.unlock();

        if (!thread->isJoinable())
        {
            thread->mlock.lock();
            while (thread->alive)
                thread->cond.wait(thread->mlock);
            thread->mlock.unlock();
            SAFE_DELETE(thread);
        }

        pthread_exit(nullptr);
    }

    bool Thread::start()
    {
        if (alive)
            return false;

        pthread_attr_t  attr;
        pthread_attr_init(&attr);
        if (!joinable)
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        if (0 != ::pthread_create(&thread, NULL, Thread::threadFunc, this))
        {
            return false;
        }
        pthread_attr_destroy(&attr);
        mlock.lock();
        while (alive)
            cond.wait(mlock);
        mlock.unlock();
        
        return true;
    }

    void Thread::join()
    {
        if (thread != 0 && joinable)
        {
            ::pthread_join(thread, nullptr);
            thread = 0;
            mlock.lock();
            while (alive)
                cond.wait(mlock);
            mlock.unlock();
        }
    }

    ThreadGroup::ThreadGroup()
    {
    }
    ThreadGroup::~ThreadGroup()
    {
        joinAll();
    }

    void ThreadGroup::add(Thread* thread)
    {
        if (!thread)
            return;
        Container::iterator it = std::find(vec.begin(), vec.end(), thread);
        if (it != vec.end())
            return;
        vec.push_back(thread);
    }

    Thread* ThreadGroup::getByIdx(int idx)
    {
        RWLock_scope_rdlock scope_lock(rwlock);
        if (idx >= vec.size())
            return nullptr;
        return vec[idx];
    }
    Thread* ThreadGroup::operator [](int idx)
    {
        return getByIdx(idx);
    }

    void ThreadGroup::joinAll()
    {
        RWLock_scope_wrlock scope_lock(rwlock);
        for (Container::iterator it = vec.begin();
                it != vec.end(); ++it)
        {
            Thread* thread = *it;
            if (thread)
            {
                thread->final();
                thread->join();
                SAFE_DELETE(thread);
            }
        }
        vec.clear();
    }

    void ThreadGroup::execAll(CallBack& cb)
    {
        RWLock_scope_rdlock scope_lock(rwlock);
        for (Container::iterator it = vec.begin();
                it != vec.end(); ++it)
        {
            cb.exec(*it);
        }
    }

}




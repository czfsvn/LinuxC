#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>

using namespace std;

namespace ns_thread 
{
    class Noncopyable
    {
        protected:
            Noncopyable()   {}
            ~Noncopyable()  {}
        private:
            Noncopyable(const Noncopyable&);
            const Noncopyable& operator = (const Noncopyable&);
    };

    class Condition;

    class Mutex : private Noncopyable
    {
        friend class Condition;
        public:
            Mutex(int kind = PTHREAD_MUTEX_FAST_NP)
            {
                pthread_mutexattr_t attr;
                ::pthread_mutexattr_init(&attr);
                ::pthread_mutexattr_settype(&attr, kind);
                ::pthread_mutex_init(&mutex, &attr);
            }

            ~Mutex()
            {
                ::pthread_mutex_destroy(&mutex);
            }

            inline void lock()
            {
                ::pthread_mutex_lock(&mutex);
            }
            inline void unlock()
            {
                ::pthread_mutex_unlock(&mutex);
            }
        private:
            pthread_mutex_t    mutex;
    };

    class Condition : private Noncopyable
    {
        public:
            Condition()
            {
                ::pthread_cond_init(&cond, nullptr);
            }
            ~Condition()
            {
                ::pthread_cond_destroy(&cond);
            }
            void broadcast()
            {
                ::pthread_cond_broadcast(&cond);
            }
            void signal()
            {
                ::pthread_cond_signal(&cond);
            }
            void wait(Mutex& mutex)
            {
                ::pthread_cond_wait(&cond, &mutex.mutex);
            }
        private:
            pthread_cond_t  cond;
    };

    class Mutex_scope_lock : private Noncopyable
    {
        public:
            Mutex_scope_lock(Mutex& m) : mlock(m)
            {
                mlock.lock();
            }
            ~Mutex_scope_lock()
            {
                mlock.unlock();
            }
        private:
            Mutex&  mlock;
    };

    class RWLock : private Noncopyable
    {
        public:
            RWLock() : rd_count(0), wr_count(0)
            {
                ::pthread_rwlock_init(&rwlock, nullptr);
            }
            ~RWLock()
            {
                ::pthread_rwlock_destroy(&rwlock);
            }
            inline void rdlock()
            {
                ::pthread_rwlock_rdlock(&rwlock);
                ++rd_count;
            }
            inline void wrlock()
            {
                ::pthread_rwlock_wrlock(&rwlock);
                ++wr_count;
                ++rd_count;
            }
            inline void unlock()
            {
                ::pthread_rwlock_unlock(&rwlock);
                --rd_count;
            }


        private:
            unsigned int        rd_count;
            unsigned int        wr_count;
            pthread_rwlock_t    rwlock;
    };

    class RWLock_scope_rdlock : private Noncopyable
    {
        public:
            RWLock_scope_rdlock(RWLock& m) : rwlock(m)
            {
                rwlock.rdlock();
            }
            ~RWLock_scope_rdlock()
            {
                rwlock.unlock();
            }
        private:
            RWLock&     rwlock;
    };

    class RWLock_scope_wrlock : private Noncopyable
    {
        public:
            RWLock_scope_wrlock(RWLock& m) : rwlock(m)
            {
                rwlock.wrlock();
            }
            ~RWLock_scope_wrlock()
            {
                rwlock.unlock();
            }
        private:
            RWLock&     rwlock;
    };

    class Thread : private Noncopyable
    {
        public:
            Thread(std::string name, bool join = true) : threadName(name),
                alive(false), complete(false), thread(0), joinable(join)
            {
            }

            virtual ~Thread()
            {
            }

            static pthread_t getCurrentThreadId()
            {
                return ::pthread_self();
            }

            //sleep for second
            static void sleep(const long sec)
            {
                ::sleep(sec);
            }
            //sleep for millisecond (1 second = 1000 millisecond)
            static void msleep(const unsigned long msec)
            {
                ::usleep(msec * 1000);
            }
            //sleep for microsecond  (1 second = 1000000 microsecond)
            static void usleep(const unsigned long usec)
            {
                ::usleep(usec);
            }
            const bool isJoinable() const
            {
                return joinable;
            }
            const bool isAlive() const
            {
                return alive;
            }
        
            virtual void run() = 0;
            static void *threadFunc(void* arg);
            bool start();
            void join();

            void final()
            {
                complete = true;
            }
            const bool isFinal() const
            {
                return complete;
            }
            bool operator ==(const Thread& other) const
            {
                return pthread_equal(thread, other.thread) != 0;
            }
            bool operator !=(const Thread& other) const
            {
                return !(operator==(other));
            }
            const std::string& getThreadName() const
            {
                return threadName;
            }

        private:
            std::string     threadName;
            Mutex           mlock;
            Condition        cond;
            volatile bool   alive;
            volatile bool   complete;
            pthread_t       thread;
            bool            joinable;
    };

    class ThreadGroup : private Noncopyable
    {
        typedef std::vector<Thread*>    Container;
        public:
            struct CallBack
            {
                virtual void exec(Thread* e) = 0;
                virtual ~CallBack() {}
            };;
            
            ThreadGroup();
            ~ThreadGroup();

            void add(Thread* thread);
            Thread* getByIdx(int idx);
            Thread* operator[](int idx);
            void joinAll();
            void execAll(CallBack& cb);
            int size()
            {
                RWLock_scope_rdlock scope_lock(rwlock);
                return vec.size();
            }

        private:
            Container   vec;
            RWLock      rwlock;

    };
}


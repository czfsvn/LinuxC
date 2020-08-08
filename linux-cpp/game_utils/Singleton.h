#ifndef __SINGLETON_H__
#define __SINGLETON_H__

template <typename T>
class SinglegonFactory
{
public:
    static T *inst()
    {
        return new T();
    }
};

template <typename T, typename FACTORY = SinglegonFactory<T>>
class Singleton
{
private:
    Singleton(const Singleton &);

    const Singleton &operator=(const Singleton &);

protected:
    static T *inst;
    Singleton() {}
    ~Singleton() {}

public:
    static void delMe()
    {
        if (instance)
        {
            delete inst;
            inst = nullptr;
        }
    }
    static T *instance()
    {
        if (!inst)
        {
            inst = FACTORY::inst();
        }
        return inst;
    }
    static T &getMe()
    {
        return *instance();
    }
};

template <typename T, typename FACTORY>
T *Singleton<T, FACTORY>::inst = nullptr;

#endif
#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
#include <typeindex>

using namespace std;

struct Any
{
    Any(void) : m_tpIndex(std:: type_index( typeid( void))){}
    Any(Any& that) : m_ptr(that.Clone()), m_tpIndex(that.m_tpIndex) {} 
    Any(Any&& that) : m_ptr(std::move(that.m_ptr)), m_tpIndex(that.m_tpIndex) {} 
    // 创建智能指针时， 对于一般的类型，通过std::decay来移除引用和cv符，而获取原始类型 

    template<typename U, class = typename std::enable_if<!std::is_same<typename std::decay<U>::type, Any>::value, U>::type> 
        Any(U&& value) : 
            m_ptr(new Derived<typename std::decay< U>:: type>(forward<U>(value))), 
            m_tpIndex(type_index(typeid(typename std::decay<U>::type)))
    {
    } 
    bool IsNull() const { return !bool( m_ptr); }

    template<class U> bool Is() const 
    { 
        return m_tpIndex == type_index(typeid(U)); 
    } 
    // 将Any转换为实际的类型 
    template<class U> 
        U& AnyCast() 
        { 
            if (!Is<U>()) 
            { 
                std::cout << "can not cast " << typeid(U).name() << " to " 
                    << m_tpIndex.name() << endl; 
                throw bad_cast(); 
            } 
            auto derived = dynamic_cast<Derived<U>*>(m_ptr.get()); 
            return derived->m_value; 
        } 
    
    Any& operator=( const Any& a) 
    { 
        if (m_ptr == a.m_ptr) 
            return *this; 
        m_ptr = a.Clone(); 
        m_tpIndex = a.m_tpIndex; 
        return *this; 
    }
private:
    struct Base; 
    typedef std::unique_ptr<Base> BasePtr; 
    struct Base 
    { 
        virtual ~ Base() {} 
        virtual BasePtr Clone() const = 0; 
    }; 
    
    template<typename T> 
        struct Derived : Base 
    { 
        template<typename U> 
            Derived(U && value) : m_value(forward<U>(value)) 
        { } 
        
        BasePtr Clone() const 
        { 
            return BasePtr( new Derived<T>( m_value)); 
        } 
        
        T m_value; 
    }; 
    BasePtr Clone() const 
    { 
        if (m_ptr != nullptr) 
            return m_ptr->Clone(); 
        return nullptr; 
    } 
    BasePtr m_ptr; 
    std::type_index m_tpIndex;
};

namespace ioc_1
{
    struct Base
    {
        virtual void Func() {}
        virtual ~Base() {}
    };

    struct DerivedB : Base
    {
        void Func() override
        {
            std::cout << "call func in DerivedB\n";
        }
    };
    struct DerivedC : Base
    {
        void Func() override
        {
            std::cout << "call func in DerivedC\n";
        }
    };
    struct DerivedD : Base
    {
        void Func() override
        {
            std::cout << "call func in DerivedD\n";
        }
    };

    class A
    {
        public:
            A(Base* base)   : m_base(base)  {}
            void Func() {   m_base->Func(); }
            ~A()
            {
                if (m_base != nullptr)
                {
                    delete m_base;
                    m_base = nullptr;
                }
            }
        private:
            Base* m_base = nullptr;
    };

    void test_1()
    {
        A* a = new A(new DerivedB);
        a->Func();
        delete a;
    }

}

namespace ioc_2
{
    template<typename T>
        class IocContainer
        {
            public:
                IocContainer()  {}
                ~IocContainer() {}

                template<typename Derived>
                    void RegisterType(std::string strKey)
                {
                    std::function<T*()> func = []{  return new Derived();   };
                    RegisterType(strKey, func);
                }

                T* Resolve(std::string strKey)
                {
                    auto iter = m_createMap.find(strKey);
                    if (iter ==  m_createMap.end())
                        return nullptr;
                    return iter->second();
                }

                std::shared_ptr<T> ResolveShared(std::string strKey)
                {
                    T* ptr = Resolve(strKey);
                    return std::shared_ptr<T>(ptr);
                }
            private:
                void RegisterType(std::string strKey, std::function<T*()> func)
                {
                    if (m_createMap.find(strKey) != m_createMap.end())
                        return;
                    m_createMap.emplace(strKey, func);
                }
            private:
                std::map<std::string, std::function<T*()>> m_createMap;
        };

    struct ICar
    {
        virtual ~ICar() { std::cout << "ICar dest called\n";    }
        virtual void test() const = 0;
    };
    struct Bus : ICar
    {
        Bus()   {}
        void test() const   {   std::cout << "Bus :: test \n";  }
    };
    struct Car : ICar
    {
        Car()   {}
        void test() const   {   std::cout << "Car :: test \n";  }
    };

    void test()
    {
        IocContainer<ICar> ioc_car;
        ioc_car.RegisterType<Bus>("bus");
        ioc_car.RegisterType<Car>("car");

        std::shared_ptr<ICar> bus = ioc_car.ResolveShared("bus");
        std::shared_ptr<ICar> car = ioc_car.ResolveShared("car");

        bus->test();
        car->test();
    }
}

namespace ioc_3
{
    class IocContainer
    {
        public:
            IocContainer()  {   std::cout << "IocContainer const\n";    }
            ~IocContainer()  {   std::cout << "IocContainer dest\n";    }

            template<typename T, typename depend>
                void RegisterType(const std::string& strKey)
                {
                    std::function<T*()> func = []{  return new T(new depend()); };
                    RegisterType(strKey, func);
                }

            template<typename T>
                T* Resolved(const std::string& strKey)
                {
                }

        private:
            std::unordered_map<std::string, Any> m_createMap;
    };
}

int main()
{
    //ioc_1::test();
    ioc_2::test();
    return 0;
}

#include <iostream>
#include <map>
#include <functional>

#define PRINT_FUNC std::cout << "========= " << __FUNCTION__ << " ================= " << std::endl;

namespace ns_m1
{
    struct User
    {
        uint32_t id = 0;
        uint32_t age = 0;
    };

    class BaseManager
    {
    public:
        BaseManager()
        {
        }

        void init(uint32_t cnt)
        {
            for (uint32_t idx = 0; idx < cnt; idx++)
            {
                User *user = new User();
                user->id = idx;
                user->age = idx + 10;
                base_map_.emplace(idx, user);
            }
        }

        void forEach(std::function<void(User *)> &&func)
        {
            for (const auto &kp : base_map_)
            {
                if (kp.second)
                    func(kp.second);
            }
        }

    private:
        std::map<uint32_t, User *> base_map_ = {};
    };

    void test()
    {
        PRINT_FUNC
        BaseManager base_mgr;
        base_mgr.init(10);

        auto lbd = [](User *user) -> void {
            if (user)
            {
                std::cout << user->id << std::endl;
            }
        };
        base_mgr.forEach(lbd);
        base_mgr.forEach([](User *user) {
            if (user)
                std::cout << user->age << std::endl;
        });
    }

    void test2()
    {
        PRINT_FUNC
        BaseManager base_mgr;
        base_mgr.init(10);

        auto lbd1 = [](User *user, uint32_t value) -> void {
            std::cout << user->id << ", value: " << value << std::endl;
        };

        auto func = std::bind(lbd1, std::placeholders::_1, 10);
        base_mgr.forEach(func);
    }
} // namespace ns_m1

namespace ns_m2
{
    void test1()
    {
        std::map<uint32_t, uint32_t> uMap;
    }
}

int main()
{
    PRINT_FUNC
    //ns_m1::test();
    ns_m1::test2();
    return 0;
}
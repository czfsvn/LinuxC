#include <iostream>
#include <set>

using namespace std;

namespace ns_timer_node
{
    class TimerNode
    {
    public:
        TimerNode(uint32_t id, uint32_t expired):timer_id(id), expired_sec(expired){}
        void run()
        {
            std::cout << "timer_id: " << timer_id << ", now: " << time(NULL) << " run.\n";
        }

        uint32_t getExpiredSec() const
        {
            return expired_sec;
        }

    private:
        uint32_t timer_id = 0;
        uint32_t expired_sec = 0;

    };
} // namespace ns_timer_node


namespace ns_timer_set
{
    class TimerModule
    {
    public:
        TimerModule(){}
        void run()
        {
            const uint32_t now = time(NULL);
            while (!timer_sets.empty())
            {
                auto iter = timer_sets.begin();
                if (!(*iter)->getExpiredSec())
                    break;

                (*iter)->run();
                timer_sets.erase(iter);
            }
        }

        void addTimer(ns_timer_node::TimerNode* node)
        {
            if (!node)
                return;

            timer_sets.emplace(node);
        }

    private:
        std::set<ns_timer_node::TimerNode*> timer_sets = {};
    };

    void main()
    {
        uint32_t now = time(nullptr);
        TimerModule timer_module;
        std::cout << "main start: " << now << std::endl;
        for (uint32_t idx = 0; idx < 10;idx++)
        {
            ns_timer_node::TimerNode* node = new ns_timer_node::TimerNode(idx, now + idx * 10);
            timer_module.addTimer(node);
        }

        while (1)
        {
                 std::this_thread::sleep_for(1s);
            timer_module.run();
        }
    }
} // namespace ns_timer_set


int main()
{
    ns_timer_set::main();
    std::cout << "Hello, Timer\n";
    return 0;
}
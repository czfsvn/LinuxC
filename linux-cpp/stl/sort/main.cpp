#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

namespace ns_std_sort
{
    struct Entry
    {
        Entry(uint32_t id) : user_id(id) {}

        uint32_t user_id = 0;
        uint32_t score   = 0;
    };

    bool cmp(const Entry& lhs, const Entry& rhs)
    {
        return lhs.score > rhs.score;
    }

    void print_ele(const std::vector<Entry>& vec)
    {
        std::stringstream oss;
        for (const auto& item : vec)
        {
            oss << "(" << item.user_id << "-" << item.score << ")";
        }
        std::cout << "\nprint_ele : " << oss.str() << std::endl;
    }

    void test1()
    {
        std::vector<Entry> vec;

        for (uint32_t idx = 0; idx < 16; idx++)
        {
            Entry entry(idx);
            entry.score = 1000;
            vec.emplace_back(std::move(entry));
        }

        vec[ 5 ].score = 111111;
        vec[ 6 ].score = 111111;
        print_ele(vec);
        std::sort(vec.begin(), vec.end(), cmp);
        print_ele(vec);
        std::sort(vec.begin(), vec.end(), cmp);
        print_ele(vec);
        std::sort(vec.begin(), vec.end(), cmp);
        print_ele(vec);
    }

    void main()
    {
        test1();
    }

}  // namespace ns_std_sort

int main()
{
    ns_std_sort::main();
    return 0;
}

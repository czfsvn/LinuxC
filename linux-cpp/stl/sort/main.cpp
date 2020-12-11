#include <algorithm>
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
        std::cout << "print_ele : " << oss.str() << std::endl;
    }

    void test1()
    {
        std::vector<Entry> vec;

        for (uint32_t idx = 0; idx < 40; idx++)
        {
            Entry entry(idx);
            entry.score = 1000;
            vec.emplace_back(std::move(entry));
        }

        vec[5].score = 111111;
        vec[6].score = 111111;
        print_ele(vec);
        std::sort(vec.begin(), vec.end(), cmp);
        print_ele(vec);
        std::sort(vec.begin(), vec.end(), cmp);
        print_ele(vec);
        // std::sort(vec.begin(), vec.end(), cmp);
        // print_ele(vec);
    }

    void main()
    {
        test1();
    }

}  // namespace ns_std_sort

namespace ns_base
{
    std::vector<uint32_t> uint32_vector()
    {
        std::vector<uint32_t> vec = { 36, 66, 2, 15, 6, 81, 6, 9 };
        return vec;
    }

    void print_vec(const std::vector<uint32_t>& vec)
    {
        std::stringstream oss;
        oss << "(";
        for (const auto& item : vec)
        {
            oss << item << ",";
        }
        oss << ")";
        std::cout << oss.str() << std::endl;
    }
}  // namespace ns_base

namespace ns_quick_sort
{
    void quick_sort(std::vector<uint32_t>& vec, const uint32_t low, const uint32_t high)
    {
        uint32_t       pos     = low;
        const uint32_t pivot   = vec[0];
        bool           success = false;
        for (uint32_t index = high; index >= 0; index--)
        {
            if (vec[index] > pivot)
                continue;

            vec[low]   = vec[index];
            vec[index] = pivot;
            pos        = index;
            success    = true;
            break;
        }
        for (uint32_t idx = 0; idx < vec.size(); idx++)
        {
            if (vec[idx] < vec[pos])
                continue;

            vec[pos] = vec[idx];
            vec[idx] = pivot;
            success  = true;
            break;
        }
        if (!success)
            return;

        quick_sort(vec, 0, vec.size());
    }
    void main()
    {
        std::vector<uint32_t> vec = ns_base::uint32_vector();
        quick_sort(vec, 0, vec.size());
        ns_base::print_vec(vec);
    }
}  // namespace ns_quick_sort

int main()
{
    // ns_std_sort::main();
    ns_quick_sort::main();
    return 0;
}

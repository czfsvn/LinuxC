#include <algorithm>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>

using namespace std;

namespace ns_base
{
    struct Entry
    { /*
         Entry(uint32_t id) : user_id(id) {}

         Entry& operator=(Entry& lhs)
         {
             this->user_id = lhs.user_id;
             this->score   = lhs.score;
             return *this;
         }*/

        uint32_t user_id = 0;
        uint32_t score   = 0;

        // std::ostream& operator<<(std::ostream& oss)
        //{
        //    oss << user_id;
        //    return oss;
        //}
    };

    std::vector<uint32_t> gen_uint_vec()
    {
        // std::vector<uint32_t> vec = { 5, 3, 7, 6, 4, 1, 0, 2, 9, 10, 8 /*, 10, 13, 4, 56, 7, 8, 90*/ };
        std::vector<uint32_t> vec = { 35, 66, 2, 15, 6, 81, 6, 9 };
        return vec;
    }

    std::list<uint32_t> gen_uint_list()
    {
        std::list<uint32_t> ulist = { 5, 3, 7, 6, 4, 1, 0, 2, 9, 10, 8, 10, 13, 4, 56, 7, 8, 90 };
        return ulist;
    }

    std::vector<Entry> gen_entry_vec()
    {
        std::vector<Entry> vec;

        for (uint32_t idx = 0; idx < 40; idx++)
        {
            Entry entry;
            entry.user_id = idx;
            entry.score   = 1000;
            vec.emplace_back(std::move(entry));
        }

        vec[5].score = 111111;
        vec[6].score = 111111;
        return vec;
    }

    bool cmp(const Entry& lhs, const Entry& rhs)
    {
        if (lhs.score < rhs.score)
            return lhs.score < rhs.score;
        // if (lhs.score != rhs.score)
        // return lhs.score > rhs.score;
        return lhs.user_id > rhs.user_id;
    }

    std::ostream& operator<<(std::ostream& oss, const Entry& entry)
    {
        oss << "(" << entry.user_id << ", " << entry.score << ")";
        return oss;
    }

    template <typename Cont>
    void print_cont(const Cont& vec)
    {
        std::stringstream oss;
        oss << "(";
        for (const auto& item : vec)
        {
            oss << item << ",";
        }
        std::cout << oss.str() << ")\n";
    }

    template <typename T>
    void Swap(T& lhs, T& rhs)
    {
        T tmp = rhs;
        rhs   = lhs;
        lhs   = tmp;
    }
    /*
    template <typename T>
    void swap(T&& lhs, T&& rhs)
    {
        // const T& tmp = rhs;
        T& tmp = std::move(rhs);
        rhs    = std::move(lhs);
        lhs    = std::move(tmp);
    }*/
}  // namespace ns_base

namespace ns_std_sort
{
    void print_ele(const std::vector<ns_base::Entry>& vec)
    {
        std::stringstream oss;
        for (const auto& item : vec)
        {
            // oss << "(" << item.user_id << "-" << item.score << ")";
            oss << item;
        }
        std::cout << "print_ele : " << oss.str() << std::endl;
    }

    void test1()
    {
        std::vector<ns_base::Entry> vec = ns_base::gen_entry_vec();
        print_ele(vec);
        for (uint32_t idx = 0; idx < 10; idx++)
        {
            std::sort(vec.begin(), vec.end(), ns_base::cmp);
            print_ele(vec);
            std::sort(vec.begin(), vec.end(), ns_base::cmp);
            print_ele(vec);
        }
        std::sort(vec.begin(), vec.end(), ns_base::cmp);
        print_ele(vec);
        std::sort(vec.begin(), vec.end(), ns_base::cmp);
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
    /*
    uint32_t patition(std::vector<uint32_t>& vec, uint32_t from, uint32_t to)
    {
        std::cout << "from: " << from << ", to: " << to << std::endl;
        const uint32_t pivot  = vec[from];
        uint32_t       pi_idx = from;
        for (; to > from; to--)
        {
            if (vec[to] > pivot)
                continue;

            ns_base::Swap(vec[pi_idx], vec[to]);
            pi_idx = to;
            for (; from <= to - 1; from++)
            {
                if (vec[from] <= pivot)
                    continue;

                ns_base::Swap(vec[pi_idx], vec[from]);
                pi_idx = from;
                break;
            }
        }
        return pi_idx;
    }
    */
    uint32_t patition(std::vector<uint32_t>& vec, uint32_t low, uint32_t high)
    {
        std::cout << "low: " << low << ", high: " << high << std::endl;
        const uint32_t pivot = vec[low];
        while (low < high)
        {
            while (low < high && vec[high] >= pivot)
                high--;

            vec[low] = vec[high];
            ns_base::print_cont(vec);
            while (low < high && vec[low] <= pivot)
                low++;

            vec[high] = vec[low];
            ns_base::print_cont(vec);
        }
        vec[low] = pivot;
        ns_base::print_cont(vec);
        return low;
    }

    uint32_t patition2(std::vector<uint32_t>& vec, uint32_t low, uint32_t high)
    {
        if (low >= vec.size() || high >= vec.size())
            return -1;

        // std::cout << "low: " << low << ", high: " << high << std::endl;
        uint32_t       pos   = (low + high) / 2;
        const uint32_t pivot = vec[pos];
        while (low < high)
        {
            while (pos < high)
            {
                if (vec[high] < pivot)
                {
                    ns_base::Swap(vec[high], vec[pos]);
                    // ns_base::print_cont(vec);
                    pos = high;
                    break;
                }
                high--;
                continue;
            }

            while (low < pos)
            {
                if (vec[low] < pivot)
                {
                    low++;
                    continue;
                }
                ns_base::Swap(vec[pos], vec[low]);
                // ns_base::print_cont(vec);
                pos = low;
                break;
            }
        }
        // vec[low] = pivot;
        // ns_base::print_cont(vec);
        return pos;
    }

    template <typename T, typename Comp>
    uint32_t patition3(std::vector<T>& vec, uint32_t low, uint32_t high, Comp&& cmp)
    {
        if (low >= vec.size() || high >= vec.size())
            return -1;

        // std::cout << "low: " << low << ", high: " << high << std::endl;
        uint32_t pos   = (low + high) / 2;
        const T  pivot = vec[pos];
        while (low < high)
        {
            while (pos < high)
            {
                // if (vec[high] < pivot)
                if (cmp(vec[high], pivot))
                {
                    ns_base::Swap(vec[high], vec[pos]);
                    // ns_base::print_cont(vec);
                    pos = high;
                    break;
                }
                high--;
                continue;
            }

            while (low < pos)
            {
                if (cmp(vec[low], pivot))
                {
                    low++;
                    continue;
                }
                ns_base::Swap(vec[pos], vec[low]);
                // ns_base::print_cont(vec);
                pos = low;
                break;
            }
        }
        // vec[low] = pivot;
        // ns_base::print_cont(vec);
        return pos;
    }

    void quick_sort(std::vector<uint32_t>& vec, uint32_t from, uint32_t to)
    {
        if (from >= to || vec.empty())
            return;

        uint32_t right = patition2(vec, from, to);
        if (right <= 0)
            return;

        quick_sort(vec, from, right - 1);
        quick_sort(vec, right + 1, to);
    }

    template <typename T, typename Cmp>
    void quick_sort(std::vector<T>& vec, uint32_t from, uint32_t to, Cmp&& cmp)
    {
        if (from >= to || vec.empty())
            return;

        uint32_t right = patition3(vec, from, to, cmp);
        if (right <= 0)
            return;

        quick_sort(vec, from, right - 1, cmp);
        quick_sort(vec, right + 1, to, cmp);
    }

    void test1()
    {
        std::vector<uint32_t> vec = ns_base::gen_uint_vec();
        quick_sort(vec, 0, vec.size() - 1);
        ns_base::print_cont(vec);
    }

    void test2()
    {
        std::vector<uint32_t> vec = ns_base::gen_uint_vec();
        quick_sort(vec, 0, vec.size() - 1, std ::less<>());
        ns_base::print_cont(vec);
    }

    void test3()
    {
        std::vector<ns_base::Entry> entry_vec = ns_base::gen_entry_vec();
        quick_sort(entry_vec, 0, entry_vec.size() - 1, ns_base::cmp);
        ns_base::print_cont(entry_vec);

        quick_sort(entry_vec, 0, entry_vec.size() - 1, ns_base::cmp);
        ns_base::print_cont(entry_vec);

        quick_sort(entry_vec, 0, entry_vec.size() - 1, ns_base::cmp);
        ns_base::print_cont(entry_vec);
    }

    void main()
    {
        test1();
        test2();
        test3();
    }
}  // namespace ns_quick_sort

namespace ns_bubble_sort
{
    void bubble_sort(std::vector<uint32_t>& vec)
    {
        bool changed = false;
        for (uint32_t idx = 0; idx < vec.size(); idx++)
        {
            if (idx + 1 >= vec.size())
                break;

            if (vec[idx] <= vec[idx + 1])
                continue;

            ns_base::Swap(vec[idx], vec[idx + 1]);
            changed = true;
        }
        if (!changed)
            return;

        return bubble_sort(vec);
    }

    template <typename Iterator, typename Comp>
    void bubble_sort(Iterator begin, Iterator end, Comp cmp)
    {
        bool     changed = false;
        Iterator from    = begin;
        for (; from != end; from++)
        {
            Iterator next = from;
            std::advance(next, 1);
            if (next == end)
                break;

            if (cmp(*next, *from))
                continue;

            ns_base::Swap(*next, *from);
            changed = true;
        }
        if (!changed)
            return;

        return bubble_sort(begin, end, cmp);
    }

    void main()
    {
        std::vector<uint32_t> vec = ns_base::gen_uint_vec();
        // bubble_sort(vec);
        bubble_sort(vec.begin(), vec.end(), std::less_equal<>());
        ns_base::print_cont(vec);

        std::list<uint32_t> ulist = ns_base::gen_uint_list();
        bubble_sort(ulist.begin(), ulist.end(), std::less_equal<>());
        ns_base::print_cont(ulist);

        std::vector<ns_base::Entry> entry_vec = ns_base::gen_entry_vec();
        bubble_sort(entry_vec.begin(), entry_vec.end(), ns_base::cmp);
        ns_base::print_cont(entry_vec);

        bubble_sort(entry_vec.begin(), entry_vec.end(), ns_base::cmp);
        ns_base::print_cont(entry_vec);
    }

}  // namespace ns_bubble_sort

namespace ns_insert_sort
{
    void insert_sort(std::vector<uint32_t>& vec)
    {
        for (uint32_t idx = 1; idx < vec.size(); idx++)
        {
            for (uint32_t index = idx; index > 0; index--)
            {
                if (vec[index] > vec[index - 1])
                    continue;
                ns_base::Swap(vec[index], vec[index - 1]);
            }
            ns_base::print_cont(vec);
        }
    }

    template <typename T, typename Comp>
    void insert_sort(std::vector<T>& vec, Comp&& cmp)
    {
        for (uint32_t idx = 1; idx < vec.size(); idx++)
        {
            for (uint32_t index = idx; index > 0; index--)
            {
                if (!cmp(vec[index], vec[index - 1]))
                    continue;

                ns_base::Swap(vec[index], vec[index - 1]);
            }
            ns_base::print_cont(vec);
        }
    }

    template <typename Iterator, typename Comp>
    void insert_sort(Iterator begin, Iterator end, Comp&& cmp)
    {
        Iterator start = begin;
        std::advance(begin, 1);
        for (; begin != end; begin++)
        {
            for (Iterator iter = begin; iter != start; iter--)
            {
                Iterator prev = iter;
                std::advance(prev, -1);
                if (!cmp(*iter, *prev))
                    continue;
                ns_base::Swap(*iter, *prev);
            }
        }
    }

    void test1()
    {
        std::vector<uint32_t> vec = ns_base::gen_uint_vec();
        // ns_base::print_cont(vec);
        insert_sort(vec);
        // ns_base::print_cont(vec);
    }

    void test2()
    {
        std::vector<uint32_t> vec = ns_base::gen_uint_vec();
        // ns_base::print_cont(vec);
        insert_sort(vec, std::less<>());
        ns_base::print_cont(vec);

        insert_sort(vec, std::greater<>());
        ns_base::print_cont(vec);
    }

    void test3()
    {
        std::vector<uint32_t> vec = ns_base::gen_uint_vec();
        // ns_base::print_cont(vec);
        insert_sort(vec.begin(), vec.end(), std::less<>());
        ns_base::print_cont(vec);

        std::list<uint32_t> ulist = ns_base::gen_uint_list();
        insert_sort(ulist.begin(), ulist.end(), std::greater<>());
        ns_base::print_cont(ulist);
    }

    void main()
    {
        test3();
    }

}  // namespace ns_insert_sort

namespace ns_bin_insert_sort
{
    void bin_insert_sort(std::vector<uint32_t>& vec)
    {
        for (uint32_t idx = 1; idx < vec.size(); idx++)
        {
            uint32_t mid = idx + 0;
        }
    }
    void test1()
    {
        std::vector<uint32_t> vec = ns_base::gen_uint_vec();
        bin_insert_sort(vec);
        ns_base::print_cont(vec);
    }
    void main()
    {
        test1();
    }

}  // namespace ns_bin_insert_sort

int main()
{
    ns_std_sort::main();
    // ns_quick_sort::main();
    // ns_bubble_sort::main();
    // ns_insert_sort::main();
    return 0;
}

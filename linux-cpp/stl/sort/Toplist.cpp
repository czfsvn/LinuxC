#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <set>
#include <thread>
#include <vector>

using namespace std;
using namespace std::chrono;

#define TIME_COST(x) TimeCost __time_cost__(x)

const uint32_t SIZE = 1000 * 1000;

uint32_t microsecond()
{
    system_clock::duration d = system_clock::now().time_since_epoch();
    return duration_cast<microseconds>(d).count();
}

uint32_t now()
{
    system_clock::duration d = system_clock::now().time_since_epoch();
    return duration_cast<seconds>(d).count();
}

class TimeCost
{
public:
    TimeCost(const std::string& func) : func_name(func) {}
    ~TimeCost()
    {
        // std::chrono::system_clock::time_point end  = std::chrono::system_clock::now();
        auto end  = std::chrono::system_clock::now();
        auto dura = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        // auto dura = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        std::cout << func_name.c_str() << "  cost: " << dura << " ms\n";
    }

private:
    std::chrono::system_clock::time_point start = chrono::system_clock::now();
    std::string                           func_name;
};

using TYPE = uint64_t;

struct SortEntry
{
    TYPE entry_id = 0;
    // std::string entry_name = {};
    TYPE timestamp = {};
    TYPE score     = {};

    SortEntry(const TYPE& id, const TYPE& t, const TYPE& s) : entry_id(id), timestamp(t), score(s) {}
};

using SortEntryPtr = std::shared_ptr<SortEntry>;

inline bool operator<(const SortEntryPtr& lhs, const SortEntryPtr& rhs)
{
    if (!lhs || !rhs)
        return false;

    if (lhs->score != rhs->score)
        return lhs->score < rhs->score;

    if (lhs->timestamp != rhs->timestamp)
        return lhs->timestamp < rhs->timestamp;

    return lhs->entry_id < rhs->entry_id;
}

namespace ns_toplist
{
    template <typename Data, uint32_t N>
    class TopList
    {
    public:
        using ItemDataPtr = typename std::shared_ptr<Data>;
        using DataVector  = typename std::vector<ItemDataPtr>;
        using VecIterator = typename DataVector::iterator;
    };

    void main()
    {
        std::cout << "hello, toplist\n";
        return;
    }
}  // namespace ns_toplist

namespace ns_vector_sort
{
    using EntryVec = std::vector<SortEntryPtr>;
    void sort_test(const TYPE& size)
    {
        EntryVec vec;
        vec.reserve(size);
        srand(now());
        for (TYPE idx = 0; idx < size; idx++)
        {
            vec.emplace_back(std::make_shared<SortEntry>(idx, microsecond(), rand()));
        }
        std::sort(vec.begin(), vec.end());
    }

    void test_help(uint32_t times)
    {
        for (uint32_t t = 0; t < times; t++)
        {
            sort_test(1000 * 1000);
        }
    }

    void main()
    {
        TIME_COST("ns_vector_sort::main");
        test_help(10);
    }
}  // namespace ns_vector_sort

namespace ns_set_sort
{
    using EntrySet = std::set<SortEntryPtr>;
    void sort_test(const TYPE& size)
    {
        EntrySet eset = {};
        srand(now());
        for (TYPE idx = 0; idx < size; idx++)
        {
            eset.emplace(std::make_shared<SortEntry>(idx, microsecond(), rand()));
        }
    }

    void test_help(uint32_t times)
    {
        for (uint32_t t = 0; t < times; t++)
        {
            sort_test(1000 * 1000);
        }
    }

    void main()
    {
        TIME_COST("ns_set_sort::main");
        test_help(10);
    }
}  // namespace ns_set_sort

namespace ns_tmp
{
    template <typename T>
    class TopList
    {
    public:
        uint32_t Size()
        {
            return vec.size();
        }

        void Clear()
        {
            vec.clear();
        }

        std::vector<T> vec = {};
    };

    template <typename T>
    class DerivedTopList : public TopList<T>
    {
    public:
        DerivedTopList() : TopList<T>() {}

        void push_item(const T& val)
        {
            // TopList<T>::vec.push_back(val);
            this->vec.push_back(val);
            std::cout << "cur: " << this->Size() << std::endl;
        }

        void forEach()
        {
            std::cout << "forEach: ======================\n";
            for (size_t i = 0; i < this->vec.size(); i++)
            {
                std::cout << i << "-" << this->vec[i] << ",";
            }
            std::cout << std::endl;
        }
    };

    void main()
    {
        DerivedTopList<uint32_t> toplist;
        for (uint32_t idx = 0; idx < 10; idx++)
            toplist.push_item(idx * 10);
        toplist.forEach();
        std::cout << "size: " << toplist.Size() << std::endl;
    }
}  // namespace ns_tmp

namespace ns_top_sort
{
    struct SortEntrySort
    {
        uint32_t id    = 0;
        uint32_t score = 0;

        bool operator==(const SortEntrySort& other)
        {
            return this->id == other.id;
        }
        /* 当存在全局函数
            bool operator<(const SortEntrySort& lhs, const SortEntrySort& rhs)
            的时候，成员函数被屏蔽了
        bool operator<(const SortEntrySort& other) const
        {
            if (this->score == other.score)
                return this->id > other.id;
            return this->score < other.score;
        }
        */
        /*  以下两个比较函数无法调用
        bool operator<(const SortEntrySort* other) const
        {
            if (!other)
                return true;
            if (this->score == other->score)
                return this->id > other->id;
            return this->score < other->score;
        }

        bool operator<(const std::shared_ptr<SortEntrySort>& other) const
        {
            if (!other)
                return true;
            if (this->score == other->score)
                return this->id > other->id;
            return this->score < other->score;
        }*/
    };
    inline bool operator<(const SortEntrySort& lhs, const SortEntrySort& rhs)
    {
        if (lhs.score == rhs.score)
            return lhs.id > rhs.id;
        return lhs.score < rhs.score;
    }
    /*
    inline bool operator<(const SortEntrySort* lhs, const SortEntrySort* rhs)
    {
        if (!rhs || !lhs)
            return true;
        if (lhs->score == rhs->score)
            return lhs->id > rhs->id;
        return lhs->score < rhs->score;
    }
    */
    inline bool operator<(const std::shared_ptr<SortEntrySort>& lhs, const std::shared_ptr<SortEntrySort>& rhs)
    {
        if (!rhs || !lhs)
            return true;
        if (lhs->score == rhs->score)
            return lhs->id > rhs->id;
        return lhs->score < rhs->score;
    }

    void test1()
    {
        std::vector<SortEntrySort> entry_vec = {};

        srand(now());
        for (uint32_t idx = 0; idx < 5; idx++)
        {
            SortEntrySort entry;
            entry.id    = idx;
            entry.score = rand() % 1000;
            entry_vec.emplace_back(entry);
        }
        std::sort(entry_vec.begin(), entry_vec.end());
        return;
    }

    void test2()
    {
        std::vector<SortEntrySort*> entry_point_vec = {};

        srand(now());
        for (uint32_t idx = 0; idx < 5; idx++)
        {
            SortEntrySort* entry = new SortEntrySort;
            entry->id            = idx;
            entry->score         = rand() % 1000;
            entry_point_vec.emplace_back(entry);
        }
        std::sort(entry_point_vec.begin(), entry_point_vec.end());
        return;
    }

    void test3()
    {
        std::vector<std::shared_ptr<SortEntrySort>> entry_shared_vec = {};

        srand(now());
        for (uint32_t idx = 0; idx < 5; idx++)
        {
            auto entry   = std::make_shared<SortEntrySort>();
            entry->id    = idx;
            entry->score = rand() % 1000;
            entry_shared_vec.emplace_back(entry);
        }
        std::sort(entry_shared_vec.begin(), entry_shared_vec.end());
        return;
    }

    void main()
    {
        test1();
        test2();
        test3();
        std::cout << "Hello, main\n";
    }
}  // namespace ns_top_sort

int main()
{
    // ns_toplist::main();
    // ns_vector_sort::main();

    // ns_set_sort::main();
    // ns_tmp::main();
    ns_top_sort::main();
    return 0;
}  // namespace )
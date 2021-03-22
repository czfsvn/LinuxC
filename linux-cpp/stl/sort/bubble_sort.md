### 冒泡排序

##### 基本思想
    简单的交换排序，从头开始扫描排序的数据元素序列，依次比较相邻两个元素大小，如果逆序，则交换他们的位置。逐步将待排序变成有序序列。
    
##### 具体过程
    1) 对待排序数据元素进行第一躺扫描，依次比较相邻两个数据元素的大小。如果前面元素大于后面元素，则交换。这样具有较大关键字的数据元素将不断后移动。最后将移动到最后的位置上
    2) 第二趟扫描仅需对前n-1个元素进行，重复以上操作，使具有次大的元素移动到第n-1的位置
    3) 依次类推，直到某一趟扫描无元素发生位置交换，则可结束排序

##### 源码展示
```
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
```

##### 算法分析
    1) 空间复杂度： O(1)
    2) 时间复杂度:  O(n2)
        最好的情况：只需要进行一趟排序，且元素不需要移动，比较次数为 n-1
        最坏的情况：原始使逆序，需要进行n-1趟排序，n-1次比较

##### 稳定性
    稳定。比较相邻两个元素的过程，只是判断是否逆序才会向后移动。

---

### 快速排序
##### 基本思想
    从待排数据元素序列中选取一个数据元素作为基准pivot, 通过一趟扫描将待排序列分成两个部分。
    其中一部分数据元素的关键字都小于或等于基准数据元素的关键字。
    而另一部分数据元素的关键字都大于或等于基准数据元素关键字。
    通过对各部分的不断划分，直到整个序列按关键字有序排列为止。
##### 具体过程
    1) 选取待排序列中的第一个数据元素为基准，将其复制到r[0]中，并令该位置为空；设置两个指针low和high，分别指向第一个数据元素和最后一个数据元素，即初始状态时r[low]为空。 
    2) 从后向前扫描，若 r[high] 的关键字大于或等于基准关键字， 则high向前移动一个位置，直到 r[high] 的关键字小于基准关键字时，将 r[high]与r[low]交换。
    3) 从前向后扫描，若r[low]的关键字小于或等于基准关键字，则low向后移动一个位置，直到r[low]的关键字大于基准关键字时，将r[low] 与r[high]交换。 
    4) 重复 步骤2和3，直至low＝high时，令r[low]＝r[0]，以r[low]为基准将待排序列划分为前后两部分，第一次划分完成。 
    5) 按照以上方法， 对各部分不断划分，直到各部分只有一个数据元素时， 整个序列排序完成。

##### 源码展示
```
uint32_t patition(std::vector<uint32_t>& vec, uint32_t low, uint32_t high)
{
    std::cout << "low: " << low << ", high: " << high << std::endl;
    const uint32_t pivot = vec[low];
    while (low < high)
    {
        while (low < high && vec[high] >= pivot)
            high--;

        vec[low] = vec[high];

        while (low < high && vec[low] <= pivot)
            low++;

        vec[high] = vec[low];
    }
    vec[low] = pivot;
    return low;
}

void quick_sort(std::vector<uint32_t>& vec, uint32_t from, uint32_t to)
{
    if (from >= to || vec.empty())
        return;

    uint32_t right = patition(vec, from, to);
    quick_sort(vec, from, right - 1);
    quick_sort(vec, right + 1, to);
}
```

##### 算法分析
    1) 空间复杂度
        由于才有递归的方式，所以需要使用递归工作栈来存放指针和参数
        最好的情况使 O(log2N), 最坏的情况使 O(n)
    2) 时间复杂度
        最好的情况：每趟排序都能将排序序列分成两个长度相同的子序列，复杂度：O(log2n)
        最坏情况：数据元素已经全部有序。复杂度：O(n2);
        一般情况下： O(log2N)
##### 稳定性
    不稳定。
---
### 直接插入排序
##### 基本思想
    基本思想是：将待排序数据元素插入到已排号序的有序表中，从而得到一个新的有序表。
##### 具体过程
    对于一个具有N个数据元素的序列，具体过程是：
    1) 将第一个数据元素看作一个已排好序的有序表
    2) 将 第 i（ 2 ≤ i ≤ n）个数据元素的关键字Ki依次与其前面数据元素的关键字Ki－1、Ki－2、…、K1进行比较，
    将所有关键字大于Ki的数据元素依次向后向后移动一个位置，
    直到某个数据元素的关键字Kj小于或者等于Ki时，
    将第i个数据元素插入到关键字为Kj的数据元素后面，即完成第i个数据元素的插入。
    3) 经过n－1次插入操作后，所有数据元素构成一个按关键字值大小排列的有序序列。
##### 源码展示
```
// 基本方法和实现
void insert_sort(std::vector<uint32_t> &vec)
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
// 高级版本1实现方式
template <typename T, typename Comp>
void insert_sort(std::vector<T> &vec, Comp &&cmp)
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
// 高级版本2实现方式
template <typename Iterator, typename Comp>
void insert_sort(Iterator begin, Iterator end, Comp &&cmp)
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
```
##### 算法分析
    1) 空间复杂度
        只借助了一个辅助单元，空间复杂度是 O(1)
    2) 时间复杂度
        O(n2)
##### 稳定性
    稳定。出现相同的关键字，位置不变。



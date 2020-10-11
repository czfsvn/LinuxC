## 关于 std::sort 不稳定的研究

### 概述
+ std::sort 是不稳定的排序。在使用的时候，需要严格遵守排序准则，否则会导致异常产生。
---
### 问题描述
- 外网玩家发来bug：某天晚上，一个排行榜发生第一名A和第二名B，在数据为发生变化的情况下，会不停的交替更换。
- 经查，该排行榜的排序规则functor 是
    ```
    bool compare_function(const std::shared_ptr<Data> lhs, const std::shared_ptr<Data> rhs)
    {
        if (lhs->detail->socre != rhs->detail->socre)
            return lhs->detail->socre > rhs->detail->socre;
        return lhs->detail->update_time < rhs->detail->update_time;
    }

    std::vector<std::shared_ptr<Data>> vec;
    std::sort(vec.begin(), vec.end(),compare_function);
    ```
    也就是说，比较的时候有两个比较顺序，先比较score, 而后比较 update_time(当时时间戳：秒)。
    凑巧的是，外网日志显示，第一名和第二名的不仅 score 相等，而且 update_time 也相等。

    需要比较的两个key都相等。这个地方会不会是导致问题的根源？
---
### 排序准则
    ---摘自《C++标准库》7.7 
    - 1、必须是非对称的
        对operator < 而言，如果 x < y 为true，那么 y < x 为false;        
        对判断式(predicate) op() 而言, 如果op(x, y) 为true， 那么op(y, x) 为false。

    - 2、必须是可传递性的
        对operator < 而言，如果 x < y 为true 且 y < z 为true， 那么 x < z 为true
        对判断式(predicate) op() 而言, 如果op(x, y) 为true 且 op(y, z) 为true， 那么 op(x, z) 为true

    - 3、必须自反的
        对operator < 而言，如果 x < x 永远为false;  
        对判断式(predicate) op() 而言, 如果op(x, x) 永远为false。

    - 4、必须有等效传递性
        如果 a 等于 b 且 b 等于 c，那么a必然等于 c
        对操作符 <, 若 !(a < b) && !(b < a)为true且 !(b < c) && !(c < b) 为true，那么!(a < c) && !(a < b) 也为true
        对于判断是 op(), 如果op(a,b)、op(b,a)、op(b,c)和op(c,b) 都为false， 那么op(a,c)、op(c,a)也为false

    这就意味着：必须区分less 和equal， 一个像operator <= 这样的准则并不满足条件

根据排序准则，发现 compare_function 确实有问题
---
### 验证问题
下面写一个类似的代码来验证问题。
```
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

int main()
{
    std::vector<Entry> vec;

    for (uint32_t idx = 0; idx < 20; idx++)
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
    return 0;
}

```
    运行程序
```
[cn@build] ./sort/sort
print_ele : (0-1000)(1-1000)(2-1000)(3-1000)(4-1000)(5-111111)(6-111111)(7-1000)(8-1000)(9-1000)(10-1000)(11-1000)(12-1000)(13-1000)(14-1000)(15-1000)(16-1000)(17-1000)(18-1000)(19-1000)
print_ele : (5-111111)(6-111111)(10-1000)(19-1000)(18-1000)(17-1000)(16-1000)(15-1000)(14-1000)(13-1000)(12-1000)(11-1000)(0-1000)(9-1000)(8-1000)(7-1000)(4-1000)(3-1000)(2-1000)(1-1000)
print_ele : (6-111111)(5-111111)(1-1000)(2-1000)(3-1000)(4-1000)(7-1000)(8-1000)(9-1000)(0-1000)(11-1000)(12-1000)(13-1000)(14-1000)(15-1000)(16-1000)(17-1000)(18-1000)(19-1000)(10-1000)
```

    描述：
        - 第一行是原始打印
        - 第二行是第一次排序后的打印
        - 第三行是第二次排序后的打印
    发现两次排序打印的结果居然不一样, 且相同数值score的元素位置大相径庭。Why？
---
### std::sort 排序原理  
    具体排序规则详见-《STL源码剖析》 6.7.9
    对于STL的sort算法，数据量大的时候，采用QuickSort, 分段递归排序。一旦分段后的数据量小于某个门槛，为避免QuickSort 的递归调用带来过大的额外负荷(overhead)，就改用InsertionSort。 如果递归层次过深。还会改用HeapSort。
    
    - QuickSort 快速排序 -- 不稳定
        + 算法描述，假设S代表将被处理的序列
            1）如果S的元素个数为0或1，结束
            2）取S中的任何一个元素，作为枢轴(pivot)v
            3）将S分割为L,R两段，使L内的每个元素均小于或等于v；使R段内的每一个元素大于或等于v
            4）对L、R递归执行QuickSort

    - InsertionSort 插入排序-- 稳定

    - HeapSort 堆排序-- 不稳定

    稳定：      如果a原本在b前面，而a=b，排序之后a仍然在b的前面
    不稳定：    如果a原本在b的前面，而a=b，排序之后a可能会出现在b的后面
---
### 解决问题
只需要优化排序准则即可：
```
    bool cmp(const Entry& lhs, const Entry& rhs)
    {
        if (lhs.score != rhs.score)
            return lhs.score > rhs.score;
        return lhs.user_id < rhs.user_id;
    }
```
---
### 总结
    由于std::sort 内部调用了 QuickSort 和 HeapSort 造成排序是不稳定的，所以导致对相同元素序列调用 std::sort 导致排行榜会发生变化。因此在使用排序的时候，必须遵守排序准则，避免产生意外的bug。

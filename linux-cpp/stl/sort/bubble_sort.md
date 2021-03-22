### ð������

##### ����˼��
    �򵥵Ľ������򣬴�ͷ��ʼɨ�����������Ԫ�����У����αȽ���������Ԫ�ش�С����������򽻻����ǵ�λ�á��𲽽����������������С�
    
##### �������
    1) �Դ���������Ԫ�ؽ��е�һ��ɨ�裬���αȽ�������������Ԫ�صĴ�С�����ǰ��Ԫ�ش��ں���Ԫ�أ��򽻻����������нϴ�ؼ��ֵ�����Ԫ�ؽ����Ϻ��ƶ�������ƶ�������λ����
    2) �ڶ���ɨ������ǰn-1��Ԫ�ؽ��У��ظ����ϲ�����ʹ���дδ��Ԫ���ƶ�����n-1��λ��
    3) �������ƣ�ֱ��ĳһ��ɨ����Ԫ�ط���λ�ý�������ɽ�������

##### Դ��չʾ
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

##### �㷨����
    1) �ռ临�Ӷȣ� O(1)
    2) ʱ�临�Ӷ�:  O(n2)
        ��õ������ֻ��Ҫ����һ��������Ԫ�ز���Ҫ�ƶ����Ƚϴ���Ϊ n-1
        ��������ԭʼʹ������Ҫ����n-1������n-1�αȽ�

##### �ȶ���
    �ȶ����Ƚ���������Ԫ�صĹ��̣�ֻ���ж��Ƿ�����Ż�����ƶ���

---

### ��������
##### ����˼��
    �Ӵ�������Ԫ��������ѡȡһ������Ԫ����Ϊ��׼pivot, ͨ��һ��ɨ�轫�������зֳ��������֡�
    ����һ��������Ԫ�صĹؼ��ֶ�С�ڻ���ڻ�׼����Ԫ�صĹؼ��֡�
    ����һ��������Ԫ�صĹؼ��ֶ����ڻ���ڻ�׼����Ԫ�عؼ��֡�
    ͨ���Ը����ֵĲ��ϻ��֣�ֱ���������а��ؼ�����������Ϊֹ��
##### �������
    1) ѡȡ���������еĵ�һ������Ԫ��Ϊ��׼�����临�Ƶ�r[0]�У������λ��Ϊ�գ���������ָ��low��high���ֱ�ָ���һ������Ԫ�غ����һ������Ԫ�أ�����ʼ״̬ʱr[low]Ϊ�ա� 
    2) �Ӻ���ǰɨ�裬�� r[high] �Ĺؼ��ִ��ڻ���ڻ�׼�ؼ��֣� ��high��ǰ�ƶ�һ��λ�ã�ֱ�� r[high] �Ĺؼ���С�ڻ�׼�ؼ���ʱ���� r[high]��r[low]������
    3) ��ǰ���ɨ�裬��r[low]�Ĺؼ���С�ڻ���ڻ�׼�ؼ��֣���low����ƶ�һ��λ�ã�ֱ��r[low]�Ĺؼ��ִ��ڻ�׼�ؼ���ʱ����r[low] ��r[high]������ 
    4) �ظ� ����2��3��ֱ��low��highʱ����r[low]��r[0]����r[low]Ϊ��׼���������л���Ϊǰ�������֣���һ�λ�����ɡ� 
    5) �������Ϸ����� �Ը����ֲ��ϻ��֣�ֱ��������ֻ��һ������Ԫ��ʱ�� ��������������ɡ�

##### Դ��չʾ
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

##### �㷨����
    1) �ռ临�Ӷ�
        ���ڲ��еݹ�ķ�ʽ��������Ҫʹ�õݹ鹤��ջ�����ָ��Ͳ���
        ��õ����ʹ O(log2N), ������ʹ O(n)
    2) ʱ�临�Ӷ�
        ��õ������ÿ�������ܽ��������зֳ�����������ͬ�������У����Ӷȣ�O(log2n)
        ����������Ԫ���Ѿ�ȫ�����򡣸��Ӷȣ�O(n2);
        һ������£� O(log2N)
##### �ȶ���
    ���ȶ���
---
### ֱ�Ӳ�������
##### ����˼��
    ����˼���ǣ�������������Ԫ�ز��뵽���ź����������У��Ӷ��õ�һ���µ������
##### �������
    ����һ������N������Ԫ�ص����У���������ǣ�
    1) ����һ������Ԫ�ؿ���һ�����ź���������
    2) �� �� i�� 2 �� i �� n��������Ԫ�صĹؼ���Ki��������ǰ������Ԫ�صĹؼ���Ki��1��Ki��2������K1���бȽϣ�
    �����йؼ��ִ���Ki������Ԫ�������������ƶ�һ��λ�ã�
    ֱ��ĳ������Ԫ�صĹؼ���KjС�ڻ��ߵ���Kiʱ��
    ����i������Ԫ�ز��뵽�ؼ���ΪKj������Ԫ�غ��棬����ɵ�i������Ԫ�صĲ��롣
    3) ����n��1�β����������������Ԫ�ع���һ�����ؼ���ֵ��С���е��������С�
##### Դ��չʾ
```
// ����������ʵ��
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
// �߼��汾1ʵ�ַ�ʽ
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
// �߼��汾2ʵ�ַ�ʽ
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
##### �㷨����
    1) �ռ临�Ӷ�
        ֻ������һ��������Ԫ���ռ临�Ӷ��� O(1)
    2) ʱ�临�Ӷ�
        O(n2)
##### �ȶ���
    �ȶ���������ͬ�Ĺؼ��֣�λ�ò��䡣



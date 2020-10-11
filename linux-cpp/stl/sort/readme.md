## ���� std::sort ���ȶ����о�

### ����
+ std::sort �ǲ��ȶ���������ʹ�õ�ʱ����Ҫ�ϸ���������׼�򣬷���ᵼ���쳣������
---
### ��������
- ������ҷ���bug��ĳ�����ϣ�һ�����а�����һ��A�͵ڶ���B��������Ϊ�����仯������£��᲻ͣ�Ľ��������
- ���飬�����а���������functor ��
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
    Ҳ����˵���Ƚϵ�ʱ���������Ƚ�˳���ȱȽ�score, ����Ƚ� update_time(��ʱʱ�������)��
    ���ɵ��ǣ�������־��ʾ����һ���͵ڶ����Ĳ��� score ��ȣ����� update_time Ҳ��ȡ�

    ��Ҫ�Ƚϵ�����key����ȡ�����ط��᲻���ǵ�������ĸ�Դ��
---
### ����׼��
    ---ժ�ԡ�C++��׼�⡷7.7 
    - 1�������ǷǶԳƵ�
        ��operator < ���ԣ���� x < y Ϊtrue����ô y < x Ϊfalse;        
        ���ж�ʽ(predicate) op() ����, ���op(x, y) Ϊtrue�� ��ôop(y, x) Ϊfalse��

    - 2�������ǿɴ����Ե�
        ��operator < ���ԣ���� x < y Ϊtrue �� y < z Ϊtrue�� ��ô x < z Ϊtrue
        ���ж�ʽ(predicate) op() ����, ���op(x, y) Ϊtrue �� op(y, z) Ϊtrue�� ��ô op(x, z) Ϊtrue

    - 3�������Է���
        ��operator < ���ԣ���� x < x ��ԶΪfalse;  
        ���ж�ʽ(predicate) op() ����, ���op(x, x) ��ԶΪfalse��

    - 4�������е�Ч������
        ��� a ���� b �� b ���� c����ôa��Ȼ���� c
        �Բ����� <, �� !(a < b) && !(b < a)Ϊtrue�� !(b < c) && !(c < b) Ϊtrue����ô!(a < c) && !(a < b) ҲΪtrue
        �����ж��� op(), ���op(a,b)��op(b,a)��op(b,c)��op(c,b) ��Ϊfalse�� ��ôop(a,c)��op(c,a)ҲΪfalse

    �����ζ�ţ���������less ��equal�� һ����operator <= ������׼�򲢲���������

��������׼�򣬷��� compare_function ȷʵ������
---
### ��֤����
����дһ�����ƵĴ�������֤���⡣
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
    ���г���
```
[cn@build] ./sort/sort
print_ele : (0-1000)(1-1000)(2-1000)(3-1000)(4-1000)(5-111111)(6-111111)(7-1000)(8-1000)(9-1000)(10-1000)(11-1000)(12-1000)(13-1000)(14-1000)(15-1000)(16-1000)(17-1000)(18-1000)(19-1000)
print_ele : (5-111111)(6-111111)(10-1000)(19-1000)(18-1000)(17-1000)(16-1000)(15-1000)(14-1000)(13-1000)(12-1000)(11-1000)(0-1000)(9-1000)(8-1000)(7-1000)(4-1000)(3-1000)(2-1000)(1-1000)
print_ele : (6-111111)(5-111111)(1-1000)(2-1000)(3-1000)(4-1000)(7-1000)(8-1000)(9-1000)(0-1000)(11-1000)(12-1000)(13-1000)(14-1000)(15-1000)(16-1000)(17-1000)(18-1000)(19-1000)(10-1000)
```

    ������
        - ��һ����ԭʼ��ӡ
        - �ڶ����ǵ�һ�������Ĵ�ӡ
        - �������ǵڶ��������Ĵ�ӡ
    �������������ӡ�Ľ����Ȼ��һ��, ����ͬ��ֵscore��Ԫ��λ�ô��ྶͥ��Why��
---
### std::sort ����ԭ��  
    ��������������-��STLԴ�������� 6.7.9
    ����STL��sort�㷨�����������ʱ�򣬲���QuickSort, �ֶεݹ�����һ���ֶκ��������С��ĳ���ż���Ϊ����QuickSort �ĵݹ���ô�������Ķ��⸺��(overhead)���͸���InsertionSort�� ����ݹ��ι���������HeapSort��
    
    - QuickSort �������� -- ���ȶ�
        + �㷨����������S���������������
            1�����S��Ԫ�ظ���Ϊ0��1������
            2��ȡS�е��κ�һ��Ԫ�أ���Ϊ����(pivot)v
            3����S�ָ�ΪL,R���Σ�ʹL�ڵ�ÿ��Ԫ�ؾ�С�ڻ����v��ʹR���ڵ�ÿһ��Ԫ�ش��ڻ����v
            4����L��R�ݹ�ִ��QuickSort

    - InsertionSort ��������-- �ȶ�

    - HeapSort ������-- ���ȶ�

    �ȶ���      ���aԭ����bǰ�棬��a=b������֮��a��Ȼ��b��ǰ��
    ���ȶ���    ���aԭ����b��ǰ�棬��a=b������֮��a���ܻ������b�ĺ���
---
### �������
ֻ��Ҫ�Ż�����׼�򼴿ɣ�
```
    bool cmp(const Entry& lhs, const Entry& rhs)
    {
        if (lhs.score != rhs.score)
            return lhs.score > rhs.score;
        return lhs.user_id < rhs.user_id;
    }
```
---
### �ܽ�
    ����std::sort �ڲ������� QuickSort �� HeapSort ��������ǲ��ȶ��ģ����Ե��¶���ͬԪ�����е��� std::sort �������а�ᷢ���仯�������ʹ�������ʱ�򣬱�����������׼�򣬱�����������bug��

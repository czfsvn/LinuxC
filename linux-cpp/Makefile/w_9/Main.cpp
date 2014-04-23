#include <iostream>
#include <ext/hash_map>

struct Entry
{
    int Id;
    int level;

    Entry() : Id(0), level(0)
    {
    }
};

void Test()
{
    __gnu_cxx::hash_map<int, Entry*> m_hashMap;
    for (int idx = 0; idx < 10; ++idx)
    {
        Entry* e = new Entry;
        e->Id = idx;
        e->level = idx;
        m_hashMap[idx] = e;
    }

    for (__gnu_cxx::hash_map<int, Entry*>::iterator it = m_hashMap.begin();
            it != m_hashMap.end(); ++it)
    {
        delete it->second;
    }
}

void TestChar()
{
    char str[200] = {0};
    std::string test ="问阿金编辑";
    snprintf(str, 200, "中立区 “%s”");
}

int main()
{
    Test();
    return 0;
}


#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <cstdlib>

using namespace std;

struct TopData
{
    int    id;
    int    score;
    std::string name;

    void operator ()() const
    //void dump() const
    {
        std::cout << "info: " << name << "--"<< id << "---" << score << std::endl;
    }
};

bool comp(const TopData& lhs, const TopData& rhs)
{
    if (lhs.score < rhs.score)
        return true;
    return false;
}

namespace ns_iter
{
class IIterator;
class ITopList
{
public:
    virtual void addElement(const TopData& data)    = 0;
    //virtual void removeElement(int id)         = 0;
    virtual IIterator*  createIterator()            = 0;
    virtual bool isExists(const TopData& data)      = 0;
    virtual void Sort()                             = 0;
};

class TopListVec : public ITopList
{
    friend class IteratorVec;
public:
    TopListVec()       {}
    ~TopListVec()      {   data_vec_.clear();  }

    IIterator*  createIterator();//     {   return new IteratorVec(this);  }

    void addElement(const TopData& data) override
    {
        if (isExists(data))
            return;

        data_vec_.emplace_back(data);
        Sort();
    }

    bool isExists(const TopData& data)
    {
        for (auto& item : data_vec_)
        {
            if (item.id == data.id)
                return true;
        }
        return false;
    }

    void Sort()
    {
        std::sort(data_vec_.begin(), data_vec_.end(), comp);
    }

private:
    std::vector<TopData>    data_vec_;
};

class TopListList : public ITopList
{
    friend class IteratorList;
public:

    TopListList()   {}
    ~TopListList()  {   data_list_.clear(); }

    IIterator*  createIterator();//     {   return new IteratorList(this);  }

    void addElement(const TopData& data) override
    {
        if (isExists(data))
            return;

        data_list_.emplace_back(data);
        Sort();
    }

    bool isExists(const TopData& data)
    {
        for (auto& item : data_list_)
        {
            if (item.id == data.id)
                return true;
        }
        return false;
    }

    virtual void Sort()
    {
        data_list_.sort(comp);
    }

private:
    std::list<TopData>      data_list_;
};

class IIterator
{
    public:
        virtual bool    isFirst()       =   0;
        virtual bool    isLast()        =   0;
        virtual void    next()          =   0;
        virtual void    previous()      =   0;

        virtual bool    hasNext()       =   0;
        virtual bool    hasPrev()   =   0;

        virtual const TopData&  getCurItem()    =   0;
};

class IteratorVec : public IIterator
{
public:
    IteratorVec(TopListVec* top)//   : top_list_(top)   {} 
    {
        top_list_ = top;
    }
    ~IteratorVec()         {   top_list_ = nullptr;    }

    virtual bool    isFirst() { return cur_idx == 0;    }
    virtual bool    isLast()  { return cur_idx >= top_list_->data_vec_.size();  }
    virtual void    next()    { cur_idx ++; }
    virtual void    previous() { cur_idx--; }
    virtual bool    hasNext()   
    {
        return isLast() ? false : true;
    }

    virtual bool    hasPrev()   
    {
        return isFirst() ? false : true;
    }

    virtual const TopData&  getCurItem()    {   return top_list_->data_vec_[cur_idx]; }
    
private:
    TopListVec* top_list_{nullptr};
    int    cur_idx{0};

};

class IteratorList : public IIterator
{
public:
    IteratorList(TopListList* top)   : top_list_(top)   {    iter = top_list_->data_list_.begin();  } 
    ~IteratorList()         {   top_list_ = nullptr;    }

    virtual bool    isFirst() { return iter == top_list_->data_list_.begin();    }
    virtual bool    isLast()  { return iter == top_list_->data_list_.end();  }
    virtual void    next()    { iter++; }
    virtual void    previous() { iter--; }
    virtual bool    hasNext()   
    {
        return isLast() ? false : true;
    }

    virtual bool    hasPrev()   
    {
        return isFirst() ? false : true;
    }

    virtual const TopData&  getCurItem()    {   return *iter; }

private:
    TopListList* top_list_{nullptr};
    std::list<TopData>::iterator iter;  
    
};

IIterator*  TopListList::createIterator()       {   return new IteratorList(this);  }
IIterator*  TopListVec::createIterator()        {   return new IteratorVec(this);  }

    void test()
    {
        {
            ITopList* tList = new TopListList;
            IIterator* lIter = tList->createIterator();
            for (int idx = 0; idx < 10; idx++)
            {
                TopData data;//{idx, rand() % 100, std::u32string(idx)};
                data.id = idx;
                data.score = rand() % 100;
                data.name = std::to_string(idx);
                tList->addElement(data);
            }

            while (lIter->hasNext())
            {
                lIter->getCurItem()();
                lIter->next();
            }
        }
        {
            ITopList* tList = new TopListVec;
            IIterator* lIter = tList->createIterator();
            for (int idx = 0; idx < 10; idx++)
            {
                TopData data;//{idx, rand() % 100, std::u32string(idx)};
                data.id = idx;
                data.score = rand() % 100;
                data.name = std::to_string(idx);
                tList->addElement(data);
            }

            while (lIter->hasNext())
            {
                lIter->getCurItem()();
                lIter->next();
            }
        }
    }
}

namespace ns_template
{
    template<typename DATA, typename CONT, /*typename Iterator,*/ int size = 10>
        class TopList
        {
            public:
                TopList(std::string name) : name_(name)   {   }
                ~TopList()      {   }

                void    addElement(const DATA& data)
                {
                    cont.emplace_back(data);
                    Sort();
                }

                void    Sort();

            private:
                std::string     name_;
                CONT            cont;
        };

    typedef TopList<TopData, std::vector<TopData>, 10> VecTopList; 
    typedef TopList<TopData, std::list<TopData>, 10> ListTopList; 

    template<> void VecTopList::Sort()
    {
        std::sort(cont.begin(), cont.end(), comp);
    }

    template<> void ListTopList::Sort()
    {
        cont.sort(comp);
    }

    void test_vec()
    {
        VecTopList vList("test Vector");
        for (int idx = 0; idx < 10; idx++)
        {
            TopData data;
            data.id = idx;
            data.score = rand() % 100;
            data.name = std::to_string(idx);
            vList.addElement(data);
        }

    }
    void test_list()
    {
        ListTopList vList("test list");
        for (int idx = 0; idx < 10; idx++)
        {
            TopData data;
            data.id = idx;
            data.score = rand() % 100;
            data.name = std::to_string(idx);
            vList.addElement(data);
        }
    }
}

int main()
{
    ns_iter::test();
    std::cout << "Hello, world!\n";
    return 0;
}

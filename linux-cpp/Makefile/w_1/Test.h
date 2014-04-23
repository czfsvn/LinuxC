#ifndef __TEST_H__
#define __TEST_H__

#include <list>

class Orgin;

class Test
{
    public:
        Test();
        ~Test();

        void Init(int num);

        void Print();

    private:
        std::list<Orgin*> m_orginList;
};


#endif

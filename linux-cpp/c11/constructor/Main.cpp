#include <iostream>

using namespace std;

namespace construct
{
    class Base
    {
        public:
            Base(int val)   {}
    };

    class Tracer
    {
        public:
            Tracer() = default;
            Tracer(const Tracer& t) = default;
            Tracer& operator = (const Tracer& t) = default;
            Tracer(Tracer&& t) = default;
            Tracer& operator = (Tracer&& t) = default;
            ~Tracer() = default;

    };

    void Test_1()
    {
        Base* aBase = new Base(1);

        Base* bBase = aBase; 

        Tracer* t1 = new Tracer;
        Tracer* t2 = new Tracer(*t1);
        Tracer* t3 = new Tracer;
        Tracer* t4 = new Tracer;

    }
}

int main()
{
    return 0;
}

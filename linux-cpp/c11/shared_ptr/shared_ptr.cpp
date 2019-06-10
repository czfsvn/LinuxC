#include <iostream>
#include <memory>
#include <list>

using namespace std;

#define PRINT_LINE  std::cout << "func: " << __func__ << std::endl;

namespace ns_shared_ptr
{
    class MyString
    {
        public:
            MyString(const char* text) 
                : m_text(text)
            {
                std::cout << "MyString Construction " << std::endl;
            }

            ~MyString() 
            {
                std::cout << "MyString Destruction " << std::endl;
            }

            void output()
            {
                std::cout << "MyString::output: " << m_text << std::endl;
            }

        private:
            std::string m_text;
    };

    void Test_1()
    {
        shared_ptr<MyString> p1;
        shared_ptr<MyString> p2 = make_shared<MyString>("Hello, smartptr");

        p1 = p2;
        p1->output();
        p2->output();
    }

    void Test_2()
    {
        shared_ptr<std::string> ps(new string("Test"));
        std::cout << "ps_count: " << ps.use_count() << std::endl;

        shared_ptr<std::string> ps1(std::move(ps));
        std::cout << "ps_count: " << ps.use_count() << std::endl;
        std::cout << "ps1_count: " << ps1.use_count() << std::endl;

        std::cout << "ps_get(): " << ps.get() << std::endl;
        std::cout << "ps1_get(): " << ps1.get() << std::endl;
    }

    void Test_3()
    {
        PRINT_LINE;
        std::cout << "before ..." << std::endl;
        shared_ptr<std::string> ps(new std::string("Test_3"));
        shared_ptr<std::string> ps1;

        std::cout << "ps_count: " << ps.use_count() << std::endl;
        std::cout << "ps1_count: " << ps1.use_count() << std::endl;
        std::cout << "ps_get(): " << ps.get() << std::endl;
        std::cout << "ps1_get(): " << ps1.get() << std::endl;

        ps1 = ps;
        std::cout << "after ..." << std::endl;
        std::cout << "ps_count: " << ps.use_count() << std::endl;
        std::cout << "ps1_count: " << ps1.use_count() << std::endl;
        std::cout << "ps_get(): " << ps.get() << std::endl;
        std::cout << "ps1_get(): " << ps1.get() << std::endl;
    }

    void Test_4()
    {
        PRINT_LINE;
        std::cout << "before ..." << std::endl;
        shared_ptr<std::string> ps(new std::string("Test_4"));
        shared_ptr<std::string> ps1;

        std::cout << "ps_count: " << ps.use_count() << std::endl;
        std::cout << "ps1_count: " << ps1.use_count() << std::endl;
        std::cout << "ps_get(): " << ps.get() << std::endl;
        std::cout << "ps1_get(): " << ps1.get() << std::endl;

        ps1 = std::move(ps);
        std::cout << "after ..." << std::endl;
        std::cout << "ps_count: " << ps.use_count() << std::endl;
        std::cout << "ps1_count: " << ps1.use_count() << std::endl;
        std::cout << "ps_get(): " << ps.get() << std::endl;
        std::cout << "ps1_get(): " << ps1.get() << std::endl;
    }
}

namespace ns_unique_ptr
{

}

namespace ns_weak_ptr
{
    void Test_1()
    {
        shared_ptr<int> sptr = make_shared<int>(2018);

        weak_ptr<int> np;
        weak_ptr<int> p1(sptr);
        weak_ptr<int> p2(p1);

        std::cout << "sptr.use_count(): " << sptr.use_count() << std::endl;
        std::cout << "np.use_count(): " << np.use_count() << std::endl;
        std::cout << "p1.use_count(): " << p1.use_count() << std::endl;
        std::cout << "p2.use_count(): " << p2.use_count() << std::endl;

        // 
        shared_ptr<int> sp1 = make_shared<int>(127);
        weak_ptr<int> p4;
        weak_ptr<int> p5;

        p4 = sp1;
        p5 = p4;

        std::cout << "sp1.use_count(): " << sp1.use_count() << std::endl;
        std::cout << "p4.use_count(): " << p4.use_count() << std::endl;
        std::cout << "p5.use_count(): " << p5.use_count() << std::endl;
    }

    void Test_2()
    {
        shared_ptr<int> sptr = make_shared<int>(127);
        weak_ptr<int> p4;
        weak_ptr<int> p5;
    }

}

namespace ns_shared_weak
{
    class Monitor
    {
        public:
            void draw()
            {
                std::cout << "[Monitor] draw\n"; 
            }
    };

    class MainBoard
    {
        public:
            void setMonitor(weak_ptr<Monitor> m)
            {
                monitor = m;
            }

            void reportEvent(const char* msg)
            {
                std::cout << "[MainBoard] reportEvent: " << msg << std::endl;
            }

            void display()
            {
                if (shared_ptr<Monitor> sm = monitor.lock())
                    sm->draw();
                else
                    std::cout << "[MainBoard] display can't lock monitor\n";
            }
        private:
            weak_ptr<Monitor>   monitor;
    };

    class KeyBoard
    {
        public:
            void setMainBoard(weak_ptr<MainBoard> board)
            {
                main_board = board;
            }

            void run()
            {
                if (shared_ptr<MainBoard> smb = main_board.lock())
                    smb->reportEvent("[KeyBoard] run Keyboard Event");
            }

        private:
            weak_ptr<MainBoard> main_board;
    };

    class Mouse
    {
        public:
            void setMainBoard(weak_ptr<MainBoard> board)
            {
                main_board = board;
            }
            void run()
            {
                if (shared_ptr<MainBoard> smb = main_board.lock())
                    smb->reportEvent("[Mouse] run mouse Event");
            }
        private:
            weak_ptr<MainBoard> main_board;
    };

    class DesktopPC
    {
        public:
            void run()
            {
                if (shared_ptr<KeyBoard> kb = keyboard.lock())
                    kb->run();

                if (shared_ptr<Mouse> mou = mouse.lock())
                    mou->run();

                if (shared_ptr<MainBoard> mb = main_board.lock())
                    mb->display();
            }

            void setMainBoard(weak_ptr<MainBoard> mb)
            {
                main_board = mb;
            }

            void setKeyBoard(weak_ptr<KeyBoard> kb)
            {
                keyboard = kb;
            }

            void setMouse(weak_ptr<Mouse> mou)
            {
                mouse = mou;
            }

            void setMonitor(weak_ptr<Monitor> mon)
            {
                monitor = mon;
            }
        private:
            weak_ptr<MainBoard>     main_board;
            weak_ptr<Monitor>       monitor;
            weak_ptr<KeyBoard>      keyboard;
            weak_ptr<Mouse>         mouse;
    };

    void Test_1()
    {
        DesktopPC pc;

        shared_ptr<MainBoard>   mb  = make_shared<MainBoard>();
        shared_ptr<Monitor>     mon = make_shared<Monitor>();
        shared_ptr<KeyBoard>    kb  = make_shared<KeyBoard>();
        shared_ptr<Mouse>       mou = make_shared<Mouse>();

        mb->setMonitor(mon);
        mou->setMainBoard(mb);
        kb->setMainBoard(mb);

        pc.setMainBoard(mb);
        pc.setMonitor(mon);
        pc.setKeyBoard(kb);
        pc.setMouse(mou);

        std::cout << "main_count: " << mb.use_count() << std::endl;
        std::cout << "monitor_count: " << mon.use_count() << std::endl;
        std::cout << "keyboard_count: " << kb.use_count() << std::endl;
        std::cout << "mouse_count: " << mou.use_count() << std::endl;

        pc.run();
        mon.reset();
        std::cout << "monitor has been destroyed!\n";
        pc.run();
    }


    class ClassB;
    class ClassA
    {
        public:
            ClassA()    {   PRINT_LINE; }
            ~ClassA()    {   PRINT_LINE; }
            
            weak_ptr<ClassB> pb;            // 弱引用能够被析构
            //shared_ptr<ClassB> pb;        // 循环引用,不能被析构
    };

    class ClassB
    {
        public:
            ClassB()    {   PRINT_LINE; }
            ~ClassB()    {   PRINT_LINE; }
            
            weak_ptr<ClassA> pa;
            //shared_ptr<ClassA> pa;
    };

    void Test_2()
    {
        shared_ptr<ClassA> spa = make_shared<ClassA>();
        shared_ptr<ClassB> spb = make_shared<ClassB>();
        spa->pb = spb;
        spb->pa = spa;
    }
}

namespace ns_shared_from_this
{
    struct Base// : std::enable_shared_from_this<Base>
    {
    };

    struct BaseShared : std::enable_shared_from_this<BaseShared>
    {
    };

    typedef std::shared_ptr<BaseShared> BasePtr;
    class BaseManager
    {
        public:
            void Reg(Base* base)    
            {
                if (!base)  return;

                base_vec_.emplace_back(base);
            }

            void Reg(BasePtr base)
            {
                if (!base)  return;
                base_ptr_vec_.emplace_back(base);
            }

            static BaseManager& Inst()
            {
                static BaseManager inst_;
                return inst_;
            }
        private:
            std::list<Base*>    base_vec_;
            std::list<BasePtr>  base_ptr_vec_;
    };
    struct Derived : Base
    {
        Derived()
        {
            BaseManager::Inst().Reg(this);
        }
    };

    struct DerivedShared : BaseShared
    {
        DerivedShared()
        {
            //BaseManager::Inst().Reg(shared_from_this());
        }
    };

    void test()
    {
        DerivedShared   ptr;
        Derived         de;

        return;
    }
}

int main()
{
    //ns_shared_ptr::Test_3();
    //ns_shared_ptr::Test_4();
    //ns_weak_ptr::Test_1();
    //ns_shared_weak::Test_1();
    //ns_shared_weak::Test_2();
    ns_shared_from_this::test();
    return 0;
}

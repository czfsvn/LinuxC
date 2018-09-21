#include <iostream>
#include <list>
#include "Thread.h"

using namespace std;

namespace test_1
{
    // from: https://www.jianshu.com/p/ec970bdcad81
    class Command
    {
    public:
        virtual void execute() = 0;
    };

    class Reciever
    {
    public:
        void    action()    {
            std::cout << "Hello, test_1::Reciever::action\n";
        }
    };

    class ConcreteCommand : public Command
    {
    public:
        ConcreteCommand(Reciever*   rcv)
        {
            reciever_ = rcv;
        }

        virtual void execute()
        {
            reciever_->action();
        }
    private:
        Reciever*       reciever_{nullptr};
        std::string     state_;
    };

    class Invoker
    {
    public:
        void    setCommand(Command* cmd)
        {
            this->command_ = cmd;
        }

        void    runCommand()
        {
            if (command_){
                command_->execute();
            }
        }
    private:
        Command*    command_;
    };

    void    assemble()
    {
        Reciever*   rcv = new Reciever();
        Command*    cmd = new ConcreteCommand(rcv);
        Invoker*    inv = new Invoker();
        inv->setCommand(cmd);
        inv->runCommand();
    }
}

namespace test_2
{
    class MainBoardApi
    {
        public:
            virtual void open() = 0;
            virtual void reset() = 0;
            virtual void close() = 0;
    };

    class GigaMainBoard : public MainBoardApi
    {
        public:
            virtual void open() 
            {
                std::cout << "Giga Main Borad is starting, please wait...\n";
                std::cout << "connecting power...\n";
                std::cout << "check device...\n";
                std::cout << "loading system...\n";
                std::cout << "machine is ready ...\n";
                std::cout << "machine is opened, you can use now\n\n";
            }

            virtual void reset()
            {
                std::cout << "Giga Main Borad is restarting, please wait...\n";
                std::cout << "machine is opened, you can use now\n\n";
            }
            virtual void close()
            {
                std::cout << "Giga Main Borad is shutting down, please wait...\n";
                std::cout << "machine is powered off!\n\n";
            }
    };

    class Command
    {
        public:
            virtual void execute() = 0;
    };

    class OpenCommand : public Command
    {
        public:
            OpenCommand(MainBoardApi* mb) : mainBorad(mb)   {   }
            virtual void execute()
            {
                if (mainBorad)
                    mainBorad->open();
            }
        private:
            MainBoardApi*  mainBorad;
    };
    class ResetCommand : public Command
    {
        public:
            ResetCommand(MainBoardApi* mb) : mainBorad(mb)   {   }
            virtual void execute()
            {
                if (mainBorad)
                    mainBorad->reset();
            }
        private:
            MainBoardApi*  mainBorad;
    };
    class CloseCommand : public Command
    {
        public:
            CloseCommand(MainBoardApi* mb) : mainBorad(mb)   {   }
            virtual void execute()
            {
                if (mainBorad)
                    mainBorad->close();
            }
        private:
            MainBoardApi*  mainBorad;
    };
    
    class Box
    {
        public:
            void    setOpenCommand(Command* cmd)    {
                openCommand_ = cmd;
            }
            void    openBtnPressed()            {
                if (openCommand_)
                    openCommand_->execute();
            }

            void    setResetCommand(Command* cmd)   {
                resetCommand_ = cmd;
            }
            void    resetBtnPressed()           {
                if (resetCommand_)
                    resetCommand_->execute();
            }

            void    setCloseCommand(Command* cmd)   {
                closeCommand_ = cmd;
            }
            void    closeBtnPressed()           {
                if (closeCommand_)
                    closeCommand_->execute();
            }

        private:
            Command*    openCommand_;
            Command*    resetCommand_;
            Command*    closeCommand_;
    };

    void    assemble()
    {
        MainBoardApi*   mb          = new GigaMainBoard();
        Command*        openCmd     = new OpenCommand(mb);
        Command*        resetCmd    = new ResetCommand(mb);
        Command*        closeCmd    = new CloseCommand(mb);

        Box*            box = new Box();
        box->setOpenCommand(openCmd);
        box->setResetCommand(resetCmd);
        box->setCloseCommand(closeCmd);

        box->openBtnPressed();
        box->resetBtnPressed();
        box->closeBtnPressed();
    }
}

namespace test_3
{
    class OperationAPI
    {
        public:
            virtual int     getResult()         =   0;
            virtual void    setResult(int res)  =   0;
            virtual void    add(int num)        =   0;
            virtual void    substract(int num)  =   0;
    };

    class Operation : public OperationAPI
    {
        public:
            virtual int     getResult()     {
                return result;
            }
            virtual void    setResult(int res)  {
                result = res;
            }
            virtual void    add(int num)    {
                result += num;
            }
            virtual void    substract(int num)  {
                result -= num;
            }
        private:
            int     result;
    };

    class Command
    {
        public:
            virtual void    execute()   =   0;
            virtual void    undo()      =   0;
    };

    class AddCommand : public Command
    {
        public:
            AddCommand(OperationAPI* op, int num) : operation(op), opNum(num)   {   }

            virtual void    execute()   {
                operation->add(opNum);
            }
            virtual void    undo()      {
                operation->substract(opNum);
            }
        private:
            int             opNum;
            OperationAPI*   operation;
    };

    class SubstractCommand : public Command
    {
        public:
            SubstractCommand(OperationAPI* op, int num) : operation(op), opNum(num)   {   }

            virtual void    execute()   {
                operation->substract(opNum);
            }
            virtual void    undo()      {
                operation->add(opNum);
            }
        private:
            int             opNum;
            OperationAPI*   operation;
    };

    class Calculator
    {
        public:
            void    setAddCmd(Command*  cmd)    {   addCmd  =   cmd;    }
            void    setSubCmd(Command*  cmd)    {   subCmd  =   cmd;    }

            void    addPressed()        {   
                addCmd->execute();    
                undoCmdList.push_back(addCmd);
            }
            void    subPressed()        {   
                subCmd->execute();    
                undoCmdList.push_back(subCmd);
            }

            void    undoPressed()       {
                if (undoCmdList.empty())
                {
                    std::cout << "sorry, undolist is empty\n";
                }
                else
                {
                    Command*    last = undoCmdList.back();
                    undoCmdList.pop_back();
                    if (!last)
                        return;

                    last->undo();
                    redoCmdList.push_back(last);
                }
            }
            void    redoPressed()       {
                if (redoCmdList.empty())
                {
                    std::cout << "sorry, redolist is empty\n";
                }
                else
                {
                    Command*    last = redoCmdList.back();
                    redoCmdList.pop_back();
                    if (!last)
                        return;
                    
                    last->execute();
                    undoCmdList.push_back(last);
                }
            }
        private:
            Command*        addCmd;
            Command*        subCmd;
            std::list<Command*> undoCmdList;
            std::list<Command*> redoCmdList;
    };

    void    assemble()
    {
        Calculator*     calc    = new Calculator();
        OperationAPI*   op      = new Operation; 
        Command*        addCmd  = new AddCommand(op, 5);
        Command*        subCmd  = new SubstractCommand(op, 3);

        calc->setAddCmd(addCmd);
        calc->setSubCmd(subCmd);

        calc->addPressed();
        std::cout << "add Pressed: " << op->getResult() << std::endl;
        calc->subPressed();
        std::cout << "add Pressed: " << op->getResult() << std::endl;

        calc->undoPressed();
        std::cout << "undo Pressed: "<< op->getResult() << std::endl;
        calc->undoPressed();
        std::cout << "undo Pressed: "<< op->getResult() << std::endl;

        calc->redoPressed();
        std::cout << "redo Pressed: "<< op->getResult() << std::endl;
        calc->redoPressed();
        std::cout << "redo Pressed: "<< op->getResult() << std::endl;
    }
}

namespace test_4
{
    class CookAPI
    {
        public:
            virtual void    cook(std::string name)  =   0;
    };

    class HotCook : public CookAPI
    {
        public:
            virtual void    cook(std::string name)
            {
                std::cout << "I am hot cooking " << name << " now\n";
            }
    };
    class CoolCook : public CookAPI
    {
        public:
            virtual void    cook(std::string name)
            {
                std::cout << "I am cool cooking " << name << " now\n";
            }
    };

    class Command 
    {
        public:
            virtual void        execute()       =   0;
            virtual Command*    getChopCommand() {  return nullptr; }
            virtual Command*    getDuckCommand() {  return nullptr; }
            virtual Command*    getPorkCommand() {  return nullptr; }
    };
    // 代码一样， 可以考虑模板类实现
    class ChopCommand : public Command
    {
        public:
            void            setCookApi(CookAPI* ck)     {   cookApi = ck;           }
            virtual void    execute()                   {   cookApi->cook("chop");  }
            virtual Command*    getChopCommand()        {  return this; }
        private:
            CookAPI*            cookApi{nullptr};
    };
    class DuckCommand : public Command
    {
        public:
            void            setCookApi(CookAPI* ck)     {   cookApi = ck;           }
            virtual void    execute()                   {   cookApi->cook("duck");  }
            virtual Command*    getDuckCommand()        {  return this; }
        private:
            CookAPI*            cookApi{nullptr};
    };
    class PorkCommand : public Command
    {
        public:
            void            setCookApi(CookAPI* ck)     {   cookApi = ck;           }
            virtual void    execute()                   {   cookApi->cook("pork");  }
            virtual Command*    getPorkCommand()        {  return this; }
        private:
            CookAPI*            cookApi{nullptr};
    };

    class MenuCommand : public Command
    {
        public:
            void        addCommand(Command* cmd)    {   cmdList.push_back(cmd); }

            virtual void execute()
            {
                for (Command* cmd : cmdList)
                {
                    cmd->execute();
                }
            }
        private:
            std::list<Command*>     cmdList;
    };

    class Waiter
    {
        public:
            void    orderDish(Command* cmd)
            {
                CookAPI*    hotCook     = new HotCook();
                CookAPI*    coolCook    = new CoolCook();

                if (cmd->getChopCommand())
                    ((ChopCommand*)cmd->getChopCommand())->setCookApi(hotCook);
                else if (cmd->getDuckCommand())
                    ((DuckCommand*)cmd->getDuckCommand())->setCookApi(hotCook);
                else if (cmd->getPorkCommand())
                    ((PorkCommand*)(cmd->getPorkCommand()))->setCookApi(coolCook);

                menuCmd->addCommand(cmd);
            }
            
            void    orderOver()
            {
                menuCmd->execute();
            }
        private:
            MenuCommand* menuCmd{new MenuCommand()};
    };

    void    assemble()
    {
        Waiter*     waiter = new Waiter();

        Command*    chopCmd     =   new ChopCommand();
        Command*    duckCmd     =   new DuckCommand;
        Command*    porkCmd     =   new PorkCommand;

        waiter->orderDish(chopCmd);
        waiter->orderDish(duckCmd);
        waiter->orderDish(porkCmd);

        waiter->orderOver();
    }
}

namespace test_5
{
    class CookAPI
    {
        public:
            virtual void    cook(int table_num, std::string name)   =   0;
    };
    class Command
    {
        public:
            virtual void    execute()                   =   0;
            virtual void    setCookAPI(CookAPI* api)    =   0;
            virtual int     getTableNum()               =   0;
    };

#define Class_Command(x, y)    \
    class x##Command : public Command                                           \
    {                                                                           \
        public:                                                                 \
            x##Command(int num) : table_num(num)    {   }                       \
            virtual void    execute()   {   cook_api->cook(table_num, y);}      \
            virtual void    setCookAPI(CookAPI* api)    {   cook_api = api;}    \
            virtual int     getTableNum()   {   return table_num;   }           \
        private:                                                                \
            CookAPI*        cook_api;                                           \
            int             table_num;                                          \
    }

    Class_Command(Chop, "Chop");
    Class_Command(Duck, "Duck");
    Class_Command(Pork, "Pork");

    class CmdQueue;
    class MenuCommand : public Command 
    {
        public:
            virtual void    execute();//   {   CmdQueue::addMenu(this);    }
            virtual void    setCookAPI(CookAPI* api)    {   }
            virtual int     getTableNum()               {   return 0;   }

            void    addCommand(Command* cmd)    {   cmd_list.push_back(cmd);    }
            std::list<Command*>& getCommands()   {   return cmd_list;            }

        private:
            std::list<Command*>             cmd_list;
    };

    class CmdQueue
    {
        public:
            static void     addMenu(MenuCommand* menu)  {
                ns_thread::Mutex_scope_lock lock(mutex);
                std::list<Command*>& cmd = menu->getCommands();
                for (Command* c: cmd)  {
                    cmd_list.push_back(c);
                }
            }
            static Command* getOneCmd() {
                ns_thread::Mutex_scope_lock lock(mutex);
                if (cmd_list.empty())
                    return nullptr;

                Command* cmd = cmd_list.front();
                cmd_list.pop_front();
                return cmd;
            }
        private:
            static std::list<Command*>             cmd_list;
            static ns_thread::Mutex                           mutex;
    };

    std::list<Command*> CmdQueue::cmd_list;
    ns_thread::Mutex CmdQueue::mutex;

    void MenuCommand::execute() {   CmdQueue::addMenu(this);    }

    class Waiter
    {
        public:
            void    orderDish(Command* cmd) {   menuCmd.addCommand(cmd);    }
            void    orderOver()             {   menuCmd.execute();          }
        private:
            MenuCommand        menuCmd;
    };

    class HotCook : public CookAPI, public ns_thread::Thread
    {
        public:
            HotCook(std::string s) : Thread(s, true), name(s)    {   }
            virtual void cook(int tblNum, std::string cmd_name)
            {
                int sec = rand() % 20;
                std::cout<< name << " is cooking " << cmd_name << " for table: " << tblNum <<std::endl;
                sleep(sec);
                std::cout<< name << " finish cooking " << cmd_name << " for table: " << tblNum 
                    << ", cost: " << sec << " seconds." <<std::endl;
            }

            virtual void run()
            {
                while (1)
                {
                    Command* cmd = CmdQueue::getOneCmd();
                    if (!cmd)
                    {
                        sleep(1);
                        continue;
                    }
                    cmd->setCookAPI(this);
                    cmd->execute();
                }
            }

        private:
            std::string     name;
    };

    void    assemble()
    {
        ns_thread::ThreadGroup group;
        HotCook* zs = new HotCook("A");
        HotCook* ls =  new HotCook("B");
        HotCook* ww = new HotCook("C");
        group.add(zs);
        group.add(ls);
        group.add(ww);

        zs->start();
        ls->start();
        ww->start();

        for (int idx = 0; idx < 10; idx ++)
        {
            Waiter waiter;
            Command*    chop = new ChopCommand(idx);
            Command*    duck = new DuckCommand(idx);
            Command*    pork = new PorkCommand(idx);

            waiter.orderDish(chop);
            waiter.orderDish(duck);
            waiter.orderDish(pork);
            
            waiter.orderOver();
        }
        while (1)
        {
            ::sleep(1000);
        }
    }
}

int main()
{
    //test_1::assemble();
    //test_2::assemble();
    //test_3::assemble();
    //test_4::assemble();
    test_5::assemble();

    return 0;
}

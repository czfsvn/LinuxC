/*
 *
 *  from:http://www.cnblogs.com/benxintuzi/p/4931258.html
 *
 */

#include "stdio.h"  // printf, scanf
#include "stdlib.h" // system

#include <string>
#include <limits.h>
#include <vector>
#include <unistd.h>

namespace ns_test1
{
    class State
    {
        public:
            virtual State* Transition(char c) = 0;
    };

    class Fsm
    {
        public:
            Fsm();
            void Reset();
            void Advance(char c);
            int EndState();
            int DoomState();

        private:
            State*  curState_;
    };

    class State1 : public State
    {
        public:
            State* Transition(char c);
    };

    class State2 : public State
    {
        public:
            State* Transition(char c);
    };

    class State3 : public State
    {
        public:
            State* Transition(char c);
    };

    class State4 : public State
    {
        public:
            State* Transition(char c);
    };

    class State5 : public State
    {
        public:
            State* Transition(char c);
    };

    class State6 : public State
    {
        public:
            State* Transition(char c);
    };


    State1  s1;
    State2  s2;
    State3  s3;
    State4  s4;
    State5  s5;
    State6  s6;

    Fsm::Fsm()  : curState_(nullptr)    {}
    void Fsm::Reset()   
    {
        curState_ = &s1;
    }
    void Fsm::Advance(char c)
    {
        if (curState_)
            curState_ = curState_->Transition(c);
    }

    int Fsm::EndState()
    {
        return curState_ == &s6;
    }

    int Fsm::DoomState()
    {
        return curState_ == nullptr;
    }

    State* State1::Transition(char c)
    {
        switch(c)
        {
            case 'A':
                return &s2;
            case 'B':
                return &s3;
            case 'C':
                return &s4;
            case 'D':
                return &s5;
            case '\0':
                return nullptr;
            default:
                return nullptr;
        }
    }

    State* State2::Transition(char c)
    {
        switch(c)
        {
            case 'E':
                return &s2;
            case 'I':
                return &s6;
            case '\0':
                return nullptr;
            default:
                return nullptr;
        }
    }
    State* State3::Transition(char c)
    {
        switch(c)
        {
            case 'F':
                return &s3;
            case 'M':
                return &s4;
            case 'J':
                return &s6;
            case '\0':
                return nullptr;
            default:
                return nullptr;
        }
    }
    State* State4::Transition(char c)
    {
        switch(c)
        {
            case 'G':
                return &s4;
            case 'K':
                return &s6;
            case '\0':
                return nullptr;
            default:
                return nullptr;
        }

    }
    State* State5::Transition(char c)
    {
        switch(c)
        {
            case 'O':
                return &s2;
            case 'H':
                return &s5;
            case 'L':
                return &s6;
            case 'N':
                return &s4;
            case '\0':
                return nullptr;
            default:
                return nullptr;
        }
    }
    State* State6::Transition(char c)
    {
        return nullptr;
    }

    void Test_FSM()
    {
        char input_string[80];
        printf("Enter input expression: ");
        scanf("%s", input_string);

        Fsm fsm;
        fsm.Reset();
        int index = 0;
        fsm.Advance(input_string[index++]);

        while (!fsm.EndState() && !fsm.DoomState())
            fsm.Advance(input_string[index++]);

        if (fsm.EndState())
            printf("\nValid input expression\n");
        else
            printf("\nInvalid input expression\n");
    }
    void Test()
    {
        Test_FSM();
        system("pause");
    }
}

namespace ns_test2
{
    const int range = CHAR_MAX + 1;

    class State
    {
        public:
            State();
            State* transition[range];
    };

    struct TransGraph
    {
        int     current_state;
        char    input_char;
        int     next_state;
    };

    class Fsm
    {
        public:
            Fsm();
            void    Reset();
            void    Advance(char c);
            int     EndState();
            int     DoomState();

        private:
            State*   curState_;
            State   state_[6];
    };

    State::State()
    {
        for (int i = 0; i < range; i++)
        {
            transition[i] = nullptr;
        }
    }

    Fsm::Fsm()
    {
        static TransGraph  graph[] = 
        {
            {1, 'A', 2}, {1, 'B', 3}, {1, 'C', 4}, {1, 'D', 5},
            {2, 'E', 2}, {2, 'I', 0},
            {3, 'F', 3}, {3, 'J', 0}, {3, 'M', 4},
            {4, 'G', 4}, {4, 'K', 0},
            {5, 'H', 5}, {5, 'L', 0}, {5, 'O', 2}, {5, 'N', 4},
            {0, 0, 0}
        };

        for (TransGraph* p_tg = graph; p_tg->current_state != 0; ++p_tg)
        {
            state_[p_tg->current_state].transition[p_tg->input_char] = &state_[p_tg->next_state];
        }

        curState_ = nullptr;
    }

    void Fsm::Reset()
    {
        curState_ = &state_[1];
    }

    void Fsm::Advance(char c)
    {
        if (curState_)
            curState_ = curState_->transition[c];
    }

    int Fsm::EndState()
    {
        return curState_ == &state_[0];
    }
    int Fsm::DoomState()
    {
        return curState_ == nullptr;
    }

    void Test_FSM()
    {
        char    input_string[80];
        printf("Enter input expression: ");
        scanf("%s", input_string);

        Fsm     fsm;
        fsm.Reset();
        int idx = 0;
        fsm.Advance(input_string[idx++]);

        while (!fsm.EndState() && !fsm.DoomState())
            fsm.Advance(input_string[idx++]);

        if (fsm.EndState())
            printf("\nValid input expression\n");
        else
            printf("\nInvalid input expression\n");
    }

    void Test()
    {
        Test_FSM();
        system("pause");
    }
}

namespace ns_test3
{
    const int range = CHAR_MAX + 1;

    class State
    {
        public:
            State();
            State* transition[range];
    };

    struct TransGraph
    {
        int     current_state;
        char    input_char;
        int     next_state;
    };

    class Fsm
    {
        public:
            Fsm(TransGraph* gf);
            virtual ~Fsm();
            void    Reset();
            void    Advance(char c);
            int     EndState();
            int     DoomState();

        private:
            State*   curState_;
            State*   state_;
    };

    State::State()
    {
        for (int i = 0; i < range; i++)
        {
            transition[i] = nullptr;
        }
    }

    Fsm::Fsm(TransGraph* p_tg)
    {
        int max_state = 0;
        for (TransGraph* tmp = p_tg; tmp->current_state != 0; ++tmp)
        {
            if (tmp->current_state > max_state)
                max_state = tmp->current_state;
            if (tmp->next_state > max_state)
                max_state = tmp->next_state;
        }

        state_ = new State[max_state + 1];

        for (TransGraph* tmp = p_tg; tmp->current_state != 0; ++tmp)
        {
            state_[tmp->current_state].transition[tmp->input_char] = &state_[tmp->next_state];
        }

        curState_ = nullptr;
    }

    Fsm::~Fsm()
    {
        delete []state_;
    }

    void Fsm::Reset()
    {
        curState_ = &state_[1];
    }

    void Fsm::Advance(char c)
    {
        if (curState_)
            curState_ = curState_->transition[c];
    }

    int Fsm::EndState()
    {
        return curState_ == &state_[0];
    }
    int Fsm::DoomState()
    {
        return curState_ == nullptr;
    }

    void Test_FSM()
    {
        char    input_string[80];
        printf("Enter input expression: ");
        scanf("%s", input_string);

        static TransGraph  graph[] = 
        {
            {1, 'A', 2}, {1, 'B', 3}, {1, 'C', 4}, {1, 'D', 5},
            {2, 'E', 2}, {2, 'I', 0},
            {3, 'F', 3}, {3, 'J', 0}, {3, 'M', 4},
            {4, 'G', 4}, {4, 'K', 0},
            {5, 'H', 5}, {5, 'L', 0}, {5, 'O', 2}, {5, 'N', 4},
            {0, 0, 0}
        };

        Fsm     fsm(graph);
        fsm.Reset();
        int idx = 0;
        fsm.Advance(input_string[idx++]);

        while (!fsm.EndState() && !fsm.DoomState())
            fsm.Advance(input_string[idx++]);

        if (fsm.EndState())
            printf("\nValid input expression\n");
        else
            printf("\nInvalid input expression\n");
    }

    void Test()
    {
        Test_FSM();
        system("pause");
    }
}

namespace ns_test4
{
    struct Base
    {
        std::vector<int> iVec;
        Base(int idx)
        {
            iVec.reserve(idx);
        }
    };

    void Test()
    {
        std::vector<Base> vBase;
        int idx = 100000;
        while (1)
        {
            vBase.push_back(Base(idx));
            ::usleep(5*1000);
        }
    }
}

int main()
{
    //ns_test1::Test();
    //ns_test2::Test();
    //ns_test3::Test();
    ns_test4::Test();
    return 0;
}

#include <iostream>
#include "Card.h"
#include <list>
#include <algorithm>
#include <random>
#include <unistd.h>

using namespace std;

#define POKER_NUM  10 

namespace ns_card
{
    class MyRand
    {
        public:
            int operator()(int index)
            {
                return rand() % POKER_NUM;
            }
    };
    void Test()
    {
        Card* card = new Card(CardShape_Spades, CardNum_3);
        std::cout << "shape: " << card->getCardName();
    }

    bool comp_player(const Player* lhs, const Player* rhs)
    {
        if (rhs->getCardType() != lhs->getCardType())
        {
            return lhs->getCardType() < rhs->getCardType();
        }
        // type is equal
        if (lhs->getCard(0)->getNumber() != rhs->getCard(0)->getNumber())
            return lhs->getCard(0)->getNumber() > rhs->getCard(0)->getNumber();

        if (lhs->getCard(1)->getNumber() != rhs->getCard(1)->getNumber())
            return lhs->getCard(1)->getNumber() > rhs->getCard(1)->getNumber();

        if (lhs->getCard(2)->getNumber() != rhs->getCard(2)->getNumber())
            return lhs->getCard(2)->getNumber() > rhs->getCard(2)->getNumber();

        return lhs->getCard(0)->getShape() < rhs->getCard(0)->getShape();
    }

    void generateCards()
    {
        std::srand ( unsigned ( std::time(0) ) );
        std::list<Player*> player_list;
        for (uint32_t idx = 0; idx < 17; idx++)
        {
            player_list.emplace_back(new Player(idx));
        }

        std::vector<Card*> cards_vec;
        for (uint32_t sp = CardShape_Spades; sp < CardShape_invalid; sp++)
        {
            for (uint32_t num = CardNum_2; num < CardNum_invalid; num++)
                cards_vec.emplace_back(new Card((CardShape)sp, (CardNumber)num));
        }

        //std::random_shuffle(cards_vec.begin(), cards_vec.end()/*, MyRand()*/);

        for (Card* c : cards_vec)
        {
            //std::cout << "card: " << c->getCardName() << std::endl;
        }

        for (uint32_t idx = 0; idx < MAX_CARD_SIZE; idx++)
        {
            for (Player* p : player_list)
            {
                uint32_t rd = rand() % cards_vec.size();
                p->addCard(cards_vec[rd]);
                auto iter = cards_vec.begin();
                std::advance(iter, rd);
                cards_vec.erase(iter);
            }
        }

        for (Player* p : player_list)
        {
            p->freshCards();
        }
        player_list.sort(comp_player);
        for (Player* p : player_list)
        {
            p->showCards();
        }
    }
}

int main()
{
    //ns_card::Test();
    uint32_t times = 2;
    while (times-- > 0)
    {
        std::cout << "------------------------\n";
        ns_card::generateCards();
        sleep(1);
    }
    return 0;
}

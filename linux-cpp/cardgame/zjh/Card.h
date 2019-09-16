#ifndef __CARD_20190914_H__
#define __CARD_20190914_H__

#include <stdint.h>
#include <vector>
#define MAX_CARD_SIZE       3

// 豹子 > 同花顺 > 童话 > 顺子 > 对子 > 单张

enum CardsType
{
    CardsType_null              =   0,      // null
    CardsType_Bomb              =   1,      // 
    CardsType_StraightFlush     =   2,
    CardsType_Flush             =   3,
    CardsType_Straight          =   4,
    CardsType_Pair              =   5,
    CardsType_Single            =   6,
    CardsType_invalid           =   7,
};

enum CardShape
{
    CardShape_null          =   0,      // null
    CardShape_Spades        =   1,      //黑桃
    CardShape_Hearts        =   2,      //红心
    CardShape_Clubs         =   3,      //梅花
    CardShape_Diamonds      =   4,      //方片
    CardShape_invalid       =   5,      // invalide
};

enum CardNumber
{
    CardNum_null            =   0,
    CardNum_2               =   2,
    CardNum_3               =   3,
    CardNum_4               =   4,
    CardNum_5               =   5,
    CardNum_6               =   6,
    CardNum_7               =   7,
    CardNum_8               =   8,
    CardNum_9               =   9,
    CardNum_10              =   10,
    CardNum_J               =   11,
    CardNum_Q               =   12,
    CardNum_K               =   13,
    CardNum_A               =   14,
    CardNum_invalid         =   15,
};

class Card
{
    public:
        Card(CardShape, CardNumber);
        ~Card();

        CardShape getShape() const    {   return shape_;   }

        CardNumber getNumber() const  {   return number_;  }

        std::string getCardName() const;

    private:
        CardShape  shape_ = CardShape_null;
        CardNumber number_ = CardNum_null;
        //int32_t value = 1;
};


class Player
{
    public:
        Player(uint32_t pid) : player_id_(pid)  {   }

        void addCard(Card* card)
        {
            if (!card)
                return;

            if (cards_vec_.size() >= MAX_CARD_SIZE)
                return;

            cards_vec_.emplace_back(card);
        }

        void freshCards();
        void freshPair();
        void freshStraight();

        CardsType getCardType() const    {   return card_type_;  }

        void showCards();

        // 是否是炸弹
        bool isBomb();

        bool isStraightFlush();

        bool isFlush();

        bool isStraight();

        bool isPair();

        bool operator < (const Player& p);
        bool operator < (const Player* p);

        Card* getCard(uint32_t idx) const 
        {
            return cards_vec_[idx];
        }

    private:
        std::vector<Card*> cards_vec_;
        uint32_t player_id_ = 0;
        CardsType card_type_ = CardsType_null;
};

#endif

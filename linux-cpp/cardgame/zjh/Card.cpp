#include "Card.h"
#include <iostream>
#include<sstream>

using namespace std;

namespace ns_card
{
    std::string getCardShape(const Card* card)
    {
        if (!card)
            return "";
        if (card->getShape() == CardShape_Spades)
            return "♠️ ";
        else if (card->getShape() == CardShape_Hearts)
            return "♥️ ";
        else if (card->getShape() == CardShape_Clubs)
            return "♣️ ";
        else if (card->getShape() == CardShape_Diamonds)
            return "♦️ ";
        return "";
    }

    std::string getNumber(const Card* card)
    {
        if (!card)
            return "";

        if (card->getNumber() == CardNum_2)
            return "2";
        else if (card->getNumber() == CardNum_3)
            return "3";
        else if (card->getNumber() == CardNum_4)
            return "4";
        else if (card->getNumber() == CardNum_5)
            return "5";
        else if (card->getNumber() == CardNum_6)
            return "6";
        else if (card->getNumber() == CardNum_7)
            return "7";
        else if (card->getNumber() == CardNum_8)
            return "8";
        else if (card->getNumber() == CardNum_9)
            return "9";
        else if (card->getNumber() == CardNum_10)
            return "10";
        else if (card->getNumber() == CardNum_J)
            return "J";
        else if (card->getNumber() == CardNum_Q)
            return "Q";
        else if (card->getNumber() == CardNum_K)
            return "K";
        else if (card->getNumber() == CardNum_A)
            return "A";
        return "";
    }

    std::string getCardType(const Player* p)
    {
        if (!p)
            return "";

        CardsType type = p->getCardType();
        if (type == CardsType_Bomb)
            return "Bomb";

        else if (type == CardsType_StraightFlush)
            return "StraightFlush";

        else if (type == CardsType_Flush)
            return "Flush";

        else if (type == CardsType_Straight)
            return "Straight";

        else if (type == CardsType_Pair)
            return "Pair";

        return "Single";
    }
    std::string getCardName(const Card* card)
    {
        if (!card)
            return "";

        std::stringstream oss;
        oss << getCardShape(card) << getNumber(card);
        return oss.str();
    }

    bool card_sort(const Card* lhs, const Card* rhs)
    {
        if (!lhs || !rhs)
            return false;

        if (lhs->getNumber() < rhs->getNumber())
            return false;

        if (lhs->getNumber() == rhs->getNumber())
        {
            return lhs->getShape() < rhs->getShape();
        }
        return true;
    }
}
Card::Card(CardShape shape, CardNumber num) : shape_(shape), number_(num)
{
}

Card::~Card()
{
}

std::string Card::getCardName() const
{
    return ns_card::getCardName(this);
}

void Player::showCards()
{
    std::cout << "Player: " << player_id_ << ", type:" << ns_card::getCardType(this) << ", cards: ";
    //std::sort(cards_vec_.begin(), cards_vec_.end(), ns_card::card_sort);
    for (Card* c : cards_vec_)
        std::cout << c->getCardName() << ", ";
    std::cout << std::endl;
}

void Player::freshCards()
{
    std::sort(cards_vec_.begin(), cards_vec_.end(), ns_card::card_sort);

    if (isBomb())
        card_type_ = CardsType_Bomb;

    else if (isStraightFlush())
        card_type_ = CardsType_StraightFlush;

    else if (isFlush())
        card_type_ = CardsType_Flush;

    else if (isStraight())
        card_type_ = CardsType_Straight;

    else if (isPair())
        card_type_ = CardsType_Pair;

    else 
        card_type_ = CardsType_Single;

    if (card_type_ == CardsType_Pair)
    {
        freshPair();  
    }
    else if (card_type_ == CardsType_Straight)
    {
        freshStraight();
    }
}

bool Player::isBomb()
{
    return cards_vec_[0]->getNumber() == cards_vec_[1]->getNumber() &&
        cards_vec_[0]->getNumber() == cards_vec_[2]->getNumber();
}

bool Player::isStraightFlush()
{
    return isFlush() && isStraight();
}

bool Player::isFlush()
{
    return cards_vec_[0]->getShape() == cards_vec_[1]->getShape() &&
        cards_vec_[0]->getShape() == cards_vec_[2]->getShape();
}

bool Player::isStraight()
{
    if ((cards_vec_[0]->getNumber() - 1) == cards_vec_[1]->getNumber() &&
        (cards_vec_[0]->getNumber() - 2) == cards_vec_[2]->getNumber())
            return true;

    if (cards_vec_[0]->getNumber() == CardNum_A && 
            cards_vec_[1]->getNumber() == CardNum_3 &&
            cards_vec_[2]->getNumber() == CardNum_2)
        return true;

    return false;
}

bool Player::isPair()
{
    return cards_vec_[0]->getNumber() == cards_vec_[1]->getNumber() ||
        cards_vec_[0]->getNumber() == cards_vec_[2]->getNumber() ||
        cards_vec_[1]->getNumber() == cards_vec_[2]->getNumber();
}

void Player::freshPair()
{
    if (cards_vec_[2]->getNumber() == cards_vec_[1]->getNumber())
    {
        Card* c0 = cards_vec_[0];
        Card* c1 = cards_vec_[1];
        Card* c2 = cards_vec_[2];

        cards_vec_.clear();

        cards_vec_.emplace_back(c1);
        cards_vec_.emplace_back(c2);
        cards_vec_.emplace_back(c0);
    }
}

void Player::freshStraight()
{
    if (cards_vec_[0]->getNumber() == CardNum_A && 
            cards_vec_[1]->getNumber() == CardNum_3 &&
            cards_vec_[2]->getNumber() == CardNum_2)
    {
        Card* c0 = cards_vec_[0];
        Card* c1 = cards_vec_[1];
        Card* c2 = cards_vec_[2];

        cards_vec_.clear();

        cards_vec_.emplace_back(c1);
        cards_vec_.emplace_back(c2);
        cards_vec_.emplace_back(c0);
    }
}

bool Player::operator < (const Player& rhs)
{
    std::cout << "operator\n";
    if (rhs.getCardType() != getCardType())
    {
        return getCardType() < rhs.getCardType();
    }
    // type is equal
    if (cards_vec_[0]->getNumber() != rhs.cards_vec_[0]->getNumber())
        return cards_vec_[0]->getNumber() > rhs.cards_vec_[0]->getNumber();

    if (cards_vec_[1]->getNumber() != rhs.cards_vec_[1]->getNumber())
        return cards_vec_[1]->getNumber() > rhs.cards_vec_[1]->getNumber();

    if (cards_vec_[2]->getNumber() != rhs.cards_vec_[2]->getNumber())
        return cards_vec_[2]->getNumber() > rhs.cards_vec_[2]->getNumber();

    return cards_vec_[0]->getShape() < rhs.cards_vec_[0]->getShape();
}

bool Player::operator < (const Player* rhs)
{
    std::cout << "operator\n";
    if (rhs->getCardType() != getCardType())
    {
        return getCardType() < rhs->getCardType();
    }
    // type is equal
    if (cards_vec_[0]->getNumber() != rhs->cards_vec_[0]->getNumber())
        return cards_vec_[0]->getNumber() > rhs->cards_vec_[0]->getNumber();

    if (cards_vec_[1]->getNumber() != rhs->cards_vec_[1]->getNumber())
        return cards_vec_[1]->getNumber() > rhs->cards_vec_[1]->getNumber();

    if (cards_vec_[2]->getNumber() != rhs->cards_vec_[2]->getNumber())
        return cards_vec_[2]->getNumber() > rhs->cards_vec_[2]->getNumber();

    return cards_vec_[0]->getShape() < rhs->cards_vec_[0]->getShape();
}

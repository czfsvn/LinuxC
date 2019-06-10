#include "Player.h"
#include "behaviac_generated/types/behaviac_types.h"

Player::Player(int idx, std::string name) : id_(idx), name_(name)
{
    CLOG("[Player] Player %s(%u)\n", name_.c_str(), id_);
}

Player::~Player()
{
    CLOG("[Player] ~Player %s(%u)\n", name_.c_str(), id_);
    if (playerAgent_)
        behaviac::Agent::Destroy(playerAgent_);
}

void Player::setAgent(PlayerAgent* pa)
{
    playerAgent_ = pa;
}

void Player::update()
{
    //CLOG("[Player] update %s(%u)", name_.c_str(), id_);
    if (!playerAgent_)
        return;

    behaviac::EBTStatus status = behaviac::BT_RUNNING;
    while (status == behaviac::BT_RUNNING)
    {
        status = playerAgent_->btexec();
    }
}

void Player::initAgent(const char* nodeName)
{
    CLOG("[Player] initAgent %s(%u)\n", name_.c_str(), id_);
}

void Player::autoMove()
{
    CLOG("[Player] autoMove %s(%u)\n", name_.c_str(), id_);
}

void Player::autoFight()
{
    CLOG("[Player] autoFight %s(%u)\n", name_.c_str(), id_);
}

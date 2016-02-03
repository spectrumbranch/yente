#include "Player.h"

Player::Player(const unsigned int& id, const unsigned int& level) : id_(id), level_(level)
{
}

Player::~Player()
{
}

Player::Player() : id_(0), level_(0)
{
}


unsigned int Player::getId() const
{
	return id_;
}

unsigned int Player::getLevel() const
{
	return level_;
}

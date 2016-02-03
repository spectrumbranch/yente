#include "RandomDataSource.h"

#include <algorithm>

RandomDataSource::RandomDataSource(const unsigned int& maxLevel, const unsigned int& totalPlayers) : 
	playerId_(0), 
	maxLevel_(maxLevel), 
	totalPlayers_(totalPlayers)
{
}

RandomDataSource::~RandomDataSource()
{
}

bool RandomDataSource::playerSortFunction(const Player& left, const Player& right)
{
	return left.getLevel() < right.getLevel();
}

bool RandomDataSource::retrievePlayerData(PlayerContainer& playerContainer, const unsigned int& nPlayers)
{
	// Retrieve data for the requested amount from the total amount of players.
	for(unsigned int playerCount = 0; playerCount < nPlayers && playerId_ <= totalPlayers_; ++playerCount)
	{
		playerContainer.push_back(Player(++playerId_, 100/*rand() % maxLevel_*/));
	}

	// Sort all the entries only if the vector is not empty
	if(!playerContainer.empty())
	{	
		std::sort(playerContainer.begin(), playerContainer.end(), RandomDataSource::playerSortFunction);
	}

	return !playerContainer.empty();
}
